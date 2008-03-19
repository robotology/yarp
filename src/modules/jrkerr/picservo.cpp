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

#include "sio_util.h"
#include "nmccom.h"
#include "picservo.h"
//---------------------------------------------------------------------------

// REMOVED
// REMOVED
// REMOVED
// REMOVED
// REMOVED
/*
extern NMCMOD	mod[]; 		//Array of modules
extern int nummod;
extern HANDLE ComPort;
*/
//---------------------------------------------------------------------------
//Returns pointer to an initialized IOMOD structure
SERVOMOD *ServoNewMod()
{
SERVOMOD *p;

p = new SERVOMOD;
p->pos = 0;
p->ad = 0;
p->vel = 0;
p->aux = 0;
p->home = 0;
p->perror = 0;
p->cmdpos = 0;
p->cmdvel = 0;
p->cmdacc = 0;
p->cmdpwm = 0;

(p->gain).kp = 0;
(p->gain).kd = 0;
(p->gain).ki = 0;
(p->gain).il = 0;
(p->gain).ol = 0;
(p->gain).cl = 0;
(p->gain).el = 0;
(p->gain).sr = 1;
(p->gain).dc = 0;
(p->gain).sm = 1;

p->stoppos = 0;
p->ioctrl = 0;
p->homectrl = 0;
p->movectrl = 0;
p->stopctrl = 0;
p->ph_adv = 0;
p->ph_off = 0;
return p;
}

//---------------------------------------------------------------------------
BOOL ServoGetStat(byte addr, JRKerrSendCmd *jrsendcmd)
{
int numbytes, numrcvd;
int i, bytecount;
byte cksum;
byte inbuf[20];
SERVOMOD *p;
char msgstr[80];

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);  //cast the data pointer to the right type

//Find number of bytes to read:
numbytes = 2;       //start with stat & cksum
if ( (jrsendcmd->mod[addr].statusitems) & SEND_POS )	numbytes +=4;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_AD ) 	numbytes +=1;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_VEL ) 	numbytes +=2;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_AUX ) 	numbytes +=1;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_HOME )	numbytes +=4;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_ID ) 	numbytes +=2;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_PERROR ) numbytes +=2;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_NPOINTS ) numbytes +=1;
numrcvd = SioGetChars(jrsendcmd->ComPort, (char *)inbuf, numbytes);

//Verify enough data was read
if (numrcvd != numbytes)
	{
            /*
    sprintf(msgstr,"ServoGetStat (%d) failed to read chars",addr);
    ErrorMsgBox(msgstr);
    */
    printf("ServoGetStat (%d) failed to read chars",addr);
    return false;
    }

//Verify checksum:
cksum = 0;
for (i=0; i<numbytes-1; i++) cksum = (byte)(cksum + inbuf[i]);
if (cksum != inbuf[numbytes-1])
	{
    sprintf(msgstr,"ServoGetStat(%d): checksum error",addr);
    ErrorMsgBox(msgstr);
    return false;
    }

//Verify command was received intact before updating status data
jrsendcmd->mod[addr].stat = inbuf[0];
if (jrsendcmd->mod[addr].stat & CKSUM_ERROR)
	{
    ErrorMsgBox("Command checksum error!");
    return false;
    }

//Finally, fill in status data
bytecount = 1;
if ( (jrsendcmd->mod[addr].statusitems) & SEND_POS )
	{
	p->pos = *( (long *)(inbuf + bytecount) );
    bytecount +=4;
    }
if ( (jrsendcmd->mod[addr].statusitems) & SEND_AD )
	{
    p->ad = inbuf[bytecount];
    bytecount +=1;
    }
if ( (jrsendcmd->mod[addr].statusitems) & SEND_VEL )
	{
	p->vel = *( (short int *)(inbuf + bytecount) );
    bytecount +=2;
    }
if ( (jrsendcmd->mod[addr].statusitems) & SEND_AUX )
	{
    p->aux = inbuf[bytecount];
    bytecount +=1;
    }
if ( (jrsendcmd->mod[addr].statusitems) & SEND_HOME )
	{
    p->home = *( (unsigned long *)(inbuf + bytecount) );
    bytecount +=4;
    }
