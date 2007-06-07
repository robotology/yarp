/*
	pcdsp.h
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef	__PCDSP_H
#	define	__PCDSP_H

#define	PCDSP_VERSION		240			/* Firmware Compatibility */
#define	PCDSP_ASCII_VERSION	"2.5.02"	/* Software Version */

#define MEI_CHECK_FIFO
/*	Default library is compiled under the large memory model:  Both
	code and data are 'far'.  If you are using another memory
	model library, just change the definitions of DATA_DIST (data distance)
	and CODE_DIST (code distance).  By keeping DATA_DIST and CODE_DIST consistant
	with the library you are linking to, you can compile your
	program using any model you like and still have it work.

	If you re-compile the library under a different model, then be sure
	to re-define DATA_DIST and CODE_DIST.
*/
#	ifdef MEI_BCPP32
#		define MEI_ASM
#	endif

#	ifdef MEI_VXI
#		ifdef PCDSP_DLL
#			define VXIWIN
#		else
#			define VXIDOS
#		endif
#		define __PROTOTYPES__
#		define __I386__
#		define BINARY_COMPATIBLE
#		define MEI_32BIT_OS
#		define MEI_SEP_RESET
#	endif

#	ifdef MEI_WINNT_BC		/* Borland Compiler */
#		define MEI_WINNT
#		define PCDSP_DLL
#	endif

#	ifdef	MEI_ASM
#		define	MEI_NO_DIST
#		define	MEI_32BIT_OS
#		define	HUGE
#	endif

#	ifdef	MEI_LYNX
#		define	MEI_NO_DIST
#     define   MEI_32BIT_OS
#     define   MEI_ASM
#	endif

#	ifdef	MEI_OS2
#		define	MEI_NO_DIST
#     define   MEI_32BIT_OS
#	endif

#	ifdef	MEI_OS2WARP
#		define	MEI_NO_DIST
#     define   MEI_32BIT_OS
#	endif

#	ifdef	MEI_OS9000
#		define	MEI_NO_DIST
#	endif

#	ifdef	MEI_OS9
#		define	MEI_NO_DIST
#		define	MEI_IOBASE
# 		define  MEI_O_BENDIAN
#		define  MEI_SWAPLONG
#		define  MEI_MC680X0_CPU
#	endif

#	ifdef MEI_VW
#		define MEI_NO_DIST
#     	define MEI_32BIT_OS
#     	ifdef MEI_MC680X0_CPU
#        	define MEI_O_BENDIAN
#			define MEI_SWAPLONG
#     	endif
#		ifdef MEI_I80X86_CPU
#			define MEI_IOMAPPED
#		endif
#	endif

#	ifdef MEI_VRTXOS
#		define	MEI_NO_DIST
#		define	MEI_32BIT_OS
#		define	MEI_O_BENDIAN
#		define	MEI_SEP_RESET
#		define	MEI_SWAPLONG
#	endif

#	ifdef __WATCOMC__
#		define	MEI_NO_DIST
#     define   MEI_32BIT_OS
#	endif

#	ifdef _NI_i386_
#		define	MEI_NO_DIST
#     define   MEI_32BIT_OS
#	endif

#	ifdef __SC__
#		define	MEI_NO_DIST
#     define   MEI_32BIT_OS
#	endif

/* Unless MEI_WIN95 is declared by the user, the compiler will build for WINNT */
#	ifdef MEI_WIN95
#		define	MEI_NO_DIST
#		define	MEI_32BIT_OS
#		define	MEI_MSOS
#	endif

/* Windows NT */
#	ifndef MEI_MSOS
#		ifdef _MSC_VER
#			ifdef _M_IX86
#				define	MEI_WINNT
#				define	MEI_MSOS
#				define	MEI_IOBASE
#			endif
#		endif
#	endif

#	ifdef MEI_MSOS
#		define FNTYPE __stdcall
#	endif

#	ifdef MEI_WINNT
#		ifndef MEI_EPC1
#			define   MEI_DD
#		endif
#		ifndef MEI_DRIVER
#			include <windows.h>
#		endif
#		define	MEI_NO_DIST
#     define   MEI_32BIT_OS
#	endif

#	ifdef	MEI_NO_DIST
#		define	DATA_DIST
#		define	CODE_DIST
#		define	NO_AMBIGUITY
#	endif

#	ifndef	DATA_DIST
#		define	DATA_DIST		far
#	endif
#  ifndef	CODE_DIST
#		define	CODE_DIST		far
#	endif

#	define	DATATYPE			DATA_DIST
#	define	PTRTYPE				DATA_DIST

/*	Some functions (like frame_m(...)) must be C_FN types.
	However, FNTYPE can be whatever the current (default C_FN)
	function parameter types are.
*/

# ifdef __QNX__
#	define	NO_AMBIGUITY
#	endif

/*	No ambiguity means that there is no 'pascal' function type; therefore
	no cdecl or pascal keywords are necessary.
*/
# ifndef NO_AMBIGUITY
#	define	C_FN				CODE_DIST cdecl
#	define	PAS_FN				CODE_DIST pascal
#  else	/* NO_AMBIGUITY is defined; no distinction between C_FN and PAS_FN. */
#	define	C_FN
#	define	PAS_FN
#  endif

#	ifndef LOCAL_FN
#		define	LOCAL_FN
#		endif

#	ifdef	PCDSP_DLL
#		define	EXPORT         _export
#		define	FNTYPE			PAS_FN _export
#else
#	ifdef	_Windows
#		ifndef	PCDSP_LIB
#		  define	FNTYPE			PAS_FN
#		else
#		  define	FNTYPE
#		endif
#	  endif
#endif

#	ifndef EXPORT
#		define	EXPORT	
#		endif

#	ifndef	FNTYPE
#		define	FNTYPE			C_FN
#		endif



#	ifdef	MEI_OS9000
#	include <types.h>
typedef unsigned char	unsigned8;
typedef unsigned short	unsigned16;
typedef unsigned long	unsigned24;
typedef unsigned long	unsigned32;
#	define	MEI_TYPES
#	endif

