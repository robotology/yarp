//---------------------------------------------------------------------------
#ifndef sio_utilH
#define sio_utilH
//---------------------------------------------------------------------------
#endif

#include <windows.h>

void ErrorPrinting(int f);
int ErrorMsgBox(char *msgstr);
int SimpleMsgBox(char *msgstr);
HANDLE SioOpen(char *name, unsigned int baudrate);
BOOL SioPutChars(HANDLE ComPort, char *stuff, int n);
DWORD SioGetChars(HANDLE ComPort, char *stuff, int n);
DWORD SioTest(HANDLE ComPort);
BOOL SioClrInbuf(HANDLE ComPort);
BOOL SioChangeBaud(HANDLE ComPort, unsigned int baudrate);
BOOL SioClose(HANDLE ComPort);







