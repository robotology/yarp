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


#include <stdio.h>

#include "nmccom.h"
//#include "picio.h"
#include "picservo.h"
//#include "picstep.h"
//#include "mainform.h"
#include "sio_util.h"



// REMOVED
// REMOVED
// REMOVED
// REMOVED
// REMOVED
//---------------------------------------------------------------------------
//Global data
/*
__declspec(dllexport) NMCMOD mod[MAXNUMMOD]; 	//Array of modules
__declspec(dllexport) int nummod = 0;				//start off with no modules
char cmdstr[20];			//use global command string
__declspec(dllexport) HANDLE ComPort;
__declspec(dllexport) int SioError = 0;
__declspec(dllexport) int IOBusy = false;
unsigned int BaudRate;
*/






//---------------------------------------------------------------------------
ADCMOD *AdcNewMod()
{
return NULL;
}

//---------------------------------------------------------------------------
//Reset all controllers with group address 'addr' (should include all modules)
 BOOL NmcHardReset(byte addr, JRKerrSendCmd *jrsendcmd)
{
int i;
char cstr[5];

//Send string of 0's to flush input buffers
cstr[0] = 0;
for (i=0; i<20; i++) SioPutChars( jrsendcmd->ComPort,cstr, 1);

//Send out reset command string
cstr[0] = 0xAA;			//Header
cstr[1] = addr;
cstr[2] = HARD_RESET;
cstr[3] = (byte)(cstr[1] + cstr[2]);  //checksum
SioPutChars( jrsendcmd->ComPort, cstr, 4);//send reset

Sleep(100);		//wait for reset to execute

// ******* o delete da erro
//for (i=0; i< jrsendcmd->nummod; i++) delete jrsendcmd->mod[i].p;  //delete and prev. declared modules

jrsendcmd->nummod = 0;
jrsendcmd->SioError = 0;	//reset the number of errors to 0

SioChangeBaud( jrsendcmd->ComPort, 19200);	//Reset the baud rate to the default

SioClrInbuf( jrsendcmd->ComPort);   //clear out any random crap left in buffer

return TRUE;
}

//---------------------------------------------------------------------------
//Change the baud rate of all controllers with group address 'groupaddr'
//(should include all modules) and also changes host's baud rate.
//There should be no group leader for 'addr'
 BOOL NmcChangeBaud(byte groupaddr, unsigned int baudrate, JRKerrSendCmd *jrsendcmd)
{
char cstr[6];

cstr[0] = 0xAA;			//Header
cstr[1] = groupaddr;
cstr[2] = 0x10 | SET_BAUD;
switch (baudrate) {
	case 19200:		cstr[3] = PB19200; break;
	case 57600:		cstr[3] = PB57600; break;
	case 115200:	cstr[3] = PB115200; break;
	case 230400:	cstr[3] = PB230400; break;
    default:	ErrorMsgBox("Baud rate not supported - using default of 19200");
    			cstr[3] = PB19200;
                baudrate = 19200;
    }
cstr[4] = (byte)(cstr[1] + cstr[2] + cstr[3]);  //checksum
SioPutChars( jrsendcmd->ComPort, cstr, 5);//send command

Sleep(100);		//Wait for command to execute

SioChangeBaud( jrsendcmd->ComPort, baudrate);	//Reset the baud rate to the default

SioClrInbuf( jrsendcmd->ComPort);   //clear out any random crap left in buffer
Sleep(100);


return true;
}

