/*
 	SERCOS.H - Sercos specific info
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef __SERCOS_H
#define	__SERCOS_H

#include "idsp.h"

#define BIT_RATE2				2
#define BIT_RATE4				4
#define SERCOS_MAX_CHANNELS		256	/* Represents AT,MDT data for a node */ 

/* S410B Memory Map 

	0x800 - 0x80A	Telegram and Service Container addresses (offsets from 0x800)
	0x80B - 0x81F	Phase12 telegram headers and data containers
	0x820 - 0x95F	Phase34 receive telegram headers and data containers
	0x960 - 0xA6B	Phase34 transmit telegram headers and data containers
	0xA6C - 0xA6D	Phase34 end marker
	0xA6E - 0xBFF	Service Containers
	0xC00 - 0xC1A	S410B Control Registers

	Telegram Format:

		Transmit (MDT):	
			Header					-> 4 words
			*Data Container Header	-> 1 word			
			*Configurable Data		-> max 30 words	

		Receive (AT):
			*Header					-> 5 words
			*Data Container Header	-> 2 words
			*Configurable Data		-> max 30 words

		End Marker:
			Header					-> 1 word
			Data					-> 1 word
		
	Service Container Format:

		Number of Containers		-> 1 word
		*Axis Number				-> 1 word
		*Initial Drive Mode			-> 1 word
		*Sercos Drive Address		-> 1 word
		*Sercos Drive Mfg			-> 1 word
		*Control Words				-> 5 words
		*Write Buffer				-> 20 words
		*Read Buffer				-> 20 words
		*Read Buffer Overhang		-> 1 word

		* = 1 per drive
*/

#define S410B_CNTRL					(0x0C00)
#define S410B_RAM					(0x800)

#define PHASE12_THEADERS_START		(0x000B)
#define PHASE34_THEADERS_START		(0x0020)
#define SC_0_START					(0x0273)	/* offset to SC control words */
#define SC_1_START					(0x02A5)
#define SC_2_START					(0x02D7)
#define SC_3_START					(0x0309)
#define SC_4_START					(0x033B)
#define SC_5_START					(0x036D)
#define SC_6_START					(0x039F)
#define SC_7_START					(0x03D1)

#define PHASE12_RTH					PHASE12_THEADERS_START
#define PHASE12_RDH					(PHASE12_RTH+5)
#define PHASE12_TTH					(PHASE12_RDH+4)
#define PHASE12_TDH					(PHASE12_TTH+4)
#define PHASE12_EM					(0x001B)

#define PHASE34_RTH					PHASE34_THEADERS_START
#define PHASE34_TTH					(0x0160)
#define PHASE34_TDH					(PHASE34_TTH+4)
#define PHASE34_EM					(0x026C)

/* S410B Control Register Address Offsets */
#define RSTFL_ADDR					0x0001
#define	INT_RDIST_ADDR				0x0004
#define PHAS0_ADDR					0x000A

/* S410B Control Register Bit Mask, Offset=RSTFL_ADDR */
#define RSTFL						0x0001
#define SWRST						0x0002
#define REPON						0x0008
#define SREGEN						0x0010
#define REGMODE						0x0020
#define SWSBAUD						0x0040
#define POLRXD						0x0080
#define PRESYNC						0x0100
#define POLTXD						0x0200
#define ENTSBAUD					0x0400
#define SBAUD						0x0800

/* S410B Control Register Bit Mask, Offset=INT_RDIST_ADDR */
#define INT_RDIST					0x0001
#define INT_FIBBR					0x0002
#define INT_MSTEARLY				0x0080
#define INT_MSTLATE					0x0100
#define INT_MSTMISS					0x0200

/* AT info */
#define MAX_AT_LEN					0x25	/* 5 + 2 + 30 */

/* service container info */
#define SC_BASE						(SC_0_START-5)
#define SC_CNTRL_MDT				0
#define SC_CNTRL_AT					1
#define SC_CNTRL_WR					2
#define SC_CNTRL_RD					3
#define SC_CNTRL_ERR				4
#define SC_CNTRL_LEN				5
#define SC_WR_LEN					0x14
#define SC_RD_LEN					0x14
#define SC_RD_INIT					0x1414
#define SC_TIMEOUT					2000		/* unsigned16 */
#define HS_MDT						0x0001
#define LS_MDT						0x0002
#define END_MDT						0x0004
#define ELEM_1						0x0008
#define ELEM_2						0x0010
#define ELEM_3						0x0018
#define ELEM_4						0x0020
#define ELEM_5						0x0028
#define ELEM_6						0x0030
#define ELEM_7						0x0038
#define ELEM_MDT					0x0038
#define SETEND						0x0040
#define M_BUSY						0x0080
#define RECERR						0x0080
#define SL_ERR						0x1000
#define INT_END_WRBUF				0x2000
#define INT_END_RDBUF				0x4000
#define INT_SC_ERR					0x0200
#define INT_HS_TIMEOUT				0x0400
#define INT_BUSY_TIMEOUT			0x0800
#define INT_CMD						0x1000

