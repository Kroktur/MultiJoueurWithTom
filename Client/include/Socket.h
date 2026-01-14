#pragma once
#include <stdexcept>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
struct Socket
{
    Socket(const addrinfo& hint, const char* port);

    void SetupConnection(const char* Ip);



    SOCKET& GetSocket();
    const SOCKET& GetSocket() const;

    void CloseSocketAndFree();

    bool Connect();

    bool isConnected();

    void Disconnect();

    int Send(const char* data,int size);

    int Recv(char* data, int size);

private:
    addrinfo m_hint;
    const char* m_port;
    addrinfo* m_result;
    SOCKET m_ServerSocket;
};