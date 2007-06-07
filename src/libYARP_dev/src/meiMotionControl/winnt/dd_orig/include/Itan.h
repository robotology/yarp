/*
	ITAN.H
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef __ITAN_H
#	define __ITAN_H

#	include "idsp.h"

/* Tangent configuration bit */
#	define	CD_TANGENT        0x1000

/* Tangent Control Block Locations */
#	define	TAN_SHIFT			(int16) 0
#	define	TAN_X				(int16) 1
#	define	TAN_Y				(int16) 2
#	define	TAN_OFFSET			(int16) 3
#	define	TAN_THETA			(int16) 9
#	define	TAN_CPR				(int16) 10
#	define	TAN_INIT_ANGLE		(int16) 11

/* Block sizes */
#	define	COMM_SIN_TABLE		(int16) 16
#	define	COMM_SIN_SIZE		(int16) 1024
#	define	COMM_BLOCK_SIZE		(COMM_SIN_SIZE + COMM_SIN_TABLE)
#	define	INTERP_BLOCK_SIZE	(int16) 32
#	define	ASIN_TABLE_SIZE		(int16) 2048
#	define	NORM_TABLE_SIZE		(int16) 2048

#	define	TAN_OFFSET_VALUE	(int16) 4

#ifdef __cplusplus
extern "C" {
#	endif

int16 FNTYPE init_tangent(int16 slave_axis, int16 x_axis, int16 y_axis, double max_v, unsigned16 counts_per_rev);
int16 FNTYPE init_angle(int16 axis, double init_angle);
int16 FNTYPE set_tangent(int16 axis, int16 on);
int16 FNTYPE set_boot_tangent(int16 axis, int16 on);

#ifdef __cplusplus
	} ;
#	endif

#endif	/* __ITAN_H */
