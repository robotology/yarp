
// a summary of the licence statement below, added by paulfitz
// CopyPolicy: GPL

// Copyright (C)2004 Dimax ( http://www.xdimax.com )
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include <iostream>
//#include "stdafx.h"
#include "I2c.h"
#include "i2cbridge.h"
#include "u2c_common_func.h"

//using namespace std;


CI2c::CI2c(void)
{
}

CI2c::~CI2c(void)
{
}

bool CI2c::OpenDevice()
{
	BYTE DevCount = U2C_GetDeviceCount();
	if (DevCount == 0)
	{
		m_Res = U2C_HARDWARE_NOT_FOUND;
		return false;
	}
	m_hDevice = U2C_OpenDevice(0);
	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		m_Res = U2C_HARDWARE_NOT_FOUND;
		return false;
	}
	return true;
}

void CI2c::CloseDevice()
{
	U2C_CloseDevice(m_hDevice);
	m_hDevice = INVALID_HANDLE_VALUE;
}

bool CI2c::Read(PU2C_TRANSACTION pTransaction)
{
	if (!OpenDevice())
		return false;
	m_Res =	U2C_Read(m_hDevice, pTransaction);
	CloseDevice();
	return m_Res == U2C_SUCCESS;
}

bool CI2c::Write(PU2C_TRANSACTION pTransaction)
{
	if (!OpenDevice())
		return false;
	m_Res = U2C_Write(m_hDevice, pTransaction);
	CloseDevice();
	return m_Res == U2C_SUCCESS;
}

void CI2c::PrintError()
{
	/*std::string results[] =
	{
		"U2C_SUCCESS",
		"U2C_BAD_PARAMETER",
		"U2C_HARDWARE_NOT_FOUND",
		"U2C_SLAVE_DEVICE_NOT_FOUND",
		"U2C_TRANSACTION_FAILED",
		"U2C_SLAVE_OPENNING_FOR_WRITE_FAILED",
		"U2C_SLAVE_OPENNING_FOR_READ_FAILED",
		"U2C_SENDING_MEMORY_ADDRESS_FAILED",
		"U2C_SENDING_DATA_FAILED",
		"U2C_NOT_IMPLEMENTED",
		"U2C_NO_ACK",
		"U2C_UNKNOWN_ERROR",
	};*/
	//cout << "I2C error: " /*<< results[m_Res]*/ << std::endl;
	printf( "I2C ERROR \n");
}