#	ifdef	MEI_OS9
#	include <types.h>
typedef unsigned char	unsigned8;
typedef unsigned short	unsigned16;
typedef unsigned long	unsigned24;
typedef unsigned long	unsigned32;
#	define	MEI_TYPES
#	endif

#ifdef   MEI_32BIT_OS
typedef char			int8 ;
typedef unsigned char	unsigned8 ;
typedef short			int16 ;
typedef unsigned short	unsigned16 ;
typedef long			int24 ;
typedef unsigned long	unsigned24 ;
typedef long			int32 ;
typedef unsigned long	unsigned32 ;
#	define	MEI_TYPES
#	endif

#	ifndef MEI_TYPES
typedef char			int8 ;
typedef unsigned char	unsigned8 ;
typedef int			int16 ;
typedef unsigned int	unsigned16 ;
typedef long			int24 ;
typedef unsigned long	unsigned24 ;
typedef long			int32 ;
typedef unsigned long	unsigned32 ;
#	endif

/*	Some simplified typedefs. */
typedef double PTRTYPE * P_DOUBLE ;
typedef int16 PTRTYPE * P_INT ;
typedef char PTRTYPE * P_CHAR ;



/*	TRUE and FALSE are for assignment purposes only. */
# ifndef FALSE
#	define FALSE			0
#	define TRUE				1
#  endif

#  define   HIGH                    (int16)1
#  define   LOW                     (int16)0

/*	Default base address. */
#	define	PCDSP_BASE				(int16)0x0300
                              

/*	Maximum possible number of axes */
#	define	PCDSP_MAX_AXES		      8

/* IO selection values */
#	define	IO_INPUT			         (int16)0   /* sets port as input */
#	define	IO_OUTPUT			      (int16)1   /* sets port as output */
/*	PORT_n are used to simplify 8255 port references. */
#  define   PORT_A                  (int16)0x00
#  define   PORT_B                  (int16)0x01
#  define   PORT_C                  (int16)0x02

/*	More PC-Status bits... (see pcdsp.h for the others (STOP, ...)) */
#  define   NO_EVENT                _NULL_STATE	/* no event... just set status to PC_COMMAND_ID. */
#  define   NEW_FRAME               _STATE_2 /* run the next frame. */
#  define   CHECK_FRAMES		      _STATE_6 /* check for out of frames */
#  define   STOP_EVENT              _STATE_8 /* pc event value */
#  define   E_STOP_EVENT            _STATE_10 /* pc event value */
#  define   ABORT_EVENT             _STATE_14 /* pc event value */

/*	axis_status(...) returned bits */
#	define	IN_SEQUENCE			      (int16)0x0010
#	define	IN_POSITION			      (int16)0x0020
#	define	IN_MOTION			      (int16)0x0040
#	define	DIRECTION			      (int16)0x0080
#	define	FRAMES_LEFT			      (int16)0x0100


/*	Digital Filter Coefficients */

//#	define	COEFFICIENTS            10 /* elements expected by (get|set)_filter */
//#	define	DF_P  				      0 /* proportional term */
//#	define	DF_I	   			      1 /* integration term */
//#	define	DF_D		   		      2 /* derivative--damping term */
//#	define	DF_ACCEL_FF             3 /* acceleration feed forward */
//#	define	DF_VEL_FF               4 /* velocity feed forward */
//#	define	DF_I_LIMIT              5 /* Integration summing limit */
//#	define	DF_OFFSET               6 /* to zero DAC output */
//#	define	DF_DAC_LIMIT		      7 /* 0..32767->0-10v; max DAC output possible.*/
//#	define	DF_SHIFT                8 /* 2^(-n) divisor */
//#	define	DF_FRICT_FF			      9 /* Friction Feedforward */

//#  define   AUX_FILTER_COEFFS       10

/*	Error Codes...		*/
#	define	MAX_ERROR_LEN		      150 /* maximum length for error message. */

