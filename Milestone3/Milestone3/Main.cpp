#include "PktDef.h"
#include "MySocket.h"
#include <thread>
#include <conio.h>

bool ExeComplete;
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
	int key;

	//Create, connect socket
	command = new MySocket(client, ip, portCmd, tcp, 128);
	command->ConnectTCP();

	//Get input and generate and send packets
	while (true) {
		//Detect keyboard being pressed
		if (_kbhit()) {
			//Get the key being pressed, it's an int don't be fooled
			key = _getch();

			//DRIVE
			//119 = w, 115 = s, 97 = a, 100 = d
			if (key == 119 || key == 115 || key == 97 || key == 100) {
				char direction;

				//Get the duration
				//Use the numbers on your keyboard, we can get 1 - 9 seconds
				int duration = _getch();

				//49 = 1, 57 = 9
				while (duration < 49 || duration > 57) {
					std::cout << "Error: Duration must be 1 - 9" << std::endl;
					int duration = _getch();
				}
				
				//Set the direction based on the key pressed
				switch (key) {
				case 119:
					direction = FORWARD;
					break;
				case 115:
					direction = BACKWARD;
					break;
				case 97:
					direction = LEFT;
					break;
				case 100:
					direction = RIGHT;
					break;
				}

				//Make the MotorBody
				MotorBody body;
				body.Direction = direction;
				body.Duration = (char)duration;

				//Populate packet
				cmdPacket.SetCmd(DRIVE);
				cmdPacket.SetBodyData((char *)&body, 2);
				cmdPacket.SetPktCount(1);
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command->SendData(buffer, sizeof(buffer));
			}

			//ARM
			//72 = up arrow, 80 = down arrow
			if (key == 72 || key == 80) {
				char action;

				//Set direction based on arrow pressed
				switch (key) {
				case 72:
					action = UP;
					break;
				case 80:
					action = DOWN;
					break;
				}

				//Make ActuatorBody
				ActuatorBody body;
				body.Action = action;

				//Populate packet
				cmdPacket.SetCmd(ARM);
				cmdPacket.SetBodyData((char *)&body, 1);
				cmdPacket.SetPktCount(1);
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command->SendData(buffer, sizeof(buffer));
			}

			//CLAW
			//75 = key left (open), 77 = key right (close)
			if (key == 75 || key == 77) {
				char action;

				//Set direction based on arrow pressed
				switch (key) {
				case 75:
					action = OPEN;
					break;
				case 77:
					action = CLOSE;
					break;
				}

				//Make ActuatorBody
				ActuatorBody body;
				body.Action = action;

				//Populate packet
				cmdPacket.SetCmd(ARM);
				cmdPacket.SetBodyData((char *)&body, 1);
				cmdPacket.SetPktCount(1);
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command->SendData(buffer, sizeof(buffer));
			}
			
			//SLEEP
			//107 - k
			if (key == 107) {
				break;
			}

			//Incorrect CRC
			// - v
			if (key == 118) {
				
			}

			//Incorrect header length
			// - b
			if (key == 98) {

			}

			//Incorrect header command
			// - n
			if (key == 110) {

			}
		}
		

		
		/*Packet with incorrect CRC
		else if (userInput == 5) {

		}

		//Packet with incorrect header length
		else if (userInput == 6) {

		}

		//Packet with multiple command bytes set to 1
		else if (userInput == 7) {

		}
	}

	//Sending SLEEP command*/
	}
}

//Logic for the telemetry thread
void tel() {
	int recv;
	char rxBuffer[128];

	MySocket tel (client, ip, portTel, tcp, 128);
	tel.ConnectTCP();

	while (true) {
		recv = tel.GetData(rxBuffer); //receive data
		
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
	bool pleaseWork = false;

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

	while (!ExeComplete) {
		if (pleaseWork == false) {
			std::thread(cmd).detach();
			std::thread(tel).detach();

			pleaseWork = true;
		}
	}
}