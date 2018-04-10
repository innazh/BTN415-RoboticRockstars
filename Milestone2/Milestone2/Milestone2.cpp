#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include "MySocket.h"

using namespace std;


void TCP_Client(bool &bDone)
{
	std::ofstream ofs("Client_Output.txt");
	if (!ofs.is_open())
		std::cout << "ERROR:  Failed to open Client_Output.txt file" << std::endl;

	ofs << "TCP_Client Thread Started" << std::endl;
	MySocket ClientSocket(SocketType::CLIENT, "127.0.0.1", 5000, ConnectionType::TCP, 100);

	std::string Pkt = "I love BTN415";

	ofs << "*** Validate UDP startup with TCP Socket" << std::endl;
	if (!ClientSocket.SetupUDP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in SetupUDP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}

	//Only continue if the TCP connection is configured correctly
	if (!ClientSocket.ConnectTCP())
		return;

	ofs << "Sending: " << Pkt << std::endl;
	ClientSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	char buff[100];
	int RxSize = ClientSocket.GetData(buff);
	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 19)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	//Try changing the configuration of a TCP socket
	if (!ClientSocket.SetIPAddr("192.168.7.2"))
		ofs << "SetIPAddr -- PASSED" << std::endl;
	else
		ofs << "SetIPAddr -- FAILED" << std::endl;

	if (!ClientSocket.SetPortNum(27000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetPortNum -- FAILED" << std::endl;


	ofs << "*** Validate TCP Socket shutdown with UDP" << std::endl;
	if (!ClientSocket.TerminateUDP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in TerminateUDP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}

	if (ClientSocket.DisconnectTCP())
		ofs << "TCP Socket Disconnect PASSED" << std::endl;
	else
		ofs << "TCP Socket Disconnect FAILED" << std::endl;

	
	//Change the port test -- performed after termination
	if (ClientSocket.SetPortNum(27000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetPortNum -- FAILED" << std::endl;

	//Only continue if the TCP connection is configured correctly
	if (!ClientSocket.ConnectTCP())
		return;

	ofs << "Sending: " << Pkt << std::endl;
	ClientSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	RxSize = ClientSocket.GetData(buff);
	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	ClientSocket.DisconnectTCP();

	ofs << "TCP_Client thread finished" << std::endl;

	ofs.close();
	bDone = true;
}

void UDP_Client(bool &bDone)
{
	std::ofstream ofs("Client_Output.txt", std::ios_base::app);
	if (!ofs.is_open())
		std::cout << "ERROR:  Failed to open Client_Output.txt file" << std::endl;

	ofs << std::endl << "*****************************" << std::endl << std::endl;
	ofs << "UDP_Client Thread Started" << std::endl;
	MySocket ClientSocket(SocketType::CLIENT, "127.0.0.1", 5000, ConnectionType::UDP, 100);
	
	ofs << "*** Validate TCP startup with UDP Socket" << std::endl;
	if (!ClientSocket.ConnectTCP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in ConnectTCP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}

	//Only continue with the thread if the socket has been setup correctly
	if (!ClientSocket.SetupUDP())
		return;

	std::string Pkt = "I love BTN415";

	ofs << "Sending: " << Pkt << std::endl;
	ClientSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	char buff[100];
	int RxSize = ClientSocket.GetData(buff);
	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 19)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	ofs << "Sending: " << Pkt << std::endl;
	ClientSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	RxSize = ClientSocket.GetData(buff);
	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 19)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	ofs << "*** Validate TCP Disconect with UDP socket" << std::endl;
	if (!ClientSocket.DisconnectTCP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in DisconnectTCP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}
	
	if (ClientSocket.TerminateUDP())
		ofs << "Terminate UDP -- PASSED" << std::endl;
	else
		ofs << "Terminate UDP -- FAILED" << std::endl;

	if(ClientSocket.SetPortNum(27000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetPortNum -- FAILED" << std::endl;
		
	if (ClientSocket.SetupUDP())
		ofs << "New UDP Socket setup PASSED" << std::endl;
	else
		ofs << "New UDP Socket setup FAILED" << std::endl;

	ofs << "Sending: " << Pkt << std::endl;
	ClientSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	RxSize = ClientSocket.GetData(buff);
	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 19)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	ClientSocket.TerminateUDP();
		
	ofs << "TCP_Client thread finished" << std::endl;

	ofs.close();
	bDone = true;
}

void TCP_Server(bool &bDone)
{
	std::ofstream ofs("Server_Output.txt");
	if (!ofs.is_open())
		std::cout << "ERROR:  Failed to open Server_Output.txt file" << std::endl;
	
	ofs << "TCP_Server Thread Started" << std::endl;
	MySocket ServerSocket(SocketType::SERVER, "127.0.0.1", 5000, ConnectionType::TCP, 100);
	
	ofs << "*** Validate UDP startup with TCP Socket" << std::endl;
	if (!ServerSocket.SetupUDP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in SetupUDP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}
	//Only continue if the TCP connection is configured
	if (!ServerSocket.ConnectTCP())
		return;

	char buff[100];
	int RxSize = ServerSocket.GetData(buff);

	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 14)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if(strcmp(buff, "I love BTN415 Too!"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	std::string Pkt = "I Love BTN415 too!";

	ofs << "Sending: " << Pkt << std::endl;
	ServerSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	//Try changing the configuration of a TCP socket
	if (!ServerSocket.SetIPAddr("192.168.7.2"))
		ofs << "SetIPAddr -- PASSED" << std::endl;
	else
		ofs << "SetIPAddr -- FAILED" << std::endl;

	if (!ServerSocket.SetPortNum(27000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetPortNum -- FAILED" << std::endl;

	ofs << "*** Validate TCP Socket shutdown with UDP" << std::endl;
	if (!ServerSocket.TerminateUDP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in TerminateUDP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}

	if (ServerSocket.DisconnectTCP())
		ofs << "TCP Socket Disconnect PASSED" << std::endl;
	else
		ofs << "TCP Socket Disconnect FAILED" << std::endl;

	//Changing the port number test -- following a disconnect
	if (ServerSocket.SetPortNum(27000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetPortNum -- FAILED" << std::endl;

	//Only continue if the TCP connection is configured
	if (!ServerSocket.ConnectTCP())
		return;

	RxSize = ServerSocket.GetData(buff);

	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 14)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415 Too!"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	ofs << "Sending: " << Pkt << std::endl;
	ServerSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	ServerSocket.DisconnectTCP();

	//Change the port and IP Address, but this type check it
	if (ServerSocket.SetIPAddr("192.168.7.2"))
		ofs << "SetIPAddr -- PASSED" << std::endl;
	else
		ofs << "SetIPAddr -- FAILED" << std::endl;

	if(ServerSocket.SetPortNum(3000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetIPortNum -- FAILED" << std::endl;

	int PortResult = ServerSocket.GetPort();
	if (PortResult == 3000)
		ofs << "GetPort -- PASSED" << std::endl;
	else
		ofs << "GetPort -- FAILED" << std::endl;

	ofs << "TCP_Client thread finished" << std::endl;

	std::string IPResult = ServerSocket.GetIPAddr();
	if(IPResult == "192.168.7.2")
		ofs << "GetIPAddr -- PASSED" << std::endl;
	else
		ofs << "GetIPAddr -- FAILED" << std::endl;

	ofs.close();
	bDone = true;
}

void UDP_Server(bool &bDone)
{
	std::ofstream ofs("Server_Output.txt", std::ios_base::app);
	if (!ofs.is_open())
		std::cout << "ERROR:  Failed to open Server_Output.txt file" << std::endl;

	ofs << std::endl << "*****************************" << std::endl << std::endl;
	ofs << "UDP_Server Thread Started" << std::endl;
	MySocket ServerSocket(SocketType::SERVER, "127.0.0.1", 5000, ConnectionType::UDP, 100);

	ofs << "*** Validate UDP startup with TCP Socket" << std::endl;
	if (!ServerSocket.ConnectTCP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in ConnectTCP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}
	//Only continue if the UDP connection is configured
	if (!ServerSocket.SetupUDP())
		return;

	char buff[100];
	int RxSize = ServerSocket.GetData(buff);

	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 14)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415 Too!"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	std::string Pkt = "I Love BTN415 too!";

	ofs << "Sending: " << Pkt << std::endl;
	ServerSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	//Try changing the configuration of a TCP socket
	if (!ServerSocket.SetIPAddr("127.0.0.1"))
		ofs << "SetIPAddr -- PASSED" << std::endl;
	else
		ofs << "SetIPAddr -- FAILED" << std::endl;

	if (!ServerSocket.SetPortNum(27000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetPortNum -- FAILED" << std::endl;

	RxSize = ServerSocket.GetData(buff);

	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 14)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415 Too!"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	ofs << "Sending: " << Pkt << std::endl;
	ServerSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	ofs << "*** Validate TCP Disconect with UDP socket" << std::endl;
	if (!ServerSocket.DisconnectTCP())
	{
		ofs << "PASSED" << std::endl;
	}
	else
	{
		ofs << "FAILED -- Logic in DisconnectTCP required" << std::endl;
		ofs << "Terminating testing" << std::endl;
		return;
	}

	if (ServerSocket.TerminateUDP())
		ofs << "Terminate UDP -- PASSED" << std::endl;
	else
		ofs << "Terminate UDP -- FAILED" << std::endl;

	//Test a validate change
	if (ServerSocket.SetPortNum(27000))
		ofs << "SetPortNum -- PASSED" << std::endl;
	else
		ofs << "SetPortNum -- FAILED" << std::endl;

	//Only continue if the UDP connection is configured
	if (!ServerSocket.SetupUDP())
		return;

	RxSize = ServerSocket.GetData(buff);

	ofs << "Msg = " << buff << ", Bytes = " << RxSize << std::endl;

	if (RxSize == 14)
		ofs << "SIZE Check -- PASSED" << std::endl;
	else
		ofs << "SIZE Check -- FAILED" << std::endl;

	if (strcmp(buff, "I love BTN415 Too!"))
		ofs << "Msg Check -- PASSED" << std::endl;
	else
		ofs << "Msg Check -- FAILED" << std::endl;

	ofs << "Sending: " << Pkt << std::endl;
	ServerSocket.SendData(Pkt.c_str(), strlen(Pkt.c_str()) + 1);

	ServerSocket.TerminateUDP();

	ServerSocket.SetIPAddr("192.168.7.2");
	std::string IPResult = ServerSocket.GetIPAddr();
	if(IPResult == "192.168.7.2")
		ofs << "GetIPAddr -- PASSED" << std::endl;
	else
		ofs << "GetIPAddr -- FAILED" << std::endl;

	ofs << "TCP_Client thread finished" << std::endl;

	ofs.close();
	bDone = true;
}

int main(int argc, char *argv[])
{
	char result;
	bool bClient = false;
	bool bServer = false;

	std::cout << "Running TCP Client-Server Test" << std::endl;
	std::thread T1(TCP_Server, std::ref(bServer));
	T1.detach();

	std::cout << "Wait a second for the server to intialize, hit any key to continue" << std::endl;
	std::cin >> result;

	std::thread T2(TCP_Client, std::ref(bClient));
	T2.detach();

	while (!bClient && !bServer) {
	}

	std::cout << "Continue with UDP Testing? (y/n): " << std::endl;
	std::cin >> result;
	if ((result == 'n') || (result == 'N'))
		return 1;
	
	std::cout << std::endl << "*************************************" << std::endl;

	std::cout << "Running UDP Client-Server Test" << std::endl;
	bClient = false;
	bServer = false;

	std::thread T3(UDP_Server, std::ref(bServer));
	T3.detach();

	std::cout << "Wait a second for the server to intialize, hit any key to continue" << std::endl;
	std::cin >> result;

	std::thread T4(UDP_Client, std::ref(bClient));
	T4.detach();

	while (!bClient && !bServer) {}

	std::cout << "Hit any key to continue" << std::endl;
	std::cin >> result;

	return 1;
}