/*
	ILSR.H
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef __ILSR_H
#	define __ILSR_H

#	define	COMM_OFFSET_POS			(int16) 0
#	define	COMM_OFFSET_NEG			(int16) 2
#	define	COMM_TABLE_LEN			(int16) 4
#	define	COMM_POINTER			(int16) 5
#	define	COMM_SHIFT				(int16) 6
#	define	COMM_OLD_THETA			(int16) 7
#	define	COMM_DIAG				(int16) 8
#	define	COMM_SIN_TABLE			(int16) 16
#	define	COMM_SIN_SIZE			(int16) 1024
#	define	COMM_BLOCK_SIZE			(COMM_SIN_SIZE + COMM_SIN_TABLE)

#	define	INTERP_BLOCK_SIZE		(int16) 32
#	define	ASIN_TABLE_SIZE			(int16) 2048
#	define	NORM_TABLE_SIZE			(int16) 2048

#	define	CD_LASER				(int16) 0x1000

# ifdef __cplusplus
extern "C" {
# endif

int16 FNTYPE init_laser_power(int16 laser_axis, int16 x_axis, double x_mult, 
	int16 y_axis, double y_mult, double max_v, int16 saturation_voltage);
int16 FNTYPE power_control(int16 axis, int16 on);

#ifdef __cplusplus
	} ;
#	endif

#endif	/* __ILSR_H */
