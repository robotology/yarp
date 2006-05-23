
/************************************************************************
//
// E_Type.h : Type definition file
//
//	History :
//
//	Reference	-	J.H.	99-01-01
//	HANDLE		-	J.M.W.	99-05-18
//  fixed clash of types with XP on VisualStudio 8 - pasa 2006
//
//
************************************************************************/


#ifndef __E_TYPE_H__
#define __E_TYPE_H__

#if defined(DOMINO_DOS16_LIBRARY)
#define _FAR_ far
#define _HUGE_ huge
#else
#define _FAR_
#define _HUGE_
#endif

#if defined(__cplusplus)
extern "C" {
#endif
/* EURESYS type definitions : may be platform dependant !! */
#if !defined(WIN32) && !defined(__WIN32__)
typedef long                    INT32;
typedef unsigned long           UINT32;
typedef long _FAR_ *            PINT32;
typedef unsigned long _FAR_ *   PUINT32;
#else
	#ifndef _BASETSD_H_
	typedef int                     INT32;
	typedef unsigned int            UINT32;
	typedef int*                    PINT32;
	typedef unsigned int*           PUINT32;
	#endif
#endif

#define VOID                    void
typedef signed char             INT8;
typedef signed short            INT16;
typedef float                   FLOAT32;
typedef double                  FLOAT64;
typedef unsigned char           UINT8;
typedef unsigned short          UINT16;
typedef void _FAR_ *            PVOID;
typedef void _FAR_ * _FAR_ *    PPVOID;
typedef signed char *           PINT8;
typedef char *                  PCHAR;
typedef signed short *          PINT16;
typedef float _FAR_ *           PFLOAT32;
typedef double _FAR_ *          PFLOAT64;
typedef unsigned char _FAR_ *   PUINT8;
typedef unsigned short _FAR_ *  PUINT16;

#if defined(__cplusplus)
typedef const char _FAR_ *      CPCHAR;
typedef const char _FAR_ *      PCCHAR;
#else
typedef char _FAR_ *            CPCHAR;
typedef char _FAR_ *            PCCHAR;
#endif  

#if !defined (__huge)
#define __huge
#endif
typedef void __huge *           HPVOID;
typedef char __huge *           HPINT8;
typedef short __huge *          HPINT16;
typedef long __huge *           HPINT32;
typedef float __huge *          HPFLOAT32;
typedef double __huge *         HPFLOAT64;
typedef unsigned char __huge *  HPUINT8;
typedef unsigned short __huge * HPUINT16;
typedef unsigned long __huge *  HPUINT32;


#if !defined (_WINDOWS) || !defined(_Windows)
#define MEMORYOBJECT HGLOBAL
#else
#define MEMORYOBJECT PVOID
#endif

/*  Windows types */
#if !defined (_INC_WINDOWS)
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long       ULONG;
typedef int                 BOOL;
#if !defined (HANDLE)
typedef void* HANDLE;
#endif
#if !defined(TRUE)
#define TRUE (1==1)
#endif
#if !defined (FALSE)
#define FALSE (0==1)
#endif
#endif

typedef UINT8 byte;
typedef UINT16 word;
typedef UINT32 dword;

#if defined (__cplusplus)
}
#endif

#endif /* __E_TYPE_H__ */

