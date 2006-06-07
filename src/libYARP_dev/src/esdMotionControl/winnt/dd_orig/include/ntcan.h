/**************************************************************************
*                                                                         *
* ntcan.h -- NTCAN-API procedure declarations and constant definitions    *
*                                                                         *
* Copyright (c) 1997-2002, esd gmbh. All rights reserved.                 *
*                                                                         *
**************************************************************************/

#ifndef _ntcan_h_
#define _ntcan_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#if !defined(EXPORT)
#define EXPORT __declspec (dllexport)    /* Macro to export from DLL */
#endif

#if !defined(CALLTYPE)
#define CALLTYPE
#endif

#ifndef UNDER_RTSS
# pragma comment(lib,"ntcan.lib")       /* Link with NTCAN.LIB (Only for VC++)*/
#endif

/*------------------ Defines ------------------------------------------------*/
#define NTCAN_EV_BASE           0x40000000
#define NTCAN_EV_USER           0x40000080
#define NTCAN_EV_LAST           0x400000FF

#define NTCAN_EV_CAN_ERROR      NTCAN_EV_BASE
#define NTCAN_EV_ECHO(NR)       0x40000040 + (NR & 0x1f)


/* mode in canOpen() */
#define NTCAN_MODE_OVERLAPPED   0x20000000      /* Open for overlapped operations */
#define NTCAN_MODE_OBJECT       0x10000000  /* Open for Rx object mode        */


/* Queue-Size in canOpen() */
#define NTCAN_MAX_TX_QUEUESIZE  2047
#define NTCAN_MAX_RX_QUEUESIZE  2047
#define NTCAN_NO_QUEUE          -1

/* Flags in can-ids */
#define NTCAN_20B_BASE          0x20000000

/* Flags in frame-length's  */
#define NTCAN_RTR               0x10
#define NTCAN_NO_DATA           0x20

/* No baudrate configured (returned from canGetBaudrate() )*/
#define NTCAN_NO_BAUDRATE           0x7FFFFFFF

/*------------------- error-codes--------------------------------------------*/
#define NTCAN_SUCCESS            ERROR_SUCCESS
#define NTCAN_RX_TIMEOUT         0xE0000001
#define NTCAN_TX_TIMEOUT         0xE0000002
#define NTCAN_TX_ERROR           0xE0000004
#define NTCAN_CONTR_OFF_BUS      0xE0000005
#define NTCAN_CONTR_BUSY         0xE0000006
#define NTCAN_CONTR_WARN         0xE0000007
#define NTCAN_NO_ID_ENABLED      0xE0000009
#define NTCAN_ID_ALREADY_ENABLED 0xE000000A
#define NTCAN_ID_NOT_ENABLED     0xE000000B
   
#define NTCAN_INVALID_FIRMWARE   0xE000000D
#define NTCAN_MESSAGE_LOST       0xE000000E
#define NTCAN_INVALID_HARDWARE   0xE000000F

#define NTCAN_PENDING_WRITE      0xE0000010
#define NTCAN_PENDING_READ       0xE0000011
#define NTCAN_INVALID_DRIVER     0xE0000012    /* wrong dll/driver-combination */

#define NTCAN_INVALID_PARAMETER         ERROR_INVALID_PARAMETER
#define NTCAN_INVALID_HANDLE            ERROR_INVALID_HANDLE
#define NTCAN_IO_INCOMPLETE             ERROR_IO_INCOMPLETE
#define NTCAN_IO_PENDING                ERROR_IO_PENDING
#define NTCAN_NET_NOT_FOUND             ERROR_FILE_NOT_FOUND  
#define NTCAN_INSUFFICIENT_RESOURCES    ERROR_NO_SYSTEM_RESOURCES

#define NTCAN_OPERATION_ABORTED         ERROR_OPERATION_ABORTED

#pragma pack(1)

typedef struct
{
  long          id;             /* can-id                                   */
  unsigned char len;            /* length of message: 0-8                   */
  unsigned char msg_lost;       /* count of lost rx-messages                */
  unsigned char reserved[2];    /* reserved                                 */
  unsigned char data[8];        /* 8 data-bytes                             */
} CMSG;


typedef struct
{
  long          evid;          /* event-id: possible range:EV_BASE...EV_LAST */
  unsigned char len;           /* length of message: 0-8                     */
  unsigned char reserved[3];   /* reserved                                   */
  union
  {
    unsigned char  c[8];
    unsigned short s[4];
    unsigned long  l[2];
  } evdata;
} EVMSG;


typedef struct
{
  unsigned short hardware;
  unsigned short firmware;
  unsigned short driver;
  unsigned short dll;
  unsigned long  boardstatus;
  unsigned char  boardid[14];
  unsigned short features;
} CAN_IF_STATUS;

#pragma pack()

/*
 * Interface/Driver feature flags
 */
