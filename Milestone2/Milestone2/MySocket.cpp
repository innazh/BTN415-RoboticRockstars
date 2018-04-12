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
	StartWSA();
}

//Destructor for cleaning up dynamically allocated memory
MySocket::~MySocket()
{
	delete Buffer;
	Buffer = nullptr;
	this->MaxSize = 0;//not sure if this is needed

	this->bConnect = false;

	if (!(this->mySocket == SERVER && this->connectionType == UDP)) closesocket(this->ConnectionSocket);

	if (this->mySocket == SERVER) closesocket(this->WelcomeSocket);
}

//Initializes the Winsock DLLs.
//Returns true if the Winsock is succesfully initialized else return false
bool MySocket::StartWSA()
{
	bool ret = true;
	WSADATA wsaData;

	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		Print("Error: Could not start DDLs");
		exit(0);
		ret = false;
	}
	return ret;
}

//Establish TCP/IP connection, return true if connection made
bool MySocket::ConnectTCP()
{
	bool res = true;

	//Check that connection type is TCP -- if not, print an error
	if (this->connectionType != TCP) {
		Print("Error in ConnectTCP: Cannot connect with UDP sockets.");
		res = false;
	}
	else {
		//TCP Server
		if (mySocket == SERVER) {
			this->WelcomeSocket = InitializeTCPSocket();
			//check if socket is created
			this->BindSocket();
			this->ListenSocket();

			if ((this->ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR) {
				res = false;
				closesocket(this->ConnectionSocket);

				Print("Error: Could not accept incoming connection.");
				std::cin.get();
				exit(0);
			}
			else {
				bConnect = true;
			}
		}
		else {
			this->ConnectionSocket = InitializeTCPSocket();

			SvrAddr.sin_family = AF_INET; //Address family type internet
			SvrAddr.sin_port = htons(Port); //port (host to network conversion)
			SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //IP address

			if ((connect(this->ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
				res = false;
				closesocket(this->ConnectionSocket);
				Print("Error: Could not connect to the server");
				std::cin.get();
				exit(0);
			}
			else {
				bConnect = true;
			}
		}
	}

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
		ret = true;
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
		}
		else {
			closesocket(this->WelcomeSocket);
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
			WelcomeSocket = InitializeUDPSocket();
			BindSocket();
		}
		else {
			SvrAddr.sin_family = AF_INET; //Address family type internet
			SvrAddr.sin_port = htons(Port); //port (host to network conversion)
			SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //IP address

			ConnectionSocket = InitializeUDPSocket();
		}
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

		}
		else {
			closesocket(this->WelcomeSocket);


		}
		r = true;
	}
	return r;
}

/* This is the broken function- udp doesnt work*/
int MySocket::SendData(const char* raw, int size)
{
	int transmitted = 0;

	if (this->connectionType == TCP) {
		transmitted = send(this->ConnectionSocket, raw, size, 0);
	}

	//I believe this is wrong, check lab 6 for how to do UDP sendto
	else if (this->connectionType == UDP) {
		if (this->mySocket == SERVER) {
			transmitted = sendto(this->WelcomeSocket, raw, size, 0, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
		}
		else {
			transmitted = sendto(this->ConnectionSocket, raw, size, 0, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
		}
	}

	return transmitted;
}

int MySocket::GetData(char* raw)
{
	int received = 0;
//	char * rxBuffer = new char[MaxSize];

	//If socket is TCP
	if (this->connectionType == TCP) {
		received = recv(this->ConnectionSocket, raw, MaxSize, 0);
	}

	//If socket is UDP
	int addrLen = sizeof(SvrAddr);

	if (this->connectionType == UDP) {
		//UDP Server
		if (mySocket == SERVER) {
			received = recvfrom(this->ConnectionSocket, raw, sizeof(raw), 0, (struct sockaddr*)&SvrAddr, &addrLen);
		}
		else {
			received = recvfrom(this->WelcomeSocket, raw, sizeof(raw), 0, (struct sockaddr*)&SvrAddr, &addrLen);
		}
	}

	if (received != 0) {
		Buffer = new char[sizeof(raw)];
		memcpy(Buffer, raw, sizeof(Buffer));
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

	if (this->bConnect == false || (this->WelcomeSocket == INVALID_SOCKET || this->ConnectionSocket == INVALID_SOCKET))
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
		Print("Error: Connection is established already with this SocketType.");
		ret = false;
	}
	return ret;
}

void MySocket::BindSocket()
{
	if (this->connectionType == TCP) {
		//configure the socket
		SvrAddr.sin_family = AF_INET; //Address family type internet
		SvrAddr.sin_port = htons(Port); //port (host to network conversion)
		SvrAddr.sin_addr.s_addr = INADDR_ANY;
	}

	if (bind(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
		Print("Error: Could not bind to the socket.");
		closesocket(this->WelcomeSocket);

		std::cin.get();
		exit(0);
	}
}


void MySocket::ListenSocket()
{
	if (listen(this->WelcomeSocket, 1) == SOCKET_ERROR) {
		closesocket(this->WelcomeSocket);

		Print("Error: Could not listen to the provided socket.");
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
		bConnect = false;

		Print("Error: Could not initialize socket");
		std::cin.get();
		exit(0);
	}

	return newBorn;
}

SOCKET MySocket::InitializeUDPSocket()
{
	SOCKET newBorn = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (newBorn == INVALID_SOCKET) {
		Print("Error: Could not initialize udp socket");
		std::cin.get();
		exit(0);
	}

	return newBorn;
}


