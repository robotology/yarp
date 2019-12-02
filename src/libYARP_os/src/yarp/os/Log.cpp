/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Log.h>

#include <yarp/os/impl/LogForwarder.h>
#include <yarp/os/impl/LogImpl.h>

#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef YARP_HAS_ACE
#    include <ace/Stack_Trace.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(YARP_HAS_EXECINFO_H)
#    include <execinfo.h>
#endif

#include <yarp/conf/system.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

#include <cstdio>


#define YARP_MAX_LOG_MSG_SIZE 1024

#define RED     (colored_output ? "\033[01;31m" : "")
#define GREEN   (colored_output ? "\033[01;32m" : "")
#define YELLOW  (colored_output ? "\033[01;33m" : "")
#define BLUE    (colored_output ? "\033[01;34m" : "")
#define MAGENTA (colored_output ? "\033[01;35m" : "")
#define CYAN    (colored_output ? "\033[01;36m" : "")
#define WHITE   (colored_output ? "\033[01;37m" : "")
#define RED_BG  (colored_output ? "\033[01;41m" : "")
#define CLEAR   (colored_output ? "\033[00m" : "")

#ifdef YARP_HAS_WIN_VT_SUPPORT
#include <windows.h>

static bool yarp_logger_vt_colors_enabled = false;
extern "C" static void yarp_logger_enable_vt_colors()
{
    DWORD handleMode = 0;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hStdout != INVALID_HANDLE_VALUE && GetConsoleMode(hStdout, &handleMode)) {
        handleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hStdout, handleMode);
    }

    yarp_logger_vt_colors_enabled = true;
}
#endif

#if defined(_WIN32) && !defined(YARP_HAS_WIN_VT_SUPPORT)
bool yarp::os::impl::LogImpl::colored_output(false);
#else
bool yarp::os::impl::LogImpl::colored_output((getenv("YARP_COLORED_OUTPUT") != nullptr)     &&  (strcmp(yarp::os::getenv("YARP_COLORED_OUTPUT"), "1") == 0));
#endif
bool yarp::os::impl::LogImpl::verbose_output((getenv("YARP_VERBOSE_OUTPUT") != nullptr)     &&  (strcmp(yarp::os::getenv("YARP_VERBOSE_OUTPUT"), "1") == 0));
bool yarp::os::impl::LogImpl::trace_output((getenv("YARP_TRACE_ENABLE") != nullptr)         &&  (strcmp(yarp::os::getenv("YARP_TRACE_ENABLE"), "1") == 0));
bool yarp::os::impl::LogImpl::debug_output((getenv("YARP_DEBUG_ENABLE") == nullptr)         || !(strcmp(yarp::os::getenv("YARP_DEBUG_ENABLE"), "0") == 0));
bool yarp::os::impl::LogImpl::forward_output((getenv("YARP_FORWARD_LOG_ENABLE") != nullptr) &&  (strcmp(yarp::os::getenv("YARP_FORWARD_LOG_ENABLE"), "1") == 0));

yarp::os::Log::LogCallback yarp::os::Log::print_callback = yarp::os::impl::LogImpl::print_callback;
yarp::os::Log::LogCallback yarp::os::Log::forward_callback = yarp::os::impl::LogImpl::forward_callback;

yarp::os::impl::LogImpl::LogImpl(const char* file,
                                 unsigned int line,
                                 const char* func) :
        file(file),
        line(line),
        func(func)
{
}


void yarp::os::impl::LogImpl::print_callback(yarp::os::Log::LogType t,
                                             const char* msg,
                                             const char* file,
                                             const unsigned int line,
                                             const char* func)
{
    switch (t) {
    case yarp::os::Log::TraceType:
        if (trace_output) {
            if (verbose_output) {
                std::cout << "[" << WHITE << "TRACE" << CLEAR << "]" << file << ":" << line << " " << WHITE << func << CLEAR << ": " << msg << std::endl;
            } else {
                std::cout << "[" << WHITE << "TRACE" << CLEAR << "]" << func << msg << std::endl;
            }
        }
        break;
    case yarp::os::Log::DebugType:
        if (debug_output) {
            if (verbose_output) {
                std::cout << "[" << GREEN << "DEBUG" << CLEAR << "]" << file << ":" << line << " " << GREEN << func << CLEAR << ": " << msg << std::endl;
            } else {
                std::cout << "[" << GREEN << "DEBUG" << CLEAR << "]" << msg << std::endl;
            }
        }
        break;
    case yarp::os::Log::InfoType:
        if (verbose_output) {
            std::cout << "[" << BLUE << "INFO" << CLEAR << "]" << file << ":" << line << " " << BLUE << func << CLEAR << ": " << msg << std::endl;
        } else {
            std::cout << "[" << BLUE << "INFO" << CLEAR << "]" << msg << std::endl;
        }
        break;
    case yarp::os::Log::WarningType:
        if (verbose_output) {
            std::cerr << "[" << YELLOW << "WARNING" << CLEAR << "]" << file << ":" << line << " " << YELLOW << func << CLEAR << ": " << msg << std::endl;
        } else {
            std::cerr << "[" << YELLOW << "WARNING" << CLEAR << "]" << msg << std::endl;
        }
        break;
    case yarp::os::Log::ErrorType:
        if (verbose_output) {
            std::cerr << "[" << RED << "ERROR" << CLEAR << "]" << file << ":" << line << " " << RED << func << CLEAR << ": " << msg << std::endl;
        } else {
            std::cerr << "[" << RED << "ERROR" << CLEAR << "]" << msg << std::endl;
        }
        break;
    case yarp::os::Log::FatalType:
        if (verbose_output) {
            std::cerr << "[" << WHITE << RED_BG << "FATAL" << CLEAR << "]" << file << ":" << line << " " << WHITE << RED_BG << func << CLEAR << ": " << msg << std::endl;
        } else {
            std::cerr << "[" << WHITE << RED_BG << "FATAL" << CLEAR << "]" << msg << std::endl;
        }
        break;
    default:
        break;
    }
}

