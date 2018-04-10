#ifndef MYSOCKET_H_
#define MYSOCKET_H_

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <string>

enum SocketType { CLIENT, SERVER };
enum ConnectionType { TCP, UDP };
const int DEFAULT_SIZE=128; 

class MySocket
{
private:
	SOCKET WelcomeSocket;
	SOCKET ConnectionSocket;

	struct sockaddr_in SvrAddr; //to store connection information
	struct sockaddr_in RespAddr; // to store the response for UDP connection
	std::string IPAddr;
	int Port;
	
	ConnectionType connectionType;
	SocketType mySocket; //to hold the socket type of the MySocket object

	char* Buffer;
	bool bConnect;  //flag to determine socket initialization
	int MaxSize; //to store the maximum number of bytes the buffer is allocated to
	bool StartWSA();

protected:

	void Print(std::string msg)
	{
		std::cout << msg << std::endl;
	}

public:
	MySocket(SocketType , std::string, unsigned int, ConnectionType, unsigned int);
	~MySocket();
	
	bool ConnectTCP(); //establish TCP/IP socket connection (3-way handshake)
	bool DisconnectTCP(); //disconnect the TCP/IP socket connection (4-way handshake)
	bool SetupUDP(); // configure UDP connection for communication
	bool TerminateUDP(); // close configured UDP connection
	int SendData(const char*, int);
	int GetData(char*);
	std::string GetIPAddr();
	bool SetIPAddr(std::string);
	bool SetPortNum(int);
	int GetPort();
	SocketType GetType();
	bool SetType(SocketType);

	void BindSocket();
	void ListenSocket();
	SOCKET InitializeTCPSocket();
	SOCKET InitializeUDPSocket();
};
#endif