if ( (jrsendcmd->mod[addr].statusitems) & SEND_ID )
	{
    jrsendcmd->mod[addr].modtype = inbuf[bytecount];
    jrsendcmd->mod[addr].modver = inbuf[bytecount+1];
    bytecount +=2;
    }
if ( (jrsendcmd->mod[addr].statusitems) & SEND_PERROR )
	{
	p->perror = *( (short int *)(inbuf + bytecount) );
    bytecount +=2;
    }
if ( (jrsendcmd->mod[addr].statusitems) & SEND_NPOINTS )
	{
    p->npoints = inbuf[bytecount];
    //bytecount +=1;
    }

return TRUE;
}
//---------------------------------------------------------------------------
 long ServoGetPos(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->pos;
}
//---------------------------------------------------------------------------
 byte ServoGetAD(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->ad;
}
//---------------------------------------------------------------------------
 short int ServoGetVel(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->vel;
}
//---------------------------------------------------------------------------
 byte ServoGetAux(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->aux;
}
//---------------------------------------------------------------------------
 long ServoGetHome(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->home;
}
//---------------------------------------------------------------------------
 short int ServoGetPError(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->perror;
}
//---------------------------------------------------------------------------
 byte ServoGetNPoints(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->npoints;
}
//---------------------------------------------------------------------------
 long ServoGetCmdPos(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->cmdpos;
}
//---------------------------------------------------------------------------
 long ServoGetCmdVel(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->cmdvel;
}
//---------------------------------------------------------------------------
 long ServoGetCmdAcc(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->cmdacc;
}
//---------------------------------------------------------------------------
 long ServoGetStopPos(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->stoppos;
}
//---------------------------------------------------------------------------
 byte ServoGetCmdPwm(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->cmdpwm;
}
//---------------------------------------------------------------------------
 byte ServoGetIoCtrl(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->ioctrl;
}
//---------------------------------------------------------------------------
 byte ServoGetHomeCtrl(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->homectrl;
}
//---------------------------------------------------------------------------
 byte ServoGetStopCtrl(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->stopctrl;
}
//---------------------------------------------------------------------------
 byte ServoGetMoveCtrl(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->movectrl;
}
//---------------------------------------------------------------------------
 byte ServoGetPhAdv(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->ph_adv;
}
//---------------------------------------------------------------------------
 byte ServoGetPhOff(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
return p->ph_off;
}
//---------------------------------------------------------------------------
 void ServoGetGain(byte addr, short int * kp, short int * kd, short int * ki,
				  short int * il, byte * ol, byte * cl, short int * el,
                  byte * sr, byte * dc, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
*kp = (p->gain).kp;
*kd = (p->gain).kd;
*ki = (p->gain).ki;
*il = (p->gain).il;
*ol = (p->gain).ol;
*cl = (p->gain).cl;
*el = (p->gain).el;
*sr = (p->gain).sr;
*dc = (p->gain).dc;
}
//---------------------------------------------------------------------------
 void ServoGetGain2(byte addr, short int * kp, short int * kd, short int * ki,
				  short int * il, byte * ol, byte * cl, short int * el,
                  byte * sr, byte * dc, byte * sm, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
*kp = (p->gain).kp;
*kd = (p->gain).kd;
*ki = (p->gain).ki;
*il = (p->gain).il;
*ol = (p->gain).ol;
*cl = (p->gain).cl;
*el = (p->gain).el;
*sr = (p->gain).sr;
*dc = (p->gain).dc;
*sm = (p->gain).sm;
}
//---------------------------------------------------------------------------
 BOOL ServoSetGain(byte addr, short int kp, short int kd, short int ki,
				  short int il, byte ol, byte cl, short int el,
                  byte sr, byte dc, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;
char cmdstr[16];

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
(p->gain).kp = kp;
(p->gain).kd = kd;
(p->gain).ki = ki;
(p->gain).il = il;
(p->gain).ol = ol;
(p->gain).cl = cl;
(p->gain).el = el;
(p->gain).sr = sr;
(p->gain).dc = dc;

*( (short int *)(cmdstr) ) = kp;
*( (short int *)(cmdstr+2) ) = kd;
*( (short int *)(cmdstr+4) ) = ki;
*( (short int *)(cmdstr+6) ) = il;
*( (byte *)(cmdstr+8) ) = ol;
*( (byte *)(cmdstr+9) ) = cl;
*( (short int *)(cmdstr+10) ) = el;
*( (byte *)(cmdstr+12) ) = sr;
*( (byte *)(cmdstr+13) ) = dc;

return NmcSendCmd(addr, SET_GAIN, cmdstr, 14, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoSetGain2(byte addr, short int kp, short int kd, short int ki,
				  short int il, byte ol, byte cl, short int el,
                  byte sr, byte dc, byte sm, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;
char cmdstr[16];

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
(p->gain).kp = kp;
(p->gain).kd = kd;
(p->gain).ki = ki;
(p->gain).il = il;
(p->gain).ol = ol;
(p->gain).cl = cl;
(p->gain).el = el;
(p->gain).sr = sr;
(p->gain).dc = dc;
(p->gain).sm = sm;

*( (short int *)(cmdstr) ) = kp;
*( (short int *)(cmdstr+2) ) = kd;
*( (short int *)(cmdstr+4) ) = ki;
*( (short int *)(cmdstr+6) ) = il;
*( (byte *)(cmdstr+8) ) = ol;
*( (byte *)(cmdstr+9) ) = cl;
*( (short int *)(cmdstr+10) ) = el;
*( (byte *)(cmdstr+12) ) = sr;
*( (byte *)(cmdstr+13) ) = dc;
*( (byte *)(cmdstr+14) ) = sm;

return NmcSendCmd(addr, SET_GAIN, cmdstr, 15, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoLoadTraj(byte addr, byte mode, long pos, long vel, long acc, byte pwm, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;
char cmdstr[16];
int count;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
p->movectrl = mode;
p->cmdpos = pos;
p->cmdvel = vel;
p->cmdacc = acc;
p->cmdpwm = pwm;

count = 0;
*( (byte *)(cmdstr + count) ) = mode;  count += 1;
if ( mode & LOAD_POS) { *( (long *)(cmdstr + count) ) = pos; count += 4; }
if ( mode & LOAD_VEL) { *( (long *)(cmdstr + count) ) = vel; count += 4; }
if ( mode & LOAD_ACC) { *( (long *)(cmdstr + count) ) = acc; count += 4; }
if ( mode & LOAD_PWM) { *( (byte *)(cmdstr + count) ) = pwm; count += 1; }

return NmcSendCmd(addr, LOAD_TRAJ, cmdstr, (byte)count, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 void ServoInitPath(byte addr, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

NmcReadStatus(addr, SEND_POS | SEND_PERROR, jrsendcmd);

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
p->last_ppoint = p->pos + p->perror;
}


/*
//---------------------------------------------------------------------------
 BOOL ServoAddPathpoints(byte addr, int npoints, long *path, int freq)
{
SERVOMOD * p;
char cmdstr[16];
long diff;
int rev;
int i;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);

for (i=0; i<npoints; i++)
  {
  diff = path[i] - p->last_ppoint;
  if (diff<0)
    {
    rev = 0x01;
    diff = -diff;
    }
  else rev = 0x00;

  //Scale the difference appropriately for path freq. used
  if (p->ioctrl & FAST_PATH)  //scale for 60/120 Hz fast path
    {
    if (freq == P_60HZ)
      {
      diff *= (256/32);
      diff |= 0x02;     //60 Hz -> set bit 1 = 1
      }
    else if (freq == P_120HZ) diff *= (256/16);
    else return(false);
    }
  else  //scale for 30/60 Hz slow path
    {
    if (freq == P_30HZ)
      {
      diff *= (256/64);
      diff |= 0x02;     //30 Hz -> set bit 1 = 1
      }
    else if (freq == P_60HZ) diff *= (256/32);
    else return(false);
    }

  diff |= rev;  //bit 0 = reverse bit

  *( (short int *)(cmdstr + 2*i) ) = (short int)diff;

  p->last_ppoint = path[i];
  }


return NmcSendCmd(addr, ADD_PATHPOINT, cmdstr, (byte)(npoints*2), addr);
}

*/



/*
//---------------------------------------------------------------------------
 BOOL ServoStartPathMode(byte groupaddr, byte groupleader)
{
return NmcSendCmd(groupaddr, ADD_PATHPOINT, NULL, 0, groupleader, jrsendcmd);
}
*/
//---------------------------------------------------------------------------
 BOOL ServoStartMove(byte groupaddr, byte groupleader, JRKerrSendCmd *jrsendcmd)
{
return NmcSendCmd(groupaddr, START_MOVE, NULL, 0, groupleader, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoResetPos(byte addr, JRKerrSendCmd *jrsendcmd)
{
return NmcSendCmd(addr, RESET_POS, NULL, 0, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoResetRelHome(byte addr, JRKerrSendCmd *jrsendcmd)
{
byte mode;

mode = REL_HOME;
return NmcSendCmd(addr, RESET_POS, (char *)(&mode), 1, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoSetPos(byte addr, long pos, JRKerrSendCmd *jrsendcmd)
{
char cmdstr[6];

cmdstr[0] = SET_POS;          //mode byte for reset pos
*( (long *)(cmdstr + 1) ) = pos;

return NmcSendCmd(addr, RESET_POS, cmdstr, 5, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoClearBits(byte addr, JRKerrSendCmd *jrsendcmd)
{
return NmcSendCmd(addr, CLEAR_BITS, NULL, 0, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoStopMotor(byte addr, byte mode, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);

mode &= (byte)(~STOP_HERE);
p->stopctrl = mode;       //make sure STOP_HERE bit is cleared

return NmcSendCmd(addr, STOP_MOTOR, (char *)(&mode), 1, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
BOOL ServoStopHere(byte addr, byte mode, long pos, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;
char cmdstr[6];

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);

p->stopctrl = mode;

cmdstr[0] = mode;
*( (long *)(cmdstr + 1) ) = pos;

return NmcSendCmd(addr, STOP_MOTOR, cmdstr, 5, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoSetIoCtrl(byte addr, byte mode, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);

p->ioctrl = mode;

return NmcSendCmd(addr, IO_CTRL, (char *)(&mode), 1, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoSetHoming(byte addr, byte mode, JRKerrSendCmd *jrsendcmd)
{
SERVOMOD * p;

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
p->homectrl = mode;

return NmcSendCmd(addr, SET_HOMING, (char *)(&mode), 1, addr, jrsendcmd);
}
//---------------------------------------------------------------------------
 BOOL ServoHardReset(byte addr, byte mode, JRKerrSendCmd *jrsendcmd)
{
return NmcSendCmd(addr, HARD_RESET, (char *)(&mode), 1, 0, jrsendcmd);
}

//---------------------------------------------------------------------------
/*
BOOL ServoSetPhase(byte addr, int padvance, int poffset, int maxpwm)
{
int i;
char mode;
SERVOMOD * p;

if (maxpwm>255 || maxpwm<128) return(false);

p = (SERVOMOD *)(jrsendcmd->mod[addr].p);
p->ph_adv = (byte)padvance;
p->ph_off = (byte)poffset;


//First set PWM to 0:
ServoLoadTraj(addr, 0x88, 0, 0, 0, 0);  //set PWM to 0 now

//Set phase advance:
for (i=0; i<padvance; i++)
  {
  mode = 0xC0; 	//set dir bit hi
  NmcSendCmd(addr, 0x04, &mode, 1, addr);
  mode = 0x80; 	//set dir bit lo
  NmcSendCmd(addr, 0x04, &mode, 1, addr);
  }
//Toggle PWM high then low:
ServoLoadTraj(addr, 0x88, 0, 0, 0, 255);  //set PWM to 255 now
ServoLoadTraj(addr, 0x88, 0, 0, 0, 0);    //set PWM to 0 now

//Set index phasing count (icount):
for (i=0; i<poffset; i++)
  {
  mode = 0xC0; 	//set dir bit hi
  NmcSendCmd(addr, 0x04, &mode, 1, addr);
  mode = 0x80; 	//set dir bit lo
  NmcSendCmd(addr, 0x04, &mode, 1, addr);
  }

ServoClearBits(addr);  //Clear any position errors

for (i=127; i<=maxpwm; i++)    
  {
  ServoLoadTraj(addr, 0x88, 0, 0, 0, (byte)i);  //ramp up PWM to 255
  if ( (NmcGetStat(addr) & POS_ERR) )     //check if power still there
    {
    ServoLoadTraj(addr, 0x88, 0, 0, 0, 0);   // set PWM to zero on error
    break;
    }
  }

Sleep(500);
return( ServoLoadTraj(addr, 0x88, 0, 0, 0, 0) );    //set PWM to 0 now

}
*/
