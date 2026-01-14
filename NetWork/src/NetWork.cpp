#include "NetWork.h"

bool NetWork::IsInit()
{
	return m_isInit;
}

bool NetWork::InitializeNetWork()
{
	if (isInit())
	{
		UnInitialize();
		throw std::runtime_error("alreadyInit");
	}

	// create Variable
	WSADATA wsaData;

	// verify if setup success
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		// error on setup
		std::cout << "WSAStartup failed with error: " <<  iResult;
		return false;
	}
	// confirm initialization 
	m_isInit = true;
	return true;
}

void NetWork::UnInitialize()
{
	if (!isInit())
		throw std::runtime_error("should be init");
	WSACleanup();
}
