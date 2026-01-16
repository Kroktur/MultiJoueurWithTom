//#undef UNICODE
//
//#define WIN32_LEAN_AND_MEAN
//
//#include <map>
//#include <windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <thread>
//#include "MySocket.h"
//#include "NetWork.h"
//
//#include <vector>
//#include <mutex>
//#include <queue>
//// Need to link with Ws2_32.lib
//#pragma comment (lib, "Ws2_32.lib")
//// #pragma comment (lib, "Mswsock.lib")
//
//#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "27015"
//
//static std::mutex mapMutex;
//static std::mutex countMutex;
//static std::mutex queueMutex;
//
//
//struct Toto
//{
//    int a;
//    float b;
//};
//
//struct ClientProcess
//{
//    int id;
//    std::map<int, TcpSocket*>* map;
//};
//
//
//void ClientThread(std::stop_token st, ClientProcess& c,std::queue<std::string>& queue)
//{
//    char recvbuf[DEFAULT_BUFLEN];
//    int recvbuflen = DEFAULT_BUFLEN;
//    while (!st.stop_requested())
//    {
//        TcpSocket* copy = nullptr;
//		{
//			std::scoped_lock lock(mapMutex);
//			copy = c.map->at(c.id);
//		}
//        int r = 0;
//        if (copy)
//            r = copy->Receive(recvbuf, recvbuflen);
//        if (r == 0)
//            break;
//
//		auto ctr = reinterpret_cast<std::string*>(recvbuf);
//
//        if (*ctr == "/quit")
//	     break;  
//        {
//            std::scoped_lock lock(queueMutex);
//            queue.push(*ctr);
//		}
//
//    	std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	}
//
//    TcpSocket* my = nullptr;
//    {
//        std::scoped_lock lock(mapMutex);
//        my = c.map->at(c.id);
//        c.map->erase(c.id);
//    }
//
//    my->Disconect();
//    SocketManager::CloseSocketAndFree(my->GetSocket(), my->GetMyAddrInfo());
//	delete my;
//	std::cout << "Client thread ending for id: " << c.id << "\n";
//}
//
//
//void ListenThread(std::stop_token st, TcpSocket& socket,std::map<int,TcpSocket*>& sockets,int& idCount, std::queue<std::string>& queue)
//{
//    TCPServerConnector::Listen(socket);
//	while (!st.stop_requested())
//	{
//        TcpSocket ClientSocket = std::move(TCPServerConnector::Accept(socket));
//            int id = 0;
//            {
//                std::scoped_lock lock(countMutex);
//                id = ++idCount;
//            }
//            {
//                std::scoped_lock lock(mapMutex);
//                sockets[id] = new TcpSocket(std::move(ClientSocket));
//            }
//                ClientProcess c{id, &sockets};
//                std::jthread* t =  new std::jthread(ClientThread, std::ref(c), std::ref(queue));
//			
//			std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    }
//	std::cout << "Listen thread ending\n";
//}
//
//
//
//
//int main(void)
//{
//    NetWork::InitializeNetWork();
//
//    // Initialize Winsock library
//    WSADATA wsaData;
//    // Error handling variable
//    int iResult;
//
//    // Create a SOCKET for the server to listen for client connections
//     // create socket info for a TCP client using IPv4
//    SocketInfo socketInfo;
//    socketInfo.ipAdress = IpAddressType::IPv4;
//    socketInfo.protocol = Protocol::TCP;
//    socketInfo.socketType = SocketType::TCP;
//    socketInfo.role = Role::SERVER;
//
//    SocketData socketData(socketInfo, DEFAULT_PORT, "localhost");
//    // create socket and set its info
//	TcpSocket socket(socketData);
//
//	TCPServerConnector::Bind(socket);
//	//TCPServerConnector::Listen(socket);
//	//TcpSocket ClientSocket =TCPServerConnector::Accept(socket);
//    int iSendResult;
//    char recvbuf[DEFAULT_BUFLEN];
//    int recvbuflen = DEFAULT_BUFLEN;
//
//    int idCount = 0;
//	std::map<int, TcpSocket*> sockets;
//	std::queue<std::string> messageQueue;
//	std::jthread j(ListenThread, std::ref(socket), std::ref(sockets), std::ref(idCount), std::ref(messageQueue));
//
//    while (true)
//    {
//		{
//			std::scoped_lock lock(queueMutex);
//            while (!messageQueue.empty())
//            {
//                auto msg = messageQueue.front();
//                messageQueue.pop();
//                std::cout << "Received message: " << msg << std::endl;
//			}
//		}
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	}
//
//   // // Receive until the peer shuts down the connection
//   // do {
//   //     
//   //     iResult = ClientSocket.Receive( recvbuf, recvbuflen);
//   //     if (!ClientSocket.IsConnected())
//   //         break;
//   //     if (iResult > 0) {
//   //         printf("Bytes received: %d\n", iResult);
//   //         // Echo the buffer back to the sender
//			//auto send = reinterpret_cast<std::string*>(recvbuf);
//   //         if (*send == std::string("/Quit"))
//   //         {
//   //             break;
//			//}
//
//   //         iSendResult = ClientSocket.Send(recvbuf, iResult);
//   //         if (iSendResult == SOCKET_ERROR) {
//   //             printf("send failed with error: %d\n", WSAGetLastError());
//   //             break;
//   //         }
//   //         printf("Bytes sent: %d\n", iSendResult);
//   //     }
//   //     else if (iResult == 0)
//   //         printf("Connection closing...\n");
//   // } while (iResult > 0);
//
//   // ClientSocket.Disconect();
//
//    if (socket.IsConnected())
//        socket.Disconect();
//    SocketManager::CloseSocketAndFree(socket.GetSocket(), socket.GetMyAddrInfo());
//    //SocketManager::CloseSocketAndFree(ClientSocket.GetSocket(), ClientSocket.GetMyAddrInfo());
//
//    NetWork::UnInitialize();
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
#include <mutex>
#include <thread>
#include <vector>

