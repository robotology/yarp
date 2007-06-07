/*
	mboard.h - Multiple Controllers in one computer support.

	This module allows a program to simply communicate with several
	controllers plugged into the same controller.

	Theory:  The controller maintains a structure called DSP which
	contains information pertaining to the base I/O address, and a bunch
	of other stuff.  A library variable called 'dspPtr' contains a pointer
	to the default DSP structure.  By varying this pointer, we can get
	the library to use another controller.

	# define MBOARD_OOM ...	     (Out of allocatable memory error code.)

	int16 totalAxes;               (total number of axes found on all controllers)
	int16 offendingBoard;          (board # producing an error)
	int16 mboards ;                (total number of boards configured.)

	int16 m_setup(int16 len, int16 * bases);
		- Use this function instead of dsp_init(...) to initialize several
		  controllers.  Simply create an array where each element contains
		  the base I/O address of a controller.  Pass that array to this
		  function, and m_setup will perform the appropriate software
		  initialization.  Suppose two 4-axis controllers are installed
		  in a computer, one at base I/O address 0x300 and the other at
		  0x310.  m_setup would be called like this:

		  	int16 bases[2] = { 0x300, 0x310 };
		  	if (m_setup(2, bases))
		  	{	(initialization failure code goes here...)
		  	}

		  totalAxes would be assigned with 8.

	int16 m_board(int16 boardno) ;
		- This function modifies the 'dspPtr' to point to a particular
		  board.  Given the above initialization code, all boards initialized
		  by m_setup(...) can be reset with this code:

	        int16 i ;
		    for (i = 0; i < mboards; i++)
		    {   m_board(i) ;
		        dsp_reset() ;
		    }

	int16 m_axis(int16 a);
		- this function sets the 'dspPtr' to point to the desired
		  controller board and returns the physical axis on that controller.
		  Following our above example, suppose the following calls:

		  	m_axis(0);	       (sets dspPtr to point to 0x300, and returns 0)
		  	m_axis(2);	       (dspPtr <- 0x300, returns 2.)
		  	m_axis(6);	       (dspPtr <- 0x310, returns 3.)

		  The value returned by m_axis(...) should be used as the axis
		  parameter for other library functions.  So, suppose you wanted to
		  examine the position of the first axis on the second board, you
		  might do something like this:

		  	double p ;
		  	get_position(m_axis(4), &p);

	Be sure to check the MM.C for an extensive sample using the MBOARD module.
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef __MBOARD_H
#define	__MBOARD_H

#	define	MBOARD_OOM			(-3)		/* calloc() failure. */

extern int16
	offendingBoard,							/* which board failed? */
	totalAxes, 								/* how many axes total? */
	mboards ;								/* how many boards total? */

#ifdef __cplusplus
extern "C" {
#	endif

int16 FNTYPE m_board(int16 boardno);			/* select a particular board */
int16 FNTYPE m_axis(int16 actual_axis);			/* select a specific board & axis */
int16 FNTYPE m_setup(int16 len, int16 * baseAddresses);	/* initialize controllers... */

#ifdef __cplusplus
	} ;
#	endif

#endif
