/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: messages.h,v 1.1 2006-06-07 10:40:47 natta Exp $
///
///

#ifndef __controllerh__
#define __controllerh__

#ifndef __ONLY_DEF
#include "dsp56f807.h"
#include "can1.h"
#endif

/* 
 * the purpose of including this file on Linux/Winnt/Qnx is to 
 *     get the definition of messages and params of the dsp controller
 * define __ONLY_DEF before inclusion on Linux/Winnt/Qnx
 */

/* beware of true/false definitions */
#ifndef __ONLY_DEF
#define false 0
#define true 1
#endif

//#define VERSION 0x0111   				/* standard/basic implementation */
//#define VERSION 0x0112				/* decouples shoulder first two joints */
#define VERSION 0x0113			   	/* decouples the third joint of the shoulder */
//#define VERSION 0x0114		    	/* feedback from the AD */
//#define VERSION 0x0115		    	/* coordinated control of the eyes */

//#define DEBUG_CAN_MSG 		1		/* conditional compile for printing can info */
//#define DEBUG_CONTROL_RATE	1 		/* for debugging control cycle rate */
//#define DEBUG_TRAJECTORY 		1		/* print encoder/joint position */
#define DEBUG_SERIAL			1		/* for debugging through terminal */
//#define DEBUG_CURRENT			1		/* for debugging current through terminal */
//#define DEBUG_CALIBRATION		1		/* for calibration debugging through terminal */
//#define EMERGENCY_DISABLED	1		/* emergency fault signal disabled */


#define DEFAULT_BOARD_ID	15		/* default ID for receiving messages (4 bit field) */
#define SMALL_BUFFER_SIZE 	10		/* for serial I/O */
#define CONTROLLER_PERIOD 	1		/* espressed in ms */
#define JN 					2		/* number of axes, can't be changed anyway */

#define MODE_IDLE					0
#define MODE_POSITION 				1
#define MODE_VELOCITY				2
#define MODE_TORQUE					3
#define MODE_CALIB_ABS_POS_SENS		0x10
#define MODE_CALIB_HARD_STOPS		0x20
#define MODE_HANDLE_HARD_STOPS		0x30

#define DEFAULT_VELOCITY 10
#define DEFAULT_ACCELERATION 10
#define DEFAULT_MAX_POSITION 5000
#define DEFAULT_MAX_VELOCITY 0x7fff
#define HALL_EFFECT_SENS_ZERO 14760

#define ADP(x,amount) x+=amount

#ifndef __ONLY_DEF
/* deals with the endianism - byte 4 is the MSB on the Pentium */
#define BYTE_1(x) (__shr(__extract_h(x), 8))
#define BYTE_2(x) (__extract_h(x) & 0xff)
#define BYTE_3(x) (__shr(__extract_l(x), 8))
#define BYTE_4(x) (__extract_l(x) & 0xff)

/* extracting from a short */
#define BYTE_L(x) (__shr(x, 8))
#define BYTE_H(x) (x & 0xff)

/* same as above, deals also with endianism */
dword BYTE_C(byte x4, byte x3, byte x2, byte x1);
	 
#define BYTE_W(x2, x1) (__shl(x1,8) | x2)

/* can bus message structure */
typedef struct canmsg_tag 
{
	byte 	CAN_data[16];					/* CAN bus message */
	dword 	CAN_messID;						/* message ID - arbitration */
	byte 	CAN_frameType;					/* data or remote frame */
	byte 	CAN_frameFormat;				/* standard or extended frame */
	byte 	CAN_length;						/* len of the data */
} canmsg_t;

#define CAN_FIFO_LEN	10					/* length of the fifo buffer */

#define CAN_EI		setRegBit (CAN_RIER, RXFIE)
#define CAN_DI		clrRegBit (CAN_RIER, RXFIE)      
/* clrRegBit(CANRIER, RXFIE), setRegBit(CANRIER, RXFIE) */
#define ADA_EI		setRegBit (ADCA_ADCR1, EOSIE)
#define ADA_DI		clrRegBit (ADCA_ADCR1, EOSIE)
#define ADB_EI		setRegBit (ADCB_ADCR1, EOSIE)
#define ADB_DI		clrRegBit (ADCB_ADCR1, EOSIE)

#define CAN_SYNCHRO_STEPS 4

#endif

/* CAN bus messages (our protocol) */
#define CHANNEL_0(x)	((x) &= 0x7F)
#define CHANNEL_1(x)	((x) |= 0x80)
#define CHANNEL(x)		(((x) & 0x80) ? 1 : 0)

