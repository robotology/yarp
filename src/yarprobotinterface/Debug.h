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

enum MsgType { TraceType, DebugType, WarningType, ErrorType, FatalType };


class Debug {
    struct Stream {
        Stream(MsgType t, const char *fn, unsigned int l, const char *f) : type(t), file(fn), line(l), func(f), ref(1) {}
        std::ostringstream oss;
        MsgType type;
        const char *file;
        unsigned int line;
        const char *func;
        int ref;
    } *stream;
public:

    inline Debug(MsgType type,
                 const char *file,
                 unsigned int line,
                 const char *func) :
        stream(new Stream(type, file, line, func))
    {
    }

    inline Debug(const Debug &o) : stream(o.stream) {
        ++stream->ref;
    }

    inline ~Debug() {
        if (!--stream->ref) {
            print_output(stream->type, stream->oss, stream->file, stream->line, stream->func);
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

    /*!
     * \brief Set the output file used by yTrace()
     */
    static void setTraceFile(const std::string &filename);

    /*!
     * \brief Set the output file used by yDebug()
     */
    static void setOutputFile(const std::string &filename);

    /*!
     * \brief Set the output file used by yWarning(), yError() and yyyTrace()
     */
    static void setErrorFile(const std::string &filename);

private:
    void print_output(MsgType t,
                      const std::ostringstream &s,
                      const char *file,
                      unsigned int line,
                      const char *func);

    static std::ofstream ftrc; /// Used by yTrace()
    static std::ofstream fout; /// Used by yDebug()
    static std::ofstream ferr; /// Used by yWarning(), yError() and yyyTrace()

    static bool colored_output;
    static bool verbose_output;
};

}

#ifdef __GNUC__
#define __YFUNCTION__ __PRETTY_FUNCTION__
#else // __GNUC__
#define __YFUNCTION__ __func__
#endif // __GNUC__

#define yTrace() RobotInterface::Debug(RobotInterface::TraceType, __FILE__, __LINE__, __YFUNCTION__)
#define yDebug() RobotInterface::Debug(RobotInterface::DebugType, __FILE__, __LINE__, __YFUNCTION__)
#define yWarning() RobotInterface::Debug(RobotInterface::WarningType, __FILE__, __LINE__, __YFUNCTION__)
#define yError() RobotInterface::Debug(RobotInterface::ErrorType, __FILE__, __LINE__, __YFUNCTION__)
#define yFatal() RobotInterface::Debug(RobotInterface::FatalType, __FILE__, __LINE__, __YFUNCTION__)

template <typename T>
inline std::ostringstream& operator<<(std::ostringstream &oss, const std::vector<T> &t)
{
    for (typename std::vector<T>::const_iterator it = t.begin(); it != t.end(); it++) {
        const T &p = *it;
        oss << p << ' ';
    }
    return oss;
}


#define YFIXME_NOTIMPLEMENTED yError() << "FIXME: NOT IMPLEMENTED";


#endif // ROBOTINTERFACE_DEBUG_H
