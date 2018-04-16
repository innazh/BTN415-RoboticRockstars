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
	int recv;
	int key;
	PktDef cmdPacket;
	PktDef * recPacket;
	char rxBuffer[128];
	bool sent = false;

	//Create, connect socket
	MySocket command(client, ip, portCmd, tcp, 128);
	command.ConnectTCP();

	//Get input and generate and send packets
	while (true) {
		key = 0;
		//Detect keyboard being pressed
		if (_kbhit() && sent == false) {
			//Get the key being pressed, it's an int don't be fooled
			key = _getch();

			//DRIVE
			//119 = w, 115 = s, 97 = a, 100 = d
			if (key == 119 || key == 115 || key == 97 || key == 100) {
				int direction = 0;
				int duration = 0;

				//Get the duration
				//Use the numbers on your keyboard, we can get 1 - 9 seconds
				int durationGet = _getch();

				//49 = 1, 57 = 9
				if (durationGet < 49 || durationGet > 57) {
					fflush(stdin);
					std::cout << "Error: Duration must be 1 - 9" << std::endl;
					int durationGet = _getch();
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

				//Set the duration based on the key pressed
				switch (durationGet) {
				case 49:
					duration = 1;
					break;
				case 50:
					duration = 2;
					break;
				case 51:
					duration = 3;
					break;
				case 52:
					duration = 4;
					break;
				case 53:
					duration = 5;
					break;
				case 54:
					duration = 6;
					break;
				case 55:
					duration = 7;
					break;
				case 56:
					duration = 8;
					break;
				case 57:
					duration = 9;
					break;
				}

				//Make the MotorBody
				MotorBody body;
				body.Direction = direction;
				body.Duration = duration;

				cmdPacket.SetCmd(DRIVE);
				cmdPacket.SetBodyData((char *)&body, 2);
				cmdPacket.SetPktCount(1);
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command.SendData(buffer, sizeof(buffer) + 1);
				sent = true;
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
				command.SendData(buffer, sizeof(buffer));
				sent = true;
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
				cmdPacket.SetCmd(CLAW);
				cmdPacket.SetBodyData((char *)&body, 1);
				cmdPacket.SetPktCount(1);
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command.SendData(buffer, sizeof(buffer));
				sent = true;
			}

			//SLEEP
			//107 - k
			if (key == 107) {
				// Set the command to sleep
				cmdPacket.SetCmd(CmdType::SLEEP);

				// Calculate the CRC
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command.SendData(buffer, sizeof(buffer));
				sent = true;
			}

			//Incorrect CRC
			// 118 - v
			if (key == 118) {
				//Make ActuatorBody
				ActuatorBody aBody;
				aBody.Action = UP;

				//Populate packet
				cmdPacket.SetCmd(CLAW);
				cmdPacket.SetBodyData((char *)&aBody, 1);
				cmdPacket.SetPktCount(1);
				cmdPacket.setBadCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command.SendData(buffer, sizeof(buffer));
				sent = true;
			}

			//Incorrect header length
			// 98 - b
			if (key == 98) {
				MotorBody bLength;
				bLength.Direction = BACKWARD;
				bLength.Duration = 5;

				//Populate packet
				cmdPacket.SetCmd(DRIVE);
				cmdPacket.SetBodyData((char *)&bLength, 1);
				cmdPacket.SetPktCount(1);
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command.SendData(buffer, sizeof(buffer));
				sent = true;
			}

			//Incorrect header command
			//110 - n
			if (key == 110) {
				MotorBody bHead;
				bHead.Direction = RIGHT;
				bHead.Duration = 5;

				//Populate packet
				cmdPacket.SetBadCmd();
				cmdPacket.SetBodyData((char *)&bHead, 2);
				cmdPacket.SetPktCount(1);
				cmdPacket.CalcCRC();

				//Generate the data and send it
				char * buffer = cmdPacket.GenPacket();
				command.SendData(buffer, sizeof(buffer));
				sent = true;
			}
		}
		
		//Receive ACK or NACK packets
		if (sent == true) {
			recv = command.GetData(rxBuffer);
			recPacket = new PktDef(rxBuffer);

			std::cout << "\nN/ACK packet received from server\n" << std::endl;

			//SLEEP command was previously sent
			if (key == 107) {
				command.DisconnectTCP();
				ExeComplete = true;
				break;
			}

			//NACK
			if (!recPacket->GetAck() && recPacket->GetCmd() != SLEEP) {
				char * bodyData = recPacket->GetBodyData();
				std::cout << "Error: " << (std::string)bodyData << std::endl;
			}

			sent = false;
		}
	}
}

//Logic for the telemetry thread
void tel() {
	int recv;
	char rxBuffer[128];

	MySocket tel(client, ip, portTel, tcp, 128);
	tel.ConnectTCP();

	while (true) {
		recv = tel.GetData(rxBuffer); //receive data

		//Packet isn't corrupted
		if (recv > 0 && recv < 13) {
			PktDef packet(rxBuffer); //create packet from data received
			packet.CalcCRC();

			//If the CRC is OK
			if (packet.CheckCRC((char *)rxBuffer, 12)) {
				//If STATUS is true
					if (packet.GetCmd() == STATUS) {
						char * rawData = packet.GenPacket();
						char * bodyData = packet.GetBodyData();

						//display raw data
						std::cout << "Data: ";
						for (int i = 0; i < packet.GetLength(); i++)
						{
							std::cout << std::hex << (unsigned int)*(rawData++) << ", ";
						}
						std::cout << std::endl;

						//Display sonar data
						std::cout << "Sonar: " << std::dec << (short int)bodyData[0] << std::endl;

						//Display arm reading
						std::cout << "Arm Position: " << (short int)bodyData[2] << std::endl << std::endl;

						//Display driving
						if (bodyData[4] & 1) {
							std::cout << "DRIVING" << std::endl;
						}
						else {
							std::cout << "STOPPED" << std::endl;
						}

						//Display arm
						if (bodyData[4] >> 1 & 1) {
							std::cout << "ARM UP, ";
						}
						else {
							std::cout << "ARM DOWN, ";
						}

						//Display claw
						if (bodyData[4] >> 3 & 1) {
							std::cout << "CLAW OPEN";
						}
						else {
							std::cout << "CLAW CLOSED";
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