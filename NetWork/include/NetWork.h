#pragma once
#include <cstdio>
#include <iostream>
#include <winsock2.h>


class NetWork
{
public:
	static bool IsInit();
	static bool InitializeNetWork();
	static void UnInitialize();
private:
	static bool m_isInit;
};
bool NetWork::m_isInit = false;