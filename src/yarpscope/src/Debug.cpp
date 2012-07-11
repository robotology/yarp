/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */


#include "Debug.h"

#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <glib/gmessages.h>


void YarpScope::Debug::print_output(MsgType t, const std::ostringstream &s)
{
    switch (t) {
        case DebugType:
            g_debug(s.str().c_str());
            break;
        case WarningType:
            g_warning(s.str().c_str());
            break;
        case ErrorType:
            g_critical(s.str().c_str());
            break;
        case FatalType:
            g_error(s.str().c_str());
            exit(1);
            break;
    }
}
