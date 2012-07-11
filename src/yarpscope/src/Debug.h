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


#ifndef YARPSCOPE_DEBUG_H
#define YARPSCOPE_DEBUG_H

#include <iosfwd>
#include <sstream>

#include <glibmm/ustring.h>

#include <yarp/os/ConstString.h>


namespace YarpScope
{

enum MsgType { DebugType, WarningType, ErrorType, FatalType };


class Debug {
    struct Stream {
        Stream(MsgType t) : type(t), ref(1) {}
        std::ostringstream oss;
        MsgType type;
        int ref;
    } *stream;
public:
    inline Debug(MsgType type) :
        stream(new Stream(type))
    {
    }

    inline Debug(const Debug &o) : stream(o.stream) { ++stream->ref; }

    inline ~Debug() {
        if (!--stream->ref) {
            print_output(stream->type, stream->oss);
            delete stream;
        }
    }

    inline Debug& operator<<(bool t) { stream->oss << (t ? "true" : "false"); stream->oss << ' '; return *this; }
    inline Debug& operator<<(char t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(signed short t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(unsigned short t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(signed int t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(unsigned int t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(signed long t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(unsigned long t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(float t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(double t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(const char* t) { stream->oss << t; stream->oss << ' '; return *this; }
    inline Debug& operator<<(const void * t) { stream->oss << t; stream->oss << ' '; return *this; }

    inline Debug& operator<<(yarp::os::ConstString t) { stream->oss << t.c_str(); stream->oss << ' '; return *this; }
    inline Debug& operator<<(Glib::ustring t) { stream->oss << t.c_str(); stream->oss << ' '; return *this; }

private:
    void print_output(MsgType t, const std::ostringstream &s);
};

}

inline YarpScope::Debug debug() { return YarpScope::Debug(YarpScope::DebugType); }
inline YarpScope::Debug warning() { return YarpScope::Debug(YarpScope::WarningType); }
inline YarpScope::Debug error() { return YarpScope::Debug(YarpScope::ErrorType); }
inline YarpScope::Debug fatal() { return YarpScope::Debug(YarpScope::FatalType); }


#endif // YARPSCOPE_DEBUG_H
