/*
	idsp.h - internal DSP library stuff.
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef	__IDSP_H
#	define	__IDSP_H

# ifdef	INTRINSIC
#	pragma intrinsic
#	endif

#	include "pcdsp.h"
#	include <stddef.h>

/*
	Defines for two-byte transfers, so that there won't be a problem
	with systems that perform actual 16-bit transfers (like the NEC V50).
*/
#	ifdef	DSP_MACROS
#		define	DSP_IN(a)		(inport(a))
#		define	DSP_OUT(a,v)	(outport(a,v))
#     define   DSP_INB(a)     (inportb(a))
#     define   DSP_OUTB(a,v)  (outportb(a,v))
#	endif

#ifndef	CRITICAL
#	ifdef	__BORLANDC__
#		ifndef MEI_WINNT
#			define	CRITICAL		disable()
#			define	ENDCRITICAL		enable()
#		endif
#	endif
#	ifndef CRITICAL
#		define	CRITICAL
#		define	ENDCRITICAL
#	endif
#endif

/*	I/O addresses (offsets from PCDSP_BASE) */
#	define	PCDSP_ADDRESS		            0
#	define	PCDSP_DATA			            2
#	define	PCDSP_RESET			            4


/*	DM/BM select address bits	*/
#	define	PCDSP_DM			               (int16)0x8000
#	define	PCDSP_BM			               (int16)0x4000
#	define	PCDSP_PM			               (int16)0x0000

/* Sercos board type definitions */
#	define	SERCOS_PC							(int16)9
#	define	SERCOS_STD							(int16)12
#	define	SERCOS_104							(int16)13

/* Sercos primary mode definitions */
#	define	SERCOS_PM_IO						(int16)0
#	define	SERCOS_PM_TORQUE					(int16)1
#	define	SERCOS_PM_VEL						(int16)2
#	define	SERCOS_PM_POS						(int16)3

/*	Configuration Block stuff. */
#	define	DM_CONFIGURATION(n)		((DSP_DM) 0x0100 + (n))
#	define	DM_SIGNATURE			(DSP_DM) DM_CONFIGURATION(0x00)
#	define	DM_AXES					(DSP_DM) DM_CONFIGURATION(0x01)
#	define	DM_PC_EVENT				(P_DSP_DM) DM_CONFIGURATION(0x02)
#	define	DM_PC_STATUS			(P_DSP_DM) DM_CONFIGURATION(0x03)
#	define	DM_INPTR				(P_DSP_DM) DM_CONFIGURATION(0x04)
#	define	DM_OUTPTR				(P_DSP_DM) DM_CONFIGURATION(0x05)
#	define	DM_TRANSFER_BLOCK		(P_DSP_DM) DM_CONFIGURATION(0x06)
#	define	DM_E_DATA				(P_DSP_PM) DM_CONFIGURATION(0x07)
#	define	DM_DATA_STRUCT			(P_DSP_DM) DM_CONFIGURATION(0x08)
#	define	DM_EVENT_FRAMES			(P_DSP_DM) DM_CONFIGURATION(0x09)
#	define	DM_FIRMWARE_VERSION		(DSP_DM) DM_CONFIGURATION(0x0A)
#	define	DM_OPTION				(DSP_DM) DM_CONFIGURATION(0x0B)
#	define	DM_IO_BLOCK				(P_DSP_DM) DM_CONFIGURATION(0x0C)
#	define	DM_GLOBAL_DATA			(P_DSP_DM) DM_CONFIGURATION(0x0D)
#	define	DM_SHADOW				(P_DSP_DM) DM_CONFIGURATION(0x0E)
#	define	DM_CLOCK				(DSP_DM) DM_CONFIGURATION(0x1F)

/* config-block pointers to boot memory */
#	define	PB_INIT_8255			(P_DSP_BM) DM_CONFIGURATION(0x11)
#	define	PB_EVENT_FRAMES			(P_DSP_BM) DM_CONFIGURATION(0x12)
#	define	PB_CONFIG_STRUCT		(P_DSP_BM) DM_CONFIGURATION(0x13)

#	define	BM_PAGE					(P_DSP_BM) (0x1000)
#	define	BM_PAGES				(3)
#	define	BM_CHECKSUM				(P_DSP_BM) (BM_PAGES * BM_PAGE - 4)
#	define	BM_PAGE_0				0x0000			/* (starting addresses) */
#	define	BM_PAGE_1				(2 * BM_PAGE)
#	define	BM_SIZE					0x4000


/*	SIGNATURE word				*/
#	define	PCDSP_SIGNATURE		         (int16)0x641C


/*	Transfer Buffer (in External DM) */
#	define	TRANSFER_CTRL			0
#	define	TRANSFER_ADDR			1
#	define	TRANSFER_BUFFER			2
/* Transfer control flags */
#	define	XFER_WORDS(n)			((n) - 1)
#	define	XFER_WRITE				(int16)0x2000
#	define	XFER_READ				(int16)0x0000 /* !XFER_WRITE */
#	define	XFER_PROGRAM			(int16)0x4000
#	define	XFER_DATA				(int16)0x0000 /* !XFER_PROGRAM */
#	define	XFER_GO					(int16)0x8000
#	define	TRANSFER_BUFF_SIZE		64 /* DSP_DM's. */
/*	how long to wait for the transfer buffer. */
#	define	TIMEOUT					(int16)0xFFFF

/*
	TRIGGER_NEGATIVE and TRIGGER_POSITIVE:
	These are OR'd with the pc-event action value to specify which direction
	causes a trigger--if the action is taken after the actual trigger value
	exceeds the given value then specify TRIGGER_POSITIVE.  If the trigger
	action is to be taken when the actual value is less than the trigger value,
	specify TRIGGER_NEGATIVE.
*/
#	define	TRIGGER_NEGATIVE		(int16)0x8000
#	define	TRIGGER_POSITIVE		(int16)0x0000 /* !TRIGGER_NEGATIVE. */

/*	User I/O ports	*/
#	define	ENCODER_0				(P_DSP_DM) 0x14
#	define	SEL_A2D					(P_DSP_DM) 0x20
#	define	DSP_SWITCHES			(P_DSP_DM) 0x30
#	define	IO_P2					(P_DSP_DM) 0x30
#	define	AUX_USER_IO				(P_DSP_DM) 0x38
#	define	IO_P3					(P_DSP_DM) 0x38
#	define	USER_IO					(P_DSP_DM) 0x40
#	define	IO_P1					(P_DSP_DM) 0x40
#	define	IO_CFG_48				(P_DSP_DM) 0x48
#	define	IO_CFG_50				(P_DSP_DM) 0x50
#	define	SEL_8254				(P_DSP_DM) 0x60
#	define	FB_FAULT				(P_DSP_DM) 0x70
#	define	FB_ILLEGAL_STATE		(P_DSP_DM) 0x71


