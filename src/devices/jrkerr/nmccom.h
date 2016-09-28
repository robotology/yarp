/*============================================

Copyright: (c) 2007 Jeffrey Kerr LLC
CopyPolicy: 3-clause BSD

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

============================================ */

//---------------------------------------------------------------------------
#ifndef nmccomH
#define nmccomH
//---------------------------------------------------------------------------
#endif

#include <windows.h>

#define MAXNUMMOD	33

typedef unsigned char byte;

typedef struct _NMCMOD {
	byte	modtype;		//module type
    byte	modver;			//module version number
	byte	statusitems;	//definition of items to be returned
	byte	stat;  			//status byte
    byte	groupaddr;		//current group address
    BOOL	groupleader;	//TRUE if group leader
    void *	p;				//pointer to specific module's data structure
    } NMCMOD;

typedef struct _JRKerrSendCmd {
          NMCMOD mod[MAXNUMMOD];
          HANDLE ComPort;        
          int SioError;
          bool IOBusy;
          int nummod;
          } JRKerrSendCmd;
          
#define MAXSIOERROR 2

//Define PIC baud rate divisors
#define	PB19200		64
#define	PB57600		21
#define	PB115200	10
#define	PB230400	5

//Module type definitions:
#define	SERVOMODTYPE	0
#define	ADCMODTYPE		1
#define	IOMODTYPE		2
#define	STEPMODTYPE		3
//The following must be created for each new module type:
//		data structure XXXMOD
//		Initializer function NewXXXMod
//		Status reading function GetXXXStat
//		NMCInit and SendNmcCmd must be modified to include calls
//			to the two functions above

#define CKSUM_ERROR		0x02	//Checksum error bit in status byte

//--------------------- ADC Module specific stuff --------------------------
typedef struct _ADCMOD {
    //******  Move all this data to the NMCMOD structure *******
	short int ad0;	//definition of items to be returned
	short int ad1;
	short int ad2;
	short int ad3;
	short int ad4;
	short int ad5;
    } ADCMOD;




//Function prototypes:
 ADCMOD *AdcNewMod();
 BOOL AdcGetStat(byte addr, JRKerrSendCmd *jrsendcmd);

//Initialization and shutdown
 int NmcInit(char *portname, unsigned int baudrate, JRKerrSendCmd *jrsendcmd);
 void InitVars( JRKerrSendCmd *jrsendcmd);
 BOOL NmcSendCmd(byte addr, byte cmd, char *datastr, byte n, byte stataddr, JRKerrSendCmd *jrsendcmd);
 void FixSioError( byte addr, JRKerrSendCmd *jrsendcmd);
 void NmcShutdown( JRKerrSendCmd *jrsendcmd);

//Module type independant commands (supported by all module types)
 BOOL NmcSetGroupAddr(byte addr, byte groupaddr, BOOL leader, JRKerrSendCmd *jrsendcmd);
 BOOL NmcDefineStatus(byte addr, byte statusitems, JRKerrSendCmd *jrsendcmd);
 BOOL NmcReadStatus(byte addr, byte statusitems, JRKerrSendCmd *jrsendcmd);
 BOOL NmcSynchOutput(byte groupaddr, byte leaderaddr, JRKerrSendCmd *jrsendcmd);
 BOOL NmcChangeBaud(byte groupaddr, unsigned int baudrate, JRKerrSendCmd *jrsendcmd);
 BOOL NmcSynchInput(byte groupaddr, byte leaderaddr, JRKerrSendCmd *jrsendcmd);
 BOOL NmcNoOp(byte addr, JRKerrSendCmd *jrsendcmd);
 BOOL NmcHardReset(byte addr, JRKerrSendCmd *jrsendcmd);

//Retrieve module type independant data from a module's data structure
 byte NmcGetStat(byte addr, JRKerrSendCmd *jrsendcmd);
 byte NmcGetStatItems(byte addr, JRKerrSendCmd *jrsendcmd);
 byte NmcGetModType(byte addr, JRKerrSendCmd *jrsendcmd);
 byte NmcGetModVer(byte addr, JRKerrSendCmd *jrsendcmd);
 byte NmcGetGroupAddr(byte addr, JRKerrSendCmd *jrsendcmd);
 BOOL NmcGroupLeader(byte addr, JRKerrSendCmd *jrsendcmd);




