#pragma once
#include "SocketInfo.h"
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "NetWork.h"
// TODO verifier a tete reposer
class Socket
{
public:
	using socket_type = SOCKET;
	Socket(const SocketInfo& info, const std::string& port = "localhost")
	: m_info(info)
	, m_port(port)
	, m_socket(INVALID_SOCKET)
	,m_startResultAddr(nullptr)
	{
		if (!NetWork::IsInit())
			throw std::runtime_error("Not Init");
		if (!m_info.IsValid())
			throw std::runtime_error("invalid SocketInfo");
	}
	~Socket()
	{
		CloseSocketAndFree();
	}
	void SetupSocket(const char* ip)
	{
		if (IsAddrSetup())
		{
			CloseSocketAndFree();
			throw std::runtime_error("close before Setup");
		}

		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));
		// for IPV4 or IPV6 addresses 
		hints.ai_family = static_cast<int>(m_info.ipAdress);
		// standard stream socket for TCP
		hints.ai_socktype = static_cast<int>(m_info.socketType);
		// use TCP protocol
		hints.ai_protocol = static_cast<int>(m_info.protocol);
		// role
		hints.ai_flags = static_cast<int>(m_info.role);

		const char* port = m_port.c_str();
		int  iResult = getaddrinfo(ip, port, &hints, &m_startResultAddr);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			throw std::runtime_error("error getaddrinfo failed");
		}
	}
	 void OpenSocket(addrinfo* info)
	{
		if (!IsAddrSetup())
		{
			CloseSocketAndFree();
			throw std::runtime_error("Setup before open");
		}

		if (IsSocketValid())
		{
			CloseSocket();
		}
		m_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		if (m_socket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			FreeAddr();
			WSACleanup();
			throw std::runtime_error("error open socket failed");
		}
	}

	void CloseSocketAndFree()
	{
		if (IsSocketValid())
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
		if (IsAddrSetup())
		{
			freeaddrinfo(m_startResultAddr);
			m_startResultAddr = nullptr;
		}
		
	}
	void CloseSocket()
	{
		if (IsSocketValid())
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
	}
	void FreeAddr()
	{
		if (IsAddrSetup())
		{
			freeaddrinfo(m_startResultAddr);
			m_startResultAddr = nullptr;
		}
	}
	
	bool IsSocketValid() const
	{
		return m_socket != INVALID_SOCKET;
	}
	bool IsAddrSetup() const
	{
		return m_startResultAddr != nullptr;
	}

	addrinfo* GetMyAddrInfo()
	{
		return m_startResultAddr;
	}
	const addrinfo* GetMyAddrInfo() const 
	{
		return m_startResultAddr;
	}

	socket_type& GetSocket()
	{
		return m_socket;
	}
	const socket_type& GetSocket() const
	{
		return m_socket;
	}

	SocketInfo GetSocketInfo() const
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
	socket_type m_socket;
	addrinfo* m_startResultAddr;

};
