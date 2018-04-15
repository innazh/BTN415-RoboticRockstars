#include "PktDef.h"
#include "MySocket.h"
#include <thread>

bool ExeComplete;
bool cmdSet = false;
bool telSet = false;
SocketType client = CLIENT;
ConnectionType tcp = TCP;
std::string ip;
int portCmd;
int portTel;

//Logic for the command thread
void cmd() {
	int userInput = 0;
	PktDef cmdPacket;
	MySocket * command;

	//Create, connect socket
	if (cmdSet == false) {
		command = new MySocket(client, ip, portCmd, tcp, 128);
		command->ConnectTCP();
		cmdSet = true;
	}

	while (true) {
		//Get information from the user for the packet
		//This gets the packet type
		//1 = MotorBody
		//2 = ActuatorBody
		while (userInput < DRIVE || userInput > CLAW) {
			std::cout << "Enter the number: \n1. DRIVE\n3. SLEEP\n4. ARM\n5. CLAW\n5. Packet with incorrect CRC\n6. Packet with incorrect header length\n7. Packet with incorrect header command byte";
			std::cout << "\nEnter: ";
			std::cin >> userInput;

			if (userInput < DRIVE || userInput > 7 || userInput == STATUS) {
				std::cout << "Error: Invalid input.\n";
			}
		}

		//DRIVE COMMAND
		if (userInput == DRIVE) {
			unsigned int direction = 0;
			unsigned int duration = 0;

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

		//SLEEP command -- see after the while loop for the rest of this
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
		//Packet with incorrect CRC
		else if (userInput == 5) {

		}

		//Packet with incorrect header length
		else if (userInput == 6) {

		}

		//Packet with multiple command bytes set to 1
		else if (userInput == 7) {

		}
	}

	//Sending SLEEP command
	
}

//Logic for the telemetry thread
void tel() {
	int recv;
	char rxBuffer[128];
	MySocket *tel;

	//Create socket
	if (telSet == false) {
		tel = new MySocket(client, ip, portTel, tcp, 128);
		tel->ConnectTCP();
		telSet = true;
	}

	while (true) {
		recv = tel->GetData(rxBuffer); //receive data
		
		//Packet isn't corrupted
		if (recv > 0 && recv < 13) {
			PktDef packet(rxBuffer); //create packet from data received
			packet.CalcCRC();

			//If the CRC is OK
			if (packet.CheckCRC((char *)rxBuffer, 12)) {
				//If STATUS is true rxBuffer[5] & 1 >> 1
				if (packet.GetCmd() == STATUS) {
					char * bodyData = packet.GetBodyData();
					std::cout << "Data: " << bodyData << std::endl; //display raw data
					
					//Display sonar data
					std::cout << "Sonar: " << bodyData[0] << " " << bodyData[2] << std::endl;

					//Display arm reading
					std::cout << "Arm: " << bodyData[2] << " " << bodyData[3] << std::endl;

					//Display DRIVE data
					if (bodyData[4] & 1 == 0) {
						std::cout << "Robot is STOPPED" << std::endl;
					}
					else {
						std::cout << "Robot is DRIVING" << std::endl;
					}

					//Display ARM data
					if (bodyData[4] & 1 >> 1 == UP) {
						std::cout << "Arm is UP, ";
					}
					else {
						std::cout << "Arm is DOWN, ";
					}

					//Display CLAW data
					if (bodyData[4] & 1 >> 2 == OPEN) {
						std::cout << "Claw is OPEN" << std::endl;
					}
					else {
						std::cout << "Claw is CLOSED" << std::endl;
					}
				}
				else {
					std::cout << "Error: STATUS not set to true" << std::endl;
				}
			}
			else {
				std::cout << "Error: CRC check failed" << std::endl;
			}
		}
		//Packet is corrupted

		else {
			std::cout << "Error: Corrupt packet received" << std::endl;
		}
	}
}

//Main function
int main(int argc, char* argv) {
	//Set ExeComplete to false
	ExeComplete = false;

	//Get the ip
	std::cout << "Enter the IP Address: ";
	std::cin >> ip;

	//Get the port - command
	std::cout << "Enter the port number (command thread): ";
	std::cin >> portCmd;

	//Get the port - telemetry
	std::cout << "Enter the port number (telemetry thread): ";
	std::cin >> portTel;

	while (ExeComplete) {
		std::thread(cmd).detach();
		std::thread(tel).detach();
	}
}