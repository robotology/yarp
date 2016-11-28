
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
#ifndef U2C_COMMON_FUNC_H
#define U2C_COMMON_FUNC_H

HANDLE _stdcall OpenU2C();
bool _stdcall CheckDriverVersion(HANDLE hDevice);
bool _stdcall ConfigureI2cSpeed(HANDLE hDevice);
void _stdcall ShowNotImplementedMessage();

#endif
