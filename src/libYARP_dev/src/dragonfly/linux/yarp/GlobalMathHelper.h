// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef GLOBALMATHHELPER
#define GLOBALMATHHELPER

#include <math.h>
#include "float.h"
#include "limits.h"
#include <unistd.h>
#include <sys/time.h>

#define UCHAR_MIN 0

#define PI 3.1415926535897932384626433832795
#define sqr(x) ((x)*(x))

inline float log_base(float x, float b) { 
    return( log(x)/log(b) );}

inline float d2r(float degrees)
{
	return( (degrees/360.0f) * (2.0f*PI) );
}

inline float r2d(float radians)
{
	return( (radians/(2.0f*PI)) * (360.0f) );
}

inline long myround(const float param)
{
    if((param - floor(param)) < 0.5f) 
        return long(floor(param));
    else 
        return long(ceil(param));
}

inline int twopow(int e)
{
    int out;
    out = 1<<e;
    return(out);
}

#endif