#include <print>

#include "MySocket.h"
struct ClientData
{
	SocketAddr address;
	uint8_t     deathCounter;
	std::string nickname;
};

bool operator==(const sockaddr_in& a, const sockaddr_in& b) 
{
	return  a.sin_addr.S_un.S_addr == b.sin_addr.S_un.S_addr;
}


bool operator==(const SocketAddr& lhs , const SocketAddr& rhs)
{
	return lhs.GetIp() == rhs.GetIp();
}
int main(int argc, char** argv)
{
	NetWork::InitializeNetWork();

	std::vector<ClientData> clients;
	std::mutex              clientMutex;

	std::jthread purgeThread([&](std::stop_token st) {
		while (!st.stop_requested())
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(2s);

			{
				std::scoped_lock lock(clientMutex);
				for (auto& c : clients)
					c.deathCounter++;

				auto it = std::remove_if(clients.begin(), clients.end(), [](const ClientData& c) {
					bool shouldRemove = c.deathCounter > 1;
					if (shouldRemove)
					{
						if (c.nickname.empty())
							std::println("Lost client: {}", c.address.GetIp());
						else
							std::println("Lost client: {}", c.nickname);
					}

					return shouldRemove;
					});
				if (it != clients.end())
					clients.erase(it);
			}
		}
		});

	SocketInfo serverInfo;
	serverInfo.ipAdress = IpAddressType::IPv4;
	serverInfo.protocol = Protocol::UDP;
	serverInfo.socketType = SocketType::UDP;
	serverInfo.role = Role::SERVER;

	SocketData serverData(serverInfo, "50960");
	

	UdpSocket ServerSocket(serverData);
	UdpBind::BindUdp(ServerSocket,ServerSocket.GetData().GetPort().c_str());


	auto HandlePulse = [&](const SocketAddr& addr)
		{
			std::scoped_lock lock(clientMutex);
			auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientData& c) { return c.address == addr; });
			if (it != clients.end())
				it->deathCounter = 0;
			else
			{
				std::println("New client: {}", addr.GetIp());
				clients.push_back({ addr, 0 });
			}
		};

	auto BroadcastMessage = [&](const std::string& author, const std::string& message)
		{
			std::string fullMessage = std::format("<{}> {}", author, message);
			std::scoped_lock lock(clientMutex);
			for (auto& c : clients)
				ServerSocket.SendTo( fullMessage.data(), fullMessage.size(), c.address);
		};

	auto SetNickname = [&](const SocketAddr& addr, const std::string& nickname)
		{
			std::scoped_lock lock(clientMutex);
			auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientData& c) { return c.address == addr; });
			if (it != clients.end())
			{
				it->nickname = nickname;
				std::println("{} is now \"{}\"", addr.GetIp(), nickname);
			}
		};

	auto GetNickname = [&](const SocketAddr& addr)
		{
			std::scoped_lock lock(clientMutex);
			auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientData& c) { return c.address == addr; });
			if (it != clients.end())
				return it->nickname;

			return std::string("(unknown)");
		};
	int count = 0;

	while (true)
	{
		char message[256];
		SocketAddr SenderAddr;
		SenderAddr.length= sizeof(sockaddr_in);

		int bytesReceived =  ServerSocket.ReceiveFrom(message, 256, SenderAddr);


		if (bytesReceived > 0)
		{
			switch (message[0])
			{
			case 'M':
				std::println("Message: {}", std::string(message + 1, bytesReceived - 1));
				BroadcastMessage(GetNickname(SenderAddr), std::string(message + 1, bytesReceived - 1));
				HandlePulse(SenderAddr);
				break;

			case 'N':
				SetNickname(SenderAddr, std::string(message + 1, bytesReceived - 1));
				HandlePulse(SenderAddr);
				break;

			case 'P':
				HandlePulse(SenderAddr);
				break;
			}
		}
	}

	SocketManager::CloseSocketAndFree(ServerSocket.GetSocket(), ServerSocket.GetMyAddrInfo());

	purgeThread.request_stop();
	purgeThread.join();
	NetWork::UnInitialize();
	return 0;
}