/*	define	STEP_CFG		   	            (P_DSP_DM) 0x48
**	define	STEP_PORTS			            (P_DSP_DM) 0x50
*/

/* constants used by frame buffer counters */
#	define	FIFO_SIZE				600
#	define	FC_OFFSET				2
#	define	AXIS_ANALOG_OFFSET		4

/*	8254 timer stuff--offsets from SEL_8254. */
#	define	PORT_CTRL				(int16)0x03
#	define	CTRL_8254				(int16)0x03

/*	Internal Data Memory	*/
#	define	PCDSP_TIMER				(P_DSP_DM) 0x3FFD /* internal memory */


/*	Interrupt support.	 */
#	define	PC_INTERRUPT_ENABLE		(STEP_CFG + PORT_C)
#	define	PC_INTERRUPT_BIT		(int16)0x80
#	define	DSP_INTERRUPT_BIT		(int16)0x40
#	define	PC_INTERRUPT_PORT		(int16)0x42
/*	pc-side of interrupt stuff */
#	define	PC_8259_EOI				(int16)0x20
#	define	PC_8259_IMR				(int16)0x21
#	define	PC_EOI					(int16)0x20


/*	How many?...			*/
#	define	PCDSP_ANALOG_CHANNELS	8
#	define	PCDSP_8254_TIMERS		3


/*	How many fractions per count? (as a double)	  While these
	happen to coencide with maximum values of UNSIGNED LONGs and
	UNSIGNED INTs, they are not affected by the compiler.  The DSP
	expects some numbers to max-out at these sizes.
*/
#	define	FRACTIONS_PER_COUNT		(4294967296.0)
#	define	SFRACTIONS_PER_COUNT	(65536.0)


/*	Switch Bit Definitions. */
#	define	SW_POS_LIMIT			(int16)0x01
#	define	SW_NEG_LIMIT			(int16)0x02
#	define	SW_HOME					(int16)0x04
#	define	SW_AMP_FAULT			(int16)0x08


/*	Analog control bits. */
#	define	ANALOG_DIFF				(int16)0x10
#	define	ANALOG_BIPOLAR			(int16)0x08


/*	Exceptional frame codes, understood by ipcdsp_exception_frame_offset */
#	define	EF_POS_STOP				0
#	define	EF_NEG_STOP				1
#	define	EF_POS_E_STOP			2
#	define	EF_NEG_E_STOP			3
#	define	EF_STOPPED				4
/*	Boot Memory E-Frames.  The - direction frames are derived by the DSP from the + ones. */
#	define	BEF_POS_STOP			0
#	define	BEF_POS_E_STOP			1
#	define	BEF_STOPPED				2




/*	External Data Memory Structure		*/
#	define	DS_PREV_ENCODER			(int16)0x00
#	define	DS_CURRENT_VEL			(int16)0x01
#	define	DS_ACTUAL_POSITION		(int16)0x02
#	define	DS_TIME					(int16)0x04
#	define	DS_JERK					(int16)0x07
#	define	DS_ACCELERATION			(int16)0x0A
#	define	DS_VELOCITY				(int16)0x0D
#	define	DS_POSITION				(int16)0x10
#	define	DS_MASTER_POS			(int16)0x14
#	define	DS_RATIO				(int16)0x15
#	define	DS_CV_1					(int16)0x1B
#	define	DS_ERROR				(int16)0x1C
#	define	DS_TRIGGER				(int16)0x1D
#	define	DS_LATCH				(int16)0x22

#	define	DS_D(n)					((int16)0x24 + (n))
                                 
#	define	DS_SIZE					((int16)0x2E)
#	define	DS(n)					(DS_SIZE * (n))


/*
	Global data structure.
*/
#	define	GD_GATE					0
#	define	GD_FEED_RATE			1
#	define	GD_CONFIG				9
#	define	GD_JOG					10
#	define	GD_SIZE					25

#	define	GDC_LATCH_STATUS		(int16)0x0001
#	define	GDC_LATCH				(int16)0x0002

/* constants used by input monitor */
#	define	MONITORED_PORTS			6
#	define	IO_STAT					0
#	define	IO_MON					1
#	define	IO_MASK					8
#	define	IO_EXPECTED				15
#	define	IO_CHANGE				22

#	define	PC_STATUS_OVERRIDE		(int16)0x4000

#	define	ACTION_MASK				PC_STATUS_EXCEPTION
#	define	PC_ACTION_MASK			(PC_STATUS_EXCEPTION | PC_STATUS_OVERRIDE)

/*	Frame Priorities.	*/
#	define	FRAME_FP(n)				(n)
#	define	FRAME_NP(n)				((n) << 4)


/* DF_... - DSP.flags bits */
#	define	DF_AXIS_BUSY			1
#	define	DF_FRAME_COMPRESS		2	/* on only if we shouldn't compress. */


/*	FCTL_... -- frame control constants.		*/
#	define	FCTL_RELEASE			(int16)0x8000
#	define	FCTL_INTERRUPT			(int16)0x4000
#	define	FCTL_HOLD				(int16)0x2000
#	define	FCTL_ADD				(int16)0x0100
#	define	FCTL_MOVE				(int16)0x0200

#	define	FCTL_DEFAULT			(int16)(FCTL_RELEASE | 0x12)

/*	FUPD_... -- frame update constants			*/
/*	FTRG_... -- frame trigger constants			*/
#	define	FUPD_JERK				(int16)0x0001
#	define	FUPD_ACCEL				(int16)0x0002
#	define	FUPD_VELOCITY			(int16)0x0004
#	define	FUPD_POSITION			(int16)0x0008
#	define	FUPD_ACTUAL				(int16)0x0010
#	define	FUPD_OUTPUT				(int16)0x0020
#	define	FTRG_TIME				(int16)0x1200
#	define	FTRG_ACCEL				(int16)0x2800
#	define	FTRG_VELOCITY			(int16)0x2B00
#	define	FTRG_POSITION			(int16)0x2F00
#	define	FTRG_INPUT_T			(int16)0x0200
#	define	FTRG_INPUT_J			(int16)0x0400
#	define	FTRG_INPUT_A			(int16)0x0700
#	define	FTRG_INPUT_V			(int16)0x0A00
#	define	FTRG_INPUT_X			(int16)0x0D00
#	define	OUTPUT_JERK				(int16)4      /* offsets within frame */
#	define	OUTPUT_ACCEL			(int16)7
#	define	OUTPUT_VELOCITY			(int16)10
#	define	OUTPUT_POSITION			(int16)13

#	define	FRAME_SIZE				20 /* unsigned16's */


/*	PC-DSP boot memory size.		*/
#	define	DSP_BM_SIZE				(4096)		/* DSP_BM's (or 16k bytes) */
#	define	DSP_PAGE_SIZE			(1024)		/* DSP_BM's (or 4k bytes) */

