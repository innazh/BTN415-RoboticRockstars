#include "MySocket.h"


MySocket::MySocket(SocketType st, std::string ip, unsigned int port, ConnectionType ct, unsigned int size) {
	this->bConnect = false;

	this->connectionType = ct;
	this->mySocket = st;

	//set port and ip
	this->Port = port;
	this->IPAddr = ip;
	
	if (size > 0) {
		this->Buffer = new char[size];
		this->MaxSize = size;
	}
	else {
		// Invalid Size
		this->Buffer = new char[DEFAULT_SIZE];
		this->MaxSize = DEFAULT_SIZE;
	}
	//start Winsock DDLs
	if (this->StartWSA()) {
		//configure the socket
		SvrAddr.sin_family = AF_INET; //Address family type internet
		SvrAddr.sin_port = htons(this->Port); //port (host to network conversion)
		SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //IP address
		if (st == CLIENT) {
			if (ct == TCP) {
				this->ConnectionSocket = InitializeTCPSocket();
			}
			else if (ct == UDP) {
				this->ConnectionSocket = InitializeUDPSocket();
			}
		}
		else if (st == SERVER) {
			if (ct == TCP) {
				this->WelcomeSocket = InitializeTCPSocket();
			}
			else if (ct == UDP) {
				this->WelcomeSocket = InitializeUDPSocket();
			}
			this->BindSocket();
			//configure listen socket
			if (ct == TCP) {
				this->ListenSocket();
			}
		}
	}
}

//Destructor for cleaning up dynamically allocated memory
MySocket::~MySocket() 
{
	delete Buffer;
	Buffer = nullptr;
	this->MaxSize = 0;//not sure if this is needed

	this->bConnect = false;
	
	if (!(this->mySocket == SERVER && this->connectionType == UDP)) closesocket(this->ConnectionSocket);
	
	if(this->mySocket==SERVER) closesocket(this->WelcomeSocket);
	
	WSACleanup();
}

