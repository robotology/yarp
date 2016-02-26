/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Time.h"

#include <ace/OS.h>


void Time::delay(double seconds) {
    ACE_Time_Value tv;
    tv.sec (long(seconds));
    tv.usec (long((seconds-long(seconds)) * 1.0e6));
    ACE_OS::sleep(tv);
}


double Time::now() {
    ACE_Time_Value timev = ACE_OS::gettimeofday ();
    return double(timev.sec()) + timev.usec() * 1e-6; 
}