#	define	idsp_write_dm(dsp, addr, value)	pcdsp_write_dm(dsp, addr, 1, &value)
#	define	idsp_read_dm(dsp, addr)	   		pcdsp_read_dm(dsp, addr, 1, NULL)
#	define	idsp_write_pm(dsp, addr, value)	pcdsp_write_pm(dsp, addr, 1, &value)
#	define	idsp_read_pm(dsp, addr)	   		pcdsp_read_pm(dsp, addr, 1, NULL)

#	define	write_dm(addr, value)		   	idsp_write_dm(dspPtr, addr, value)
#	define	read_dm(addr)					idsp_read_dm(dspPtr, addr)

#	define	ANALOG_MAX				(4095)


#	define	STEP_CFG				((P_DSP_DM) idsp_read_dm(dspPtr, 0x10e))
#	define	STEP_PORTS				((P_DSP_DM) idsp_read_dm(dspPtr, 0x10e) + 3)


/*	LAXIS.last values. */
#	define	LA_UNDEFINED			0
#	define	LA_COMMAND				1
#	define	LA_VELOCITY				2



/*	Software & Hardware limit identifiers	*/
#	define	CONFIGURABLE_LIMITS			17
#	define	CL_CONFIG_DATA				0
#	define	CL_INTERNAL_OFFSET			1
#	define	CL_HOME_PORT_OFFSET			2
#	define	CL_HOME						3
#	define	CL_DEVICE_FAULT				4
#	define	CL_POS_OVERTRAVEL			5
#	define	CL_NEG_OVERTRAVEL			6
#	define	CL_X_NEG					7
#	define	CL_X_POS					8
#	define	CL_ERROR					9
#	define	CL_IN_POSITION				10
#	define	CL_STATUS_PORT				11
#	define	CL_AMP_ENABLE_MASK			12
#	define	CL_IN_POSITION_MASK			13
#	define	CL_ANALOG_CHANNEL			14
#	define	CL_DAC_CHANNEL				15
#	define	CL_AUX_ENCODER				16


/* CL_CONFIG_DATA stuff */
#	define	CD_ANALOG					(int16)0x01
#	define	CD_PARALLEL					(int16)0x02
#	define	CD_FEEDBACK_MASK			(int16)0x07
#	define	CD_DUAL						(int16)0x08
#	define	CD_CAM						(int16)0x10
#	define	CD_INTEGRATION				(int16)0x20
#	define	CD_JOG						(int16)0x40
#	define	CD_UNIPOLAR					(int16)0x80
#	define	CD_STEPPER					(int16)0x100
#	define	CD_USER_ANALOG				(int16)0x200
#	define	CD_FEEDBACK_CHECK			(int16)0x400
#	define	CD_SIN_INTERP				(int16)0x800
#	define	CD_SIN_COMM					(int16)0x2000
#	define	CD_KILL_AXIS				(int16)0x8000


/* configuration structure offsets */

#	define	ED_CONFIG_DATA				0
#	define	ED_INTERNAL_OFFSET			1
#	define	ED_HOME_PORT_OFFSET			2
#	define	ED_HOME_MASK				3
#	define	ED_HOME_ACTION				4
#	define	ED_AMP_FAULT_MASK			5
#	define	ED_AMP_FAULT_ACTION			6
#	define	ED_POS_LIMIT_MASK			7
#	define	ED_POS_LIMIT_ACTION			8
#	define	ED_NEG_LIMIT_MASK			9
#	define	ED_NEG_LIMIT_ACTION			10

/*	Software Limits. */
#	define	ED_X_NEG					11
#	define	ED_X_NEG_ACTION				13
#	define	ED_X_POS					14
#	define	ED_X_POS_ACTION				16
#	define	ED_ERROR_LIMIT				17
#	define	ED_ERROR_LIMIT_ACTION		18
#	define	ED_IN_POS_LIMIT				19
#	define	ED_STATUS_PORT				20
#	define	ED_AMP_ENABLE_MASK			21
#	define	ED_IN_POS_MASK				22
#	define	ED_ANALOG_CHANNEL			23
#	define	ED_DAC_CHANNEL				24
#	define	ED_AUX_ENCODER				25

/*	Tuning parameters... */
#	define	ED_C(n)						(0x1A + (n))
#	define	ED_C0						ED_C(0)
#	define	ED_C1						ED_C(1)
#	define	ED_C2						ED_C(2)
#	define	ED_C3						ED_C(3)
#	define	ED_C4						ED_C(4)
#	define	ED_C5						ED_C(5)
#	define	ED_C6						ED_C(6)
#	define	ED_C7						ED_C(7)
#	define	ED_C8						ED_C(8)
#	define	ED_C9						ED_C(9)
/* AUX_FILTER filter Coeffs */
#	define	ED_I0						ED_C(10)
#	define	ED_I1						ED_C(11)
#	define	ED_I2						ED_C(12)
#	define	ED_I3						ED_C(13)
#	define	ED_I4						ED_C(14)
#	define	ED_I5						ED_C(15)
#	define	ED_I6						ED_C(16)
#	define	ED_I7						ED_C(17)
#	define	ED_I8						ED_C(18)
#	define	ED_I9						ED_C(19)

#	define	ED_SIZE						46
#	define	ED(n)						(ED_SIZE * (n))


#	define	dsp_in_sequence(dsp, axis)		((pcdsp_status(dsp, axis) & IN_SEQUENCE) != 0)
#	define	dsp_in_position(dsp, axis)		((pcdsp_status(dsp, axis) & IN_POSITION) != 0)
#	define	dsp_in_motion(dsp, axis)		((pcdsp_status(dsp, axis) & IN_MOTION) != 0)
#	define	dsp_neg_direction(dsp, axis)	((pcdsp_status(dsp, axis) & DIRECTION) != 0)
#	define	dsp_frames_left(dsp, axis)		((pcdsp_status(dsp, axis) & FRAMES_LEFT) != 0)
#	define	dsp_idle(dsp, axis)				(pcdsp_set_event(dsp, axis, ABORT_EVENT))
#	define	dsp_run(dsp, axis)				(pcdsp_set_event(dsp, axis, PC_STATUS_OVERRIDE))
#	define	dsp_stop(dsp, axis)				(pcdsp_set_event(dsp, axis, STOP_EVENT))
#	define	dsp_e_stop(dsp, axis)			(pcdsp_set_event(dsp, axis, E_STOP_EVENT))
#	define	dsp_clear_status(dsp, axis)		{ if (pcdsp_event(dsp, axis) > ABORT_EVENT) pcdsp_set_event(dsp, axis, PC_STATUS_OVERRIDE); }
#	define	DSP_IN_MOTION		            ( IN_SEQUENCE | FRAMES_LEFT | IN_MOTION )
#	define	DSP_DONE						( DSP_IN_MOTION | IN_POSITION )

/*	-1's in the filter_map cause the coefficient to be ignored. */
#	define	ED_IGNORE		               (-1)

#	define	MF_MOVE				            0
#	define	MF_ADD				            1
#	define	MF_DATA_AREA		            (0xFFFF)


