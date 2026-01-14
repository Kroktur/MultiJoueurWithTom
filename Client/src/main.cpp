#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <ostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "Socket.h"

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
   
    WSADATA wsaData;
    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }


    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    // for IPV4 or IPV6 addresses 
    hints.ai_family = AF_INET;
    // standard stream socket for TCP
    hints.ai_socktype = SOCK_STREAM;
    // use TCP protocol
    hints.ai_protocol = IPPROTO_TCP;

    Socket c(hints, DEFAULT_PORT);

    Toto toto;
	toto.a = 42;

    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int t = 0;
    do {
        if (c.isConnected() == false)
        {
            c.SetupConnection("localhost");
            bool connected = false;
            while (!connected)
            {
                connected = c.Connect();
            }
            iResult = 1;
        }
        else {
            t++;
            if (t >= 10)
				break;
            c.Send(reinterpret_cast<const char*>(&toto), sizeof(&toto));
            iResult = c.Recv(recvbuf, recvbuflen);
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
    if (c.isConnected())
        c.Disconnect();
    c.CloseSocketAndFree();
    WSACleanup();

    return 0;
}