#	define	DSP_OK					0 /* no problems. */
#	define	DSP_NOT_INITIALIZED     1 /* be sure to call dsp_init(...) */
#	define	DSP_NOT_FOUND			2 /* illegal I/O base or DSP firmware */
#	define	DSP_INVALID_AXIS		3
#	define	DSP_ILLEGAL_ANALOG      4 /* analog channel < 0 or > 7 */
#	define	DSP_ILLEGAL_IO          5 /* illegal i/o port. */
#	define	DSP_OUT_OF_MEMORY       6 /* out of internal DSP memory. */
#	define	DSP_FRAME_UNALLOCATED	7 /* download of unallocated frame */
#	define	DSP_ILLEGAL_PARAMETER	8 /* move with zero accel or vel. */
#	define	DSP_ILLEGAL_CONVERSION	9 /* zero CountsPerDistance or SecondsPerPeriod */
#	define	DSP_FRAME_NOT_CONNECTED 10 /* unload of unconnected frame. */
#	define	DSP_FIRMWARE_VERSION    11 /* PC Library/DSP firmware version mismatch */
#	define	DSP_ILLEGAL_TIMER       12 /* invalid 8254 timer selected */
#	define	DSP_STRUCTURE_SIZE		13 /* the size of a frame must be FRAME_SIZE words. */
#	define	DSP_TIMEOUT_ERROR       14 /* wait for transfer block exceeded allowable range. */
#	define	DSP_RESOURCE_IN_USE		15 /* some global variable is already being used. */
#	define	DSP_CHECKSUM			16 /* boot memory is corrupt. */
#	define	DSP_CLEAR_STATUS		17 /* Can't clear status now. */
#	define	DSP_NO_MAP              18 /* map axes not called */
#	define	DSP_NO_ROOM             19 /* no room in FIFO buffer */
#	define	DSP_BAD_FIRMWARE_FILE   20 /* specified firmware file is bad or cannot be opened */
#	define	DSP_ILLEGAL_ENCODER_CHANNEL	21 /* nonexistant encoderchannel specified */
#	define	DSP_FUNCTION_NOT_AVAILABLE	22 /* Function is no longer available */
#	define	DSP_NO_EXTERNAL_BUFFER		23 /* External buffer is full or does not exist */
#	define	DSP_NT_DRIVER				24
#	define	DSP_FUNCTION_NOT_APPLICABLE	25
#	define	DSP_NO_DISTANCE				26 /* no change in distance */
#	define	DSP_FIRMWARE_CHECKSUM		27 /* firmware buffer error */
#	define	DSP_SERCOS_SLAVE_ERROR		100
#	define	DSP_SERCOS_INVALID_PARAM	101
#	define	DSP_SERCOS_DISTORTED		102
#	define	DSP_SERCOS_LOOP_OPEN		103
#	define	DSP_SERCOS_EARLY			104
#	define	DSP_SERCOS_LATE				105
#	define	DSP_SERCOS_MST_MISSING		106
#	define	DSP_SERCOS_DRIVE_INIT		107
#	define	DSP_SERCOS_INVALID_DRIVE_TYPE	108
#	define	DSP_SERCOS_INVALID_DRIVE_NUMBER	109
#	define	DSP_SERCOS_INVALID_DRIVE_ADDR	110
#	define	DSP_SERCOS_DUPLICATE_DRIVE_ADDR	111
#	define	DSP_SERCOS_PROC_FAILURE			112
#	define	DSP_SERCOS_AXIS_ASSIGNMENT		113
#	define	DSP_SERCOS_RESET				114
#	define	DSP_SERCOS_VARIABLE_READ		115
#	define	DSP_SERCOS_INVALID_IDN_AT		116
#	define	DSP_SERCOS_INVALID_IDN_MDT		117
#	define	DSP_SERCOS_127_FAILURE			118
#	define	DSP_SERCOS_128_FAILURE			119
#	define	DSP_SERCOS_IDN_NOT_AVAILABLE	120
#	define	DSP_SERCOS_NO_CHANNEL			121
#	define	DSP_SERCOS_ELEMENT_MISSING		122
#	define	DSP_SERCOS_SHORT_TRANS			123
#	define	DSP_SERCOS_LONG_TRANS			124
#	define	DSP_SERCOS_STATIC_VAL			125
#	define	DSP_SERCOS_WRITE_PROTECT		126
#	define	DSP_SERCOS_MIN					127
#	define	DSP_SERCOS_MAX					128
#	define	DSP_SERCOS_INVALID_DATA			129
#	define	DSP_SERCOS_PROTOCOL				130
#	define	DSP_SERCOS_HS_TIMEOUT			131
#	define	DSP_SERCOS_BUSY					132
#	define	DSP_SERCOS_CMD					133
#	define	DSP_SERCOS_M_NOT_READY			134
#	define	DSP_SERCOS_SC_TIMEOUT			135
#	define	DSP_SERCOS_REC_ERR				136
#	define	DSP_SERCOS_INVALID_CYCLE_TIME	137
#	define	DSP_SERCOS_USER_AT				138
#	define	DSP_SERCOS_USER_MDT				139

/*	Link related */
#	define LINK_ACTUAL				1
#	define LINK_COMMAND				2
#	define LINK_UNLINK				(-1)

/*	Cam related */
#	define CAM_ACTUAL				0
#	define CAM_COMMAND				1

/* Integration Modes */
#	define	IM_STANDING				0
#	define	IM_ALWAYS				1

/* Position Feedback Devices */
#	define	FB_ENCODER				0
#	define	FB_ANALOG				1
#	define	FB_PARALLEL				2

/* Home and Index Configurations */
#  define   HOME_ONLY               0
#  define   LOW_HOME_AND_INDEX      1
#  define   INDEX_ONLY              2  
#  define   HIGH_HOME_AND_INDEX     3

/* #defines for dsp_step_speed */
#  define   DISABLE_STEP_OUTPUT     0
#  define   FAST_STEP_OUTPUT        1
#  define   MEDIUM_STEP_OUTPUT      2
#  define   SLOW_STEP_OUTPUT        3

#  define   POSITIVE_SENSE          1
#  define   NEGATIVE_SENSE          0


/* pc_status Action Masks/dsp_status(...) flags		*/
#	define	NO_ACTION				(int16)0x0000 /* ignore. */
#	define	STOP					(int16)0x0002 /* controlled stop. */
#	define	E_STOP					(int16)0x0004 /* violent stop. */
#	define	ABORT					(int16)0x0008 /* amp shutdown. */

/* more dsp_status(...) returned flags. */
#	define	HOME_ID					(int16)0x0010
#	define	POS_LIMIT_ID			(int16)0x0020
#	define	NEG_LIMIT_ID			(int16)0x0040
#	define	AMP_FAULT_ID			(int16)0x0080
#	define	A_LIMIT_ID				(int16)0x0100
#	define	V_LIMIT_ID				(int16)0x0200
#	define	X_NEG_LIMIT_ID			(int16)0x0400
#	define	X_POS_LIMIT_ID			(int16)0x0800
#	define	ERROR_LIMIT_ID			(int16)0x1000
#	define	PC_COMMAND_ID			(int16)0x2000
#  define   OUT_OF_FRAMES_ID		(int16)0x0060
                                 
#	define	PC_STATUS_EXCEPTION     (int16)0x000F

#  define   JOG_X                   0
#  define   JOG_Y                   1
#  define   JOG_Z                   2

/* source ID's for setting action values */

typedef enum
{	ID_NONE = 0,
    ID_HOME_SWITCH,
    ID_POS_LIMIT,	
    ID_NEG_LIMIT,	
    ID_AMP_FAULT,	
    ID_A_LIMIT,		
    ID_V_LIMIT,		
    ID_X_NEG_LIMIT,	
    ID_X_POS_LIMIT,	
    ID_ERROR_LIMIT,	
    ID_PC_COMMAND,	
    ID_OUT_OF_FRAMES,
    ID_FEEDBACK_FAULT,
    ID_FEEDBACK_ILLEGAL_STATE,
    ID_AXIS_COMMAND
}   SOURCE_ID;

/* state values for actions (events) */
typedef enum
{
    _NULL_STATE = 0,
    _STATE_1,
    _STATE_2,
    _STATE_3,
    _STATE_4,
    _STATE_5,
    _STATE_6,
    _STATE_7,
    _STATE_8,
    _STATE_9,
    _STATE_10,
    _STATE_11,
    _STATE_12,
    _STATE_13,
    _STATE_14,
    _STATE_15
}   PCDSP_STATE;



