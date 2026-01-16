//#undef UNICODE
//
//#define WIN32_LEAN_AND_MEAN
//
//#include <iostream>
//#include <ostream>
//#include <windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string>
//
//#include "MySocket.h"
//
//// Need to link with Ws2_32.lib
//#pragma comment (lib, "Ws2_32.lib")
//// #pragma comment (lib, "Mswsock.lib")
//
//#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "27015"
//
//struct Toto
//{
//    int a;
//    float b;
//};
//
//int main(void)
//{
//	// Initialize Network
//  NetWork::InitializeNetWork();
//  int iResult = 1;
//
//  // create socket info for a TCP client using IPv4
//	SocketInfo socketInfo;
//	socketInfo.ipAdress = IpAddressType::IPv4;
//	socketInfo.protocol = Protocol::TCP;
//	socketInfo.socketType = SocketType::TCP;
//	socketInfo.role = Role::CLIENT;
//
//	SocketData socketData(socketInfo, DEFAULT_PORT, "localhost");
//	// create socket and set its info
//
//    TcpSocket socket(socketData);
//    std::string toto;
//	toto = "42";
//
//    const char* sendbuf = "this is a test";
//    char recvbuf[DEFAULT_BUFLEN];
//    int recvbuflen = DEFAULT_BUFLEN;
//
//    SocketManager::OpenSocket(socket.GetSocket(), socket.GetMyAddrInfo());
//    int t = 0;
//    do {
//        if (!socket.IsConnected())
//        {
//            while (!socket.IsConnected())
//            {
//                TCPClientConnector::Connect(socket);
//            }
//        }
//        else {
//            std::getline(std::cin, toto);
//             iResult = socket.Send(reinterpret_cast<const char*>(&toto), sizeof(toto));
//            if (iResult == 0)
//                break;
//           /* iResult = socket.Receive(recvbuf, recvbuflen);
//            if (iResult > 0)
//            {
//				auto test = reinterpret_cast<std::string*>(recvbuf);
//				std::cout << "Received string: " << *test << std::endl;
//                recvbuf[iResult] = '\0';
//            }*/
//        }
//    } while (iResult > 0);
//
//
//
//    // cleanup
//    if (socket.IsConnected())
//		socket.Disconect();
//    SocketManager::CloseSocketAndFree(socket.GetSocket(), socket.GetMyAddrInfo());
//	NetWork::UnInitialize();
//
//    return 0;
//}






#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <chrono>
#include <iostream>
#include <print>
#include <thread>

#include "NetWork.h"
#include "MySocket.h"
#include "SocketInfo.h"
int main(int argc, char** argv)
{
    NetWork::InitializeNetWork();

    SocketAddr serverAddr = SocketAddr::Create(IpAddressType::IPv4,"50960", "127.0.0.1");


    SocketInfo socketInfo;
    	socketInfo.ipAdress = IpAddressType::IPv4;
    	socketInfo.protocol = Protocol::UDP;
    	socketInfo.socketType = SocketType::UDP;
    	socketInfo.role = Role::CLIENT;
    
	SocketData socketData(socketInfo, "50960", "127.0.0.1");
	UdpSocket ClientSocket(socketData);
    UdpBind::BindUdp(ClientSocket);


    std::jthread pulseThread([&](std::stop_token st) {
        while (!st.stop_requested()) {
            ClientSocket.SendTo("P", 1, serverAddr);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        });

    std::print("Enter a nickname: ");
    std::string nickname;
    std::getline(std::cin, nickname);

    std::string msg = "N" + nickname;
    ClientSocket.SendTo( msg.data(), msg.size(), serverAddr);

    std::jthread recvThread([&](std::stop_token st) {
        while (!st.stop_requested()) {
            char buffer[256];
            int bytes = ClientSocket.ReceiveFrom(buffer, 256,serverAddr);
            if (bytes > 0)
                std::println("{}", std::string(buffer, bytes));
        }
        });

    std::string input;
    while (std::getline(std::cin, input))
    {
        std::string msg = "M" + input;
        ClientSocket.SendTo( msg.data(), msg.size(), serverAddr);
    }

    recvThread.request_stop();
    pulseThread.request_stop();
	SocketManager::CloseSocketAndFree(ClientSocket.GetSocket(), ClientSocket.GetMyAddrInfo());
    NetWork::UnInitialize();
    return 0;
}



