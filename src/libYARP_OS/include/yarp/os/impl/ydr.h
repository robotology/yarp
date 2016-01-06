// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


// Functions for serialization in YARP form.
// Approximates a subset of the XDR API, with "xdr" -> "ydr".
// 
// This is part of an experiment to unify some APIs with the player project


#ifndef YARP2_YDR
#define YARP2_YDR

#ifdef __cplusplus
extern "C" {
#endif


#define YDR_ENCODE (0)
#define YDR_DECODE (1)
#define YDR_FREE (2)

    typedef struct YDR YDR;
    struct YDR {
        void *controller;
    };
  
    extern int ydrmem_create(YDR *ydrs, void* buf, unsigned int buflen, int op);
    extern int ydr_u_int(YDR *ydrs, unsigned int *p);
    extern int ydr_u_short(YDR *ydrs, unsigned short *p);
    extern int ydr_u_char(YDR *ydrs, unsigned char *p);
    extern int ydr_double(YDR *ydrs, double *p);
    extern int ydr_float(YDR *ydrs, float *p);
    extern int ydr_bool(YDR *ydrs, int *p);
    extern int ydr_char(YDR *ydrs, char *p);
    extern int ydr_short(YDR *ydrs, short *p);
    extern int ydr_int(YDR *ydrs, int *p);
    extern int ydr_bytes(YDR *ydrs, char **data, unsigned int *count, 
                         unsigned int max_count);
    extern unsigned int ydr_getpos(YDR *ydrs);
    extern void ydr_destroy(YDR *ydrs);


    // not implemented yet
    extern int ydr_array(YDR *ydrs, void *addr, unsigned int *size,
                         unsigned int maxsize, unsigned int elsize,
                         void *substore);
    extern int ydr_vector(YDR *ydrs, void *addr, unsigned int count,
                          unsigned int elsize, void *substore);
    extern int ydr_longlong_t(YDR *ydrs, long long int *p);

#ifdef __cplusplus
}
#endif


#endif
