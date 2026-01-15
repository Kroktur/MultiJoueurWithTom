#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <map>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include "MySocket.h"
#include "NetWork.h"

#include <vector>
#include <mutex>
#include <queue>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

static std::mutex mapMutex;
static std::mutex countMutex;
static std::mutex queueMutex;


struct Toto
{
    int a;
    float b;
};

struct ClientProcess
{
    int id;
    std::map<int, TcpSocket*>* map;
};


void ClientThread(std::stop_token st, ClientProcess& c,std::queue<std::string>& queue)
{
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    while (!st.stop_requested())
    {
        TcpSocket* copy = nullptr;
		{
			std::scoped_lock lock(mapMutex);
			copy = c.map->at(c.id);
		}
        int r = 0;
        if (copy)
            r = copy->Receive(recvbuf, recvbuflen);
        if (r == 0)
            break;

		auto ctr = reinterpret_cast<std::string*>(recvbuf);

        if (*ctr == "/quit")
	     break;  
        {
            std::scoped_lock lock(queueMutex);
            queue.push(*ctr);
		}

    	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

    TcpSocket* my = nullptr;
    {
        std::scoped_lock lock(mapMutex);
        my = c.map->at(c.id);
        c.map->erase(c.id);
    }

    my->Disconect();
    SocketManager::CloseSocketAndFree(my->GetSocket(), my->GetMyAddrInfo());
	delete my;
	std::cout << "Client thread ending for id: " << c.id << "\n";
}


void ListenThread(std::stop_token st, TcpSocket& socket,std::map<int,TcpSocket*>& sockets,int& idCount, std::queue<std::string>& queue)
{
    TCPServerConnector::Listen(socket);
	while (!st.stop_requested())
	{
        TcpSocket ClientSocket = TCPServerConnector::Accept(socket);
            int id = 0;
            {
                std::scoped_lock lock(countMutex);
                id = ++idCount;
            }
            {
                std::scoped_lock lock(mapMutex);
                sockets[id] = new TcpSocket(std::move(ClientSocket));
            }
                ClientProcess c{id, &sockets};
                std::jthread* t =  new std::jthread(ClientThread, std::ref(c), std::ref(queue));
			
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
	std::cout << "Listen thread ending\n";
}




int main(void)
{
    NetWork::InitializeNetWork();

    // Initialize Winsock library
    WSADATA wsaData;
    // Error handling variable
    int iResult;

    // Create a SOCKET for the server to listen for client connections
     // create socket info for a TCP client using IPv4
    SocketInfo socketInfo;
    socketInfo.ipAdress = IpAddressType::IPv4;
    socketInfo.protocol = Protocol::TCP;
    socketInfo.socketType = SocketType::TCP;
    socketInfo.role = Role::SERVER;

    SocketData socketData(socketInfo, DEFAULT_PORT, "localhost");
    // create socket and set its info
	TcpSocket socket(socketData);

	TCPServerConnector::Bind(socket);
	//TCPServerConnector::Listen(socket);
	//TcpSocket ClientSocket =TCPServerConnector::Accept(socket);
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int idCount = 0;
	std::map<int, TcpSocket*> sockets;
	std::queue<std::string> messageQueue;
	std::jthread j(ListenThread, std::ref(socket), std::ref(sockets), std::ref(idCount), std::ref(messageQueue));

    while (true)
    {
		{
			std::scoped_lock lock(queueMutex);
            while (!messageQueue.empty())
            {
                auto msg = messageQueue.front();
                messageQueue.pop();
                std::cout << "Received message: " << msg << std::endl;
			}
		}
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

   // // Receive until the peer shuts down the connection
   // do {
   //     
   //     iResult = ClientSocket.Receive( recvbuf, recvbuflen);
   //     if (!ClientSocket.IsConnected())
   //         break;
   //     if (iResult > 0) {
   //         printf("Bytes received: %d\n", iResult);
   //         // Echo the buffer back to the sender
			//auto send = reinterpret_cast<std::string*>(recvbuf);
   //         if (*send == std::string("/Quit"))
   //         {
   //             break;
			//}

   //         iSendResult = ClientSocket.Send(recvbuf, iResult);
   //         if (iSendResult == SOCKET_ERROR) {
   //             printf("send failed with error: %d\n", WSAGetLastError());
   //             break;
   //         }
   //         printf("Bytes sent: %d\n", iSendResult);
   //     }
   //     else if (iResult == 0)
   //         printf("Connection closing...\n");
   // } while (iResult > 0);

   // ClientSocket.Disconect();

    if (socket.IsConnected())
        socket.Disconect();
    SocketManager::CloseSocketAndFree(socket.GetSocket(), socket.GetMyAddrInfo());
    //SocketManager::CloseSocketAndFree(ClientSocket.GetSocket(), ClientSocket.GetMyAddrInfo());

    NetWork::UnInitialize();

    return 0;
}
