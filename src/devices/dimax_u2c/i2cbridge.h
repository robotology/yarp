
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

#ifndef I2C_BRIDGE_H_10DF0DED_E85F_4f14_88D9_610BEA2211F7

#define I2C_BRIDGE_H_10DF0DED_E85F_4f14_88D9_610BEA2211F7

#ifdef __linux__
/*
* Compatibility stuff
*/
#include <sys/types.h>
typedef int         BOOL;        
#define TRUE 1
#define FALSE 0
typedef u_int8_t    BYTE;
typedef u_int8_t    UCHAR;
typedef u_int16_t   WORD; 
typedef u_int32_t   DWORD; 
typedef u_int32_t   ULONG; 
typedef void*       HANDLE;
#define _stdcall	/* nothing */
#define INVALID_HANDLE_VALUE ((HANDLE)(-1))
#endif

#ifdef WIN32
//#include <windows.h>
//#include <winbase.inl>
//#include <windef.h>

// For the base types (LONG_PTR)
#include <basetsd.h>

// From WinDef.h
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef unsigned char UCHAR;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
#define FALSE               0
#define TRUE                1
typedef void *HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

// from MAPIWin.h
#ifndef ZeroMemory
#define	ZeroMemory(pb,cb)			memset((pb),0,(cb))
#define FillMemory(pb,cb,b)			memset((pb),(b),(cb))
#define CopyMemory(pbDst,pbSrc,cb)	do								\
									{								\
										size_t _cb = (size_t)(cb);	\
										if (_cb)					\
											memcpy(pbDst,pbSrc,_cb);\
									} while (FALSE)
#define MoveMemory(pbDst,pbSrc,cb)	memmove((pbDst),(pbSrc),(cb))
#endif

#endif /* WIN32*/

#pragma pack(push, 1)

typedef enum
{
	U2C_SUCCESS = 0,                            /*!< API Function was successful */
	U2C_BAD_PARAMETER = 1,                      /*!< API Function got bad input parameter */
	U2C_HARDWARE_NOT_FOUND = 2,                 /*!< U2C Device not found */
	U2C_SLAVE_DEVICE_NOT_FOUND = 3,             /*!< I2C Slave device not found */
	U2C_TRANSACTION_FAILED = 4,                 /*!< I2C or SPI transaction failed */
	U2C_SLAVE_OPENNING_FOR_WRITE_FAILED = 5,    /*!< I2C Slave did not acknowledge write slave address */
	U2C_SLAVE_OPENNING_FOR_READ_FAILED = 6,     /*!< I2C Slave did not acknowledge read slave address */
	U2C_SENDING_MEMORY_ADDRESS_FAILED = 7,      /*!< I2C Slave did not acknowledge internal address output */
	U2C_SENDING_DATA_FAILED = 8,                /*!< I2C Slave did not acknowledge data output */
	U2C_NOT_IMPLEMENTED = 9,                    /*!< Operation is not implemented by U2C API */
	U2C_NO_ACK = 10,                            /*!< Got no ACK from Slave */
	U2C_DEVICE_BUSY = 11,                       /*!< U2C Device Busy */
	U2C_MEMORY_ERROR = 12,                      /*!< Memory operation (like malloc) failed */
	U2C_UNKNOWN_ERROR = 13,                     /*!< Undocumented error */
	U2C_I2C_CLOCK_SYNCH_TIMEOUT = 14,			/*!< I2C Slave did not release SCL line */
} U2C_RESULT;


/**
* \brief I2C Transaction Descriptor
*
* This structure contains all required information to perform I2C transaction
*/
typedef struct _U2C_TRANSACTION
{
	BYTE nSlaveDeviceAddress;   /*!< I2C Slave device address in 7 bit format */
	BYTE nMemoryAddressLength;  /*!< Slave Device Internal address size */
	DWORD nMemoryAddress;       /*!< Slave Device Internal address (Memory address) */
	WORD nBufferLength;         /*!< Transaction buffer length. Can be from 1 up to 256 */
	BYTE Buffer[256];           /*!< Transaction Buffer */
} U2C_TRANSACTION, *PU2C_TRANSACTION;

