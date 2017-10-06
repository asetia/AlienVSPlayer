#pragma once
#ifndef NETWORKSERVICES_H
#define NETWORKSERVICES_H

#include <winsock2.h>
#include <Windows.h>

class NetworkServices
{

public:

	static int sendMessage(SOCKET curSocket, char * message, int messageSize);
	static int receiveMessage(SOCKET curSocket, char * buffer, int bufSize);

};

#endif
