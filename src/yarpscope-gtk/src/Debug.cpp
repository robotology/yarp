/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Debug.h"

#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <glib.h>


void YarpScope::Debug::print_output(MsgType t, const std::ostringstream &s)
{
    switch (t) {
        case DebugType:
            g_debug("%s", s.str().c_str());
            break;
        case WarningType:
            g_warning("%s", s.str().c_str());
            break;
        case ErrorType:
            g_critical("%s", s.str().c_str());
            break;
        case FatalType:
            g_error("%s", s.str().c_str());
            exit(1);
            break;
    }
}