void yarp::os::impl::LogImpl::forward_callback(yarp::os::Log::LogType t,
                                               const char* msg,
                                               const char* file,
                                               const unsigned int line,
                                               const char* func)
{
    if (!forward_output) {
        return;
    }

    std::stringstream stringstream_buffer;
    LogForwarder& theForwarder = LogForwarder::getInstance();

    switch (t) {
    case yarp::os::Log::TraceType:
        if (trace_output) {
            if (verbose_output) {
                stringstream_buffer << "[TRACE]" << file << ":" << line << " " << func << ": " << msg << std::endl;
            } else {
                stringstream_buffer << "[TRACE]" << func << msg << std::endl;
            }
            theForwarder.forward(stringstream_buffer.str());
        }
        break;
    case yarp::os::Log::DebugType:
        if (debug_output) {
            if (verbose_output) {
                stringstream_buffer << "[DEBUG]" << file << ":" << line << " " << func << ": " << msg << std::endl;
            } else {
                stringstream_buffer << "[DEBUG]" << msg << std::endl;
            }
            theForwarder.forward(stringstream_buffer.str());
        }
        break;
    case yarp::os::Log::InfoType:
        if (verbose_output) {
            stringstream_buffer << "[INFO]" << file << ":" << line << " " << func << ": " << msg << std::endl;
        } else {
            stringstream_buffer << "[INFO]" << msg << std::endl;
        }
        theForwarder.forward(stringstream_buffer.str());
        break;
    case yarp::os::Log::WarningType:
        if (verbose_output) {
            stringstream_buffer << "[WARNING]" << file << ":" << line << " " << func << ": " << msg << std::endl;
        } else {
            stringstream_buffer << "[WARNING]" << msg << std::endl;
        }
        theForwarder.forward(stringstream_buffer.str());
        break;
    case yarp::os::Log::ErrorType:
        if (verbose_output) {
            stringstream_buffer << "[ERROR]" << file << ":" << line << " " << func << ": " << msg << std::endl;
        } else {
            stringstream_buffer << "[ERROR]" << msg << std::endl;
        }
        theForwarder.forward(stringstream_buffer.str());
        break;
    case yarp::os::Log::FatalType:
        if (verbose_output) {
            stringstream_buffer << "[FATAL]" << file << ":" << line << " " << func << ": " << msg << std::endl;
        } else {
            stringstream_buffer << "[FATAL]" << msg << std::endl;
        }
        theForwarder.forward(stringstream_buffer.str());
        yarp_print_trace(stderr, file, line);
        break;
    default:
        break;
    }
}

yarp::os::Log::Log(const char* file,
                   unsigned int line,
                   const char* func) :
        mPriv(new yarp::os::impl::LogImpl(file, line, func))
{
#ifdef YARP_HAS_WIN_VT_SUPPORT
    if (!yarp_logger_vt_colors_enabled) {
        yarp_logger_enable_vt_colors();
    }
#endif
}

yarp::os::Log::Log() :
        mPriv(new yarp::os::impl::LogImpl(nullptr, 0, nullptr))
{
#ifdef YARP_HAS_WIN_VT_SUPPORT
    if (!yarp_logger_vt_colors_enabled) {
        yarp_logger_enable_vt_colors();
    }
#endif
}

yarp::os::Log::~Log()
{
    delete mPriv;
}