#define NTCAN_FEATURE_FULL_CAN       (1<<0)       /* Full CAN controller     */
#define NTCAN_FEATURE_CAN_20B        (1<<1)       /* CAN 2.OB support        */
#define NTCAN_FEATURE_DEVICE_NET     (1<<2)       /* Device net adapter      */
#define NTCAN_FEATURE_CYCLIC_TX      (1<<3)       /* Cyclic Tx support       */
#define NTCAN_FEATURE_RX_OBJECT_MODE (1<<4)       /* Rx object mode support  */


/*
 * IOCTL codes for canIoctl()
 */
#define NTCAN_IOCTL_FLUSH_RX_FIFO    0x0001       /* Flush Rx FIFO           */
#define NTCAN_IOCTL_GET_RX_MSG_COUNT 0x0002       /* Ret # CMSG in Rx FIFO   */
#define NTCAN_IOCTL_GET_RX_TIMEOUT   0x0003       /* Ret configured Rx tout  */
#define NTCAN_IOCTL_GET_TX_TIMEOUT   0x0004       /* Ret configured Tx tout  */


/*...prototypes..............................................................*/
EXPORT DWORD CALLTYPE canSetBaudrate(HANDLE handle,     /* nt-handle         */
                                     DWORD baud );      /* baudrate-constant */

/*...prototypes..............................................................*/
EXPORT DWORD    CALLTYPE canGetBaudrate(    /* ret NTCAN_OK, if success      */    
                HANDLE handle,              /* nt-handle                     */
                DWORD *baud );              /* baudrate-constant             */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canOpen(           /* ret NTCAN_OK, if success      */  
                int           net,          /* net number                    */
                unsigned long flags,        /*                               */
                long          txqueuesize,  /* nr of entries in message queue*/
                long          rxqueuesize,  /* nr of entries in message queue*/
                long          txtimeout,    /* tx-timeout in miliseconds     */
                long          rxtimeout,    /* rx-timeout in miliseconds     */
                HANDLE        *handle );    /* out: nt-handle                */
/*...........................................................................*/
EXPORT DWORD   CALLTYPE canClose(        /* ret NTCAN_OK, if success         */
                HANDLE        handle );  /* handle                           */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE      canIdAdd(  /* ret NTCAN_OK, if success         */
                HANDLE        handle,    /* read handle                      */
                long          id  );     /* can identifier                   */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE      canIdRangeAdd(  /* ret NTCAN_OK, if success    */
                HANDLE        handle,    /* read handle                      */
                long          idStart,   /* 1st can identifier               */
                long          idEnd  );  /* last can identifier              */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canIdDelete(    /* ret NTCAN_OK, if success         */
                HANDLE        handle,    /* read handle                      */
                long          id  );     /* can identifier                   */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canIdRangeDelete( /* ret 0, if success              */
                HANDLE        handle,    /* read handle                      */
                long          idStart,   /* 1st can identifier               */
                long          idEnd  );  /* last can identifier              */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canGetOverlappedResult( /* ret NTCAN_OK, if success */
                HANDLE        handle,    /* handle                           */
                OVERLAPPED    *ovrlppd,  /* overlapped-structure             */
                long          *len,      /* out: cnt of available CMSG-Buffer*/
                BOOL          bWait );   /* FALSE =>do not wait, else wait   */ 
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canTake(        /* ret 0, if success                */
                HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                long          *len );    /* out: size of CMSG-Buffer         */
                                         /* in:  count of read messages      */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canRead(        /* ret 0, if success                */
                HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                long          *len,      /* out: size of CMSG-Buffer         */ 
                                         /* in:  count of read messages      */
                OVERLAPPED    *ovrlppd); /* NULL or overlapped-structure     */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canSend(        /* ret 0, if success                */
                HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                long          *len );    /* size of CMSG-Buffer              */ 
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canWrite(       /* ret 0, if success                */
                HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                long          *len,      /* size of CMSG-Buffer              */ 
                OVERLAPPED    *ovrlppd); /* NULL or overlapped-structure     */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canReadEvent(
                HANDLE        handle,    /* handle                           */
                EVMSG         *evmsg,    /* ptr to event-msg-buffer          */
                OVERLAPPED    *ovrlppd); /* NULL or overlapped-structure     */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canSendEvent(
                HANDLE        handle,    /* handle                           */
                EVMSG         *evmsg );  /* ptr to event-msg-buffer          */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canStatus(
                HANDLE        handle,    /* handle                           */
                CAN_IF_STATUS *cstat );  /* ptr to can-status-buffer         */
/*...........................................................................*/
EXPORT DWORD    CALLTYPE canIoctl(
                HANDLE        handle,  /* handle                             */
                unsigned long ulCmd,   /* Command                            */
                void *        pArg );  /* Ptr to command specific argument   */
          

#ifdef __cplusplus
}
#endif

#endif /* _ntcan_h_ */

