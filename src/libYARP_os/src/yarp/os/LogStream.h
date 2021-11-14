/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <tuple>
#include <type_traits>
#include <utility>

namespace yarp::os {

class YARP_os_API LogStream
{
    struct Stream
    {
        Stream(Log::LogType t,
               const char* fn,
               unsigned int l,
               const char* f,
               const char* id,
               const double ct,
               const yarp::os::Log::Predicate pred,
               const LogComponent& c) :
                type(t),
                file(fn),
                line(l),
                func(f),
                id(id),
                systemtime(yarp::os::SystemClock::nowSystem()),
                networktime(!yarp::os::Time::isClockInitialized() ? 0.0 : (yarp::os::Time::isSystemClock() ? systemtime : yarp::os::Time::now())),
                externaltime(ct),
                pred(pred),
                comp(c),
                ref(1)
        {
        }
        std::ostringstream oss;
        Log::LogType type;
        const char* file;
        unsigned int line;
        const char* func;
        const char* id;
        double systemtime;
        double networktime;
        double externaltime;
        const yarp::os::Log::Predicate pred;
        const LogComponent& comp;
        int ref;
        bool nospace {false};
    } * stream;

public:
    inline LogStream(Log::LogType type,
                     const char* file,
                     unsigned int line,
                     const char* func,
                     const char* id,
                     const double externaltime,
                     const yarp::os::Log::Predicate pred = nullptr,
                     const LogComponent& comp = Log::defaultLogComponent()) :
            stream(new Stream(type, file, line, func, id, externaltime, pred, comp))
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
            if (!stream->pred || stream->pred()) {
                std::string s = stream->oss.str();
                if (!s.empty()) {
                    // remove the last character if it an empty space (i.e.
                    // always unless the user defined an operator<< that
                    // does not add an empty space.
                    if (s.back() == ' ') {
                        s.pop_back();
                    } else {
                        yarp::os::Log(stream->file, stream->line, stream->func, nullptr, yarp::os::Log::logInternalComponent()).warning(
                            "' ' was expected. Some `operator<<` does not add an extra space at the end");
                    }
                    // remove the last character if it is a \n
                    if (s.back() == '\n') {
                        yarp::os::Log(stream->file, stream->line, stream->func, nullptr, yarp::os::Log::logInternalComponent()).warning(
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
                            stream->externaltime,
                            stream->comp,
                            stream->id);
            }

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
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(char t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(signed short t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(unsigned short t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(signed int t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(unsigned int t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(signed long t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(unsigned long t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(signed long long t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(unsigned long long t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(float t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(double t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(const char* t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }
    inline LogStream& operator<<(const void* t)
    {
        stream->oss << t;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }

    inline LogStream& operator<<(const std::string& t)
    {
        stream->oss << t.c_str();
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }

#if !defined(SWIG)
    template <typename T1, typename T2>
    inline LogStream& operator<<(const std::pair<T1, T2>& t)
    {
        bool nospace = stream->nospace;
        stream->nospace = true;
        stream->oss << '{';
        *this << t.first;
        stream->oss << ", ";
        *this << t.second << '}';
        stream->nospace = nospace;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }

    template <typename Container, typename = typename Container::value_type>
    inline LogStream& operator<<(const Container& cont)
    {
        return streamInternal(cont);
    }

    template <typename Arr, typename std::enable_if_t<std::is_array_v<Arr>, bool> = true>
    inline LogStream& operator<<(const Arr& arr)
    {
        return streamInternal(arr);
    }

    template <typename... Args>
    inline LogStream& operator<<(const std::tuple<Args...>& t)
    {
        bool nospace = stream->nospace;
        stream->nospace = true;
        return tupleInternal<0>(t, nospace);
    }

private:
    template <typename C>
    inline LogStream& streamInternal(const C& c)
    {
        bool nospace = stream->nospace;
        stream->nospace = true;
        stream->oss << '[';
        for (auto it = std::begin(c); it != std::end(c); ++it) {
            if (it != std::begin(c)) {
                stream->oss << ", ";
            }
            *this << *it;
        }
        stream->oss << ']';
        stream->nospace = nospace;
        if (!stream->nospace) {
            stream->oss << ' ';
        }
        return *this;
    }

    template <std::size_t I, typename... Args>
    inline LogStream& tupleInternal(const std::tuple<Args...>& t, bool nospace)
    {
        if constexpr (I == 0) {
            stream->oss << '{';
        }
        if constexpr (I < sizeof...(Args)) {
            if constexpr (I != 0) {
                stream->oss << ", ";
            }
            *this << std::get<I>(t);
            return tupleInternal<I + 1>(t, nospace);
        } else {
            stream->oss << '}';
            stream->nospace = nospace;
            if (!stream->nospace) {
                stream->oss << ' ';
            }
        }
        return *this;
    }
#endif // !defined(SWIG)

}; // class LogStream

} // namespace yarp::os

#endif // YARP_OS_LOGSTREAM_H
