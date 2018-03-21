/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_LOGSTREAM_H
#define YARP_OS_LOGSTREAM_H

#include <iosfwd>
#include <sstream>
#include <vector>

#include <yarp/os/api.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Log.h>
#include <yarp/os/Os.h>

#include <cstdio>
#include <cstdlib>

namespace std {
template <typename T>
std::ostream& operator<<(std::ostream &os, const std::vector<T> &t);
}

namespace yarp {
namespace os {

class YARP_OS_API LogStream {
    struct Stream {
        Stream(Log::LogType t, const char *fn, unsigned int l, const char *f) : type(t), file(fn), line(l), func(f), ref(1) {}
               std::ostringstream oss;
               Log::LogType type;
               const char *file;
               unsigned int line;
               const char *func;
               int ref;
    } *stream;
public:

    inline LogStream(Log::LogType type,
                     const char *file,
                     unsigned int line,
                     const char *func) :
        stream(new Stream(type, file, line, func))
    {
    }

    inline LogStream(const LogStream &o) : stream(o.stream) {
        ++stream->ref;
    }

    inline ~LogStream() {
        if (!--stream->ref) {
            if (Log::print_callback) {
                Log::print_callback(stream->type, stream->oss.str().c_str(), stream->file, stream->line, stream->func);
            }
            if (Log::forward_callback) {
                Log::forward_callback(stream->type, stream->oss.str().c_str(), stream->file, stream->line, stream->func);
            }
            if (stream->type == yarp::os::Log::FatalType) {
                yarp_print_trace(stderr, stream->file, stream->line);
                delete stream;
                std::exit(-1);
            }
            delete stream;
        }
    }

    inline LogStream& operator<<(bool t) {
        stream->oss << (t ? "true" : "false");
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(char t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed short t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned short t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed int t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned int t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed long long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned long long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(float t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(double t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(const char* t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(const void * t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }

#ifndef YARP_CONSTSTRING_IS_STD_STRING
    inline LogStream& operator<<(const std::string &t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
#endif

    inline LogStream& operator<<(const yarp::os::ConstString &t) {
        stream->oss << t.c_str();
        stream->oss << ' ';
        return *this;
    }

    template <typename T>
    inline LogStream& operator<<(const std::vector<T> &t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
}; // class LogStream

} // namespace os
} // namespace yarp



template <typename T>
inline std::ostream& std::operator<<(std::ostream &os, const std::vector<T> &t)
{
    os << '[';
    for (typename std::vector<T>::const_iterator it = t.begin(); it != t.end(); ++it) {
        const T &p = *it;
        if (it != t.begin()) {
            os << ", ";
        }
        os << p;
    }
    os << ']';
    return os;
}

#endif // YARP_OS_LOGSTREAM_H
