/*
 *  This file is part of gPortScope
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


#ifndef GPORTSCOPE_DEBUG_H
#define GPORTSCOPE_DEBUG_H

#include <iosfwd>
#include <sstream>

#include <glibmm/ustring.h>

#include <yarp/os/ConstString.h>


namespace GPortScope
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

inline GPortScope::Debug debug() { return GPortScope::Debug(GPortScope::DebugType); }
inline GPortScope::Debug warning() { return GPortScope::Debug(GPortScope::WarningType); }
inline GPortScope::Debug error() { return GPortScope::Debug(GPortScope::ErrorType); }
inline GPortScope::Debug fatal() { return GPortScope::Debug(GPortScope::FatalType); }


#endif // GPORTSCOPE_DEBUG_H
