#pragma once
#include <winsock2.h>
#include <stdexcept>

enum class IpAddressType : int
{
    INVALID = -1,
    IPv4 = AF_INET,
    IPv6 = AF_INET6,
    Both = AF_UNSPEC
};

enum class Protocol : int
{
    INVALID = -1,
    TCP = IPPROTO_TCP,
	UDP = IPPROTO_UDP
};

enum class SocketType : int
{
    INVALID = -1,
    TCP = SOCK_STREAM,
    UDP = SOCK_DGRAM
};

enum class Role : int
{
    INVALID = -1,
    SERVER = AI_PASSIVE,
    CLIENT = 0
};


class SocketHolder;

struct SocketInfo
{
    friend SocketHolder;
    SocketInfo();
     IpAddressType ipAdress;
     Protocol protocol;
     SocketType socketType;
     Role role;
    bool IsValid() const;
    bool ProtocolAndSocketCompatible() const;
};