#define HS_AT						0x0001
#define BUSY_AT						0x0002
#define ERR_AT						0x0004
#define CMD_AT						0x0008
#define RECERR						0x0080

#define STATUSADDR					0x36A0
#define C_DATA_START_IN				0x3700
#define C_DATA_START_OUT			0x3710
#define C_DATA_SIZE					0x20

#define MAX_SERCOS_STRING_LEN		65532	/* bytes */

/* Procedure Command/Status bits */
#define PROCEDURE_SET				0x1
#define	PROCEDURE_ENABLE			0x2
#define PROCEDURE_IN_PROGRESS		0x4
#define PROCEDURE_ERROR				0x8
#define PROCEDURE_DATA_NOT_VALID	0x100

/*	Sercos Filter Coefficients, for use with set/get filter */
#define DF_VLPG						0 /* velocity loop proportional term */
#define DF_VLIG						1 /* velocity loop integration time term */
#define DF_PLG						1 /* position loop proportional term */
#define DF_CLPG						1 /* current loop proportional term */

#define MAX_CYCLIC_IDNS				30
#define MAX_CYCLIC_IDN_WORDS		30

typedef struct {
	int16 axis;
	unsigned16 address;
	unsigned16 at_data;
	unsigned16 mdt_data;
	unsigned16 t1_min;
	unsigned16 t1;
	unsigned16 t2;
	unsigned16 t3;
	unsigned16 t4;
	unsigned16 t5;
	unsigned16 t_scyc;
	unsigned16 t_ncyc;
	unsigned16 t_atmt;
	unsigned16 t_atat;
	unsigned16 t_mtsy;
	unsigned16 t_mtsg;
	unsigned16 p_mdt;
	unsigned16 mdt_len;
	unsigned16 at_data_len;
	unsigned16 mdt_data_len;
	unsigned16 drive_mode;
	unsigned16 drive_mfg;
	unsigned16 telegram_type;
	unsigned16 cyclic_data_in_len;
	unsigned16 in_data[MAX_CYCLIC_IDNS];
	unsigned16 n_cyclic_in_words;
	unsigned16 master_cyclic_data_in_len;
	unsigned16 cyclic_data_out_len;
	unsigned16 out_data[MAX_CYCLIC_IDNS];
	unsigned16 n_cyclic_out_words;
	unsigned16 master_cyclic_data_out_len;
} DRIVE_CFG;

/* drive manufacturers */
#define LUTZE						0
#define PACSCI						1
#define MODICON						2
#define INDRAMAT					3
#define KOLLMORGEN					4
#define MEI							5
#define SANYO_DENKI					6
#define OTHER						32

typedef struct {
	unsigned16 idn;
	long value;
	unsigned16 drive_addr;
} DRIVE_IDNS;
typedef DRIVE_IDNS *DriveIdns;

typedef struct {
	int16 drive_axis;
	unsigned16 drive_addr;
	unsigned16 drive_mode;
	unsigned16 drive_mfg;
} DRIVE_INFO;
typedef DRIVE_INFO *DriveInfo;

typedef struct {
	unsigned16 idn;
	long value;
	int16 error;	/* error code from set/get_idn(...) */
} IDNS;
typedef IDNS *Idns;

#define MAX_E2_INTS					30
#define MAX_E4_INTS					6

typedef struct {
	unsigned16 elem_1;
	unsigned16 elem_2[MAX_E2_INTS];
	unsigned long elem_3;
	unsigned16 elem_4[MAX_E4_INTS];
	long elem_5;
	long elem_6;
} IDN_ATTRIBUTES;
typedef IDN_ATTRIBUTES *IdnAttributes;

typedef struct {
	unsigned16 idn;
	unsigned16 drive_addr;
} CYCLIC_DATA;
typedef CYCLIC_DATA *CyclicData;

#ifndef MEI_RW_FUNCT
#  define serc_read(addr) dsp_read_pm(addr)
#  define serc_write(addr,value) dsp_write_pm(addr,value)
#else
int16 serc_read(unsigned16 addr);
void serc_write(unsigned16 addr, int16 value);
#endif

/* Macros */
#define sc_handshake(sc_ptr)	((serc_read((unsigned16)(sc_ptr+SC_CNTRL_AT))&HS_AT)^HS_AT)
#define sc_address(channel)		(0x800+serc_read((unsigned16)(0x802+channel)))

