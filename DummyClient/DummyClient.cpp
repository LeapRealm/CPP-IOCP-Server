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
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errorCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errorCode << endl;
		return 0;
	}

	// 연결할 목적지는? (IP주소 + Port)
	SOCKADDR_IN serverAddress = {}; // IPv4
	serverAddress.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	serverAddress.sin_port = ::htons(7777); // 80 : HTTP

	// htons : host to network short
	// Little-Endian vs Big-Endian
	// ex) 0x12345678 4바이트 정수
	// low [0x78][0x56][0x34][0x12] high <= Little-Endian
	// low [0x12][0x34][0x56][0x78] high <= Big-Endian <= Network

	if (::connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		cout << "Connect ErrorCode : " << errorCode << endl;
		return 0;
	}

	cout << "Connected To Server!" << endl;

	while (true)
	{
		this_thread::sleep_for(1s);
	}

	// Socket 리소스 반환
	::closesocket(clientSocket);

	// WinSock 종료
	::WSACleanup();
}
