/*============================================

Copyright: (c) 2007 Jeffrey Kerr LLC
CopyPolicy: 3-clause BSD

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

============================================ */

#include <stdio.h>

#include "sio_util.h"
//---------------------------------------------------------------------------
// Global variables
int printerrors = 1;
//---------------------------------------------------------------------------
 int SimpleMsgBox(char *msgstr)
{
return MessageBox(NULL, msgstr,"",MB_TASKMODAL | MB_SETFOREGROUND);
}
//---------------------------------------------------------------------------
 void ErrorPrinting(int f)
{
printerrors = f;
}
//---------------------------------------------------------------------------
 int ErrorMsgBox(char *msgstr)
{
if (printerrors)
  return MessageBox(NULL, msgstr,"",MB_TASKMODAL | MB_SETFOREGROUND);
else return(0);
}
//---------------------------------------------------------------------------
//Opens "COM1:" thru "COM4:", returns a handle to be used by other
//SIO operations.  Sets up read and write timeouts.
//*** Add parameter for baud rate ***
 HANDLE SioOpen(char *name, unsigned int baudrate)
{
BOOL RetStat;
COMMCONFIG cc;
COMMTIMEOUTS ct;
HANDLE ComHandle;
DWORD winrate;
char msgstr[50];

//Open COM port as a file
ComHandle = CreateFile(name, GENERIC_READ | GENERIC_WRITE ,0, NULL,
			           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);


while (TRUE)
	{
	if (ComHandle == INVALID_HANDLE_VALUE)
		{
        sprintf(msgstr,"%s failed to open",name);
		ErrorMsgBox(msgstr);
        break;
    	}

    switch (baudrate) {
		case 9600: 		winrate = CBR_9600; break;
		case 19200: 	winrate = CBR_19200; break;
		case 38400: 	winrate = CBR_38400; break;
		case 57600: 	winrate = CBR_57600; break;
		case 115200: 	winrate = CBR_115200; break;
		case 230400: 	winrate = 230400; break;
    	default:		ErrorMsgBox("Baud rate not supported - using default of 19200");
    					winrate = CBR_19200;
    	}

    //Fill in COM port config. structure & set config.
    cc.dwSize = sizeof(DCB) + sizeof(WCHAR) + 20;
    cc.wVersion = 1;

    cc.dcb.DCBlength = sizeof(DCB);
    cc.dcb.BaudRate = winrate;
    cc.dcb.fBinary = 1;
    cc.dcb.fParity = 0;
    cc.dcb.fOutxCtsFlow = 0;
    cc.dcb.fOutxDsrFlow = 0;
    cc.dcb.fDtrControl = DTR_CONTROL_DISABLE;
    cc.dcb.fDsrSensitivity = 0;
    cc.dcb.fTXContinueOnXoff = 0;
    cc.dcb.fOutX = 0;
    cc.dcb.fInX = 0;
    cc.dcb.fErrorChar = 0;
    cc.dcb.fNull = 0;
    cc.dcb.fRtsControl = RTS_CONTROL_DISABLE;
    cc.dcb.fAbortOnError = 0;
    cc.dcb.XonLim = 100;
    cc.dcb.XoffLim = 100;
    cc.dcb.ByteSize = 8;
    cc.dcb.Parity = NOPARITY;
    cc.dcb.StopBits = ONESTOPBIT;
    cc.dcb.XonChar = 'x';
    cc.dcb.XoffChar = 'y';
    cc.dcb.ErrorChar = 0;
    cc.dcb.EofChar = 0;
    cc.dcb.EvtChar = 0;

    cc.dwProviderSubType = PST_RS232;
    cc.dwProviderOffset = 0;
    cc.dwProviderSize = 0;

    RetStat = SetCommConfig(ComHandle, &cc, sizeof(cc));
	if (RetStat == 0)
    	{
    	ErrorMsgBox("Failed to set COMM configuration");
        break;
        }

    //Set read/write timeout values for the file
    ct.ReadIntervalTimeout = 0;  		//ignore interval timing
    ct.ReadTotalTimeoutMultiplier = 2; 	//2 msec per char
    ct.ReadTotalTimeoutConstant = 50;  		//plus add'l 50 msec
    ct.WriteTotalTimeoutMultiplier = 2;	//Set max time per char written
    ct.WriteTotalTimeoutConstant = 50;	//plus additional time

	RetStat = SetCommTimeouts(ComHandle, &ct);
    if (RetStat == 0)
    	{
        ErrorMsgBox("Failed to set Comm timeouts");
        break;
        }

    break;
	}

return(ComHandle);
}
//---------------------------------------------------------------------------
//Change the baud rate to the specified values.  Valid rates are:
//9600, 19200, 38400, 57600, 115200.  Returns TRUE on success.
 BOOL SioChangeBaud(HANDLE ComPort, unsigned int baudrate)
{
BOOL RetStat;
DWORD winrate;
DCB cs;

RetStat = GetCommState(ComPort, &cs);
if (RetStat == false) return RetStat;
switch (baudrate) {
	case 9600: 		winrate = CBR_9600; break;
	case 19200: 	winrate = CBR_19200; break;
	case 38400: 	winrate = CBR_38400; break;
	case 57600: 	winrate = CBR_57600; break;
	case 115200: 	winrate = CBR_115200; break;
	case 230400: 	winrate = 230400; break;
    default:		ErrorMsgBox("Baud rate not supported");
    				return false;
    }
cs.BaudRate = winrate;
RetStat = SetCommState(ComPort, &cs);
if (RetStat == false) return RetStat;
return true;
}

//---------------------------------------------------------------------------
//Write out N chars to the comport, returns only after chas have been sent
//return 0 on failure, non-zero on success
 BOOL SioPutChars(HANDLE ComPort, char *stuff, int n)
{
BOOL RetStat;
DWORD nums;

RetStat = WriteFile(ComPort, stuff,n, &nums, NULL);
if (RetStat == 0) ErrorMsgBox("SioPutChars failed");
return RetStat;
}
//---------------------------------------------------------------------------
//Read n chars into the array stuff (not null terminated)
//Function returns the number of chars actually read.
 DWORD SioGetChars(HANDLE ComPort, char *stuff, int n)
{
BOOL RetStat;
DWORD numread;

RetStat = ReadFile(ComPort, stuff, n, &numread, NULL);
if (RetStat == 0) ErrorMsgBox("SioReadChars failed");

return numread;
}
//---------------------------------------------------------------------------
//Returns the number of chars in a port's input buffer
 DWORD SioTest(HANDLE ComPort)
{
COMSTAT cs;
DWORD Errors;
BOOL RetStat;

RetStat = ClearCommError(ComPort, &Errors, &cs);
if (RetStat == 0) ErrorMsgBox("SioTest failed");
return cs.cbInQue;
}
//---------------------------------------------------------------------------
//Purge all chars from the input buffer
 BOOL SioClrInbuf(HANDLE ComPort)
{
BOOL RetStat;

RetStat = PurgeComm(ComPort, PURGE_RXCLEAR);
if (RetStat == 0) ErrorMsgBox("SioClrInbuf failed");

return RetStat;
}
//---------------------------------------------------------------------------
//Close a previously opened COM port
 BOOL SioClose(HANDLE ComPort)
{
return(CloseHandle(ComPort));
}
//---------------------------------------------------------------------------