//#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
//#include <Windows.h>
//
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")
//
//#include <chrono>
//#include <mutex>
//#include <thread>
//#include <vector>
//
//#include <print>
//
//struct ClientData
//{
//	sockaddr_in address;
//	uint8_t     deathCounter;
//	std::string nickname;
//};
//
//bool operator==(const sockaddr_in& a, const sockaddr_in& b)
//{
//	return a.sin_family == b.sin_family && a.sin_port == b.sin_port && a.sin_addr.S_un.S_addr == b.sin_addr.S_un.S_addr;
//}
//
//int main(int argc, char** argv)
//{
//	WSADATA wsaData;
//	WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//	std::vector<ClientData> clients;
//	std::mutex              clientMutex;
//
//	std::jthread purgeThread([&](std::stop_token st) {
//		while (!st.stop_requested())
//		{
//			using namespace std::chrono_literals;
//			std::this_thread::sleep_for(2s);
//
//			{
//				std::scoped_lock lock(clientMutex);
//				for (auto& c : clients)
//					c.deathCounter++;
//
//				auto it = std::remove_if(clients.begin(), clients.end(), [](const ClientData& c) {
//					bool shouldRemove = c.deathCounter > 1;
//					if (shouldRemove)
//					{
//						if (c.nickname.empty())
//							std::println("Lost client: {}", c.address.sin_addr.S_un.S_addr);
//						else
//							std::println("Lost client: {}", c.nickname);
//					}
//
//					return shouldRemove;
//					});
//				if (it != clients.end())
//					clients.erase(it);
//			}
//		}
//		});
//
//	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	auto serverAddr = sockaddr_in{
//		.sin_family = AF_INET,
//		.sin_port = htons(50960),
//		.sin_addr = {.S_un = {.S_addr = htonl(INADDR_ANY) }},
//	};
//	bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
//
//	auto HandlePulse = [&](const sockaddr_in& addr)
//		{
//			std::scoped_lock lock(clientMutex);
//			auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientData& c) { return c.address == addr; });
//			if (it != clients.end())
//				it->deathCounter = 0;
//			else
//			{
//				std::println("New client: {}", addr.sin_addr.S_un.S_addr);
//				clients.push_back({ addr, 0 });
//			}
//		};
//
//	auto BroadcastMessage = [&](const std::string& author, const std::string& message)
//		{
//			std::string fullMessage = std::format("<{}> {}", author, message);
//			std::scoped_lock lock(clientMutex);
//			for (auto& c : clients)
//				sendto(serverSocket, fullMessage.data(), fullMessage.size(), 0, (SOCKADDR*)&c.address, sizeof(c.address));
//		};
//
//	auto SetNickname = [&](const sockaddr_in& addr, const std::string& nickname)
//		{
//			std::scoped_lock lock(clientMutex);
//			auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientData& c) { return c.address == addr; });
//			if (it != clients.end())
//			{
//				it->nickname = nickname;
//				std::println("{} is now \"{}\"", addr.sin_addr.S_un.S_addr, nickname);
//			}
//		};
//
//	auto GetNickname = [&](const sockaddr_in& addr)
//		{
//			std::scoped_lock lock(clientMutex);
//			auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientData& c) { return c.address == addr; });
//			if (it != clients.end())
//				return it->nickname;
//
//			return std::string("(unknown)");
//		};
//
//	while (true)
//	{
//		char message[256];
//		sockaddr_in SenderAddr;
//		int SenderAddrSize = sizeof(SenderAddr);
//
//		int bytesReceived = recvfrom(serverSocket, message, 256, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
//
//		if (bytesReceived > 0)
//		{
//			switch (message[0])
//			{
//			case 'M':
//				std::println("Message: {}", std::string(message + 1, bytesReceived - 1));
//				BroadcastMessage(GetNickname(SenderAddr), std::string(message + 1, bytesReceived - 1));
//				HandlePulse(SenderAddr);
//				break;
//
//			case 'N':
//				SetNickname(SenderAddr, std::string(message + 1, bytesReceived - 1));
//				HandlePulse(SenderAddr);
//				break;
//
//			case 'P':
//				HandlePulse(SenderAddr);
//				break;
//			}
//		}
//	}
//
//	closesocket(serverSocket);
//
//	purgeThread.request_stop();
//	purgeThread.join();
//	WSACleanup();
//	return 0;
//}