//---------------------------------------------------------------------------
//Initialize misc network variables
 void InitVars( JRKerrSendCmd *jrsendcmd )
{
int i;

jrsendcmd->mod[0].modtype = 0;   //Default to a known module type for module 0
jrsendcmd->mod[0].modver = 0;
jrsendcmd->mod[0].stat = 0;
jrsendcmd->mod[0].statusitems = 0;
jrsendcmd->mod[0].groupaddr = 0xFF;
jrsendcmd->mod[0].groupleader = false;

for (i=1; i<MAXNUMMOD; i++)
	{
	jrsendcmd->mod[i].modtype = 0xFF;
	jrsendcmd->mod[i].modver = 0;
    jrsendcmd->mod[i].stat = 0;
    jrsendcmd->mod[i].statusitems = 0;
    jrsendcmd->mod[i].groupaddr = 0xFF;
    jrsendcmd->mod[i].groupleader = false;
    }
}
//---------------------------------------------------------------------------
//Initialize the network of controllers with sequential addresses starting
//at 0.  Retunrs the number of controllers found.
 int NmcInit(char *portname, unsigned int baudrate, JRKerrSendCmd *jrsendcmd)
{
DWORD numread;
byte addr;
char cstr[20];
SERVOMOD *p;
//char mess[40];

InitVars( jrsendcmd);
  
jrsendcmd->ComPort = SioOpen( portname, baudrate);    //Open with default rate of 19200
if ( jrsendcmd->ComPort == INVALID_HANDLE_VALUE) return 0;

NmcHardReset(0xFF, jrsendcmd);
NmcHardReset(0xFF, jrsendcmd);


addr = 1;
while (1)
	{
    //First set the address to a unique value:
    cstr[0] = 0xAA;		//Header
    cstr[1] = 0;  		//Send to default address of 0
    cstr[2] = 0x20 | SET_ADDR;
    cstr[3] = addr;  	//Set new address sequentially
    cstr[4] = 0xFF;		//Set group address to 0xFF
    cstr[5] = (byte)(cstr[1] + cstr[2] + cstr[3] + cstr[4]);  //checksum
    SioPutChars( jrsendcmd->ComPort, cstr, 6);  //Send command

    numread = SioGetChars( jrsendcmd->ComPort, cstr, 2);  //get back status, cksum
    if (numread!=2) break;	//if no response, punt out of loop
    if (cstr[0] != cstr[1])
    	{
        ErrorMsgBox("Status checksum error - please reset the Network");
        SioClose( jrsendcmd->ComPort);
        return(0);
        }

    //Read the device type
    cstr[0] = 0xAA;		//Header
    cstr[1] = addr;  		//Send to addr
    cstr[2] = 0x10 | READ_STAT;
    cstr[3] = SEND_ID;  //send ID only
    cstr[4] = (byte)(cstr[1] + cstr[2] + cstr[3]);  //checksum
    SioPutChars( jrsendcmd->ComPort, cstr, 5);  //Send command

    numread = SioGetChars( jrsendcmd->ComPort, cstr, 4);  //get back stat, type, ver, cksum
    if (numread!=4)
    	{
        ErrorMsgBox("Could not read device type");
        SioClose( jrsendcmd->ComPort);
        return(0);
        }
    jrsendcmd->mod[addr].stat = cstr[0];
    jrsendcmd->mod[addr].modtype = cstr[1];
    jrsendcmd->mod[addr].modver = cstr[2];
    jrsendcmd->mod[addr].statusitems = 0;
    jrsendcmd->mod[addr].groupaddr = 0xFF;
    jrsendcmd->mod[addr].groupleader = false;
    switch ( jrsendcmd->mod[addr].modtype)
    	{
        case SERVOMODTYPE: 	jrsendcmd->mod[addr].p = ServoNewMod();
        					//fix for change in I/O CTRL command
        					if ((jrsendcmd->mod[addr].modver<10) || (jrsendcmd->mod[addr].modver>20))
                              {
        					  p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
                              p->ioctrl = IO1_IN | IO2_IN;
                              }
        					break;
        case ADCMODTYPE: 	jrsendcmd->mod[addr].p = AdcNewMod(); break;
//        case IOMODTYPE: 	mod[addr].p = IoNewMod(); break;
//        case STEPMODTYPE: 	mod[addr].p = StepNewMod(); break;
        }

    addr++;		//increment the address
    }

jrsendcmd->nummod = addr-1;

if (jrsendcmd->nummod > 0)
	{
    NmcChangeBaud(0xFF, baudrate, jrsendcmd);
	//if (nummod>1) sprintf(mess,"%d Modules found", nummod);
    //else sprintf(mess,"%d Module found", nummod);
	//ErrorMsgBox(mess);
    }
else
	{
    //ErrorMsgBox("No modules found on the network.\nPlease check power and connections.");
    SioClose( jrsendcmd->ComPort);
    }

return( jrsendcmd->nummod );	//Return number of modules
}
//---------------------------------------------------------------------------
 BOOL AdcGetStat(byte addr)
{
return false;
}
//---------------------------------------------------------------------------
//Sends a command to addr and shoves the returned status data in the
//structure for module address stataddr.  If stataddr = 0, the command
//is taken as a group command with no group leader (no status returned).
//n is the number of auxilliary data bytes.
 BOOL NmcSendCmd(byte addr, byte cmd, char *datastr, byte n, byte stataddr, JRKerrSendCmd *jrsendcmd)
{
byte cksum;
char mess[40];
byte outstr[20];
int i;
BOOL iostat;

//If too many SIO errors, punt
if ( jrsendcmd->SioError > MAXSIOERROR) return(false);

jrsendcmd->IOBusy = true;

//Check if data is to be returned from to a known module type
if ( !( ( jrsendcmd->mod[stataddr].modtype==SERVOMODTYPE) ||
        ( jrsendcmd->mod[stataddr].modtype==ADCMODTYPE) ||
        ( jrsendcmd->mod[stataddr].modtype==IOMODTYPE)  ||
        ( jrsendcmd->mod[stataddr].modtype==STEPMODTYPE) ) )
	{
    sprintf(mess,"Module type %d not supported", jrsendcmd->mod[stataddr].modtype);
    ErrorMsgBox(mess);
    return (false);
    }

//Calculate the adjust command byte, calculate checksum and send the command
cksum = 0;
outstr[0] = 0xAA;	//start with header byte
outstr[1] = addr;	//add on address byte
cksum += outstr[1];
outstr[2] = (byte)(((n<<4) & 0xF0) | cmd);
cksum += outstr[2];
for (i=0; i<n; i++)
	{
    outstr[i+3] = datastr[i];
	cksum += outstr[i+3];
    }
outstr[n+3] = cksum;

SioClrInbuf( jrsendcmd->ComPort);  //Get rid of any old input chars
SioPutChars( jrsendcmd->ComPort, (char *)outstr, n+4); //Send the command string

if (stataddr == 0)		//If a group command w/ no leader, add delay then exit
	{
    Sleep(60);
    jrsendcmd->IOBusy = false;
	return true;
    }

switch ( jrsendcmd->mod[stataddr].modtype) {
	case SERVOMODTYPE:	iostat = ServoGetStat(stataddr, jrsendcmd); break;
	case ADCMODTYPE:	iostat = AdcGetStat(stataddr); break;
//	case IOMODTYPE:		iostat = IoGetStat(stataddr); break;
//	case STEPMODTYPE:	iostat = StepGetStat(stataddr); break;
    }

if (iostat == false)
	{
    jrsendcmd->SioError++;
    FixSioError( stataddr, jrsendcmd);
    }
else jrsendcmd->SioError = 0;

jrsendcmd->IOBusy = false;
return iostat;
}