/*
 * this is 8 bits long, MSB is the channel (0 or 1). 
 */
#define CAN_NO_MESSAGE				0
#define CAN_CONTROLLER_RUN		 	1
#define CAN_CONTROLLER_IDLE			2
#define CAN_TOGGLE_VERBOSE			3
#define CAN_CALIBRATE_ENCODER		4
#define CAN_ENABLE_PWM_PAD			5
#define CAN_DISABLE_PWM_PAD			6
#define CAN_GET_CONTROL_MODE		7
#define CAN_MOTION_DONE				8

#define CAN_WRITE_FLASH_MEM			10
#define CAN_READ_FLASH_MEM			11

#define CAN_GET_ENCODER_POSITION	20
#define CAN_SET_DESIRED_POSITION	21
#define CAN_GET_DESIRED_POSITION	22
#define CAN_SET_DESIRED_VELOCITY	23
#define CAN_GET_DESIRED_VELOCITY	24
#define CAN_SET_DESIRED_ACCELER		25
#define CAN_GET_DESIRED_ACCELER		26

#define CAN_SET_ENCODER_POSITION	29
#define CAN_GET_ENCODER_VELOCITY	61
#define CAN_SET_COMMAND_POSITION	62

#define CAN_POSITION_MOVE			27
#define CAN_VELOCITY_MOVE			28

#define CAN_SET_P_GAIN				30
#define CAN_GET_P_GAIN				31
#define CAN_SET_D_GAIN				32
#define CAN_GET_D_GAIN				33
#define CAN_SET_I_GAIN				34
#define CAN_GET_I_GAIN				35
#define CAN_SET_ILIM_GAIN			36
#define CAN_GET_ILIM_GAIN			37
#define CAN_SET_OFFSET				38
#define CAN_GET_OFFSET				39
#define CAN_SET_SCALE				40
#define CAN_GET_SCALE				41
#define CAN_SET_TLIM				42
#define CAN_GET_TLIM				43

#define CAN_SET_BOARD_ID			50
#define CAN_GET_BOARD_ID			51

#define CAN_GET_ERROR_STATUS		60

#define CAN_GET_PID_OUTPUT			63
#define CAN_GET_PID_ERROR			55

#define CAN_SET_MIN_POSITION		64
#define CAN_GET_MIN_POSITION		65
#define CAN_SET_MAX_POSITION		66
#define CAN_GET_MAX_POSITION		67
#define CAN_SET_MAX_VELOCITY		68
#define CAN_GET_MAX_VELOCITY		69

/* special messages for inter board communication/synchronization */
//#define CAN_GET_ACTIVE_ENCODER_POSITION 70
//#define CAN_SET_ACTIVE_ENCODER_POSITION 71

#define CAN_SET_CURRENT_LIMIT		72
#define CAN_SET_BCAST_POLICY		73

#define NUM_OF_MESSAGES				74

/* error status values */
#define ERROR_NONE					0			/* no error, all ok */
#define ERROR_UNSPECIFIED			1			/* generic error */
#define ERROR_MODE					2			/* mode error, can't apply command in current mode */
#define ERROR_FMT					3			/* format error, command in wrong format */
#define ERROR_SEND					4			/* can't send answer back */

/* 
 * class 1 messages, broadcast 
 * when in bcast mode, messages are sent periodically by the controller
 *
 */
#define CAN_BCAST_NONE				0
#define CAN_BCAST_POSITION			1
#define CAN_BCAST_PID_VAL			2
#define CAN_BCAST_FAULT				3
#define CAN_BCAST_CURRENT			4
#define CAN_BCAST_MAX_MSGS			5

/* */

#define GetModeAsString(mode) \
	(mode == MODE_IDLE) ? "idle" : \
	(mode == MODE_POSITION) ? "position" : "velocity"

/* this shouldn't be included in Linux/Winnt */
#ifndef __ONLY_DEF

/* prototypes from trajectory.c */
Int16 init_trajectory (byte jj, Int32 current, Int32 final, Int16 speed);
Int32 step_trajectory (byte jj);
Int16 abort_trajectory (byte jj, Int32 limit);

/* prototypes from controller.c */
byte serial_interface (void);
byte can_interface (void);
byte calibrate (byte channel, Int16 param);
byte writeToFlash (word addr);
byte readFromFlash (word addr);
void generatePwm (byte i);
Int32 step_velocity (byte jj);
void can_send_broadcast(void);

void print_can (byte data[], byte len, char c);
void print_can_error (CAN1_TError *e);

#endif

#endif