/**
* \brief I2C Transactions pack element
*/ 
typedef struct _U2C_TRANSACTION_PACK
{
	BOOL rw;                    /*!< 1-Read 0-Write */
	U2C_TRANSACTION tr;         /*!< Transaction descriptor _U2C_TRANSACTION */
	int rc;                     /*!< Transaction result */ 
} U2C_TRANSACTION_PACK, *PU2C_TRANSACTION_PACK;

// I2C bus frequency values:
#define U2C_I2C_FREQ_FAST   0
#define U2C_I2C_FREQ_STD    1
#define U2C_I2C_FREQ_83KHZ  2
#define U2C_I2C_FREQ_71KHZ  3
#define U2C_I2C_FREQ_62KHZ  4
#define U2C_I2C_FREQ_50KHZ  6
#define U2C_I2C_FREQ_25KHZ  16
#define U2C_I2C_FREQ_10KHZ  46
#define U2C_I2C_FREQ_5KHZ   96
#define U2C_I2C_FREQ_2KHZ   242


// SPI bus frequency values:
#define U2C_SPI_FREQ_200KHZ  0
#define U2C_SPI_FREQ_100KHZ  1
#define U2C_SPI_FREQ_83KHZ  2
#define U2C_SPI_FREQ_71KHZ  3
#define U2C_SPI_FREQ_62KHZ  4
#define U2C_SPI_FREQ_50KHZ  6
#define U2C_SPI_FREQ_25KHZ  16
#define U2C_SPI_FREQ_10KHZ  46
#define U2C_SPI_FREQ_5KHZ   96
#define U2C_SPI_FREQ_2KHZ   242



typedef struct _U2C_SLAVE_ADDR_LIST
{
	BYTE nDeviceNumber;
	BYTE List[256];
} U2C_SLAVE_ADDR_LIST, *PU2C_SLAVE_ADDR_LIST;

typedef struct _U2C_VERSION_INFO
{
	WORD MajorVersion;
	WORD MinorVersion;
} U2C_VERSION_INFO, *PU2C_VERSION_INFO;

typedef enum 
{
	LS_RELEASED,
	LS_DROPPED_BY_I2C_BRIDGE,
	LS_DROPPED_BY_SLAVE,
	LS_RAISED_BY_I2C_BRIDGE,
} U2C_LINE_STATE;


#pragma pack(pop)
// I2CBridge Board Initialization Routines
BYTE _stdcall U2C_GetDeviceCount();
U2C_RESULT _stdcall U2C_GetSerialNum(HANDLE hDevice, long* pSerialNum); 
U2C_RESULT _stdcall U2C_IsHandleValid(HANDLE hDevice); 
HANDLE _stdcall U2C_OpenDevice(BYTE nDevice);
HANDLE _stdcall U2C_OpenDeviceBySerialNum(long nSerialNum); 
U2C_RESULT _stdcall U2C_CloseDevice(HANDLE hDevice);
U2C_RESULT _stdcall U2C_GetFirmwareVersion(HANDLE hDevice, PU2C_VERSION_INFO pVersion);
U2C_RESULT _stdcall U2C_GetDriverVersion(HANDLE hDevice, PU2C_VERSION_INFO pVersion);
U2C_VERSION_INFO _stdcall U2C_GetDllVersion(); 

// I2C high level and configuration routines
U2C_RESULT _stdcall U2C_SetI2cFreq(HANDLE hDevice, BYTE Frequency);
U2C_RESULT _stdcall U2C_GetI2cFreq(HANDLE hDevice, BYTE *pFrequency);
U2C_RESULT _stdcall U2C_SetClockSynch(HANDLE hDevice, BOOL Enable);
U2C_RESULT _stdcall U2C_GetClockSynch(HANDLE hDevice, BOOL *pEnable);
U2C_RESULT _stdcall U2C_Read(HANDLE hDevice, PU2C_TRANSACTION pTransaction);
U2C_RESULT _stdcall U2C_Write(HANDLE hDevice, PU2C_TRANSACTION pTransaction);
U2C_RESULT _stdcall U2C_RW_Pack(HANDLE hDevice, PU2C_TRANSACTION_PACK pTransaction, int count);
U2C_RESULT _stdcall U2C_ScanDevices(HANDLE hDevice, PU2C_SLAVE_ADDR_LIST pList);

