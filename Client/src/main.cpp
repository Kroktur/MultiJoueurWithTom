#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <ostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "MySocket.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

struct Toto
{
    int a;
    float b;
};

int main(void)
{

	// Initialize Network
  NetWork::InitializeNetWork();
  int iResult = 1;

  // create socket info for a TCP client using IPv4
	SocketInfo socketInfo;
	socketInfo.ipAdress = IpAddressType::IPv4;
	socketInfo.protocol = Protocol::TCP;
	socketInfo.socketType = SocketType::TCP;
	socketInfo.role = Role::CLIENT;

	// create socket and set its info
    SocketTcp socket = SocketTcp(socketInfo,DEFAULT_PORT,"localhost");
    

    Toto toto;
	toto.a = 42;

    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    
    int t = 0;
    do {
        if (!socket.IsConnected())
        {
            while (!socket.IsConnected())
            {
                socket.Connect();
            }
        }
        else {
            t++;
            if (t >= 10)
				break;
            SocketManager::Send(socket.GetSocket().GetSocket(),reinterpret_cast<const char*>(&toto), sizeof(toto));
            iResult =SocketManager::Recv(socket.GetSocket().GetSocket(),recvbuf, recvbuflen);
            if (iResult > 0)
            {
				auto test = reinterpret_cast<Toto*>(recvbuf);
				if (test->a == 100)
				{
					std::cout << "Received struct with a = 100" << std::endl;
				}
                recvbuf[iResult] = '\0';
                printf("Bytes received: %s\n", recvbuf);
            }
        }
    } while (iResult > 0);



    // cleanup
    if (socket.IsConnected())
		socket.Disconnect();
    SocketManager::CloseSocketAndFree(socket.GetSocket().GetSocket(), socket.GetMyAddrInfo());
	NetWork::UnInitialize();

    return 0;
}