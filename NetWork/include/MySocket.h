#pragma once
#include "SocketInfo.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetWork.h"
using socket_type = SOCKET;
using addr_type = addrinfo;




struct SocketData
{
	SocketData() = default;
	SocketData(const SocketInfo& info, const std::string& port)
		: m_info(info)
		, m_port(port)
	{
	}
	~SocketData() = default;

	SocketData(const SocketData&) = default;
	SocketData(SocketData&&) = default;

	SocketData& operator=(const SocketData&) = default;
	SocketData& operator=(SocketData&&) = default;

	void SetupInfo(const SocketInfo& info)
	{
		m_info = info;
	}
	void SetupPort(const std::string& port)
	{
		m_port = port;
	}

	SocketInfo GetInfo() const
	{
		return m_info;
	}
	std::string GetPort() const
	{
		return m_port;
	}
private:
	SocketInfo m_info;
	std::string m_port;
};

class SocketManager
{
public:
	SocketManager() = default;
	~SocketManager() = default;
	SocketManager(const SocketManager&) = delete;
	SocketManager(SocketManager&&) = delete;

	SocketManager& operator=(const SocketManager&) = delete;
	SocketManager& operator=(SocketManager&&) = delete;

	static bool OpenSocket(socket_type& MySocket, addr_type* info)
	{
		// if network not initialized
		if (!NetWork::IsInit())
		{
			// throw error
			throw std::runtime_error("Network not Init");
		}

		// if address is not setup 
		if (!IsAddrSetup(info))
		{
			printf("addr is not setup please close setup it");
			return false;
		}
		// if socket is already open 
		if (IsSocketValid(MySocket))
		{
			// close before open
			CloseSocket(MySocket);
		}
		// open socket
		MySocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		// if failed to open
		if (MySocket == INVALID_SOCKET) {
			// send error message free address and uninitialize network
			printf("socket failed with error: %ld\n", WSAGetLastError());
			return  false;
		}
		return true;
	}
	static void CloseSocket(socket_type& socket)
	{
		if (IsSocketValid(socket))
		{
			closesocket(socket);
			socket = INVALID_SOCKET;
		}
	}
	static bool IsSocketValid(const socket_type& socket)
	{
		return socket != INVALID_SOCKET;
	}

	static bool IsAddrSetup(const addr_type* addr)
	{
		return addr != nullptr;
	}
	static void FreeAddr(addr_type* addr)
	{
		if (IsAddrSetup(addr))
		{
			freeaddrinfo(addr);
			addr = nullptr;
		}
	}

	static bool SetupAddrInfo(const SocketData& info,const char* ip,addr_type*& addrResult)
	{
		// if network not initialized
		if (!NetWork::IsInit())
		{
			// throw error
			throw std::runtime_error("Network not Init");
		}

		if (IsAddrSetup(addrResult))
		{
			FreeAddr(addrResult);
		}

		if (!info.GetInfo().IsValid())
		{
			return false;
		}
		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));

		// for IPV4 or IPV6 addresses 
		hints.ai_family = static_cast<int>(info.GetInfo().ipAdress);
		// standard stream socket for TCP
		hints.ai_socktype = static_cast<int>(info.GetInfo().socketType);
		// use TCP protocol
		hints.ai_protocol = static_cast<int>(info.GetInfo().protocol);
		// role
		hints.ai_flags = static_cast<int>(info.GetInfo().role);

		std::string portStr = info.GetPort();
		const char* port = portStr.c_str();

		int  iResult = getaddrinfo(ip, port, &hints, &addrResult);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			return false;
		}
		return true;
	}
	static void CloseSocketAndFree(socket_type& socket, addr_type* addr)
	{
		if (IsSocketValid(socket))
		{
			closesocket(socket);
			socket = INVALID_SOCKET;
		}
		if (IsAddrSetup(addr))
		{
			freeaddrinfo(addr);
			addr = nullptr;
		}
	}

	static int Send(socket_type& socket,const char* data, int size)
		{
			int iResult = send(socket, data, size, 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				CloseSocket(socket);
				WSACleanup();
				throw std::runtime_error("send failed");
			}
			return iResult;
		}
	static int Recv(socket_type& socket, char* data, int size)
		{
			int iResult = recv(socket, data, size, 0);
			if (iResult == SOCKET_ERROR) {
				printf("recv failed with error: %d\n", WSAGetLastError());
				CloseSocket(socket);
					WSACleanup();
				throw std::runtime_error("recv failed");
			}
			return iResult;
		}

};


