#include "Socket.h"
Socket::Socket(const addrinfo& hint, const char* port) : m_hint(hint), m_port(port), m_result(nullptr), m_ServerSocket(INVALID_SOCKET)
{
}

 void Socket::SetupConnection(const char* Ip)
{
	int  iResult = getaddrinfo(Ip, m_port, &m_hint, &m_result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		throw std::runtime_error("error getaddrinfo failed");
	}
}

 bool Socket::Connect()
{
	static int iResult = 0;
	if (m_result == nullptr)
	{
		throw std::runtime_error("SetupConnection first");
	}

	// verifier qu'on es pas deja connecter
	addrinfo* ptr;

	for (ptr = m_result; ptr != NULL; ptr = ptr->ai_next)
	{
		m_ServerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_ServerSocket == INVALID_SOCKET) {
			printf("socket failed with error: %i\n", WSAGetLastError());
			freeaddrinfo(m_result);
			m_result = nullptr;
			WSACleanup();
			throw std::runtime_error("socket failed");
		}
		iResult = connect(m_ServerSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			CloseSocketAndFree();
			m_ServerSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	if (m_ServerSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		return false;
	}

	freeaddrinfo(m_result);
	return true;
}

 SOCKET& Socket::GetSocket()
{
	if (m_ServerSocket == INVALID_SOCKET)
		throw std::runtime_error("socket invalid");
	return m_ServerSocket;
}

const SOCKET& Socket::GetSocket() const
{
	if (m_ServerSocket == INVALID_SOCKET)
		throw std::runtime_error("socket invalid");
	return m_ServerSocket;
}

void Socket::CloseSocketAndFree()
{
	closesocket(m_ServerSocket);
}

 bool Socket::isConnected()
{
	if (m_ServerSocket == INVALID_SOCKET)
		return false;
	return true;
}

 void Socket::Disconnect()
{
	if (!isConnected())
		throw std::runtime_error("not connected");
	int iResult = shutdown(m_ServerSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		CloseSocketAndFree();
		WSACleanup();
		return throw std::runtime_error("shutDownFailed");
	}
}
 int Socket::Send(const char* data, int size)
{
	int iResult = send(m_ServerSocket, data, size, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		CloseSocketAndFree();
		WSACleanup();
		throw std::runtime_error("send failed");
	}
	return iResult;
}

 int Socket::Recv(char* data, int size)
{
	int iResult = recv(m_ServerSocket, data, size, 0);
	if (iResult == SOCKET_ERROR) {
		printf("recv failed with error: %d\n", WSAGetLastError());
		CloseSocketAndFree();
		WSACleanup();
		throw std::runtime_error("recv failed");
	}
	return iResult;
}