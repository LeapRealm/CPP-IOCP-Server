#include "pch.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	// WinSock 초기화 (ws2_32 라이브러리 초기화)
	// 관련 정보가 wsaData에 채워짐
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// af : Address Family (AF_INET = IPv4, AF_INET6 = IPV6)
	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	// protocol : 0 (알아서 정해줌)
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errorCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errorCode << endl;
		return 0;
	}

	// 나의 주소는? (IP주소 + Port)
	SOCKADDR_IN serverAddress = {}; // IPv4
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = ::htonl(INADDR_ANY); // 알아서 정해줘
	serverAddress.sin_port = ::htons(7777); // 80 : HTTP

	// 소켓에 나의 주소 등록
	if (::bind(listenSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		cout << "Bind ErrorCode : " << errorCode << endl;
		return 0;
	}

	// 클라이언트의 연결을 기다리기 시작
	int backlog = 10; // 최대 대기열 수
	if (::listen(listenSocket, backlog) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		cout << "Listen ErrorCode : " << errorCode << endl;
		return 0;
	}

	while (true)
	{
		// 연결을 시도한 클라이언트와 연결
		SOCKADDR_IN clientAddress = {};
		int32 addressLength = sizeof(clientAddress);

		SOCKET clientSocket = ::accept(listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddress), &addressLength);
		if (clientSocket == INVALID_SOCKET)
		{
			int32 errorCode = ::WSAGetLastError();
			cout << "Accept ErrorCode : " << errorCode << endl;
			return 0;
		}

		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddress.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "Client Connected! IP = " << ipAddress << endl;
	}

	// WinSock 종료
	::WSACleanup();
}
