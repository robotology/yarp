/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_LOGSTREAM_H
#define YARP_OS_LOGSTREAM_H

#include <yarp/os/api.h>

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Os.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Time.h>

#include <cstdio>
#include <cstdlib>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>


namespace std {
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& t);
}

namespace yarp {
namespace os {

class YARP_os_API LogStream
{
    struct Stream
    {
        Stream(Log::LogType t, const char* fn, unsigned int l, const char* f, const LogComponent& c) :
                type(t),
                file(fn),
                line(l),
                func(f),
                systemtime(yarp::os::SystemClock::nowSystem()),
                networktime(!yarp::os::Time::isClockInitialized() ? 0.0 : (yarp::os::Time::isSystemClock() ? systemtime : yarp::os::Time::now())),
                comp(c),
                ref(1)
        {
        }
        std::ostringstream oss;   // NOLINT(misc-non-private-member-variables-in-classes)
        Log::LogType type;        // NOLINT(misc-non-private-member-variables-in-classes)
        const char* file;         // NOLINT(misc-non-private-member-variables-in-classes)
        unsigned int line;        // NOLINT(misc-non-private-member-variables-in-classes)
        const char* func;         // NOLINT(misc-non-private-member-variables-in-classes)
        double systemtime;        // NOLINT(misc-non-private-member-variables-in-classes)
        double networktime;       // NOLINT(misc-non-private-member-variables-in-classes)
        const LogComponent& comp; // NOLINT(misc-non-private-member-variables-in-classes)
        int ref;                  // NOLINT(misc-non-private-member-variables-in-classes)
    } * stream;

public:
    inline LogStream(Log::LogType type,
                     const char* file,
                     unsigned int line,
                     const char* func,
                     const LogComponent& comp = Log::defaultLogComponent()) :
            stream(new Stream(type, file, line, func, comp))
    {
    }

    inline LogStream(const LogStream& o) :
            stream(o.stream)
    {
        ++stream->ref;
    }

    inline ~LogStream()
    {
        if (!--stream->ref) {
            std::string s = stream->oss.str();
            if (!s.empty()) {
                // remove the last character if it an empty space (i.e.
                // always unless the user defined an operator<< that
                // does not add an empty space.
                if (s.back() == ' ') {
                    s.pop_back();
                } else {
                    yarp::os::Log(stream->file, stream->line, stream->func, yarp::os::Log::logInternalComponent()).warning(
                        "' ' was expected. Some `operator<<` does not add an extra space at the end");
                }
                // remove the last character if it is a \n
                if (s.back() == '\n') {
                    yarp::os::Log(stream->file, stream->line, stream->func, yarp::os::Log::logInternalComponent()).warning(
                        "Removing extra \\n (stream-style)");
                    s.pop_back();
                }
            }
            Log::do_log(stream->type,
                        s.c_str(),
                        stream->file,
                        stream->line,
                        stream->func,
                        stream->systemtime,
                        stream->networktime,
                        stream->comp);
            if (stream->type == yarp::os::Log::FatalType) {
                yarp_print_trace(stderr, stream->file, stream->line);
                delete stream;
                std::exit(-1);
            }
            delete stream;
        }
    }

    inline LogStream& operator<<(bool t)
    {
        stream->oss << (t ? "true" : "false");
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(char t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed short t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned short t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed int t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned int t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed long t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned long t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(signed long long t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(unsigned long long t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(float t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(double t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(const char* t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline LogStream& operator<<(const void* t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }

    inline LogStream& operator<<(const std::string& t)
    {
        stream->oss << t.c_str();
        stream->oss << ' ';
        return *this;
    }

    template <typename T>
    inline LogStream& operator<<(const std::vector<T>& t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
}; // class LogStream

} // namespace os
} // namespace yarp


template <typename T>
inline std::ostream& std::operator<<(std::ostream& os, const std::vector<T>& t)
{
    os << '[';
    for (typename std::vector<T>::const_iterator it = t.begin(); it != t.end(); ++it) {
        const T& p = *it;
        if (it != t.begin()) {
            os << ", ";
        }
        os << p;
    }
    os << ']';
    return os;
}

#endif // YARP_OS_LOGSTREAM_H