/* Exported functions */
#ifdef __cplusplus
extern "C" {
#endif

/* llserc.c (internal functions) */
int16 FNTYPE get_sercos_channel(int16 axis, unsigned16 * channel);
int16 FNTYPE enable_sc_transmission(void);
int16 FNTYPE set_drive(unsigned16 drive_addr);
int16 FNTYPE set_drive_sc(unsigned16 drive_addr, unsigned16 channel);
int16 FNTYPE slave_err_check(unsigned16 sc_ptr);
int16 FNTYPE wait_sc(unsigned16 sc_ptr);
int16 FNTYPE check_sc(unsigned16 sc_ptr);
int16 FNTYPE clear_sc(unsigned16 sc_ptr);
int16 FNTYPE initialize_service_container(unsigned16 sc_ptr, unsigned16 idn);
int16 FNTYPE read_idn_attributes(unsigned16 channel, unsigned16 idn, IDN_ATTRIBUTES *attr);
int16 FNTYPE read_idn(unsigned16 channel, unsigned16 idn, int16 *n_words, unsigned16 *data, int16 variable_length);
int16 FNTYPE write_idn(unsigned16 channel, unsigned16 idn, int16 *n_words, unsigned16 *data, int16 variable_length);
int16 FNTYPE wait_cycles(int16 n_cycles);
int16 FNTYPE get_element(unsigned16 channel, unsigned16 idn, unsigned16 element, int16 n_words, unsigned16 *elem);
int16 FNTYPE get_element_variable(unsigned16 channel, unsigned16 idn, unsigned16 element, int16 *n_words, unsigned16 *elem);
int16 FNTYPE start_exec_proc(unsigned16 channel, unsigned16 proc);
int16 FNTYPE cancel_exec_proc(unsigned16 channel, unsigned16 proc);
int16 FNTYPE exec_proc_done(unsigned16 channel, unsigned16 proc, int16 *done);
int16 FNTYPE execute_proc(unsigned16 channel, unsigned16 proc);
int16 FNTYPE read_idn_string(unsigned16 channel, unsigned16 idn, char *str);
int16 FNTYPE get_element_1(unsigned16 channel, unsigned16 idn, unsigned16 *elem_1);
int16 FNTYPE get_element_2(unsigned16 channel, unsigned16 idn, unsigned16 *elem_2);
int16 FNTYPE get_element_3(unsigned16 channel, unsigned16 idn, unsigned long *elem_3);
int16 FNTYPE get_element_4(unsigned16 channel, unsigned16 idn, unsigned16 *elem_4);
int16 FNTYPE get_element_5(unsigned16 channel, unsigned16 idn, long *elem_5);
int16 FNTYPE get_element_6(unsigned16 channel, unsigned16 idn, long *elem_6);
int16 FNTYPE get_sercos_data_length(unsigned16 channel, unsigned16 idn, unsigned16 element, int16 *nbytes);


/* mlserc.c */
int16 FNTYPE set_idn(int16 axis, unsigned16 idn, long val);
int16 FNTYPE get_idn(int16 axis, unsigned16 idn, long *val);
int16 FNTYPE get_idn_attributes(int16 axis, unsigned16 idn, IDN_ATTRIBUTES *attr);
int16 FNTYPE set_idns(int16 axis, unsigned16 dr_addr, unsigned16 nidns, IDNS *idns);
int16 FNTYPE get_idns(int16 axis, unsigned16 dr_addr, unsigned16 firstidn, unsigned16 nidns, IDNS *idns);
int16 FNTYPE start_exec_procedure(int16 axis, unsigned16 procedure);
int16 FNTYPE cancel_exec_procedure(int16 axis, unsigned16 procedure);
int16 FNTYPE exec_procedure_done(int16 axis, unsigned16 procedure, int16 *done);
int16 FNTYPE execute_procedure(int16 axis, unsigned16 procedure);
int16 FNTYPE enable_sercos_amplifier(int16 axis, int16 enable);
int16 FNTYPE sercos_enabled(int16 axis, P_INT state);
int16 FNTYPE set_sercos_velocity_filter(int16 axis, long *coeffs);
int16 FNTYPE set_sercos_position_filter(int16 axis, long *coeffs);
int16 FNTYPE set_sercos_current_filter(int16 axis, long *coeffs);
int16 FNTYPE get_sercos_velocity_filter(int16 axis, long *coeffs);
int16 FNTYPE get_sercos_position_filter(int16 axis, long *coeffs);
int16 FNTYPE get_sercos_current_filter(int16 axis, long *coeffs);
int16 FNTYPE turn_on_sercos_led(void);
int16 FNTYPE turn_off_sercos_led(void);
int16 FNTYPE read_cyclic_at_data(int16 axis, unsigned16 offset);
int16 FNTYPE read_cyclic_mdt_data(int16 axis, unsigned16 offset);
int16 FNTYPE write_cyclic_mdt_data(int16 axis, unsigned16 offset, int16 data);
int16 FNTYPE change_operation_mode(int16 axis, unsigned16 mode);
int16 FNTYPE get_idn_size(int16 axis, unsigned16 idn, int16 *size);
int16 FNTYPE get_drive_status(int16 axis, int16 *status);
int16 FNTYPE reset_sercos_drive(int16 axis);
int16 FNTYPE get_idn_string(int16 axis, unsigned16 dr_addr, unsigned16 idn, char *msg);
int16 FNTYPE get_sercos_phase(int16 *phase);

/* sercdiag.c */
int16 FNTYPE get_class_1_diag(int16 axis, int16 *code, char *msg);
int16 FNTYPE get_class_2_diag(int16 axis, int16 *code, char *msg);

/* sercmisc.c */
int16 FNTYPE get_phase2_idnlists(int16 *size, unsigned16 *list);
int16 FNTYPE get_phase3_idnlists(int16 *size, unsigned16 *list);


#ifdef __cplusplus
	} ;
#endif

#endif /* __SERCOS_H */
