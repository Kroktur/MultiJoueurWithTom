#pragma once
#include <memory>
#include <winsock2.h>
#include <stdexcept>
#include <string>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

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

struct SocketInfo
{
    SocketInfo();
     IpAddressType ipAdress;
     Protocol protocol;
     SocketType socketType;
     Role role;
    bool IsValid() const;
    bool ProtocolAndSocketCompatible() const;
};


struct SocketAddr
{
	static SocketAddr Create(IpAddressType ipAdress,const std::string& port, const char* ip)
	    {
			SocketAddr result;
		    if (ipAdress == IpAddressType::IPv4)
			{
                sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_port = htons(std::stoi(port));
                inet_pton(AF_INET, ip, &addr.sin_addr);

				result.addr = *reinterpret_cast<sockaddr*>(&addr);
				result.length = sizeof(sockaddr_in);
			}
			else if (ipAdress == IpAddressType::IPv6)
			{
	            sockaddr_in6 addr;
				addr.sin6_family = AF_INET6;
				addr.sin6_port = htons(std::stoi(port));
                inet_pton(AF_INET6, ip, &addr.sin6_addr);

				result.addr =*reinterpret_cast<sockaddr*>(&addr);
	            result.length = sizeof(sockaddr_in6);
			}
			else
			{
				throw std::runtime_error("Invalid IP Address type on bind");
			}
			return result;
	    }
	SocketAddr() : addr(sockaddr()), length(0) {}
	SocketAddr(const SocketAddr& other) = default;
    SocketAddr(SocketAddr&& other) noexcept = default;
	SocketAddr& operator=(const SocketAddr& other) = default;
	SocketAddr& operator=(SocketAddr&& other) noexcept = default;
    bool IsValid() const 
	{
		return length > 0;
	}
    sockaddr addr;
    int length;
	std::string GetIp() const 
    {
        if (ip.empty())
            GenerateIp();
		return ip;
    }
    void ClearIp()
	{
        ip.clear();
	}
private:
   mutable std::string ip;
	void GenerateIp() const
    {
        if (addr.sa_family == AF_INET)
        {
            const sockaddr_in* ipv4 = reinterpret_cast<const sockaddr_in*>(&addr);
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipStr, sizeof(ipStr));
        	ip = ipStr;
        }
        else if (addr.sa_family == AF_INET6)
        {
            const sockaddr_in6* ipv6 = reinterpret_cast<const sockaddr_in6*>(&addr);
            char ipStr[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipStr, sizeof(ipStr));
            ip = ipStr;
        }
        else 
			throw std::runtime_error("invalid");
    }
};