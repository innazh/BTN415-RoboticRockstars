#include "PktDef.h"

//Set header info and CRC to zero, set data to nullptr
PktDef::PktDef() {
	CmdPkt.header.PktCount = 0;

	CmdPkt.header.Ack = 0;
	CmdPkt.header.Arm = 0;
	CmdPkt.header.Claw = 0;
	CmdPkt.header.Drive = 0;
	CmdPkt.header.Length = 0;
	CmdPkt.header.Padding = 0;
	CmdPkt.header.Sleep = 0;
	CmdPkt.header.Status = 0;

	CmdPkt.Data = nullptr;
	CmdPkt.CRC = 0;

	RawBuffer = nullptr;
}

//Parse data from buffer and populate header, data and CRC
PktDef::PktDef(char* buffer) {

	RawBuffer = nullptr;

	//Get pktCount
	memcpy(&CmdPkt.header.PktCount, &buffer[0], sizeof(CmdPkt.header.PktCount));

	//Bitshifting to get commands
	CmdPkt.header.Drive = buffer[4] & 1;
	CmdPkt.header.Status = (buffer[4] >> 1) & 1;
	CmdPkt.header.Sleep = (buffer[4] >> 2) & 1;
	CmdPkt.header.Arm = (buffer[4] >> 3) & 1;
	CmdPkt.header.Claw = (buffer[4] >> 4) & 1;
	CmdPkt.header.Ack = (buffer[4] >> 5) & 1;
	CmdPkt.header.Padding = 0;

	//Get Length
	memcpy(&CmdPkt.header.Length, &buffer[5], sizeof(CmdPkt.header.Length));

	int sizeOfBodyData = CmdPkt.header.Length - emptyPacket;

	if (sizeOfBodyData > 0) {
		//Get data
		CmdPkt.Data = new char[sizeOfBodyData]; //allocate space for data
		memcpy(CmdPkt.Data, &buffer[6], sizeOfBodyData * sizeof(*CmdPkt.Data));

		//Get CRC
		memcpy(&CmdPkt.CRC, &buffer[6 + sizeOfBodyData * sizeof(*CmdPkt.Data)], sizeof(CmdPkt.CRC));
	}
	else
	{
		CmdPkt.Data = nullptr;
		memcpy(&CmdPkt.CRC, &buffer[6], sizeof(CmdPkt.CRC));
	}
}

//Set packet's command flag according to flag
//Switch statement is fairly self-explanatory
void PktDef::SetCmd(CmdType flag) {
	switch (flag) {
	case DRIVE:
		CmdPkt.header.Drive = 1;

		CmdPkt.header.Arm = 0;
		CmdPkt.header.Claw = 0;
		CmdPkt.header.Sleep = 0;
		CmdPkt.header.Status = 0;
		break;

	case SLEEP:
		CmdPkt.header.Sleep = 1;

		CmdPkt.header.Arm = 0;
		CmdPkt.header.Claw = 0;
		CmdPkt.header.Drive = 0;
		CmdPkt.header.Status = 0;
		break;

	case ARM:
		CmdPkt.header.Arm = 1;

		CmdPkt.header.Sleep = 0;
		CmdPkt.header.Claw = 0;
		CmdPkt.header.Drive = 0;
		CmdPkt.header.Status = 0;
		break;

	case CLAW:
		CmdPkt.header.Claw = 1;

		CmdPkt.header.Arm = 0;
		CmdPkt.header.Sleep = 0;
		CmdPkt.header.Drive = 0;
		CmdPkt.header.Status = 0;
		break;

	case ACK:
		CmdPkt.header.Ack = 1;
		break;

	default:
		//think of the default later
		std::cout << "Error: Invalid command" << std::endl;
	}
}

//Allocate packet's Body field to copy rawdata into buffer
void PktDef::SetBodyData(char* rawdata, int len) {
	if (CmdPkt.Data != nullptr) {
		delete[] CmdPkt.Data;
		CmdPkt.Data = nullptr;
	}
	//SetBodyData - needs to set the length in the packet header information as well.
	CmdPkt.header.Length = emptyPacket + len;

	CmdPkt.Data = new char[len];
	//CmdPkt.Data = rawdata;
	memcpy(CmdPkt.Data, rawdata, len);
}

//Set pktCount to count
void PktDef::SetPktCount(int count) {
	CmdPkt.header.PktCount = count;
}

//Return active command
//GetCmd - should not return ACK as a valid command flag
CmdType PktDef::GetCmd() {
	CmdType cmd;

	if (CmdPkt.header.Drive == 1) {
		cmd = DRIVE;
	}
	else if (CmdPkt.header.Status == 1) {
		cmd = STATUS;
	}
	else if (CmdPkt.header.Sleep == 1) {
		cmd = SLEEP;
	}
	else if (CmdPkt.header.Arm == 1) {
		cmd = ARM;
	}
	else if (CmdPkt.header.Claw == 1) {
		cmd = CLAW;
	}

	return cmd;
}

