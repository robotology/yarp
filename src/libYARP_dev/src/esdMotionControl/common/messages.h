// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta, Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


//
// $Id: messages.h,v 1.3 2006-10-25 09:06:05 babybot Exp $
//
//

#ifndef __canbusmessagesh__
#define __canbusmessagesh__

#define MODE_IDLE					0
#define MODE_POSITION 				1
#define MODE_VELOCITY				2
#define MODE_TORQUE					3
#define MODE_CALIB_ABS_POS_SENS		0x10
#define MODE_CALIB_HARD_STOPS		0x20
#define MODE_HANDLE_HARD_STOPS		0x30

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

#endif
