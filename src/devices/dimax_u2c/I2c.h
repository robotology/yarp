
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

//#pragma once
#ifndef I2C_H
#define I2C_H
#include "i2cbridge.h"


class CI2c
{
public:
	CI2c(void);
	virtual ~CI2c(void);
	bool Read(PU2C_TRANSACTION pTransaction);
	bool Write(PU2C_TRANSACTION pTransaction);
	void PrintError();
private:
	U2C_RESULT m_Res;
	bool OpenDevice();
	void CloseDevice();
	HANDLE m_hDevice;
};
#endif