//Returns false if ack = 0, returns true otherwise
bool PktDef::GetAck() {
	if (CmdPkt.header.Ack == 0) {
		return false;
	}
	else {
		return true;
	}
}

//Return packet Length in bytes
int PktDef::GetLength() {
	//return sizeof(CmdPkt.header.Length);
	return CmdPkt.header.Length;
}

//Returns a pointer to the body packet
char* PktDef::GetBodyData() {
	return CmdPkt.Data;
}

//Returns value of pktCount
int PktDef::GetPktCount() {
	return CmdPkt.header.PktCount;
}

//Calculate CRC based on a pointer to a RAW data buffer, 
//and size of packet (in bytes) located in the buffer
//Return true if CRC matches the CRC of the buffer packet
bool PktDef::CheckCRC(char* buffer, int size) {
	
	// Set the return flag to false
	bool ret = false;
	int numOnes = 0;

	// Create a temp Buffer
	char  *buffercrc = buffer;
	
	// Loop for each byte
	for (int i = 0; i < size - 1; i++)
	{
		// Loop for each bit
		for (int x = 0; x < 8; x++)
			// Check the current bit
			if( (*buffercrc >> x) & 1)
				numOnes++;

		// Increase the size of the buffer by one byte.
		buffercrc += sizeof(char);
	}

	// Check that CRC is correct
	if (numOnes == (int)*buffercrc) ret = true;
	
	return ret;

}

//Calculate CRC and set packet's CRC parameter
void PktDef::CalcCRC() {
	int numOnes = 0;
	int sizeOfBodyData = CmdPkt.header.Length - emptyPacket;

	int loopIntFor = 8 * sizeof(CmdPkt.header.PktCount);
	
	//num of 1s in PktCount and Length
	for (int i = 0; i < loopIntFor; i++)
	{
		numOnes += (CmdPkt.header.PktCount >> i) & 1;
	}

	//num of 1s in bit field flags 
	if (CmdPkt.header.Status & 1) { numOnes++; }
	if (CmdPkt.header.Ack & 1) { numOnes++; }
	if (CmdPkt.header.Drive & 1) { numOnes++; }
	else if (CmdPkt.header.Arm & 1) { numOnes++; }
	else if (CmdPkt.header.Claw & 1) { numOnes++; }
	else if (CmdPkt.header.Sleep & 1) { numOnes++; }

	//num of 1s in Length
	for (int i = 0; i < 8; i++)
	{
		numOnes += (CmdPkt.header.Length >> i) & 1;
	}

	//num of 1s in body
	if (sizeOfBodyData > 0) {

		for (int out = 0; out < sizeOfBodyData; out++)
		{
			for (int i = 0; i < 8; i++)
			{
				numOnes += (CmdPkt.Data[out] >> i) & 1;
			}
		}
	}
	//Set CRC value
	CmdPkt.CRC = numOnes;
}

//Allocate RawBuffer, transfer data from packet into RawBuffer
//Return address of RawBuffer

/*GenPacket - needs to clean up RxBuffer before proceeding.  
You have a potential memory leak if the object is reused.   
Data should be allocated based on the header information as well.  
Use length.*/

char *PktDef::GenPacket() {

	int sizeOfBodyData = CmdPkt.header.Length - emptyPacket;

	if (RawBuffer != nullptr) {
		delete[] RawBuffer;
		RawBuffer = nullptr;
	}

	RawBuffer = new char[CmdPkt.header.Length];

	//Copy PktCount from header
	memcpy(&RawBuffer[0], &CmdPkt.header.PktCount, sizeof(CmdPkt.header.PktCount));

	//Copy flags from header
	char * flags = (char*)&CmdPkt.header.PktCount;//notsuure
	RawBuffer[4] = *flags;//notsuure

	//Copy Length from header
	memcpy(&RawBuffer[5], &CmdPkt.header.Length, sizeof(CmdPkt.header.Length));

	if (sizeOfBodyData > 0)
	{
		//Copy Data (body) and CRC
		memcpy(&RawBuffer[6], CmdPkt.Data, sizeOfBodyData * sizeof(*CmdPkt.Data));
		memcpy(&RawBuffer[6 + sizeOfBodyData * sizeof(*CmdPkt.Data)], &CmdPkt.CRC, sizeof(CmdPkt.CRC));
	}
	else
	{
		CmdPkt.Data = nullptr;
		memcpy(&RawBuffer[6], &CmdPkt.CRC, sizeof(CmdPkt.CRC));
	}

	return RawBuffer;
}