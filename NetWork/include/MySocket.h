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
	SocketData(const SocketInfo& info, const std::string& port,const char* ip)
		: m_info(info)
		, m_port(port)
		, m_ip(ip)
	
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
	const char* GetIp() const
	{
		return m_ip;
	}
private:
	SocketInfo m_info;
	std::string m_port;
	const char* m_ip;
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

	static bool OpenSocket(socket_type& MySocket, addr_type*& info)
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
	static void FreeAddr(addr_type*& addr)
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
	static void CloseSocketAndFree(socket_type& socket, addr_type*& addr)
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

};

class TcpSocket
{
public:
	
	TcpSocket(const SocketData& data = SocketData{})
		: m_data(data)
		, m_socket(std::make_unique<socket_type>(INVALID_SOCKET))
		, m_isConnected(false)
	{
		if (!NetWork::IsInit())
			throw std::runtime_error("Network not Init");
	}
	TcpSocket(const socket_type& socket,bool isConected, const SocketData& data)
		: m_data(data)
		, m_socket(std::make_unique<socket_type>(socket))
		, m_isConnected(isConected){}
	TcpSocket(const TcpSocket&) = delete;
	TcpSocket(TcpSocket&&) = default;

	TcpSocket& operator=(const TcpSocket&) = delete;
	TcpSocket& operator=(TcpSocket&&) = default;

	~TcpSocket() = default;

	bool IsConnected() const
	{
		return m_isConnected;
	}
	socket_type& GetSocket()  
	{
		return *m_socket;
	}
	const socket_type& GetSocket() const
	{
		return *m_socket;
	}
	SocketData GetData() const
	{
		return m_data;
	}

	void Disconect()
	{
		if (!IsConnected())
			return;

		if (SocketManager::IsSocketValid(*m_socket))
		{
			shutdown(*m_socket, SD_SEND);
			SocketManager::CloseSocket(*m_socket);
			*m_socket = INVALID_SOCKET;
			m_isConnected = false;
		}
		else
		{
			throw std::runtime_error("Socket not valid on disconnect");
		}
	}
	int Send(const char* data, int size)
	{
		if (!IsConnected())
			return 0;

		int iResult = send(*m_socket, data, size, 0);

		if (iResult == 0)
		{
			m_isConnected = false;
			return 0;
		}

		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			m_isConnected = false;
			return 0;
		}
		return iResult;
	}
	int Receive(char* data, int size)
	{
		if (!IsConnected())
			return 0;

		int iResult = recv(*m_socket, data, size, 0);

		if (iResult == 0)
		{
			m_isConnected = false;
			return 0;
		}
		
		if (iResult == SOCKET_ERROR) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			m_isConnected = false;
			return 0;
		}
	

		return iResult;
	}
	void SetSocket(socket_type& socket, bool isConected)
	{
		m_socket = std::make_unique<socket_type>(std::move(socket));
		m_isConnected = isConected;
	}
	void SetData(const SocketData& data)
	{
		m_data = data;
	}
	void EnableConnection()
	{
		m_isConnected = true;
	}
	void GenerateAddrInfo()
	{
		if (!m_data.GetInfo().IsValid())
			throw std::runtime_error("SocketInfo not valid");
		SocketManager::SetupAddrInfo(m_data, m_data.GetIp(), m_addrInfo);
		if (!SocketManager::IsAddrSetup(m_addrInfo))
			throw std::runtime_error("Address not setup");
	}
	void ClearAddrInfo()
	{
		if (SocketManager::IsAddrSetup(m_addrInfo))
			SocketManager::FreeAddr(m_addrInfo);
	}

	addr_type*& GetMyAddrInfo()
	{
		if (!SocketManager::IsAddrSetup(m_addrInfo))
			GenerateAddrInfo();
		return m_addrInfo;
	}
private:
	SocketData m_data;
	std::unique_ptr<socket_type> m_socket;
	int m_isConnected;
	addr_type* m_addrInfo = nullptr;
};

struct TCPClientConnector
{
	static bool Connect(TcpSocket& socket)
	{
		if (socket.IsConnected())
			return true;

		const SocketInfo& info = socket.GetData().GetInfo();

		if (!info.IsValid())
			throw std::runtime_error("SocketInfo not valid on connect");

		if (info.role != Role::CLIENT)
			throw std::runtime_error("Only client sockets can connect");

		for (auto* ptr = socket.GetMyAddrInfo(); ptr != nullptr; ptr = ptr->ai_next)
		{
			SocketManager::OpenSocket(socket.GetSocket(), ptr);
			int iResult = connect(socket.GetSocket(), ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult != SOCKET_ERROR)
			{
				socket.EnableConnection();
				socket.ClearAddrInfo();
				return true;
			}
			SocketManager::CloseSocket(socket.GetSocket());
		}
		return false;
	}
};

struct TCPServerConnector
{
	static void Bind(TcpSocket& socket)
	{
		if (socket.IsConnected())
			throw std::runtime_error("Socket already connected on bind");

		const SocketInfo& info = socket.GetData().GetInfo();
		if (!info.IsValid())
			throw std::runtime_error("SocketInfo not valid on connect");

		if (info.role != Role::SERVER)
			throw std::runtime_error("Only server sockets can bind");

		SocketManager::OpenSocket(socket.GetSocket(), socket.GetMyAddrInfo());
		auto addr = socket.GetMyAddrInfo();
		int iResult = bind(socket.GetSocket(), addr->ai_addr, (int)addr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			throw std::runtime_error("bind failed");
		}
	}
	static void Listen(TcpSocket& socket)
	{
		if (socket.IsConnected())
			return;

		const SocketInfo& info = socket.GetData().GetInfo();
		if (!info.IsValid())
			throw std::runtime_error("SocketInfo not valid on connect");

		if (info.role != Role::SERVER)
			throw std::runtime_error("Only server sockets can listen");

		if (!SocketManager::IsSocketValid(socket.GetSocket()))
			throw std::runtime_error("Socket not valid on listen, bind first");

		int iResult = listen(socket.GetSocket(), SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			throw std::runtime_error("listen failed");
		}
	}
	static TcpSocket Accept(TcpSocket& socket)
	{
		socket_type result;
		result = accept(socket.GetSocket(), NULL, NULL);
		if (!SocketManager::IsSocketValid(result)) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			throw std::runtime_error("accept failed");
		}
		return TcpSocket{result, true,socket.GetData() };
	}
};