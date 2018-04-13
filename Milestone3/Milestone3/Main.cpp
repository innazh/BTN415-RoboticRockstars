#include "PktDef.h"
#include "MySocket.h"
#include <thread>

bool ExeComplete;

//Logic for the command thread
void cmd() {
	int userInput = 0;
	PktDef cmdPacket;
	std::string ip;
	int port;
	SocketType client = CLIENT;
	ConnectionType tcp = TCP;

	//Get the ip and port
	std::cout << "Enter the IP Address: ";
	std::cin >> ip;
	std::cout << "Enter the port number: ";
	std::cin >> port;


	//Create socket
	MySocket command(client, ip, port, tcp, 0);
	
	//Connect to the server
	command.ConnectTCP();

	while (true) {
		//Get information from the user for the packet
		//This gets the packet type
		//1 = MotorBody
		//2 = ActuatorBody
		while (userInput < DRIVE || userInput > CLAW) {
			std::cout << "Enter the number: \n1. DRIVE\n3. SLEEP\n4. ARM\n5. CLAW";
			std::cin >> userInput;

			if (userInput < DRIVE || userInput > CLAW) {
				std::cout << "Error: Invalid input.\n";
			}
		}

		//DRIVE COMMAND
		if (userInput == DRIVE) {
			unsigned char direction = 0;
			unsigned char duration = 0;

			//Direction
			while (direction < FORWARD || direction > LEFT) {
				std::cout << "Enter the number:\n1. Forward\n2. Backward\n3. Right\n4.Left";
				std::cin >> direction;

				if (direction < FORWARD || direction > LEFT) {
					std::cout << "Error: Invalid input." << std::endl;
				}
			}

			//Duration
			std::cout << "Enter the duration in seconds: ";
			std::cin >> duration;

			//Make the MotorBody
			MotorBody body;
			body.Direction = direction;
			body.Duration = duration;

			//Populate packet
			cmdPacket.SetCmd(DRIVE);
			cmdPacket.SetBodyData((char *)&body, 2);
			cmdPacket.SetPktCount(1);
			cmdPacket.CalcCRC();
		}

		//SLEEP command
		else if (userInput == SLEEP) {
			break;
		}

		//ARM command
		else if (userInput == ARM) {
			unsigned char action = 0;

			while (action < UP || action > DOWN) {
				std::cout << "Enter:5. Up\n6. Down";
				std::cin >> action;

				if (action < UP || action > DOWN) {
					std::cout << "Error: Invalid input.";
				}
			}

			//Make ActuatorBody
			ActuatorBody body;
			body.Action = action;

			//Populate packet
			cmdPacket.SetCmd(ARM);
			cmdPacket.SetBodyData((char *)&body, 1);
			cmdPacket.SetPktCount(1);
			cmdPacket.CalcCRC();
		}

		//CLAW command
		else if (userInput == CLAW) {
			unsigned char action = 0;

			while (action < OPEN || action > CLOSE) {
				std::cout << "Enter:7. Open\n8. Close";
				std::cin >> action;

				if (action < OPEN || action > CLOSE) {
					std::cout << "Error: Invalid input.";
				}
			}

			//Make ActuatorBody
			ActuatorBody body;
			body.Action = action;

			//Populate packet
			cmdPacket.SetCmd(ARM);
			cmdPacket.SetBodyData((char *)&body, 1);
			cmdPacket.SetPktCount(1);
			cmdPacket.CalcCRC();
		}
	}

	//Sending SLEEP command
	
}

//Logic for the telemetry thread
void tel() {
	std::string ip;
	int port;
	SocketType client = CLIENT;
	ConnectionType tcp = TCP;

	//Get the ip and port
	std::cout << "Enter the IP Address: ";
	std::cin >> ip;
	std::cout << "Enter the port number: ";
	std::cin >> port;

	//Create socket
	MySocket tel(client, ip, port, tcp, 0);
}

int main(int argc, char* argv) {
	//Set ExeComplete to false
	ExeComplete = false;

	//Spawn threads
	std::thread (cmd).detach();
	std::thread (tel).detach();
}