void yarp::os::Log::trace(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg != nullptr) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        buf[YARP_MAX_LOG_MSG_SIZE - 1] = 0;
        if (w > 0 && buf[w - 1] == '\n') {
            buf[w - 1] = 0;
        }
        if (print_callback != nullptr) {
            print_callback(yarp::os::Log::TraceType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback != nullptr) {
            forward_callback(yarp::os::Log::TraceType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (w > YARP_MAX_LOG_MSG_SIZE - 1) {
            yarp::os::Log(mPriv->file, mPriv->line, mPriv->func).warning("Previous trace message was truncated");
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::trace() const
{
    return yarp::os::LogStream(yarp::os::Log::TraceType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::debug(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg != nullptr) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        buf[YARP_MAX_LOG_MSG_SIZE - 1] = 0;
        if (w > 0 && buf[w - 1] == '\n') {
            buf[w - 1] = 0;
        }
        if (print_callback != nullptr) {
            print_callback(yarp::os::Log::DebugType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback != nullptr) {
            forward_callback(yarp::os::Log::DebugType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (w > YARP_MAX_LOG_MSG_SIZE - 1) {
            yarp::os::Log(mPriv->file, mPriv->line, mPriv->func).warning("Previous debug message was truncated");
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::debug() const
{
    return yarp::os::LogStream(yarp::os::Log::DebugType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::info(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg != nullptr) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        buf[YARP_MAX_LOG_MSG_SIZE - 1] = 0;
        if (w > 0 && buf[w - 1] == '\n') {
            buf[w - 1] = 0;
        }
        if (print_callback != nullptr) {
            print_callback(yarp::os::Log::InfoType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback != nullptr) {
            forward_callback(yarp::os::Log::InfoType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (w > YARP_MAX_LOG_MSG_SIZE - 1) {
            yarp::os::Log(mPriv->file, mPriv->line, mPriv->func).warning("Previous info message was truncated");
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::info() const
{
    return yarp::os::LogStream(yarp::os::Log::InfoType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::warning(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg != nullptr) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        buf[YARP_MAX_LOG_MSG_SIZE - 1] = 0;
        if (w > 0 && buf[w - 1] == '\n') {
            buf[w - 1] = 0;
        }
        if (print_callback != nullptr) {
            print_callback(yarp::os::Log::WarningType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback != nullptr) {
            forward_callback(yarp::os::Log::WarningType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (w > YARP_MAX_LOG_MSG_SIZE - 1) {
            yarp::os::Log(mPriv->file, mPriv->line, mPriv->func).warning("Previous warning message was truncated");
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::warning() const
{
    return yarp::os::LogStream(yarp::os::Log::WarningType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::error(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg != nullptr) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        buf[YARP_MAX_LOG_MSG_SIZE - 1] = 0;
        if (w > 0 && buf[w - 1] == '\n') {
            buf[w - 1] = 0;
        }
        if (print_callback != nullptr) {
            print_callback(yarp::os::Log::ErrorType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback != nullptr) {
            forward_callback(yarp::os::Log::ErrorType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (w > YARP_MAX_LOG_MSG_SIZE - 1) {
            yarp::os::Log(mPriv->file, mPriv->line, mPriv->func).warning("Previous error message was truncated");
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::error() const
{
    return yarp::os::LogStream(yarp::os::Log::ErrorType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::fatal(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg != nullptr) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        buf[YARP_MAX_LOG_MSG_SIZE - 1] = 0;
        if (w > 0 && buf[w - 1] == '\n') {
            buf[w - 1] = 0;
        }
        if (print_callback != nullptr) {
            print_callback(yarp::os::Log::FatalType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback != nullptr) {
            forward_callback(yarp::os::Log::FatalType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (w > YARP_MAX_LOG_MSG_SIZE - 1) {
            yarp::os::Log(mPriv->file, mPriv->line, mPriv->func).warning("Previous fatal message was truncated");
        }
    }
    va_end(args);
    yarp_print_trace(stderr, mPriv->file, mPriv->line);
    std::exit(-1);
}

yarp::os::LogStream yarp::os::Log::fatal() const
{
    return yarp::os::LogStream(yarp::os::Log::FatalType, mPriv->file, mPriv->line, mPriv->func);
}

void yarp::os::Log::setLogCallback(yarp::os::Log::LogCallback cb)
{
    print_callback = cb;
}

void yarp_print_trace(FILE* out, const char* file, int line)
{
#ifdef YARP_HAS_ACE
    YARP_UNUSED(file);
    YARP_UNUSED(line);
    ACE_Stack_Trace st(-1);
    // TODO demangle symbols using <cxxabi.h> and abi::__cxa_demangle
    //      when available.
    fprintf(out, "%s", st.c_str());
#elif defined(YARP_HAS_EXECINFO_H)
    const size_t max_depth = 100;
    size_t stack_depth;
    void* stack_addrs[max_depth];
    char** stack_strings;
    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);
    fprintf(out, "Assertion thrown at %s:%d by code called from:\n", file, line);
    for (size_t i = 1; i < stack_depth; i++) {
        fprintf(out, " --> %s\n", stack_strings[i]);
    }
    free(stack_strings); // malloc()ed by backtrace_symbols
    fflush(out);
#else
    // Not implemented on this platform
#endif
}
