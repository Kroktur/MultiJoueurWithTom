#include "SocketInfo.h"
SocketInfo::SocketInfo():ipAdress(IpAddressType::INVALID)
                         ,protocol(Protocol::INVALID)
                         ,socketType(SocketType::INVALID)
                         ,role(Role::INVALID)
{}

bool SocketInfo::IsValid() const
{
	if (ipAdress != IpAddressType::INVALID &&
		protocol != Protocol::INVALID && 
		socketType != SocketType::INVALID &&
		role != Role::INVALID)
	{
		return  ProtocolAndSocketCompatible();
	}
	return false;
}

bool SocketInfo::ProtocolAndSocketCompatible() const
{
	switch (protocol)
	{
	case Protocol::TCP:
		return socketType == SocketType::TCP;
	case Protocol::UDP:
		return socketType == SocketType::UDP;
		default :
		throw std::runtime_error("should be valid");
	}
}
