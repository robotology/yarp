/*
 	SERCRSET.H - SERCOS RESET specific info
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef __SERCRESET_H
#define	__SERCRESET_H

#include "sercos.h"

/* drive types */
#define TORQMODE			1
#define VELMODE				2
#define POSMODE				3
#define VELOCITY_STD		5
#define POSITION_STD		6
#define POS_VEL_STD			7
#define EXT_VELMODE			9
#define DUAL_LOOP_VELMODE	10
#define ANALOG_VELMODE		11			/* PAC-SCI SPECIFIC */
#define ANALOG_TORQUEMODE	12			/* PAC-SCI SPECIFIC */
#define USERMAP				13

typedef struct {
	unsigned16 addr;
	unsigned16 data;
} SERC_CTL_REG;

#define PHASE0				0x0
#define PHASE1				0x1
#define PHASE2				0x2
#define PHASE3				0x3
#define PHASE4				0x4

#define MSGS_PER_AXIS		6
#define MAX_IDNLIST_LEN		30

#define PHASE12_MAX_SAMPLE_RATE		500		/* samples/sec */
#define PHASE12_MIN_CYCLE_TIME		2000	/* uS */

#define F_MCLK				16		/* MHZ */

extern int16 assignedDrive[PCDSP_MAX_AXES];
extern int16 driveCount;
typedef struct{
	char msgstr[MAX_ERROR_LEN];
	} MsgStr;
extern MsgStr driveMsgs[PCDSP_MAX_AXES][MSGS_PER_AXIS];
extern int16 logCount[PCDSP_MAX_AXES];
extern int16 phase2_idncount[PCDSP_MAX_AXES];
extern unsigned16 phase2_idnlist[PCDSP_MAX_AXES][MAX_IDNLIST_LEN];
extern int16 phase3_idncount[PCDSP_MAX_AXES];
extern unsigned16 phase3_idnlist[PCDSP_MAX_AXES][MAX_IDNLIST_LEN];

/* Exported functions */
#ifdef __cplusplus
extern "C" {
#endif

/* Internal Functions */
/* LLSRSET.C */
int16 FNTYPE find_drives(int16 ndrives);
int16 FNTYPE loop_open(void);
int16 FNTYPE loop_closed(void);
int16 FNTYPE check_23(int16 ndrives);
int16 FNTYPE check_34(int16 ndrives);
int16 FNTYPE check_addrs(int16 ndrives, DriveInfo dinfo);
int16 FNTYPE get_asc(unsigned16 channel, int16 dr_num, unsigned16 idn);
int16 FNTYPE zero_log_count(void);
int16 FNTYPE zero_phase2_idncount(void);
int16 FNTYPE zero_phase3_idncount(void);

/* SERCRSET.C */
int16 FNTYPE cfg_drives(int16 ndrives);
int16 FNTYPE enable_drives(int16 ndrives);
int16 FNTYPE get_cfg_type(int16 ndrives, DriveInfo dinfo);
int16 FNTYPE do_cyclic_data(int16 ndrives);

/* User Functions */
/* SERCRSET.C */
int16 FNTYPE configure_phase2_idns(unsigned16 nidns, DriveIdns didns);
int16 FNTYPE configure_phase3_idns(unsigned16 nidns, DriveIdns didns);
int16 FNTYPE configure_at_data(unsigned16 natdata, CyclicData at_data);
int16 FNTYPE configure_mdt_data(unsigned16 nmdtdata, CyclicData mdt_data);
int16 FNTYPE get_drive_addresses(int16 baud, int16 * ndrives, unsigned16 * dr_addrs);
int16 FNTYPE serc_reset(int16 baud, int16 ndrives, DriveInfo dinfo);

#ifdef __cplusplus
	} ;
#endif

#endif /* __SERCRESET_H */