#	define	LFIXED_SIZE						(sizeof(LFIXED) / sizeof(DSP_DM))
#	define	copy_lfixed(dest, src)			(((dest).whole = (src).whole), ((dest).frac = (src).frac))
#	define	copy_lfixed_to_fixd(dest, src)	(((dest).whole = (int16)((src).whole)), ((dest).frac = (src).frac))
#	define	inc_lfixed(nn)					if (!(++(nn).frac))		(nn).whole++
#	define	dec_lfixed(nn)					if (!(nn).frac)	(nn).whole-- ;  (nn).frac--
#	define	neg_lfixed(nn)					{ (nn).whole=-(nn).whole; (nn).frac=~(nn).frac; if ((nn).frac) (nn).whole--; }
#	define	abs_lfixed(nn)					if ((nn).whole < 0)  neg_lfixed(nn)
#	define	__dir(a, b)						((a) < (b)? -1 : ((a) == (b)? 0 : 1))
#	define	cmp_lfixed(a, b)				(((a).whole == (b).whole)? __dir((a).frac, (b).frac) : __dir((a).whole, (b).whole))
#	define	sign_lfixed(nn)					((nn).whole < 0? -1 : ((nn).whole > 0? 1 : ((nn).frac == 0? 0 : 1)))

#	define	add_lfixed(dst, src)			__add_lfixed(&(dst), &(src))
#	define	sub_lfixed(dst, src)			__sub_lfixed(&(dst), &(src))



#	define	dsp_enable(dsp, axis) 		   (pcdsp_enable((dsp), axis, FALSE))
#	define	dsp_disable(dsp, axis)		   (pcdsp_enable((dsp), axis, TRUE))


#	define	HIGH_16(uns32)					(unsigned16) (((uns32) & 0xFFFF0000L) >> 16)
#	define	LOW_16(uns32)					(unsigned16) ((uns32) & 0xFFFF)
#	define	SET_HIGH_32(u32, uns16)			(u32) |= (((unsigned32) uns16) << 16)
#	define	SET_LOW_32(u32, uns16)			(u32) |= ((unsigned32) uns16)

/*	Direction configuration stuff. */
#	define	SD_BIDIRECTIONAL				0x8000

/* Frame redirection defines */
#  define   FRAME_OUTPUT_BOARD				0
#  define   FRAME_OUTPUT_BUFFER				1
#  define   FRAME_STRUCT_SIZE				48



typedef unsigned16	DSP_DM ;
typedef unsigned24	DSP_PM ;
typedef unsigned32	DSP_BM ;	/* boot memory is in 32-bit chunks, */
								/* of which only 24 bits are used. */
typedef DSP_DM		P_DSP_DM;	/* pointer into data-memory */
typedef DSP_DM		P_DSP_PM;	/* pointer into program-memory */
typedef DSP_DM		P_DSP_BM;	/* pointer into boot-memory */



#ifdef MEI_MSOS	/* WINNT or WIN95 */
#	pragma pack(2)
#	endif

#ifdef __SC__
#	pragma SC align 1
#	endif

/*	DSP fixed-point numbers are in these formats:  */

/*	48 bit (16 whole, 32 fraction) fixed point - velocities, accels, and jerks. */
typedef struct {
	unsigned32	frac ;
	int16		whole ;
	} FIXD ;

/*	64 bit (32 whole, 32 fraction) longer-fixed point--command position */
typedef struct {
	unsigned32	frac ;
	int32		whole ;
	} LFIXED ;

#ifdef MEI_MSOS	/* WINNT or WIN95 */
#	pragma pack()
#	endif

#ifdef __SC__
#	pragma SC align
#	endif


typedef struct {
	int16		enable ;		/* TRUE to actually use UOM's. */
	double		countsPerDistance ;
	double		secondsPerPeriod ;
	double		accelPeriod ;
	} CONVERSION ;


typedef struct {
	int16		last ;
	LFIXED		pos,
		   		vel ;
	int16		gate ;
	} LAXIS ;

typedef struct __FRAME FRAME;
typedef struct __DSP DSP;

typedef struct __FRAME_BUFFER {
   int16		length;   
   int16		frames;
   int16		frames_left;   
   FRAME *		pbuffer;
   } FRAME_BUFFER;

/* there will be one SERCDATA structure per axis, indexed by axis # */
struct SERCDATA {
	unsigned16 channel;			/* service container assigned to axis */
	unsigned16 mode;			/* primary operation mode of drive: IDN 32 */
	unsigned16 drive_addr;
	unsigned16 drive_mfg;
};

struct __DSP {
	unsigned16		iobase, address, data, reset ;
	int16			*mei_dsp_base;
	int32			timer_scale;
#ifdef MEI_WINNT
	HANDLE		dsp_file;
	int16		board;
#endif
	int16		ok;							/* TRUE or FALSE */
	int16		sercos;						/* 1 if card is Sercos, 0 if not */
	DSP_DM		signature, version, option;
	int16 		axes;						/* actual number of axes. */
	P_DSP_DM	pc_event, pc_status ;		/* pointers... */
	P_DSP_DM	inptr, outptr ;				/* added @ in */
	P_DSP_DM	infree, outfree ;
	P_DSP_DM	transfer ;
	P_DSP_DM	data_struct ;
	P_DSP_DM	e_data ;
	P_DSP_DM	global_data ;

	DSP_DM		sample_clock ;
	unsigned	sample_rate ;					/* samples per second. */
	
	CONVERSION	conversion[PCDSP_MAX_AXES];/* conversion factors... */
	LAXIS		laxis[PCDSP_MAX_AXES];		/* last commanded vel/pos/whatever. */

	int16		analog_control[PCDSP_ANALOG_CHANNELS], analog_channel ;
	int16		frame_control[PCDSP_MAX_AXES],
				frame_action[PCDSP_MAX_AXES],
				flags[PCDSP_MAX_AXES] ;

	int16		jog_x, jog_y, jog_z ;

	int16 (FNTYPE *FRAME_ALLOCATE)(FRAME *dst, DSP *dsp, int16 axis);
	int16 (FNTYPE *FRAME_DOWNLOAD)(FRAME *pframe);
	FRAME_BUFFER   frame_buffer;

	struct SERCDATA sercdata[PCDSP_MAX_AXES];
	};

typedef DSP PTRTYPE * PDSP ;

#ifdef MEI_MSOS	/* WINNT or WIN95 */
#	pragma pack(2)
#	endif

#ifdef __SC__
#	pragma SC align 1
#	endif

typedef struct {										/*word offset*/
	P_DSP_DM	next ;			/*	points to DM */			/* 0 */
	DSP_DM		control;		/*	FCTL_... */				/* 1 */
	unsigned32	time ;										/* 2 */
	FIXD		jerk,										/* 4 */
				accel,										/* 7 */
				velocity ;									/* 10 */
	LFIXED	  	position ;									/* 13 */
	DSP_DM	  	trig_update ;	/* FUPD_... | FTRG_... */	/* 17 */
	DSP_DM	  	trig_action ;	/*  pc_event values... */	/* 18 */
	DSP_DM	  	output ;									/* 19 */
	} _FRAME ;	/* 20 words total. */

