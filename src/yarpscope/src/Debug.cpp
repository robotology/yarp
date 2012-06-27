/*
 *  This file is part of Yarp Port Scope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
