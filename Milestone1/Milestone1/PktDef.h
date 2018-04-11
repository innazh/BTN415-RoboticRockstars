#ifndef PKTDEF_H_
#define PKTDEF_H_

//INCLUDES
#include <string>
#include <iostream>

/* PACKET HEADER - 6 BYTES
* PktCount = number of times packet has been sent
* Drive/Sleep/Status/etc are commands.  1 = active command!
* Padding rounds out the number of bits
* Length is the number of bytes in the packet
*/
struct Header {
	unsigned int PktCount;
	unsigned char Sleep : 1;
	unsigned char Status : 1;
	unsigned char Drive : 1;
	unsigned char Claw : 1;
	unsigned char Arm : 1;
	unsigned char Ack : 1;
	unsigned char Padding : 2;
	unsigned char Length;
};

/* PACKET BODY - MOTOR BODY - 2 BYTES
* Commands for driving the robot
* 1 = Forward, 2 = Backward, 3 = Right, 4 = left
* Duration is in seconds
*/
struct MotorBody {
	unsigned char Direction;
	unsigned char Duration;
};

/* PACKET BODY - ACTUATOR BODY - 1 BYTE
* Commands for moving the robot's arm and claw
* 5 = Up, 6 = Down, 7 = Close Claw, 8 = Open Claw
*/
struct ActuatorBody {
	unsigned char Action;
};


//Define the command types
enum CmdType {
	DRIVE = 1,
	STATUS,
	SLEEP,
	ARM,
	CLAW,
	ACK
};

//Define directions, arm movements, and size of the header
const unsigned int FORWARD = 1;
const unsigned int BACKWARD = 2;
const unsigned int RIGHT = 3;
const unsigned int LEFT = 4;
const unsigned int UP = 5;
const unsigned int DOWN = 6;
const unsigned int OPEN = 7;
const unsigned int CLOSE = 8;
const unsigned int HEADERSIZE = 6;
const unsigned int MOTORSIZE = 2;
const unsigned int ACTUATORSIZE = 1;

//PKTDEF CLASS
class PktDef {
private:
	//Contains the header, a command as the body, and CRC for tail
	struct CmdPacket {
		Header header;
		char* Data;
		char CRC;
	} CmdPkt;

	char *RawBuffer; //Store all data in PktDef instance to transmit over TCP/IP
	const int emptyPacket = 7;

	//Public member functions
public:
	PktDef();
	PktDef(char*);
	void SetCmd(CmdType);
	void SetBodyData(char*, int);
	void SetPktCount(int);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	char* GetBodyData();
	int GetPktCount();
	bool CheckCRC(char*, int);
	void CalcCRC();
	char *GenPacket();
};
#endif