class Socket
{
public:
	Socket()
	: m_info(SocketData())
	, m_socket(INVALID_SOCKET)
	{}
	Socket(socket_type socket,const SocketData& info = SocketData{})
		: m_info(info)
		, m_socket(socket)
	{}
	~Socket()
	{
		if (SocketManager::IsSocketValid(m_socket))
			SocketManager::CloseSocket(m_socket);
	}
	Socket(const Socket&) = default;
	Socket(Socket&&) = default;

	Socket& operator=(const Socket&) = default;
	Socket& operator=(Socket&&) = default;

	const socket_type& GetSocket() const
	{
		return m_socket;
	}
	socket_type& GetSocket()
	{
		return m_socket;
	}
	void SetInfo(const SocketData& info)
	{
		m_info = info;
	}
	SocketData GetInfo() const
	{
		return m_info;
	}
private:
	SocketData m_info;
	socket_type m_socket;
};


class SocketTcp
{
public:
	SocketTcp(const SocketInfo& info, const std::string& port, const char* ip )
		: m_socket()
		, m_addrInfo(nullptr)
		, m_isConnected(false)
		,m_info(info)
		, m_ip(ip)
		, m_port(port)
	{
		if (!NetWork::IsInit())
			throw std::runtime_error("Network not Init");

		if (!info.IsValid())
			throw std::runtime_error("SocketInfo not valid");

		if (info.protocol != Protocol::TCP)
			throw std::runtime_error("SocketInfo not TCP protocol");

		SocketData socketData(m_info, m_port);
		m_socket.SetInfo(socketData);

		
		SocketManager::SetupAddrInfo(socketData, m_ip, m_addrInfo);

		if (!SocketManager::IsAddrSetup(m_addrInfo))
			throw std::runtime_error("Address not setup");
	}
	SocketTcp() = default;
	SocketTcp(const Socket& socket, bool isConnected)
		: m_socket(socket)
		, m_addrInfo(nullptr)
		, m_isConnected(isConnected)
		, m_ip(nullptr)
		, m_port("")
		, m_info()
	{
		if (!SocketManager::IsSocketValid(m_socket.GetSocket()) && m_isConnected)
			throw std::runtime_error("Socket not valid but isConnected true");

	}
	~SocketTcp() = default;
	SocketTcp(const SocketTcp&) = default;
	SocketTcp(SocketTcp&&) = default;
	SocketTcp& operator=(const SocketTcp&) = default;
	SocketTcp& operator=(SocketTcp&&) = default;

	Socket& GetSocket()
	{
		return m_socket;
	}
	const Socket& GetSocket() const
	{
		return m_socket;
	}
	addr_type* GetMyAddrInfo()
	{
		return m_addrInfo;
	}
	const addr_type* GetMyAddrInfo() const
	{
		return m_addrInfo;
	}
	void ChangeTargetIp(const char* ip)
	{
		if (!m_info.IsValid())
			throw std::runtime_error("SocketInfo not valid");

		if (m_info.role != Role::CLIENT)
			throw std::runtime_error("Only client sockets can change target IP");

		m_ip = ip;
		SocketData socketData(m_info, m_port);
		m_socket.SetInfo(socketData);
		SocketManager::SetupAddrInfo(socketData, m_ip, m_addrInfo);
		if (!SocketManager::IsAddrSetup(m_addrInfo))
			throw std::runtime_error("Address not setup");
	}
	void SetInfo(const SocketInfo& info)
	{
		m_info = info;
	}
	void SetPort( const std::string& port)
	{
		m_port = port;
	}


