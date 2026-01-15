#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <ostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

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

	SocketData socketData(socketInfo, DEFAULT_PORT, "localhost");
	// create socket and set its info

    TcpSocket socket(socketData);
    std::string toto;
	toto = "42";

    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    
    int t = 0;
    do {
        if (!socket.IsConnected())
        {
            while (!socket.IsConnected())
            {
                TCPClientConnector::Connect(socket);
            }
        }
        else {
            std::getline(std::cin, toto);
             iResult = socket.Send(reinterpret_cast<const char*>(&toto), sizeof(toto));
            if (iResult == 0)
                break;
           /* iResult = socket.Receive(recvbuf, recvbuflen);
            if (iResult > 0)
            {
				auto test = reinterpret_cast<std::string*>(recvbuf);
				std::cout << "Received string: " << *test << std::endl;
                recvbuf[iResult] = '\0';
            }*/
        }
    } while (iResult > 0);



    // cleanup
    if (socket.IsConnected())
		socket.Disconect();
    SocketManager::CloseSocketAndFree(socket.GetSocket(), socket.GetMyAddrInfo());
	NetWork::UnInitialize();

    return 0;
}