// I2C low level routines
U2C_RESULT _stdcall U2C_Start(HANDLE hDevice);
U2C_RESULT _stdcall U2C_RepeatedStart(HANDLE hDevice);
U2C_RESULT _stdcall U2C_Stop(HANDLE hDevice);
U2C_RESULT _stdcall U2C_PutByte(HANDLE hDevice, BYTE Data);
U2C_RESULT _stdcall U2C_GetByte(HANDLE hDevice, BYTE *pData);
U2C_RESULT _stdcall U2C_PutByteWithAck(HANDLE hDevice, BYTE Data);
U2C_RESULT _stdcall U2C_GetByteWithAck(HANDLE hDevice, BYTE *pData, BOOL bAck);
U2C_RESULT _stdcall U2C_PutAck(HANDLE hDevice, BOOL bAck);
U2C_RESULT _stdcall U2C_GetAck(HANDLE hDevice);

// I2c wire level routines
U2C_RESULT _stdcall U2C_ReadScl(HANDLE hDevice, U2C_LINE_STATE *pState);
U2C_RESULT _stdcall U2C_ReadSda(HANDLE hDevice, U2C_LINE_STATE *pState);
U2C_RESULT _stdcall U2C_ReleaseScl(HANDLE hDevice);
U2C_RESULT _stdcall U2C_ReleaseSda(HANDLE hDevice);
U2C_RESULT _stdcall U2C_DropScl(HANDLE hDevice);
U2C_RESULT _stdcall U2C_DropSda(HANDLE hDevice);

// GPIO routines
U2C_RESULT _stdcall U2C_SetIoDirection(HANDLE hDevice, ULONG Value, ULONG Mask);
U2C_RESULT _stdcall U2C_GetIoDirection(HANDLE hDevice, ULONG *pValue);
U2C_RESULT _stdcall U2C_IoWrite(HANDLE hDevice, ULONG Value, ULONG Mask);
U2C_RESULT _stdcall U2C_IoRead(HANDLE hDevice, ULONG *pValue);
U2C_RESULT _stdcall U2C_SetSingleIoDirection(HANDLE hDevice, ULONG IoNumber, BOOL bOutput);
U2C_RESULT _stdcall U2C_GetSingleIoDirection(HANDLE hDevice, ULONG IoNumber, BOOL *pbOutput);
U2C_RESULT _stdcall U2C_SingleIoWrite(HANDLE hDevice, ULONG IoNumber, BOOL Value);
U2C_RESULT _stdcall U2C_SingleIoRead(HANDLE hDevice, ULONG IoNumber, BOOL *pValue);

// SPI configuration routines
U2C_RESULT _stdcall U2C_SpiSetConfig(HANDLE hDevice, BYTE CPOL, BYTE CPHA);
U2C_RESULT _stdcall U2C_SpiGetConfig(HANDLE hDevice, BYTE *pCPOL, BYTE *pCPHA);
U2C_RESULT _stdcall U2C_SpiSetConfigEx(HANDLE hDevice, DWORD Config);
U2C_RESULT _stdcall U2C_SpiGetConfigEx(HANDLE hDevice, DWORD *pConfig);
U2C_RESULT _stdcall U2C_SpiSetFreq(HANDLE hDevice, BYTE Frequency);
U2C_RESULT _stdcall U2C_SpiGetFreq(HANDLE hDevice, BYTE *pFrequency);
U2C_RESULT _stdcall U2C_SpiReadWrite(HANDLE hDevice, BYTE *pOutBuffer, BYTE *pInBuffer, WORD Length);
U2C_RESULT _stdcall U2C_SpiWrite(HANDLE hDevice, BYTE *pOutBuffer, WORD Length);
U2C_RESULT _stdcall U2C_SpiRead(HANDLE hDevice, BYTE *pInBuffer, WORD Length);

#endif //I2C_BRIDGE_H_10DF0DED_E85F_4f14_88D9_610BEA2211F7
