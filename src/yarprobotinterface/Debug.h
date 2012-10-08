/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef ROBOTINTERFACE_DEBUG_H
#define ROBOTINTERFACE_DEBUG_H

#include <iosfwd>
#include <sstream>
#include <vector>

#include <yarp/os/ConstString.h>


namespace RobotInterface
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

    inline Debug(const Debug &o) : stream(o.stream) {
        ++stream->ref;
    }

    inline ~Debug() {
        if (!--stream->ref) {
            print_output(stream->type, stream->oss);
            delete stream;
        }
    }

    inline Debug& operator<<(bool t) {
        stream->oss << (t ? "true" : "false");
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(char t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(signed short t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(unsigned short t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(signed int t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(unsigned int t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(signed long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(unsigned long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(signed long long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(unsigned long long t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(float t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(double t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(const char* t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }
    inline Debug& operator<<(const void * t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }

    inline Debug& operator<<(const std::string &t) {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }

    inline Debug& operator<<(yarp::os::ConstString t) {
        stream->oss << t.c_str();
        stream->oss << ' ';
        return *this;
    }

    template <typename T>
    inline Debug& operator<<(const std::vector<T> &t)
    {
        stream->oss << t;
        stream->oss << ' ';
        return *this;
    }

private:
    void print_output(MsgType t, const std::ostringstream &s);
};

}

inline RobotInterface::Debug debug() {
    return RobotInterface::Debug(RobotInterface::DebugType);
}
inline RobotInterface::Debug warning() {
    return RobotInterface::Debug(RobotInterface::WarningType);
}
inline RobotInterface::Debug error() {
    return RobotInterface::Debug(RobotInterface::ErrorType);
}
inline RobotInterface::Debug fatal() {
    return RobotInterface::Debug(RobotInterface::FatalType);
}


template <typename T>
inline std::ostringstream& operator<<(std::ostringstream &oss, const std::vector<T> &t)
{
    for (typename std::vector<T>::const_iterator it = t.begin(); it != t.end(); it++) {
        const T &p = *it;
        oss << p;
    }
    return oss;
}


#endif // ROBOTINTERFACE_DEBUG_H