	bool IsConnected() const
	{
		return m_isConnected;
	}
	void Disconnect()
	{
		if (!IsConnected())
			return;

		if (SocketManager::IsSocketValid(m_socket.GetSocket()))
		{
			shutdown(m_socket.GetSocket(), SD_SEND);
			SocketManager::CloseSocket(m_socket.GetSocket());
			m_socket.GetSocket() = INVALID_SOCKET;
			m_isConnected = false;
		}
		else
		{
			throw std::runtime_error("Socket not valid on disconnect");
		}
	}

	bool Connect()
	{
		if (IsConnected())
			return true;

		if (!SocketManager::IsAddrSetup(m_addrInfo))
			throw std::runtime_error("Address not setup on connect");

		if (!m_info.IsValid())
			throw std::runtime_error("SocketInfo not valid on connect");

		if (m_info.role != Role::CLIENT)
			throw std::runtime_error("Only client sockets can connect");

		for (auto* ptr = m_addrInfo; ptr != nullptr; ptr = ptr->ai_next)
		{
			SocketManager::OpenSocket(m_socket.GetSocket(), ptr);
			int iResult = connect(m_socket.GetSocket(), ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult != SOCKET_ERROR)
			{
				m_isConnected = true;
				return true;
			}
			SocketManager::CloseSocket(m_socket.GetSocket());
		}
		return false;
	}
	void Bind()
	{
		if (IsConnected())
			throw std::runtime_error("Socket already connected on bind");
		
		if (!SocketManager::IsAddrSetup(m_addrInfo))
			throw std::runtime_error("Address not setup on connect");

		if (!m_info.IsValid())
			throw std::runtime_error("SocketInfo not valid on connect");

		if (m_info.role != Role::SERVER)
			throw std::runtime_error("Only server sockets can bind");

		SocketManager::OpenSocket(m_socket.GetSocket(), m_addrInfo);


		int iResult = bind(m_socket.GetSocket(), m_addrInfo->ai_addr, (int)m_addrInfo->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			SocketManager::CloseSocket(m_socket.GetSocket());
			throw std::runtime_error("bind failed");
		}
	}
	void Listen()
	{
		if (IsConnected())
			return;

		if (!SocketManager::IsAddrSetup(m_addrInfo))
			throw std::runtime_error("Address not setup on connect");

		if (!m_info.IsValid())
			throw std::runtime_error("SocketInfo not valid on connect");

		if (m_info.role != Role::SERVER)
			throw std::runtime_error("Only server sockets can listen");

		if (!SocketManager::IsSocketValid(m_socket.GetSocket()))
			throw std::runtime_error("Socket not valid on listen, bind first");

		int iResult = listen(m_socket.GetSocket(), SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			SocketManager::CloseSocket(m_socket.GetSocket());
			throw std::runtime_error("listen failed");
		}
	}

	SocketTcp Accept()
	{
		socket_type result;
		result = accept(m_socket.GetSocket(), NULL, NULL);
		if (!SocketManager::IsSocketValid(result)) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			SocketManager::CloseSocket(m_socket.GetSocket());
			throw std::runtime_error("accept failed");
		}
		return SocketTcp{ result, true };
	}
private:
	Socket m_socket;
	addr_type* m_addrInfo = nullptr;
	bool m_isConnected;
	const char* m_ip;
	std::string m_port;
	SocketInfo m_info;
};

class SocketTcp2
{
public:
	SocketTcp2(const Socket& socket,bool isConnected) : m_socket(socket),m_isConnected(isConnected){}
	int Send( const char* data, int size)
	{
		if (!IsConnected())
			return 0;

		int iResult = send(m_socket.GetSocket(), data, size, 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			SocketManager::CloseSocket(m_socket.GetSocket());
			NetWork::UnInitialize();
			throw std::runtime_error("send failed");
		}
		return iResult;
	}
	int Receive(char* data , int size)
	{
		if (!IsConnected())
			return 0;

		int iResult = recv(m_socket.GetSocket(), data, size, 0);
		if (iResult == SOCKET_ERROR) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			SocketManager::CloseSocket(m_socket.GetSocket());
			NetWork::UnInitialize();
			throw std::runtime_error("recv failed");
		}
		if (iResult == 0)
			m_isConnected = false;

		return iResult;
	}
	bool IsConnected() const
	{
		return m_isConnected;
	}
private:
	Socket m_socket;
	bool m_isConnected;
};