//---------------------------------------------------------------------------
//Attempt to re-synch communications
 void FixSioError( byte addr, JRKerrSendCmd *jrsendcmd)
{
int i;
char teststr[2];

//If too many errors - prompt to reset
if ( jrsendcmd->SioError >= MAXSIOERROR)
	{
    ErrorMsgBox("Multiple communication errors - please the Network");
    return;
    }

//Otherwise, send out a null string to re-synch
teststr[0] = 0;
for (i=0; i<30; i++) SioPutChars( jrsendcmd->ComPort, teststr, 1);
Sleep(100);
SioClrInbuf( jrsendcmd->ComPort);
}
//---------------------------------------------------------------------------
 BOOL NmcSetGroupAddr(byte addr, byte groupaddr, BOOL leader, JRKerrSendCmd *jrsendcmd)
{
char cmdstr[2];

if (!(groupaddr & 0x80)) return false;  //punt if not valid group address

cmdstr[0] = addr;
cmdstr[1] = groupaddr;
if (leader) cmdstr[1] &= 0x7F;  //clear upper bit if a group leader
jrsendcmd->mod[addr].groupaddr = groupaddr;
jrsendcmd->mod[addr].groupleader = leader;
return NmcSendCmd(addr, SET_ADDR, cmdstr, 2, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
/*
 BOOL NmcSynchOutput(byte groupaddr, byte leaderaddr)
//Synchronous output command issued to groupaddr.  Status data goes to
//the leaderaddr.  If no leader, use leaderaddr = 0.
{
return NmcSendCmd(groupaddr, SYNCH_OUT, NULL, 0, leaderaddr);
}
*/
//---------------------------------------------------------------------------
//Synchronous input command issued to groupaddr.  Status data goes to
//the leaderaddr.  If no leader, use leaderaddr = 0.
/*
 BOOL NmcSynchInput(byte groupaddr, byte leaderaddr)
{
return NmcSendCmd(groupaddr, SYNCH_INPUT, NULL, 0, leaderaddr);
}
*/

//---------------------------------------------------------------------------
 BOOL NmcNoOp(byte addr, JRKerrSendCmd *jrsendcmd)
{
return NmcSendCmd(addr, NOP, NULL, 0, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL NmcReadStatus(byte addr, byte statusitems, JRKerrSendCmd *jrsendcmd)
{
char cmdstr[2];
byte oldstat;
BOOL retval;

cmdstr[0] = statusitems;
oldstat = jrsendcmd->mod[addr].statusitems;
jrsendcmd->mod[addr].statusitems = statusitems;
retval = NmcSendCmd(addr, READ_STAT, cmdstr, 1, addr, jrsendcmd);
jrsendcmd->mod[addr].statusitems = oldstat;

return retval;
}
//---------------------------------------------------------------------------
 BOOL NmcDefineStatus(byte addr, byte statusitems, JRKerrSendCmd *jrsendcmd)
{
char cmdstr[2];

cmdstr[0] = statusitems;
jrsendcmd->mod[addr].statusitems = statusitems;
return NmcSendCmd(addr, DEF_STAT, cmdstr, 1, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 byte NmcGetStat(byte addr, JRKerrSendCmd *jrsendcmd)
{
return jrsendcmd->mod[addr].stat;
}
//---------------------------------------------------------------------------
 byte NmcGetStatItems(byte addr, JRKerrSendCmd *jrsendcmd)
{
return jrsendcmd->mod[addr].statusitems;
}
//---------------------------------------------------------------------------
 byte NmcGetModType(byte addr, JRKerrSendCmd *jrsendcmd)
{
return jrsendcmd->mod[addr].modtype;
}
//---------------------------------------------------------------------------
 byte NmcGetModVer(byte addr, JRKerrSendCmd *jrsendcmd)
{
return jrsendcmd->mod[addr].modver;
}
//---------------------------------------------------------------------------
 byte NmcGetGroupAddr(byte addr, JRKerrSendCmd *jrsendcmd)
{
return jrsendcmd->mod[addr].groupaddr;
}
//---------------------------------------------------------------------------
 BOOL NmcGroupLeader(byte addr, JRKerrSendCmd *jrsendcmd)
{
return jrsendcmd->mod[addr].groupleader;
}
//---------------------------------------------------------------------------
 void NmcShutdown( JRKerrSendCmd *jrsendcmd)
{
//int retval;

//retval = MessageBox(NULL, "Do you wish to reset the NMC network?",
//                                 "",MB_YESNO | MB_DEFBUTTON1 | MB_TASKMODAL);
//
//if (retval == IDYES)
//	{
//	if (ComPort!=INVALID_HANDLE_VALUE && ComPort!=NULL) NmcHardReset(0xFF);
//    }
//else
//	{
//    if (BaudRate != 19200)
//      	{
//      	ErrorMsgBox("Returning NMC Baud rate to 19200 (default)");
//      	NmcChangeBaud(0xFF, 19200);
//      	}
//    }
if ( jrsendcmd->ComPort!=INVALID_HANDLE_VALUE && jrsendcmd->ComPort!=NULL) NmcHardReset(0xFF, jrsendcmd);

jrsendcmd->nummod = 0;    
SioClose( jrsendcmd->ComPort);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
