/*
 * Copyright (C) 2005,2006 Mike P. Blow, Lars Olsson, Assif Mirza 
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <iostream>
#include "Servo.h"

using namespace std;

#define HI_BYTE(millis) (millis >> 8)
#define LO_BYTE(millis) (millis & 0x00ff)
#define MILLIS(hi_byte, lo_byte) ((hi_byte << 8) | lo_byte)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Servo::Servo()
: m_nCurrData(0)
{
	CI2c I2C;
	printf("Servo constructor called\n");
}

Servo::~Servo()
{
	//? DESTRUCT I2C?
}

Servo servo;

void Servo::init(){
	//set up U2C board
	HANDLE handle	= U2C_OpenDevice(0x00);
	U2C_SetI2cFreq(handle,U2C_I2C_FREQ_STD);
	U2C_SetClockSynch(handle, 1);
	printf("init servo called\n");		
}


void Servo::setPosition(int servoNumber, int servoPosition, int servoSpeed){
	int lopos = LO_BYTE(servoPosition);
	int hipos = HI_BYTE(servoPosition);

	printf( "C++ Servo::setPosition servo=%d position=%d (hi=%x,lo=%x) speed=%d\n",servoNumber, servoPosition, hipos,lopos, servoSpeed);
	//printf( "lopos=%x\n",lopos);
	//printf( "hipos=%x\n",hipos);*/


	if (m_nCurrData == MAX_DATA_LEN){
		I2C.PrintError();//"Servo::setPosition:m_nCurrData == MAX_DATA_LEN");
		return;
	}

	m_DataBuffer[0]=(BYTE)servoSpeed;
	m_DataBuffer[1]=(BYTE)lopos;
	m_DataBuffer[2]=(BYTE)hipos;
	m_nCurrData = 3;

	U2C_TRANSACTION Transaction;
	Servo::result res;

	m_Command = cWrite;
	res = FillTransaction(&Transaction, servoNumber);
	if (res == Servo::rSuccess)
	{
		if (!I2C.Write(&Transaction))
			I2C.PrintError();//"Servo::setPosition:!I2C.Write(&Transaction)");
	}
}


int Servo::getPosition(int servoNumber){
	Servo::result res;
	U2C_TRANSACTION Transaction;
	res = FillTransaction(&Transaction, servoNumber);
	if (res != Servo::rSuccess)
	{
		I2C.PrintError();//"Servo::getPosition:res != Servo::rSuccess");
		return res;
	}
	
	if (!I2C.Read(&Transaction))
	{
		I2C.PrintError();//"Servo::getPosition:!I2C.Read(&Transaction)");
	}
	return MILLIS(Transaction.Buffer[2],Transaction.Buffer[1]);
}





Servo::result Servo::FillTransaction(PU2C_TRANSACTION pRequest, int servoNumber/*, BYTE m_dataBuffer[]*/)
{
	ZeroMemory(pRequest, sizeof(U2C_TRANSACTION));
	result rValue;

	if (rSuccess != (rValue = FillSlaveDeviceAddress(pRequest, SLAVE_ADDRESS)))
		return rValue;
	if (rSuccess != (rValue = FillMemoryAddressLength(pRequest, MEMORY_ADDRESS_LENGTH)))
		return rValue;
	if (rSuccess != (rValue = FillMemoryAddress(pRequest, (BYTE)servoNumber)))
		return rValue;
	//if (rSuccess != (rValue = CheckBuffer()))
	//	return rValue;
	if (rSuccess != (rValue = FillBuffer(pRequest/*, m_DataBuffer*/)))
		return rValue;

	return rSuccess;
}

Servo::result Servo::FillSlaveDeviceAddress(PU2C_TRANSACTION pTransaction, BYTE slaveDeviceAddress)
{
	pTransaction->nSlaveDeviceAddress = slaveDeviceAddress;
	return rSuccess;
}

Servo::result Servo::FillMemoryAddressLength(PU2C_TRANSACTION pTransaction, BYTE memoryAddressLength)
{
	pTransaction->nMemoryAddressLength = memoryAddressLength;
	return rSuccess;
}

Servo::result Servo::FillMemoryAddress(PU2C_TRANSACTION pTransaction, BYTE servoNumber)
{
	pTransaction->nMemoryAddress = servoNumber;
	return rSuccess;
}

/*Servo::result Servo::CheckBuffer()
{
	switch(m_Command)
	{
	case cRead:
		if (!m_bDataLengthInited)
			return rAbsentDataLength;
		if (m_nCurrData != 0)
			return rDataInReadCommand;
		break;
	case cWrite:
		if ((m_bDataLengthInited) && (m_nDataLength != m_nCurrData))
			return rDataLengthMismatch;
		if (m_nCurrData == 0)
			return rAbsentData;
		break;
	default:
		return rInvalidCommand;
	}
	return rSuccess;
}*/

Servo::result Servo::FillBuffer(PU2C_TRANSACTION pTransaction/*, BYTE m_dataBuffer[]*/)
{
	switch (m_Command)
	{
	case cRead:
		pTransaction->nBufferLength = m_nDataLength;
		break;
	case cWrite:
		pTransaction->nBufferLength = m_nCurrData;
		break;
	default:
		return rInvalidCommand;
	}
	CopyMemory(pTransaction->Buffer, m_DataBuffer, m_nCurrData);
	return rSuccess;

	/*write case only
	pTransaction->nBufferLength = m_nCurrData;
	CopyMemory(pTransaction->Buffer, m_DataBuffer, m_nCurrData);
	return rSuccess;*/
}