//Initializes the Winsock DLLs.
//Returns true if the Winsock is succesfully initialized else return false
bool MySocket::StartWSA() 
{
	bool ret = true;
	WSADATA wsaData;

	if ((WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) {
		Print("Could not start DDLs");
		exit(0);
		ret = false;
	}
	return ret;
}

//Establish TCP/IP connection, return true if connection made
bool MySocket::ConnectTCP() 
{
	bool res = true;

	if (this->connectionType == TCP) {
		if (this->mySocket == SERVER) {
			this->ConnectionSocket = SOCKET_ERROR;
			if ((this->ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR)
			{
				Print("Could not accept connection");
				closesocket(this->WelcomeSocket);
				WSACleanup();
				res = false;
			}
		}
		else if (this->mySocket == CLIENT) {
			if ((connect(this->ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
			{
				Print("Client: Could not connect");
				closesocket(this->ConnectionSocket);
				WSACleanup();
				res = false;
			}
		}
	}
	else res = false;



	if (this->connectionType == TCP) {
		if (this->mySocket == SERVER) {
			this->ConnectionSocket = SOCKET_ERROR;
			if ((this->ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR)
			{
				Print("Could not accept connection");
				closesocket(this->WelcomeSocket);
				WSACleanup();
				res = false;
			}
		}
		else if (this->mySocket == CLIENT) {
			if ((connect(this->ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
			{
				Print("Client: Could not connect");
				closesocket(this->ConnectionSocket);
				WSACleanup();
				res = false;
			}
		}
	}
	else res = false;


	return res;
}
//Changes the default port number depending on the success criteria
//Returns an error (false) if the connection is already established and true if the connection is not established
bool MySocket::SetPortNum(int PortNum) 
{
	bool ret = false;

	if (this->bConnect == false || WelcomeSocket == INVALID_SOCKET)
	{
		this->Port = PortNum;
		SvrAddr.sin_port = htons(Port); // conversion just to avoid the big/little endian issue
		ret =  true;
	}
	else ret = false;
	
	return ret;
}
//Disconnect from a TCP/IP socket
bool MySocket::DisconnectTCP() 
{
	bool r = false;

	this->bConnect = false;

	if (this->connectionType == TCP)
	{
		if (this->mySocket == CLIENT) {
			closesocket(this->ConnectionSocket);
			WSACleanup();
		}
		else {
			closesocket(this->WelcomeSocket);
			WSACleanup();
		}
		r = true;
	}
	return r;
}


//Configure UDP sockets
bool MySocket::SetupUDP() {
	bool ret = false;

	//Check if the socket is UDP
	if (this->connectionType == UDP) {
		ret = true;

		//Check if socket is a server
		if (this->mySocket == SERVER) {
			ConnectionSocket = InitializeUDPSocket();
			BindSocket();
		}
		else {
			ConnectionSocket = InitializeUDPSocket();
		}

	}
	else {
		Print("Error: Cannot configure a TCP Socket as a UDP Socket");
	}

	return ret;
}



//Close UDP socket
bool MySocket::TerminateUDP() 
{
	bool r = false;

	this->bConnect = false;

	if (this->connectionType == UDP) 
	{
		if (this->mySocket == CLIENT) {
			closesocket(this->ConnectionSocket);
			WSACleanup();
		}
		else {
			closesocket(this->WelcomeSocket);

			WSACleanup();
		}
		r = true;
	}
	return r;
}

//Do I need to set my buffer to the data I'm sending?
int MySocket::SendData(const char* raw, int size) 
{
	int transmitted = 0;

	if (this->connectionType == TCP) {

		transmitted = send(this->ConnectionSocket, raw, size, 0);
	}
	
	else if (this->connectionType == UDP) {
		if (this->mySocket == CLIENT) {
			transmitted = sendto(this->ConnectionSocket, raw, size, 0,
				(struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
		}
		else if (this->mySocket == SERVER) {
			transmitted = sendto(this->WelcomeSocket, raw, size, 0, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
		}
	}

	if (this->connectionType == TCP) {
		
		transmitted = send(this->ConnectionSocket, raw, size, 0);
	}
	
	else if (this->connectionType == UDP) {
		if (this->mySocket == CLIENT) {
			transmitted = sendto(this->ConnectionSocket, raw, size, 0,
				(struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
		}
		else if (this->mySocket == SERVER) {
			transmitted = sendto(this->WelcomeSocket, raw, size, 0, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
		}
	}


	return transmitted;
}

int MySocket::GetData(char* raw) 
{
	int received = 0;
	
	if (this->connectionType == TCP) {
		received = recv(this->ConnectionSocket, this->Buffer, this->MaxSize, 0);
	}
	
	else if (this->connectionType == UDP) {
		if (this->mySocket == SERVER) {
			int addr_len = sizeof(this->RespAddr);
			received = recvfrom(this->WelcomeSocket, this->Buffer, this->MaxSize, 0,
				(sockaddr *) &this->RespAddr, &addr_len);
		}
		else if (this->mySocket == CLIENT) {
			int addr_len = sizeof(SvrAddr);
			received = recvfrom(this->ConnectionSocket, this->Buffer, this->MaxSize, 0, (sockaddr *)&this->RespAddr,
				&addr_len);
		}
	}
	
	if (received != 0) {
		for (int i = 0; i < received; i++) {
			raw[i] = this->Buffer[i];
		}
	}
	return received;
}

////Returns the default IPAddress of the Mysocket object
std::string MySocket::GetIPAddr() 
{
	return this->IPAddr;
}

//Changes the default IPAddr depending on the success criteria
//Returns an error (false) if the connection is already established and true if the connection is not established
bool MySocket::SetIPAddr(std::string IPAddress) 
{
	bool ret = false;
	if (this->bConnect == false || (this->WelcomeSocket == INVALID_SOCKET||this->ConnectionSocket==INVALID_SOCKET))
	{
		this->IPAddr = IPAddress;
		SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); 
		ret = true;
	}
	else ret = false;
	return ret;
}


//Returns the default Port number of the MySocket object
int MySocket::GetPort() 
{
	return this->Port;
}

//Returns default SocketType of the MySocket object
SocketType MySocket::GetType() 
{
	return this->mySocket;
}

//Changes the default SocketType depending on the success criteria
//Returns an error (false) if the connection is already established and true if the connection is not established
bool MySocket::SetType(SocketType socketType) 
{
	bool ret = false;
	if (this->bConnect == false || (this->mySocket == SERVER && WelcomeSocket == INVALID_SOCKET))
	{
		this->mySocket = socketType;
		ret = true;
	}
	else
	{
		Print("***ERROR!!! Connection is established already with this SocketType!***");
		ret = false;
	}
	return ret;
}

void MySocket::BindSocket()
{
	if (this->mySocket == TCP) {
		if ((bind(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			Print("Could not bind to the socket");
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cin.get();
			exit(0);
		}
	}

	else {
		struct sockaddr_in SrvAddr;
		SrvAddr.sin_family = AF_INET;
		SrvAddr.sin_port = htons(Port);
		SrvAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(WelcomeSocket, (struct sockaddr *)&SrvAddr, sizeof(SrvAddr)) == SOCKET_ERROR) {
			closesocket(WelcomeSocket);
			WSACleanup();
			exit(0);
		}

		if ((bind(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			Print("Could not bind to the socket");
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cin.get();
			exit(0);

		}
	}
}

void MySocket::ListenSocket()
{
	if (listen(this->WelcomeSocket, 1) == SOCKET_ERROR) {
		Print("Could not listen to the provided socket.");
		closesocket(this->WelcomeSocket);
		WSACleanup();
		std::cin.get();
		exit(0);
	}
	else {
		Print("Waiting for client connection");
	}
}

SOCKET MySocket::InitializeTCPSocket()
{
	SOCKET newBorn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (newBorn == INVALID_SOCKET) {
		Print("Could not initialize socket");
		WSACleanup();
		std::cin.get();
		exit(0);
	}
	else this->bConnect = true;
	return newBorn;
}

SOCKET MySocket::InitializeUDPSocket()
{
	SOCKET newBorn = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	
	if (newBorn == INVALID_SOCKET) {
		WSACleanup();
		Print("Could not initialize socket");
	}
	if (newBorn == INVALID_SOCKET) {
		WSACleanup();
		Print("Could not initialize udp socket");

		std::cin.get();
		exit(0);
	}
	else this->bConnect = true;
	return newBorn;
}


