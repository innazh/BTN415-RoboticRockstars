#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "PktDef.h"
using namespace std;

int main()
{
	std::ofstream ofs("Output.txt");
	if (!ofs.is_open())
		std::cout << "Failed to open output file.   Screenshots are required" << std::endl;

	MotorBody DriveCmd;
	DriveCmd.Direction = FORWARD;
	DriveCmd.Duration = 20;

	ActuatorBody myAction;

	PktDef TestPkt;
	char *ptr;

	//Testing the PktDef creation interface based on user input
	TestPkt.SetCmd(DRIVE);
	TestPkt.SetBodyData((char *)&DriveCmd, 2);
	TestPkt.SetPktCount(1);
	TestPkt.CalcCRC();
	ptr = TestPkt.GenPacket();

	ofs << showbase
		<< internal
		<< setfill('0');

	char result[9] = { 0x01, 0000, 0000, 0000, 0x04, 0x09, 0x01, 0x14, 0x07 };
	bool bPass = true;
	for (int x = 0; x < (int)TestPkt.GetLength(); x++)
	{
		if ((unsigned int)*ptr != result[x])
			bPass = false;

		ofs << hex << setw(4) << (unsigned int)*(ptr++) << ", ";
	}
	if (bPass)
		ofs << "-- PASSED" << std::endl;
	else
		ofs << "-- FAILED" << std::endl;

	ofs << endl;
	TestPkt.SetCmd(ACK);
	TestPkt.SetPktCount(2);
	TestPkt.CalcCRC();
	ptr = TestPkt.GenPacket();

	char result2[9] = { 0x02, 0000, 0000, 0000, 0x24, 0x09, 0x01, 0x14, 0x08 };
	bPass = true;
	for (int x = 0; x < (int)TestPkt.GetLength(); x++)
	{
		if ((unsigned int)*ptr != result2[x])
			bPass = false;

		ofs << hex << setw(4) << (unsigned int)*(ptr++) << ", ";
	}
	if (bPass)
		ofs << "-- PASSED" << std::endl;
	else
		ofs << "-- FAILED" << std::endl;

	ofs << endl;
	TestPkt.SetCmd(ARM);
	myAction.Action = UP;
	TestPkt.SetBodyData((char *)&myAction, 1);
	TestPkt.SetPktCount(3);
	TestPkt.CalcCRC();
	ptr = TestPkt.GenPacket();

	char result3[8] = { 0x03, 0000, 0000, 0000, 0x10, 0x08, 0x05, 0x06 };
	bPass = true;
	for (int x = 0; x < (int)TestPkt.GetLength(); x++)
	{
		if ((unsigned int)*ptr != result3[x])
			bPass = false;

		ofs << hex << setw(4) << (unsigned int)*(ptr++) << ", ";
	}
	if (bPass)
		ofs << "-- PASSED" << std::endl;
	else
		ofs << "-- FAILED" << std::endl;

	ofs << endl;
	TestPkt.SetCmd(CLAW);
	myAction.Action = OPEN;
	TestPkt.SetBodyData((char *)&myAction, 1);
	TestPkt.SetPktCount(4);
	TestPkt.CalcCRC();
	ptr = TestPkt.GenPacket();

	char result4[8] = { 0x04, 0000, 0000, 0000, 0x08, 0x08, 0x07, 0x06 };
	bPass = true;
	for (int x = 0; x < (int)TestPkt.GetLength(); x++)
	{
		if ((unsigned int)*ptr != result4[x])
			bPass = false;

		ofs << hex << setw(4) << (unsigned int)*(ptr++) << ", ";
	}
	if (bPass)
		ofs << "-- PASSED" << std::endl;
	else
		ofs << "-- FAILED" << std::endl;

	ofs << endl << noshowbase << dec;


	//Testing the PktDef creation interface based on an RxBuffer of RAW Data
	char buffer[9] = { 0x01, 0000, 0000, 0000, 0x01, 0x09, 0x01, 0x14, 0x07 };
	PktDef RxPkt(buffer);
	if(RxPkt.GetCmd() == SLEEP)
		ofs << "CommandID:  " << RxPkt.GetCmd() << " -- PASSED" << endl;
	else
		ofs << "CommandID:  " << RxPkt.GetCmd() << " -- FAILED" << endl;

	if(RxPkt.GetPktCount() == 1)
		ofs << "PktCount:   " << RxPkt.GetPktCount() << " -- PASSED" << endl;
	else
		ofs << "PktCount:   " << RxPkt.GetPktCount() << " -- FAILED" << endl;

	if(RxPkt.GetLength() == 9)
		ofs << "Pkt Length: " << RxPkt.GetLength() << " -- PASSED" << endl;
	else
		ofs << "Pkt Length: " << RxPkt.GetLength() << " -- FAILED" << endl;

	bPass = true;
	ptr = RxPkt.GetBodyData();
	ofs << showbase << hex;
	ofs << "Byte 1 " << (int)*ptr << endl;
	if ((int)*ptr++ != 0x01)
		bPass = false;
	
	ofs << "Byte 2 " << (int)*ptr << endl;
	if ((int)*ptr != 0x14)
		bPass = false;

	if(bPass)
		ofs << "Body Data -- PASSED" << endl;
	else
		ofs << "Body Data -- FAILED" << endl;

	//Testing the PktDef CRC Calculation
	
	//Test #1 -- Validation of a Correct CRC Calculation
	char CRCGoodTest[9] = { 0x02, 0x00, 0x00, 0x00, 0x02, 0x09, 0x11, 0x24, 0x08 };
	bool Result = RxPkt.CheckCRC(CRCGoodTest, 9);
	if (Result)
		ofs << "CRC Test #1 -- PASSED" << endl;
	else
		ofs << "CRC Test #1 -- FAILED" << endl;

	//Test #2 -- Validawtion of an Incorrect CRC Calculation
	char CRCBadTest[9] = { 0x02, 0x00, 0x00, 0x00, 0x02, 0x09, 0x11, 0x24, 0x17 };
	Result = RxPkt.CheckCRC(CRCBadTest, 9);
	if (Result)
		ofs << "CRC Test #1 -- FAILED" << endl;
	else
		ofs << "CRC Test #1 -- PASSED" << endl;


	ofs.close();

	return 1;
}
