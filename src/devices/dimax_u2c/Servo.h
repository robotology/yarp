/*
 * Copyright (C) 2005,2006 Mike P. Blow, Lars Olsson, Assif Mirza 
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */
#ifndef SERVO_H
#define SERVO_H

#include "I2c.h"
#include "i2cbridge.h"

#define MEMORY_ADDRESS_LENGTH 0x01
#define SLAVE_ADDRESS 0x61
#define MAX_DATA_LEN 256

class Servo  
{
public:
	enum result
	{
		rSuccess,
		rHelpRequested,
		rBadParameter,
		rInvalidCommand, // read or write expected

		rInvalidSlaveDeviceAddress,
		rAbsentSlaveDeviceAddress,
			
		rInvalidMemoryAddressLength,
		rDuplicatedMemoryAddressLengthLength,
		rAbsentMemoryAddressLength,

		rInvalidMemoryAddress,
		rAbsentMemoryAddress,

		rInvalidDataLength, 
		rDuplicatedDataLength,
		rAbsentDataLength,

		rInvalidData,
		rDataLengthMismatch,
		rAbsentData,
		rDataInReadCommand // have been found redundant data in read command
	};

	enum command
	{
		cRead,
		cWrite
	};

	Servo();
	virtual ~Servo();
	void init();
	void setPosition(int servoNumber, int servoPosition, int servoSpeed);
	int getPosition(int servoNumber);

private:
	CI2c I2C;
//	PU2C_TRANSACTION m_pRequest;
	unsigned short m_nDataLength; 
	//bool m_bDataLengthInited;

	BYTE m_nMemoryAddressLength;
	BYTE m_nSlaveDeviceAddress;
	DWORD m_nMemoryAddress;
	BYTE m_DataBuffer[MAX_DATA_LEN];
	BYTE hi_byte;
	BYTE lo_byte;

	//bool m_bMemoryAddressLengthInited;
	//bool m_bSlaveDeviceAddressInited;
	//bool m_bMemoryAddressInited;
	command m_Command;
	int m_nCurrData;
	int millis;

	/*enum
	{
		command_expected,
		data_expected,
	} m_State;*/

	result FillTransaction(PU2C_TRANSACTION pRequest, int servoNumber/*, BYTE m_dataBuffer[]*/);
	result FillSlaveDeviceAddress(PU2C_TRANSACTION pTransaction, BYTE slaveDeviceAddress);
	result FillMemoryAddressLength(PU2C_TRANSACTION pTransaction, BYTE memoryAddressLength);
	result FillMemoryAddress(PU2C_TRANSACTION pTransaction, BYTE servoNumber);
	//result CheckBuffer();
	result FillBuffer(PU2C_TRANSACTION pTransaction/*, BYTE m_dataBuffer[]*/);
};

#endif