#ifdef MEI_MSOS	/* WINNT or WIN95 */
#	pragma pack()
#	endif

#ifdef __SC__
#	pragma SC align 1
#	endif

struct __FRAME{
	_FRAME		f ;
	PDSP		dsp ;	   		/*	dsp to which this frame is attached */
	int16		axis ;			/*	0..(DSP.axes-1) */
	P_DSP_DM	current ;		/*	DM allocated address */
	};

typedef FRAME PTRTYPE * PFRAME ;


/*	frame_allocate function. */
typedef int16 (FNTYPE * FRAME_IDLE) (PDSP, int16); /* return != DSP_OK to terminate... */

typedef LFIXED PTRTYPE * PFIXED ;
typedef CONVERSION PTRTYPE * PCONVERSION ;
typedef LAXIS PTRTYPE * PLAXIS ;
typedef DSP_DM PTRTYPE * PDSP_DM ;
typedef DSP_BM PTRTYPE * PDSP_BM ;


# ifndef NO_PROTOTYPES

#  ifdef __cplusplus
extern "C" {
#	endif

/* idsp.c: */
extern LFIXED DATATYPE fixed_zero;
int16 FNTYPE pcdsp_init_board_comm(PDSP pdsp, unsigned16 iobase);
int16 FNTYPE pcdsp_reset(PDSP dsp);
int16 FNTYPE pcdsp_sick(PDSP dsp, int16 axis);
int16 FNTYPE pcdsp_init_axis_check(PDSP dsp, int16 axis);
int16 FNTYPE pcdsp_init_check(PDSP dsp);
int16 FNTYPE pcdsp_set_address(PDSP dsp, unsigned iobase);
int16 FNTYPE pcdsp_read_pointers(PDSP pdsp);
int16 FNTYPE pcdsp_set_last(PDSP pdsp, int16 axis, int16 last, PFIXED lastvalue);
int16 FNTYPE pcdsp_interrupt_enable(PDSP pdsp, int16 enable);
int16 FNTYPE pcdsp_arm_latch(PDSP pdsp, int16 enable);
int16 FNTYPE pcdsp_latch_status(PDSP pdsp);
int16 FNTYPE pcdsp_jog_axis(PDSP pdsp, int16 axis, int16 jog_channel, DSP_DM c, DSP_DM d, DSP_DM m1, DSP_DM m2);
int16 FNTYPE pcdsp_jog_enable(PDSP pdsp, int16 axis);
int16 FNTYPE pcdsp_jog_disable(PDSP pdsp, int16 axis);
int16 FNTYPE pcdsp_set_integrator(PDSP pdsp, int16 axis, int16 value);
int16 FNTYPE pcdsp_axes(PDSP pdsp);

#	ifndef	DSP_MACROS
unsigned16 FNTYPE DSP_IN(int16 pp);
void FNTYPE DSP_OUT(int16 pp, unsigned16 vv);
unsigned char FNTYPE DSP_INB(int16 pp);
void FNTYPE DSP_OUTB(int16 pp, unsigned char vv);
#	endif

#ifdef MEI_DD
/* DD_INTERRUPT is only used in the DOS model. */
#	define	DD_INTERRUPT			(0x80)

# ifndef MEI_WINNT
#	define	IOCTL_PCDSP_INIT		0x0F
#	define	IOCTL_PCDSP_OUT			0x10
#	define	IOCTL_PCDSP_IN			0x11
#	define	IOCTL_PCDSP_ALLOCATE	0x12
#	define	IOCTL_PCDSP_DOWNLOAD	0x13
#	define	IOCTL_PCDSP_BLOCK		0x14		/* transfer block */
#	define	IOCTL_PCDSP_WAIT		0x15
#	define	IOCTL_GET_DATA			0x16
#	endif

int dd_call(int function, int16 * ptr) ;
#	endif


/* ifixed.c: */
void __add_lfixed(PFIXED dst, PFIXED src);
void __sub_lfixed(PFIXED dst, PFIXED src);


/* iframe.c:  internal PC-DSP frame manipulation routines. */
extern int16 DATATYPE frame_wait;	/* value returned when free list is empty. */
int16 FNTYPE __default_frame_idle(PDSP dsp, int16 axis) ; /* default value for frame_idle. */
extern FRAME_IDLE DATATYPE frame_idle; /* function called when free list is empty. */
extern int16 DATATYPE __frames_allocated, __frames_downloaded;
void FNTYPE frame_clear(PFRAME dst) ; /* zero all fields in a frame. */
int16 FNTYPE frame_allocate(PFRAME dst, PDSP dsp, int16 axis); /* reserve DSP memory for a frame. */
int16 FNTYPE buffer_allocate(PFRAME dst, PDSP dsp, int16 axis); /* reserve DSP memory for a frame. */
int16 FNTYPE _frame_download(PFRAME src, int16 connect); /* copy a frame int16o allocated DSP memory. */
int16 FNTYPE frame_download(PFRAME src); 
int16 FNTYPE buffer_download(PFRAME src); 
int16 FNTYPE frame_download_and_detach(PFRAME src); 
int16 FNTYPE pcdsp_set_gate(PDSP pdsp, int16 andgate, int16 orgate);
int16 FNTYPE dsp_set_gate(PDSP pdsp, int16 axis);
int16 FNTYPE dsp_reset_gate(PDSP pdsp, int16 axis) ;
int16 FNTYPE dsp_clear_gate(PDSP pdsp, int16 axis);
int16 FNTYPE __frame_uncompress(PFRAME pframe, PDSP_DM pdsp_dm) ;
DSP_DM * FNTYPE __frame_compress(PFRAME pframe) ;


/* imemory.c: */
extern unsigned DATATYPE _pcdsp_timeout ; /* number of iterations to wait for transfer block */
int16 FNTYPE pcdsp_transfer_block(PDSP dsp, int16 read, int16 program, P_DSP_DM addr, unsigned length, PDSP_DM buffer);
int16 FNTYPE pcdsp_read_dm(PDSP dsp, P_DSP_DM addr, unsigned len, PDSP_DM dest);
int16 FNTYPE pcdsp_write_dm(PDSP dsp, P_DSP_DM addr, unsigned len, PDSP_DM src);
int16 FNTYPE pcdsp_read_pm(PDSP dsp, P_DSP_DM addr, unsigned len, PDSP_DM dest);
int16 FNTYPE pcdsp_write_pm(PDSP dsp, P_DSP_DM addr, unsigned len, PDSP_DM src);


/* llframe.c - frames in DSP memory. */
int16 FNTYPE pcdsp_frame_command(PDSP pdsp, char command, PFRAME f, void PTRTYPE * parm);
int16 C_FN pcdsp_frame(PDSP pdsp, PFRAME f, P_CHAR c, ...) ;
int16 FNTYPE pcdsp_read_frame(PDSP pdsp, P_DSP_DM addr, PFRAME frame); /*	read a frame from any address.	*/
int16 FNTYPE pcdsp_write_frame(PDSP pdsp, PFRAME frame); /*	write a frame without attaching it to an axis.	*/
int16 FNTYPE pcdsp_get_frame(PDSP pdsp, P_DSP_DM addr, PFRAME frame); /*	Compute which axis we're connected to.	*/
int16 FNTYPE pcdsp_dwell_frame(PDSP pdsp, int16 axis) ;


/* llpos.c - low-level axis feedback */
int16 FNTYPE pcdsp_command(PDSP pdsp, int16 axis, PFIXED dst); /*	gets the current commanded position int16o dst. */
int16 FNTYPE pcdsp_set_command(PDSP pdsp, int16 axis, PFIXED src); /*	sets the current command position. */
int16 FNTYPE pcdsp_actual(PDSP pdsp, int16 axis, PFIXED dst); /*	gets the current encoder position int16o dst. */
int16 FNTYPE pcdsp_set_actual(PDSP pdsp, int16 axis, PFIXED src); /*	sets the current actual position register. */
int16 FNTYPE pcdsp_set_position(PDSP pdsp, int16 axis, PFIXED pos);
int16 FNTYPE pcdsp_get_velocity(PDSP pdsp, int16 axis, PFIXED dst);
int16 FNTYPE pcdsp_set_vel(PDSP pdsp, int16 axis, PFIXED src) ;
int16 FNTYPE pcdsp_get_acceleration(PDSP pdsp, int16 axis, PFIXED dst) ;
int16 FNTYPE pcdsp_set_accel(PDSP pdsp, int16 axis, PFIXED src);
int16 FNTYPE pcdsp_get_jerk(PDSP pdsp, int16 axis, PFIXED dst);
int16 FNTYPE pcdsp_set_jerk(PDSP pdsp, int16 axis, PFIXED src);
int16 FNTYPE pcdsp_get_error(PDSP pdsp, int16 axis, PFIXED dst) ;
int16 FNTYPE pcdsp_latched(PDSP pdsp, int16 axis, PFIXED dst) ;


/* llio.c - low-level input/output drivers.  */
int16 FNTYPE dsp_port_address(int16 port, P_INT address) ;
int16 FNTYPE pcdsp_get_io(PDSP dsp, int16 port);
int16 FNTYPE pcdsp_set_io(PDSP dsp, int16 port, int16 value);
int16 FNTYPE pcdsp_init_io(PDSP dsp, int16 port, int16 config);
int16 FNTYPE pcdsp_config_analog(PDSP dsp, int16 ch, int16 diff, int16 bipolar);
int16 FNTYPE pcdsp_start_analog(PDSP dsp, int16 channel) ;
int16 FNTYPE pcdsp_read_analog(PDSP dsp) ;
int16 FNTYPE pcdsp_switches(PDSP dsp, int16 axis);
int16 FNTYPE pcdsp_enable(PDSP dsp, int16 axis, int16 enable); /*	Amp-enable output. */
int16 FNTYPE pcdsp_enabled(PDSP dsp, int16 axis, P_INT enabled);
int16 FNTYPE pcdsp_step_speed(PDSP dsp, int16 axis, P_INT spd);
int16 FNTYPE pcdsp_set_step_speed(PDSP dsp, int16 axis, int16 spd);
int16 FNTYPE pcdsp_closed_loop(PDSP dsp, int16 axis);
int16 FNTYPE pcdsp_set_closed_loop(PDSP dsp, int16 axis, int16 closed);
int16 FNTYPE pcdsp_home_index_config(PDSP dsp, int16 axis);
int16 FNTYPE pcdsp_set_home_index_config(PDSP dsp, int16 axis, int16 config);
int16 FNTYPE pcdsp_init_timer(PDSP dsp, int16 counter, int16 mode);
int16 FNTYPE pcdsp_set_timer(PDSP dsp, int16 channel, unsigned16 t);
int16 FNTYPE pcdsp_get_timer(PDSP pdsp, int16 channel) ;
int16 FNTYPE pcdsp_io_mon(int16 port, int16 *status);
int16 FNTYPE pcdsp_get_io_mon_mask(int16 port, int16 *mask, int16 *value);
int16 FNTYPE pcdsp_set_io_mon_mask(int16 port, int16 mask, int16 value);
int16 FNTYPE pcdsp_clear_io_mon(void);
int16 FNTYPE pcdsp_io_changed(void);


/* llef.c - low level exception frames.  */
int16 FNTYPE pcdsp_read_ef(PDSP pdsp, PFRAME frame, int16 axis, int16 e_frame);
int16 FNTYPE pcdsp_write_ef(PDSP pdsp, PFRAME frame);


/* llsetvel.c - just downloads frames for a controlled velocity move.  */
int16 FNTYPE pcdsp_load_velocity(PDSP pdsp, int16 axis, PFIXED newvel, PFIXED newaccel);


/* llmove.c - an all integer trapezoidal profile move routine.  */
int16 FNTYPE pcdsp_ll_move(PDSP pdsp, int16 axis, PFIXED vel, PFIXED veltime, PFIXED accel, PFIXED acceltime);
int16 LOCAL_FN dsp_set_last_command(PDSP pdsp, int16 axis, double final);
double LOCAL_FN dsp_last_command(PDSP pdsp, int16 axis);
int16 LOCAL_FN idsp_check_3params(double * x, double * v, double * a);
int16 LOCAL_FN idsp_check_4params(double * x, double * v, double * a, double * j);

/* mlerror.c */
P_CHAR FNTYPE _error_msg(int16 code);
int16 FNTYPE _dsp_error(void);

/*	llstatus.c */
int16 FNTYPE pcdsp_state(PDSP pdsp, int16 axis);
int16 FNTYPE pcdsp_status(PDSP pdsp, int16 axis);
int16 FNTYPE pcdsp_event(PDSP pdsp, int16 axis);
int16 FNTYPE pcdsp_set_event(PDSP pdsp, int16 axis, DSP_DM event);
/* (pcdsp_event(dsp, axis) & (ABORT_EVENT-1)) | PC_STATUS_OVERRIDE)) */


/*	llpid.c */
int16 FNTYPE pcdsp_set_filter(PDSP dsp, int16 axis, PDSP_DM value);
int16 FNTYPE pcdsp_get_filter(PDSP dsp, int16 axis, PDSP_DM dest);
int16 FNTYPE pcdsp_set_aux_filter(PDSP dsp, int16 axis, PDSP_DM value);
int16 FNTYPE pcdsp_get_aux_filter(PDSP dsp, int16 axis, PDSP_DM dest);


/*	llconfig.c ... parameter is CL_... */
typedef enum { MaskAction, ValueAction, WholeAction, Mask } CONFIGTYPES ;
P_DSP_DM FNTYPE pcdsp_address(PDSP pdsp, int16 axis, int16 parameter, CONFIGTYPES * type) ;
int16 FNTYPE pcdsp_set_config_struct(PDSP pdsp, int16 axis, int16 parameter, PFIXED pfixed, PDSP_DM paction) ;
int16 FNTYPE pcdsp_get_config_struct(PDSP pdsp, int16 axis, int16 parameter, PFIXED pfixed, PDSP_DM paction) ;
int16 FNTYPE pcdsp_set_boot_config_struct(PDSP pdsp, int16 axis, int16 parameter, PFIXED pfixed, PDSP_DM paction) ;
int16 FNTYPE pcdsp_get_boot_config_struct(PDSP pdsp, int16 axis, int16 parameter, PFIXED pfixed, PDSP_DM paction) ;


/* llbm.c */
int16 FNTYPE pcdsp_read_bm(PDSP dsp, P_DSP_DM addr, unsigned len, PDSP_DM dest);
int16 FNTYPE pcdsp_write_bm(PDSP dsp, P_DSP_DM addr, unsigned len, PDSP_DM src);
unsigned16 FNTYPE pcdsp_get_checksum(PDSP pdsp); /* read stored checksum. */
unsigned16 FNTYPE pcdsp_checksum(PDSP pdsp);	/* calculate new checksum. */
int16 FNTYPE pcdsp_write_checksum(PDSP pdsp, unsigned16 sum, int16 save);
int16 FNTYPE pcdsp_check(PDSP pdsp);
int16 FNTYPE dsp_write_bm(PDSP dsp, P_DSP_DM addr, unsigned len, P_CHAR src);
int16 FNTYPE dsp_read_bm(PDSP dsp, P_DSP_DM addr, unsigned len, P_CHAR dest);
int16 FNTYPE pcdsp_save(PDSP pdsp);	/* non-volatile save */


/* llfirm.c */
int16 FNTYPE check_firmware(char *buf);
int16 FNTYPE pcdsp_upload_firmware(char *bm_buf);
int16 FNTYPE pcdsp_load_firmware(char *bm_buf);


/* mldsp.c */
extern DSP DATATYPE dsp ; /*	The global/default dsp varaible. */
extern PDSP DATATYPE dspPtr ;
int16 FNTYPE set_dspPtr(PDSP pdsp);
int16 FNTYPE get_dspPtr(PDSP PTRTYPE * pdsp);
int16 FNTYPE pcdsp_init(PDSP pdsp, unsigned16 iobase);
int16 FNTYPE dsp_rset(void);
int16 FNTYPE inc_frame_count(int16 axis);
int16 FNTYPE set_frame_output(int16 output_type, int16 length, char * buffer);
int16 FNTYPE get_frames_in_buffer(int16 * frames);
int16 FNTYPE download_frames_from_buffer(int16 frames, char * buffer);

/* mlframe.c - Medium-level PC-DSP frame manipulation routines. */
int16 C_FN EXPORT frame_m(PFRAME f, P_CHAR c, ...);

/* mluom.c - units of measure conversion.  */
typedef int16 (FNTYPE * DSP_FIXER)(PDSP, int16, double, PFIXED) ;
typedef int16 (FNTYPE * DSP_DOUBLER) (PDSP, int16, PFIXED, P_DOUBLE) ;

/* mlstatus.c - status conversions */
extern int16 decode_source[32];
extern int16 decode_event[16];
extern int16 encode_event[16];
int16 FNTYPE pcdsp_in_sequence(PDSP pdsp, int16 axis);
int16 FNTYPE pcdsp_frames_left(PDSP pdsp, int16 axis) ;


double FNTYPE ipcdsp_double(PFIXED fdsp);
void FNTYPE ipcdsp_fixed(double src, PFIXED dst);
int16 FNTYPE ipcdsp_fixed_flat(PDSP pdsp, int16 axis, double src, PFIXED dst) ;
int16 FNTYPE ipcdsp_fixed_jerk(PDSP pdsp, int16 axis, double src, PFIXED dst);
int16 FNTYPE ipcdsp_fixed_accel(PDSP pdsp, int16 axis, double src, PFIXED dst);
int16 FNTYPE ipcdsp_fixed_vel(PDSP pdsp, int16 axis, double src, PFIXED dst);
int16 FNTYPE ipcdsp_fixed_pos(PDSP pdsp, int16 axis, double src, PFIXED dst);
int16 FNTYPE ipcdsp_fixed_time(PDSP pdsp, int16 axis, double src, PFIXED dst);
int16 FNTYPE pcdsp_init_conversion(PDSP dsp, int16 axis);
int16 FNTYPE pcdsp_set_aconversion(PDSP dsp, int16 axis, double cpd, double spp, double app);
int16 FNTYPE pcdsp_get_aconversion(PDSP pdsp, int16 axis, P_DOUBLE cpd, P_DOUBLE spp, P_DOUBLE app);
int16 FNTYPE pcdsp_set_conversion(PDSP dsp, int16 axis, double cpd, double spp);
int16 FNTYPE ipcdsp_double_flat(PDSP pdsp, int16 axis, PFIXED src, P_DOUBLE dst) ;
int16 FNTYPE ipcdsp_double_jerk(PDSP pdsp, int16 axis, PFIXED src, P_DOUBLE dst);
int16 FNTYPE ipcdsp_double_accel(PDSP pdsp, int16 axis, PFIXED src, P_DOUBLE dst);
int16 FNTYPE ipcdsp_double_vel(PDSP pdsp, int16 axis, PFIXED src, P_DOUBLE dst);
int16 FNTYPE ipcdsp_double_pos(PDSP pdsp, int16 axis, PFIXED src, P_DOUBLE dst);
int16 FNTYPE ipcdsp_double_time(PDSP pdsp, int16 axis, PFIXED src, P_DOUBLE dst);

/* mlmove.c */
int16 FNTYPE dsp_load_move(PDSP pdsp, int16 axis, double final, double v, double a);
int16 LOCAL_FN ipcdsp_last_frame(PDSP dsp, int16 axis, double x) ;

/* llink.c */
int16 FNTYPE pcdsp_set_link(PDSP dsp, int16 axis, P_DSP_DM master_addr, PFIXED ratio) ;
int16 FNTYPE pcdsp_endlink(PDSP pdsp, int16 slave);


/* bconfig.c */
int16 FNTYPE pcdsp_get_boot_filter(PDSP pdsp, int16 axis, PDSP_DM coeffs) ;
int16 FNTYPE pcdsp_set_boot_filter(PDSP pdsp, int16 axis, PDSP_DM coeffs) ;
int16 FNTYPE pcdsp_get_boot_io(PDSP pdsp, int16 port) ;
int16 FNTYPE pcdsp_set_boot_io(PDSP pdsp, int16 port, int16 value) ;
int16 FNTYPE pcdsp_init_boot_io(PDSP pdsp, int16 port, int16 config) ;
int16 FNTYPE pcdsp_read_boot_ef(PDSP pdsp, PFRAME frame, int16 axis, int16 e_frame) ;
int16 FNTYPE pcdsp_write_boot_ef(PFRAME frame) ;



#	ifdef DEBUG
/* mldebug.c - contains routines for debugging your programs.  */
int16 FNTYPE pcdsp_dump_frame(PFRAME pframe) ;
#	  endif

int16 FNTYPE dsp_setup(unsigned16 addr);

/* mlef.c */
int16 FNTYPE pcdsp_config_ef(PDSP dsp, int16 axis, int16 ef, int16 set_int);


/* mlconfig.c */
extern int16 idsp_filter_map[] ;
extern int16 idsp_aux_filter_map[] ;

int16 FNTYPE pcdsp_stepper(PDSP dsp, int16 axis);
int16 FNTYPE pcdsp_set_unipolar(DSP * dsp, int16 axis, int16 state);
int16 FNTYPE pcdsp_set_stepper(PDSP dsp, int16 axis, int16 stepper);


/* mldspf.c */
int16 FNTYPE dsp_move_frame(PFRAME f, int16 axis, int16 opcode, P_DSP_DM destination,
		P_DSP_DM source1, P_DSP_DM source2, int16 words, PDSP_DM buffer) ;
int16 FNTYPE _dsp_io_frame(int16 axis, int16 address, int16 ormask, int16 andmask) ;
int16 FNTYPE _dsp_io_trigger(int16 axis, int16 addr, int16 bit, int16 state) ;

/* mlloop.c */
int16 FNTYPE stop_frame_loops(int16 n_axes, int16 *axis_map, DSP_DM *last_frame);
int16 FNTYPE release_loop_frames(int16 n_axes, int16 *axis_map, DSP_DM *last_frame);
int16 FNTYPE e_stop_loop_and_release_frames(int16 axis);

/* mlspline.c */
int16 FNTYPE start_spline_motion(int16 n_axes, int16 *axis_map);
int16 FNTYPE load_spline_motion(int16 n_axes, int16 *axis_map, int16 n_points, 
                           long *(*point_list), double *g, int16 *end_flag);
int16 FNTYPE load_periodic_motion(int16 n_axes, int16 *axis_map, int16 n_points, 
                      long *(*point_list), double *g, DSP_DM *last_frame);


/* idsp.c (again) */
extern int16 dsp_io_problem ;
#ifdef MEI_OS2WARP
int16 FNTYPE dsp_open(void);
void FNTYPE dsp_close(void);
#endif
#ifdef	MEI_WINNT
#define MaxBoards 8
extern LPCSTR device_name[MaxBoards];

#  define PCDSP_TYPE 40000
#  define IOCTL_PCDSP_READ_PORT_UCHAR \
    CTL_CODE( PCDSP_TYPE, 0x900, METHOD_BUFFERED, FILE_READ_ACCESS )
#  define IOCTL_PCDSP_READ_PORT_USHORT \
    CTL_CODE( PCDSP_TYPE, 0x901, METHOD_BUFFERED, FILE_READ_ACCESS )
#  define IOCTL_PCDSP_READ_PORT_ULONG \
    CTL_CODE( PCDSP_TYPE, 0x902, METHOD_BUFFERED, FILE_READ_ACCESS )
#  define IOCTL_PCDSP_WRITE_PORT_UCHAR \
    CTL_CODE(PCDSP_TYPE,  0x910, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#  define IOCTL_PCDSP_WRITE_PORT_USHORT \
    CTL_CODE(PCDSP_TYPE,  0x911, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#  define IOCTL_PCDSP_WRITE_PORT_ULONG \
    CTL_CODE(PCDSP_TYPE,  0x912, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#	define	IOCTL_PCDSP_INIT		\
    CTL_CODE(PCDSP_TYPE,  0x920, METHOD_BUFFERED, FILE_READ_ACCESS)
#	define	IOCTL_PCDSP_OUT			\
    CTL_CODE(PCDSP_TYPE,  0x921, METHOD_BUFFERED, FILE_READ_ACCESS)
#	define	IOCTL_PCDSP_IN			\
    CTL_CODE(PCDSP_TYPE,  0x922, METHOD_BUFFERED, FILE_READ_ACCESS)
#	define	IOCTL_PCDSP_ALLOCATE	\
    CTL_CODE(PCDSP_TYPE,  0x923, METHOD_BUFFERED, FILE_READ_ACCESS)
#	define	IOCTL_PCDSP_DOWNLOAD	\
    CTL_CODE(PCDSP_TYPE,  0x924, METHOD_BUFFERED, FILE_READ_ACCESS)
#	define	IOCTL_PCDSP_BLOCK		\
    CTL_CODE(PCDSP_TYPE,  0x925, METHOD_BUFFERED, FILE_READ_ACCESS)
#	define	IOCTL_PCDSP_WAIT		\
    CTL_CODE(PCDSP_TYPE,  0x926, METHOD_BUFFERED, FILE_READ_ACCESS)
#	define	IOCTL_GET_DATA			\
    CTL_CODE(PCDSP_TYPE,  0x927, METHOD_BUFFERED, FILE_READ_ACCESS)

/* Define Interrupt Sources */
#define MEI_IO_MON   1
#define MEI_FRAME    2
#define MEI_EVENT    3
#define MEI_TOGGLE   4

typedef struct  _WRITE_INPUT {
   ULONG    PortNumber;     /* Port # to write to */
   USHORT   ShortData;
}   WRITE_INPUT;

typedef struct _WRITE_INPUT_CHAR{
   ULONG    PortNumber;
   UCHAR    CharData;
} WRITE_INPUT_CHAR;

typedef struct _INTERRUPT_DATA{
   USHORT   InterruptType;
   USHORT   Axis;
} INTERRUPT_DATA, *PINTERRUPT_DATA;

typedef struct _GET_DATA_DATA{
	USHORT	len;
	USHORT	addr;
	USHORT	datum_len;
	USHORT	offset;
} GET_DATA_DATA, *PGET_DATA_DATA;

int16 FNTYPE dsp_open(void) ;
int16 FNTYPE mei_sleep_until_interrupt(HANDLE file_handle, USHORT Axis, USHORT InterruptType);
int16 FNTYPE get_data_from_driver(int16 address, int16 data_offset, int16 datum_len, 
	int16 pieces_of_data, int16 *data);
void __cdecl dsp_close(void) ;
#	endif

#ifdef MEI_SWAPLONG
int16 FNTYPE mei_swaplong(unsigned32 *p);
int16 FNTYPE mei_swapfixed(LFIXED *p);
#endif /* MEI_SWAPLONG */

#  ifdef __cplusplus
	} ;
#	endif

#  endif

#	endif