# ifndef NO_PROTOTYPES

#  ifdef	__cplusplus
extern "C" {
#	  endif



#	ifndef	NO_HIGHLEVEL


typedef double VECT[PCDSP_MAX_AXES];

/*	HLDSP.C				*/
int16 FNTYPE start_point_list(void);
int16 FNTYPE set_move_speed(double speed);
int16 FNTYPE set_move_accel(double accel);
int16 FNTYPE set_corner_sharpness(double sharpness);
int16 FNTYPE set_arc_division(double degrees);
int16 FNTYPE move_2(double x, double y);
int16 FNTYPE move_3(double x, double y, double z);
int16 FNTYPE move_4(double x, double y, double z, double w);
int16 FNTYPE move_n(P_DOUBLE x);
int16 FNTYPE arc_2(double x_center, double y_center,  double angle);
int16 FNTYPE end_point_list(void);
int16 FNTYPE set_points(int16 points);
int16 FNTYPE start_motion(void);
int16 FNTYPE stop_motion(void);
int16 FNTYPE all_done(void);
int16 FNTYPE map_axes(int16 n_axes, int16 *map_array);
int16 FNTYPE add_arc(VECT center, double angle, double division);
int16 FNTYPE add_point(VECT point);
int16 FNTYPE get_last_point(VECT x);
int16 FNTYPE set_move_output(unsigned16 value);
int16 FNTYPE change_move_bit(int16 bitNo, int16 state);
int16 FNTYPE set_move_bit(int16 bitNo);
int16 FNTYPE reset_move_bit(int16 bitNo);
int16 FNTYPE arc_optimization(int16 opt) ;
int16 FNTYPE set_move_ratio(P_DOUBLE ratio) ;
int16 FNTYPE set_path_mode(int16 mode);
int16 FNTYPE set_path_filter(int16 length);


#		endif



/* mldsp.c */
int16 FNTYPE dsp_init(int16 iobase) ;
int16 FNTYPE dsp_reset(void) ;
unsigned16 FNTYPE dsp_sample_rate(void) ;
int16 FNTYPE dsp_version(void) ;
int16 FNTYPE dsp_option(void) ;
int16 FNTYPE set_sample_rate(unsigned16 rate) ;
int16 FNTYPE dsp_axes(void) ;
int16 FNTYPE dsp_stepper(int16 axis);
int16 FNTYPE dsp_read_dm(unsigned16 addr);
int16 FNTYPE dsp_write_dm(unsigned16 addr, int16 dm);
int16 FNTYPE dsp_read_pm(unsigned16 addr);
int16 FNTYPE dsp_write_pm(unsigned16 addr, int16 pm);
int16 FNTYPE dsp_feed_rate(double override);
int16 FNTYPE axis_feed_rate(int16 axis, double override);
int16 FNTYPE dsp_interrupt_enable(int16 state);
int16 FNTYPE set_dac_output(int16 axis, int16 voltage);
int16 FNTYPE get_dac_output(int16 axis, P_INT voltage);
int16 FNTYPE dsp_encoder(int16 axis) ;
int16 FNTYPE set_gates(int16 length, P_INT axes);
int16 FNTYPE reset_gates(int16 length, P_INT axes);
int16 FNTYPE set_gate(int16 axis) ;
int16 FNTYPE reset_gate(int16 axis) ;
int16 FNTYPE jog_axis(int16 axis, int16 jog_channel, int16 c, int16 d, double m1, double m2, int16 enable);
int16 FNTYPE reset_integrator(int16 axis);
unsigned long FNTYPE get_frame_time(int16 axis);
unsigned16 FNTYPE get_dsp_time(void);
unsigned16 FNTYPE get_last_transfer_time(void);
int16 FNTYPE frames_to_execute(int16 axis);
int16 FNTYPE fifo_space(void);

/* mlpos.c */
int16 FNTYPE get_command(int16 axis, P_DOUBLE dst);
int16 FNTYPE set_command(int16 axis, double src);
int16 FNTYPE get_position(int16 axis, P_DOUBLE dst);
int16 FNTYPE set_position(int16 axis, double pos);
int16 FNTYPE get_velocity(int16 axis, P_DOUBLE vel);
int16 FNTYPE set_velocity(int16 axis, double vel);
int16 FNTYPE get_accel(int16 axis, P_DOUBLE accel);
int16 FNTYPE set_accel(int16 axis, double accel);
int16 FNTYPE get_jerk(int16 axis, P_DOUBLE jerk);
int16 FNTYPE set_jerk(int16 axis, double jerk);
int16 FNTYPE get_error(int16 axis, P_DOUBLE error);
int16 FNTYPE get_latched_position(int16 axis, P_DOUBLE dst) ;
int16 FNTYPE arm_latch(int16 enable) ;
int16 FNTYPE latch_status(void) ;
int16 FNTYPE latch(void) ;


/* mlmove.c */
int16 FNTYPE start_t_move(int16 axis, double posn, double vel, double accel, double decel);
int16 FNTYPE t_move(int16 axis, double position, double velocity, double acceleration, double deceleration);
int16 FNTYPE start_move(int16 axis, double final, double v, double a);
int16 FNTYPE move(int16 axis, double position, double velocity, double acceleration) ;
int16 FNTYPE start_move_all(int16 len, P_INT axes, P_DOUBLE pos, P_DOUBLE vel, P_DOUBLE accel) ;
int16 FNTYPE wait_for_all(int16 len, P_INT axes) ;
int16 FNTYPE move_all(int16 len, P_INT axes, P_DOUBLE pos, P_DOUBLE vel, P_DOUBLE accel) ;
int16 FNTYPE get_last_command(int16 axis, P_DOUBLE pos) ;
int16 FNTYPE set_last_command(int16 axis, double pos);

/* mlrsmove.c */
int16 FNTYPE start_rs_move(int16 axis, double distance, double vel, double accel, double jerk);
int16 FNTYPE rs_move(int16 axis, double distance, double velocity, double acceleration, double jerk);

/* mlsmove.c */
int16 FNTYPE start_s_move(int16 axis, double final, double vel, double accel, double jerk) ;
int16 FNTYPE s_move(int16 axis, double final, double vel, double accel, double jerk) ;
int16 FNTYPE start_sprof_move(int16 axis, double final, double vel, double accel, double jerk) ;
int16 FNTYPE sprof_move(int16 axis, double final, double vel, double accel, double jerk) ;

/* mlpmove.c - haystack motion. */
int16 FNTYPE start_p_move(int16 axis, double final, double vel, double accel, double jerk) ;
int16 FNTYPE p_move(int16 axis, double final, double vel, double accel, double jerk) ;

/* mlconfig.c */
int16 FNTYPE set_positive_sw_limit(int16 axis, double position, int16 action) ;
int16 FNTYPE get_positive_sw_limit(int16 axis, P_DOUBLE position, P_INT action) ;
int16 FNTYPE set_negative_sw_limit(int16 axis, double position, int16 action) ;
int16 FNTYPE get_negative_sw_limit(int16 axis, P_DOUBLE position, P_INT action) ;
int16 FNTYPE set_in_position(int16 axis, double window) ;
int16 FNTYPE get_in_position(int16 axis, P_DOUBLE window) ;
int16 FNTYPE set_error_limit(int16 axis, double window, int16 action) ;
int16 FNTYPE get_error_limit(int16 axis, P_DOUBLE window, P_INT action) ;
int16 FNTYPE set_positive_limit(int16 axis, int16 action) ;
int16 FNTYPE set_positive_level(int16 axis, int16 level) ;		/* TRUE or FALSE */
int16 FNTYPE set_negative_limit(int16 axis, int16 action) ;
int16 FNTYPE set_negative_level(int16 axis, int16 level) ;
int16 FNTYPE get_positive_limit(int16 axis, P_INT action) ;
int16 FNTYPE get_positive_level(int16 axis, P_INT level);		/* TRUE or FALSE */
int16 FNTYPE get_negative_limit(int16 axis, P_INT action) ;
int16 FNTYPE get_negative_level(int16 axis, P_INT level);
int16 FNTYPE set_home(int16 axis, int16 action) ;
int16 FNTYPE set_home_level(int16 axis, int16 level) ;
int16 FNTYPE get_home(int16 axis, P_INT action) ;
int16 FNTYPE get_home_level(int16 axis, P_INT level) ;
int16 FNTYPE set_amp_fault (int16 axis, int16 action);
int16 FNTYPE set_amp_fault_level(int16 axis, int16 level) ;
int16 FNTYPE get_amp_fault (int16 axis, P_INT action) ;
int16 FNTYPE get_amp_fault_level (int16 axis, P_INT level) ;
int16 FNTYPE set_amp_enable_level(int16 axis, int16 level);
int16 FNTYPE get_amp_enable_level(int16 axis, P_INT level) ;
int16 FNTYPE set_filter(int16 axis, P_INT coefficients) ;
int16 FNTYPE get_filter(int16 axis, P_INT coefficients) ;
int16 FNTYPE set_aux_filter(int16 axis, P_INT coefficients) ;
int16 FNTYPE get_aux_filter(int16 axis, P_INT coefficients) ;
int16 FNTYPE set_dual_loop(int16 axis, int16 velocity_axis, int16 dual) ;
int16 FNTYPE get_dual_loop(int16 axis, P_INT velocity_axis, P_INT dual) ;
int16 FNTYPE set_feedback(int16 axis, int16 t) ;
int16 FNTYPE get_feedback(int16 axis, P_INT t) ;
int16 FNTYPE set_cam(int16 master_axis, int16 cam_axis, int16 cam, int16 source) ;
int16 FNTYPE set_analog_channel(int16 axis, int16 chan, int16 differential, int16 bipolar);
int16 FNTYPE get_analog_channel(int16 axis, P_INT chan, P_INT differential, P_INT bipolar);
int16 FNTYPE set_dac_channel(int16 axis, int16 chan);
int16 FNTYPE get_dac_channel(int16 axis, P_INT chan);
int16 FNTYPE set_aux_encoder(int16 axis, int16 encoder_channel, int16 source);
int16 FNTYPE get_aux_encoder(int16 axis, int16 *encoder_channel, int16 source);
int16 FNTYPE set_integration(int16 axis, int16 im) ;
int16 FNTYPE get_integration(int16 axis, P_INT im) ;
int16 FNTYPE set_unipolar(int16 axis, int16 state) ;
int16 FNTYPE is_unipolar(int16 axis) ;
int16 FNTYPE set_axis_analog(int16 axis, int16 state);
int16 FNTYPE get_axis_analog(int16 axis, P_INT state);
int16 FNTYPE set_axis(int16 axis, int16 enable);
int16 FNTYPE get_axis(int16 axis, P_INT enable);
int16 FNTYPE set_feedback_check(int16 axis, int16 state);
int16 FNTYPE get_feedback_check(int16 axis, P_INT state);

/* mlio.c: */
int16 FNTYPE get_io(int16 port, P_INT value);
int16 FNTYPE set_io(int16 port, int16 value);
int16 FNTYPE init_io(int16 port, int16 io_select);
int16 FNTYPE init_analog(int16 channel, int16 diff, int16 bipolar);
int16 FNTYPE get_analog(int16 channel, P_INT value);
int16 FNTYPE start_analog(int16 channel) ;
int16 FNTYPE read_analog(P_INT value) ;
int16 FNTYPE read_axis_analog(int16 axis, P_INT value);
int16 FNTYPE io_mon(int16 port, int16 *status);
int16 FNTYPE set_io_mon_mask(int16 port, int16 mask, int16 value);
int16 FNTYPE get_io_mon_mask(int16 port, int16 *mask, int16 *value);
int16 FNTYPE clear_io_mon(void);
int16 FNTYPE io_changed(void);
int16 FNTYPE bit_on(int16 bitNo);
int16 FNTYPE change_bit(int16 bitNo, int16 state);
int16 FNTYPE set_bit(int16 bitNo);
int16 FNTYPE reset_bit(int16 bitNo);
int16 FNTYPE home_switch(int16 axis);
int16 FNTYPE pos_switch(int16 axis);
int16 FNTYPE neg_switch(int16 axis);
int16 FNTYPE amp_fault_switch(int16 axis);
int16 FNTYPE set_amp_enable(int16 axis, int16 state);
int16 FNTYPE get_amp_enable(int16 axis, P_INT state);
int16 FNTYPE enable_amplifier(int16 axis);
int16 FNTYPE disable_amplifier(int16 axis);
int16 FNTYPE set_home_index_config(int16 axis, int16 config);
int16 FNTYPE get_home_index_config(int16 axis, P_INT config);
int16 FNTYPE dsp_step_speed(int16 axis, P_INT spd);
int16 FNTYPE dsp_set_step_speed(int16 axis, int16 spd);
int16 FNTYPE dsp_closed_loop(int16 axis, P_INT closed);
int16 FNTYPE dsp_set_closed_loop(int16 axis, int16 closed);
int16 FNTYPE dsp_set_stepper(int16 axis, int16 stepper);
int16 FNTYPE init_timer(int16 channel, int16 mode);
int16 FNTYPE set_timer(int16 channel, unsigned16 t);
int16 FNTYPE get_timer(int16 channel, unsigned16 PTRTYPE * t) ;


/* mlstatus.c */
int16 FNTYPE set_stop(int16 axis);
int16 FNTYPE set_e_stop(int16 axis);
int16 FNTYPE controller_idle(int16 axis);
int16 FNTYPE controller_run(int16 axis);
int16 FNTYPE clear_status(int16 axis);
int16 FNTYPE in_sequence(int16 axis);
int16 FNTYPE in_position(int16 axis);
int16 FNTYPE in_motion(int16 axis);
int16 FNTYPE negative_direction(int16 axis);
int16 FNTYPE frames_left(int16 axis);
int16 FNTYPE motion_done(int16 axis) ;			/* axis_done except for in_position */
int16 FNTYPE axis_done(int16 axis);
int16 FNTYPE axis_status(int16 axis);			/* status bits (IN_POSITION, etc.) */
int16 FNTYPE axis_state(int16 axis);			/* ..._EVENT */
int16 FNTYPE axis_source(int16 axis);			/* ID_... */
int16 FNTYPE wait_for_done(int16 axis) ;


/* mllink.c */
int16 FNTYPE mei_link(int16 master, int16 slave, double ratio, int16 source);
int16 FNTYPE endlink(int16 slave);


/* mlef.c */
int16 FNTYPE set_stop_rate(int16 axis, double rate);
int16 FNTYPE set_e_stop_rate(int16 axis, double rate);
int16 FNTYPE get_stop_rate(int16 axis, P_DOUBLE rate);
int16 FNTYPE get_e_stop_rate(int16 axis, P_DOUBLE rate);
int16 FNTYPE interrupt_on_event(int16 axis, int16 state);


/* mlsetvel.c */
int16 FNTYPE v_move(int16 axis, double newvel, double accel);

/* mlerror.c */
int16 FNTYPE error_msg(int16 code, P_CHAR dst);
extern int16 DATATYPE dsp_error ;

/* mldspf.c */
int16 FNTYPE dsp_end_sequence(int16 axis);
int16 FNTYPE dsp_dwell(int16 axis, double duration) ;
int16 FNTYPE dsp_jerk(int16 axis, double jerk, double duration) ;
int16 FNTYPE dsp_accel(int16 axis, double accel, double duration) ;
int16 FNTYPE dsp_velocity(int16 axis, double vel, double duration) ;
int16 FNTYPE dsp_position(int16 axis, double position, double duration) ;
int16 FNTYPE dsp_marker(int16 axis, P_INT p) ;
unsigned16 FNTYPE dsp_goto(int16 axis, int16 p) ;
int16 FNTYPE dsp_control(int16 axis, int16 bit, int16 set) ;
int16 FNTYPE dsp_frame_action(int16 axis, int16 action) ;
int16 FNTYPE dsp_io_frame(int16 axis, int16 port, int16 ormask, int16 andmask) ;
int16 FNTYPE dsp_action(int16 axis, int16 trigger, int16 action) ;
int16 FNTYPE dsp_home_action(int16 axis, int16 action) ;
int16 FNTYPE dsp_positive_limit_action(int16 axis, int16 action);
int16 FNTYPE dsp_negative_limit_action(int16 axis, int16 action);
int16 FNTYPE dsp_error_action(int16 axis, int16 action);
int16 FNTYPE dsp_io_trigger(int16 axis, int16 bit, int16 state) ;
int16 FNTYPE dsp_io_trigger_mask(int16 axis, int16 port, int16 mask, int16 state);
int16 FNTYPE dsp_axis_command(int16 axis, int16 destaxis, int16 event) ;
int16 FNTYPE dsp_position_trigger(int16 axis, int16 triggeraxis, 
                      double triggerpos, int16 trigger_sense, int16 actual, int16 action);
int16 FNTYPE dsp_actual_position_trigger(int16 axis, double pos, int16 trigger_sense);
int16 FNTYPE dsp_command_position_trigger(int16 axis, double pos, int16 trigger_sense);
int16 FNTYPE dsp_set_filter(int16 axis, P_INT coeffs);

/* envutil.c */
extern int16 DATATYPE dsp_base ;
typedef int16 (LOCAL_FN * USER_SEG) (P_CHAR, P_CHAR) ;
extern USER_SEG user_seg ;
int16 FNTYPE understand_dsp(P_CHAR) ;
int16 FNTYPE do_dsp(void) ;

/*	backward.c 

	This file contains routines that are included simply for backward
	compatibility.  Although I don't forsee ever losing any of the
	functions in this file, I do recommend working toward using
	the newer (and we hope better) replacements.
*/


int16 FNTYPE dsp_status(int16 axis) ;
int16 FNTYPE set_position_limit(int16 axis,  double lowest, int16 lowAction, double highest, int16 highAction) ;
int16 FNTYPE get_position_limit(int16 axis, P_DOUBLE lowest,  P_INT lowAction, P_DOUBLE highest, P_INT highAction) ;
int16 FNTYPE set_pos_limit(int16 axis, double low, double high, int16 action) ;
int16 FNTYPE get_pos_limit(int16 axis, P_DOUBLE low, P_DOUBLE high, P_INT action) ;
int16 FNTYPE set_limit_actions(int16 axis, int16 level, int16 negAction, int16 posAction);
int16 FNTYPE get_limit_actions(int16 axis, P_INT level, P_INT negAction, P_INT posAction) ;
int16 FNTYPE set_limit_action(int16 axis, int16 activeLevel, int16 action) ;
int16 FNTYPE get_limit_action(int16 axis, P_INT activeLevel, P_INT action) ;
int16 FNTYPE set_home_action(int16 axis, int16 level, int16 action) ;
int16 FNTYPE get_home_action(int16 axis, P_INT level, P_INT action) ;
int16 FNTYPE set_amp_fault_action (int16 axis, int16 level, int16 action) ;
int16 FNTYPE get_amp_fault_action (int16 axis, P_INT level, P_INT action) ;
int16 FNTYPE start_for_home(int16 axis, double vel, double accel);
int16 FNTYPE goto_home(int16 axis, double vel, double accel);
int16 FNTYPE set_index_required(int16 axis, int16 indexRequired);
int16 FNTYPE get_index_required(int16 axis, P_INT indexRequired);
int16 FNTYPE set_boot_index_required(int16 axis, int16 indexRequired);
int16 FNTYPE get_boot_index_required(int16 axis, P_INT indexRequired);
int16 FNTYPE set_vel_limit(int16 axis, double limit, int16 action) ;
int16 FNTYPE get_vel_limit(int16 axis, P_DOUBLE limit, P_INT action) ;
int16 FNTYPE set_accel_limit(int16 axis, double limit, int16 action) ;
int16 FNTYPE get_accel_limit(int16 axis, P_DOUBLE limit, P_INT action) ;
int16 FNTYPE set_boot_vel_limit(int16 axis, double limit, int16 action) ;
int16 FNTYPE get_boot_vel_limit(int16 axis, P_DOUBLE limit, P_INT action) ;
int16 FNTYPE set_boot_accel_limit(int16 axis, double limit, int16 action) ;
int16 FNTYPE get_boot_accel_limit(int16 axis, P_DOUBLE limit, P_INT action) ;
int16 FNTYPE set_conversion(int16 axis, double cpd, double spp);
int16 FNTYPE get_conversion(int16 axis, P_DOUBLE cpd, P_DOUBLE spp);
int16 FNTYPE set_aconversion(int16 axis, double cpd, double spp, double app);
int16 FNTYPE get_aconversion(int16 axis, P_DOUBLE cpd, P_DOUBLE spp, P_DOUBLE app);
int16 FNTYPE interrupt_on_stop(int16 axis, int16 state);
int16 FNTYPE interrupt_on_e_stop(int16 axis, int16 state);
int16 FNTYPE boot_interrupt_on_stop(int16 axis, int16 state) ;
int16 FNTYPE boot_interrupt_on_e_stop(int16 axis, int16 state) ;


/* bconfig.c */
int16 FNTYPE get_boot_filter(int16 axis, P_INT coeffs) ;
int16 FNTYPE set_boot_filter(int16 axis, P_INT coeffs) ;
int16 FNTYPE get_boot_aux_filter(int16 axis, P_INT coeffs) ;
int16 FNTYPE set_boot_aux_filter(int16 axis, P_INT coeffs) ;
unsigned FNTYPE mei_checksum(void) ;		/* update the boot memory checksum. */
unsigned FNTYPE static_checksum(void);
int16 FNTYPE set_boot_io(int16 port, int16 value) ;
int16 FNTYPE get_boot_io(int16 port, P_INT value) ;
int16 FNTYPE init_boot_io(int16 port, int16 direction) ;
int16 FNTYPE change_boot_bit(int16 bitNo, int16 state) ;
int16 FNTYPE boot_bit_on(int16 bitNo) ;
int16 FNTYPE set_boot_home_index_config(int16 axis, int16 config) ;
int16 FNTYPE get_boot_home_index_config(int16 axis, P_INT config) ;
int16 FNTYPE dsp_boot_step_speed(int16 axis, P_INT spd) ;
int16 FNTYPE dsp_set_boot_step_speed(int16 axis, int16 spd) ;
int16 FNTYPE dsp_boot_closed_loop(int16 axis, P_INT closed) ;
int16 FNTYPE dsp_set_boot_closed_loop(int16 axis, int16 closed) ;	/* this affects axes in pairs. */
int16 FNTYPE dsp_set_boot_stepper(int16 axis, int16 stepper) ;	/* this affects axes in pairs. */
int16 FNTYPE dsp_boot_stepper(int16 axis) ;
int16 FNTYPE set_boot_stop_rate(int16 axis, double rate) ;
int16 FNTYPE set_boot_e_stop_rate(int16 axis, double rate) ;
int16 FNTYPE get_boot_stop_rate(int16 axis, P_DOUBLE rate) ;
int16 FNTYPE get_boot_e_stop_rate(int16 axis, P_DOUBLE rate) ;
int16 FNTYPE is_boot_unipolar(int16 axis) ;
int16 FNTYPE set_boot_unipolar(int16 axis, int16 state) ;
int16 FNTYPE set_boot_dual_loop(int16 axis, int16 velocity_axis, int16 dual) ;
int16 FNTYPE get_boot_dual_loop(int16 axis, P_INT velocity_axis, P_INT dual) ;
int16 FNTYPE set_boot_feedback(int16 axis, int16 t) ;
int16 FNTYPE get_boot_feedback(int16 axis, P_INT t) ;
int16 FNTYPE set_boot_analog_channel(int16 axis, int16 chan, int16 differential, int16 bipolar) ;
int16 FNTYPE get_boot_analog_channel(int16 axis, P_INT chan, P_INT differential, P_INT bipolar) ;
int16 FNTYPE set_boot_dac_channel(int16 axis, int16 chan);
int16 FNTYPE get_boot_dac_channel(int16 axis, P_INT chan);
int16 FNTYPE set_boot_cam(int16 master_axis, int16 cam_axis, int16 cam, int16 source);
int16 FNTYPE set_boot_aux_encoder(int16 axis, int16 encoder_channel, int16 source);
int16 FNTYPE get_boot_aux_encoder(int16 axis, int16 * encoder_channel, int16 source);
int16 FNTYPE set_boot_integration(int16 axis, int16 im) ;
int16 FNTYPE get_boot_integration(int16 axis, P_INT im) ;
int16 FNTYPE set_boot_axis_analog(int16 axis, int16 state);
int16 FNTYPE get_boot_axis_analog(int16 axis, P_INT state);
int16 FNTYPE set_boot_axis(int16 axis, int16 enable);
int16 FNTYPE get_boot_axis(int16 axis, P_INT enable);


/* blimits.c */
int16 FNTYPE set_boot_positive_sw_limit(int16 axis, double position, int16 action) ;
int16 FNTYPE get_boot_positive_sw_limit(int16 axis, P_DOUBLE position, P_INT action) ;
int16 FNTYPE set_boot_negative_sw_limit(int16 axis, double position, int16 action) ;
int16 FNTYPE get_boot_negative_sw_limit(int16 axis, P_DOUBLE position, P_INT action);
int16 FNTYPE set_boot_in_position(int16 axis, double window) ;
int16 FNTYPE get_boot_in_position(int16 axis, P_DOUBLE window) ;
int16 FNTYPE set_boot_error_limit(int16 axis, double window, int16 action) ;
int16 FNTYPE get_boot_error_limit(int16 axis, P_DOUBLE window, P_INT action) ;
int16 FNTYPE set_boot_positive_limit(int16 axis, int16 action) ;
int16 FNTYPE set_boot_positive_level(int16 axis, int16 level);		/* TRUE or FALSE */
int16 FNTYPE set_boot_negative_limit(int16 axis, int16 action) ;
int16 FNTYPE set_boot_negative_level(int16 axis, int16 level) ;
int16 FNTYPE get_boot_positive_limit(int16 axis, P_INT action) ;
int16 FNTYPE get_boot_positive_level(int16 axis, P_INT level) ;		/* TRUE or FALSE */
int16 FNTYPE get_boot_negative_limit(int16 axis, P_INT action) ;
int16 FNTYPE get_boot_negative_level(int16 axis, P_INT level) ;
int16 FNTYPE set_boot_home(int16 axis, int16 action) ;
int16 FNTYPE set_boot_home_level(int16 axis, int16 level) ;
int16 FNTYPE get_boot_home(int16 axis, P_INT action) ;
int16 FNTYPE get_boot_home_level(int16 axis, P_INT level) ;
int16 FNTYPE set_boot_amp_fault (int16 axis, int16 action) ;
int16 FNTYPE set_boot_amp_fault_level(int16 axis, int16 level) ;
int16 FNTYPE get_boot_amp_fault (int16 axis, P_INT action) ;
int16 FNTYPE get_boot_amp_fault_level (int16 axis, P_INT level) ;
int16 FNTYPE set_boot_amp_enable_level (int16 axis, int16 level) ;
int16 FNTYPE get_boot_amp_enable_level (int16 axis, P_INT level) ;
int16 FNTYPE set_boot_amp_enable (int16 axis, int16 level) ;
int16 FNTYPE get_boot_amp_enable (int16 axis, P_INT level) ;


/* mlrmove.c */
int16 FNTYPE start_r_move(int16 axis, double distance, double vel, double accel) ;
int16 FNTYPE r_move(int16 axis, double distance, double velocity, double acceleration);

/* llfirm.c */
int16 FNTYPE upload_firmware_file(char *file);
int16 FNTYPE download_firmware_file(char *file);

/*	OPTIONAL CODE... */
# ifdef OPTION

/*	IO1.C	*/
/*	Option 1: Serial opto-isolated I/O board. */
#	if (OPTION == 1)
#     define	OPTO_INPUT     8
#		define	OPTO_STATUS    16
#		define	OPTO_MASK      24
#		define   OPTO_OUTPUT    32
/* used for type parameter in set/get_punch_address */
#     define   PUNCH_END      0
#     define   CLUTCH_BRAKE   1
#     define   FEED_START     2

unsigned FNTYPE opto_address(int16 port);
unsigned FNTYPE opto_input(int16 port);
unsigned FNTYPE opto_status(int16 port);
void FNTYPE opto_output(int16 port, unsigned value);
int16 FNTYPE set_home_address(int16 axis, int16 address, int16 mask);
int16 FNTYPE start_punch(unsigned16 axis_mask,
      double PTRTYPE *x, double PTRTYPE *v, double PTRTYPE *a);
int16 FNTYPE single_punch(void);
int16 FNTYPE set_nibble_mode(int16 n_mode);
int16 FNTYPE set_punch_times(int16 axis, long anticipation_time, long end_time);
int16 FNTYPE get_punch_times(int16 axis, long PTRTYPE *anticipation_time, long PTRTYPE *end_time);
void FNTYPE set_punch_address(int16 type, int16 address, unsigned16 or_mask,
      unsigned16 and_mask);
void FNTYPE get_punch_address(int16 type, int16 *address, unsigned16 PTRTYPE *or_mask,
      unsigned16 PTRTYPE *and_mask);
int16 FNTYPE low_stat(int16 axis);
int16 FNTYPE low_rt_stat(int16 axis);
int16 FNTYPE frame_count(int16 axis);
void FNTYPE cancel_punch(void);
int16 FNTYPE init_rotary_axis(int16 axis, double length);
int16 FNTYPE start_rotary_move(int16 axis, double position, double vel, double accel) ;
int16 FNTYPE get_rotary_position(int16 axis, double PTRTYPE * result);
int16 FNTYPE set_rotary_position(int16 axis, double position);

#	  endif /* Option = 1 */

#	endif  /* Options */

#  ifdef	__cplusplus
	} ;
#	  endif

#	endif /* no prototypes... */

#	endif
