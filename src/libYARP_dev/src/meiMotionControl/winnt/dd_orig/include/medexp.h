/*
	medexp.h - exports for internal dynamic library data	
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef __MEINT_H
#	define __MEINT_H

#include "sercos.h"
#include "sercrset.h"
	
#ifdef __cplusplus
extern "C" {
#	endif

#ifdef MEI_MSVC40
_declspec(dllimport) PDSP PdspPtr;
_declspec(dllimport) DSP Pdsp;
_declspec(dllimport) int16 Pdsp_base;
_declspec(dllimport) int16 Pdsp_error;
_declspec(dllimport) int16 Pdsp_io_problem;
_declspec(dllimport) int16 PassignedDrive[PCDSP_MAX_AXES]; 
_declspec(dllimport) int16 PdriveCount;
_declspec(dllimport) MsgStr	PdriveMsgs[PCDSP_MAX_AXES][MSGS_PER_AXIS];
_declspec(dllimport) int16 PlogCount[PCDSP_MAX_AXES];
_declspec(dllimport) int16 Pphase2_idncount[PCDSP_MAX_AXES];
_declspec(dllimport) unsigned16 Pphase2_idnlist[PCDSP_MAX_AXES][30];
_declspec(dllimport) int16 Pphase3_idncount[PCDSP_MAX_AXES];
_declspec(dllimport) unsigned16 Pphase3_idnlist[PCDSP_MAX_AXES][30];

#	define dspPtr	PdspPtr
#	define dsp	Pdsp
#	define dsp_base Pdsp_base
#	define dsp_error Pdsp_error
#	define dsp_io_problem Pdsp_io_problem
#	define assignedDrive PassignedDrive
#	define driveCount PdriveCount
#	define driveMsgs PdriveMsgs
#	define logCount PlogCount
#	define phase2_idncount Pphase2_idncount
#	define phase2_idnlist Pphase2_idnlist
#	define phase3_idncount Pphase3_idncount
#	define phase3_idnlist Pphase3_idnlist
#endif

#ifdef MEI_MSVC20
extern PDSP					*PdspPtr;
extern DSP					*Pdsp;
extern int16				*Pdsp_base;
extern int16				*Pdsp_error;
extern int16				*Pdsp_io_problem;
extern int16				(*PassignedDrive)[PCDSP_MAX_AXES];
extern int16				*PdriveCount;
extern MsgStr				(*PdriveMsgs)[PCDSP_MAX_AXES][8];
extern int16				(*PlogCount)[PCDSP_MAX_AXES];
extern int16				(*Pphase2_idncount)[PCDSP_MAX_AXES];
extern unsigned16			(*Pphase2_idnlist)[PCDSP_MAX_AXES][30];
extern int16				(*Pphase3_idncount)[PCDSP_MAX_AXES];
extern unsigned16	  		(*Pphase3_idnlist)[PCDSP_MAX_AXES][30];


#	define dspPtr			(*PdspPtr)
#	define dsp				(*Pdsp)
#	define dsp_base			(*Pdsp_base)
#	define dsp_error		(*Pdsp_error)
#	define dsp_io_problem	(*Pdsp_io_problem)
#	define assignedDrive	(*PassignedDrive)
#	define driveCount		(*PdriveCount)
#	define driveMsgs		(*PdriveMsgs)
#	define logCount			(*PlogCount)
#	define phase2_idncount	(*Pphase2_idncount)
#	define phase2_idnlist	(*Pphase2_idnlist)
#	define phase3_idncount	(*Pphase3_idncount)
#	define phase3_idnlist	(*Pphase3_idnlist)
#endif

#ifdef __cplusplus
	} ;
#	endif

#endif	/* __MEINT_H */
