/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Log.h>

#include <yarp/os/LogComponent.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/LogForwarder.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/Storable.h>

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mutex>
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

#ifdef YARP_HAS_WIN_VT_SUPPORT
#include <windows.h>
#endif


class yarp::os::impl::LogPrivate
{
public:
    LogPrivate(const char* file,
               const unsigned int line,
               const char* func,
               const LogComponent& comp);

    void log(yarp::os::Log::LogType type,
             const char *msg,
             va_list args) const;

    static void print_callback(yarp::os::Log::LogType t,
                               const char* msg,
                               const char* file,
                               const unsigned int line,
                               const char* func,
                               double systemtime,
                               double networktime,
                               const char* comp_name);

    static void forward_callback(yarp::os::Log::LogType t,
                                 const char* msg,
                                 const char* file,
                                 const unsigned int line,
                                 const char* func,
                                 double systemtime,
                                 double networktime,
                                 const char* comp_name);

    // Calls the right print and forward callbacks
    static void do_log(yarp::os::Log::LogType type,
                       const char* msg,
                       const char* file,
                       const unsigned int line,
                       const char* func,
                       double systemtime,
                       double networktime,
                       const LogComponent& comp_name);

    // This is a LogCallback that calls the print callback that is currently
    // set, even if this is changed later
    static void default_print_callback(yarp::os::Log::LogType type,
                                       const char* msg,
                                       const char* file,
                                       const unsigned int line,
                                       const char* func,
                                       double systemtime,
                                       double networktime,
                                       const char* comp_name)
    {
        if (auto cb = current_print_callback.load()) {
            cb(type, msg, file, line, func, systemtime, networktime, comp_name);
        }
    }

    // This is a LogCallback that calls the forward callback that is currently
    // set, even if this is changed later
    static void default_forward_callback(yarp::os::Log::LogType type,
                                         const char* msg,
                                         const char* file,
                                         const unsigned int line,
                                         const char* func,
                                         double systemtime,
                                         double networktime,
                                         const char* comp_name)
    {
        if (auto cb = current_forward_callback.load()) {
            cb(type, msg, file, line, func, systemtime, networktime, comp_name);
        }
    }

    static const LogComponent& logInternalComponent();

#ifdef YARP_HAS_WIN_VT_SUPPORT
    bool enable_vt_colors();
#endif

    const char* file;         // NOLINT(misc-non-private-member-variables-in-classes)
    const unsigned int line;  // NOLINT(misc-non-private-member-variables-in-classes)
    const char* func;         // NOLINT(misc-non-private-member-variables-in-classes)
    double systemtime;        // NOLINT(misc-non-private-member-variables-in-classes)
    double networktime;       // NOLINT(misc-non-private-member-variables-in-classes)
    const LogComponent& comp; // NOLINT(misc-non-private-member-variables-in-classes)

    static std::atomic<bool> yarprun_format;
    static std::atomic<bool> colored_output;
    static std::atomic<bool> verbose_output;
    static std::atomic<bool> compact_output;
    static std::atomic<bool> debug_output;
    static std::atomic<bool> trace_output;
    static std::atomic<bool> forward_output;
    static std::atomic<bool> forward_codeinfo;
    static std::atomic<bool> forward_hostname;
    static std::atomic<bool> forward_processinfo;
    static std::atomic<bool> forward_backtrace;
    static std::atomic<bool> debug_log;
    static std::atomic<bool> quiet;
    static std::atomic<bool> verbose;
#ifdef YARP_HAS_WIN_VT_SUPPORT
    static std::atomic<bool> vt_colors_enabled;
#endif

    static std::atomic<yarp::os::Log::LogType> minimumPrintLevel;
    static std::atomic<yarp::os::Log::LogType> minimumForwardLevel;

    static std::atomic<Log::LogCallback> current_print_callback;
    static std::atomic<Log::LogCallback> current_forward_callback;

    static const yarp::os::LogComponent log_internal_component;
};

// BEGIN Utilities

#define BOLD_RED     (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[01;31m" : "")
#define BOLD_GREEN   (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[01;32m" : "")
#define BOLD_YELLOW  (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[01;33m" : "")
#define BOLD_BLUE    (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[01;34m" : "")
#define BOLD_MAGENTA (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[01;35m" : "")
#define BOLD_CYAN    (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[01;36m" : "")
#define BOLD_WHITE   (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[01;37m" : "")
#define RED          (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[31m" : "")
#define GREEN        (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[32m" : "")
#define YELLOW       (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[33m" : "")
#define BLUE         (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[34m" : "")
#define MAGENTA      (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[35m" : "")
#define CYAN         (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[36m" : "")
#define WHITE        (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[37m" : "")
#define RED_BG       (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[41m" : "")
#define GREEN_BG     (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[42m" : "")
#define YELLOW_BG    (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[43m" : "")
#define BLUE_BG      (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[44m" : "")
#define MAGENTA_BG   (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[45m" : "")
#define CYAN_BG      (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[46m" : "")
#define WHITE_BG     (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[47m" : "")
#define CLEAR        (yarp::os::impl::LogPrivate::colored_output.load() ? "\033[00m" : "")

namespace {

inline bool from_env(const char* name, bool defaultvalue)
{
    const char *strvalue = yarp::os::getenv(name);

    if(!strvalue) { return defaultvalue; }

    if(strcmp(strvalue, "1") == 0) { return true; }
    if(strcmp(strvalue, "true") == 0) { return true; }
    if(strcmp(strvalue, "True") == 0) { return true; }
    if(strcmp(strvalue, "TRUE") == 0) { return true; }
    if(strcmp(strvalue, "on") == 0) { return true; }
    if(strcmp(strvalue, "On") == 0) { return true; }
    if(strcmp(strvalue, "ON") == 0) { return true; }

    if(strcmp(strvalue, "0") == 0) { return false; }
    if(strcmp(strvalue, "false") == 0) { return false; }
    if(strcmp(strvalue, "False") == 0) { return false; }
    if(strcmp(strvalue, "FALSE") == 0) { return false; }
    if(strcmp(strvalue, "off") == 0) { return false; }
    if(strcmp(strvalue, "Off") == 0) { return false; }
    if(strcmp(strvalue, "OFF") == 0) { return false; }

    return defaultvalue;
}

inline const char* logTypeToString(yarp::os::Log::LogType t)
{
    switch (t) {
    case yarp::os::Log::TraceType:
        return "TRACE";
    case yarp::os::Log::DebugType:
        return "DEBUG";
    case yarp::os::Log::InfoType:
        return "INFO";
    case yarp::os::Log::WarningType:
        return "WARNING";
    case yarp::os::Log::ErrorType:
        return "ERROR";
    case yarp::os::Log::FatalType:
        return "FATAL";
    default:
        return "";
    }
}

inline const char* logTypeToColor(yarp::os::Log::LogType t)
{
    switch (t) {
    case yarp::os::Log::TraceType:
        return BOLD_WHITE;
    case yarp::os::Log::DebugType:
        return BOLD_GREEN;
    case yarp::os::Log::InfoType:
        return BOLD_BLUE;
    case yarp::os::Log::WarningType:
        return BOLD_YELLOW;
    case yarp::os::Log::ErrorType:
        return BOLD_RED;
    case yarp::os::Log::FatalType:
        return BOLD_WHITE;
    case yarp::os::Log::LogTypeReserved:
        return CYAN;
    default:
        return "";
    }
}

inline const char* logTypeToBgColor(yarp::os::Log::LogType t)
{
    switch (t) {
    case yarp::os::Log::FatalType:
        return RED_BG;
    case yarp::os::Log::TraceType:
    case yarp::os::Log::DebugType:
    case yarp::os::Log::InfoType:
    case yarp::os::Log::WarningType:
    case yarp::os::Log::ErrorType:
    default:
        return "";
    }
}

std::string backtrace()
{
#ifdef YARP_HAS_ACE
    ACE_Stack_Trace st(-1);
    // TODO demangle symbols using <cxxabi.h> and abi::__cxa_demangle
    //      when available.
    return {st.c_str()};
#elif defined(YARP_HAS_EXECINFO_H)
    const size_t max_depth = 100;
    size_t stack_depth;
    void* stack_addrs[max_depth];
    char** stack_strings;
    stack_depth = ::backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);
    std::ostringstream ost;
    for (size_t i = 1; i < stack_depth; i++) {
        ost << stack_strings[i] << '\n';
    }
    free(stack_strings); // malloc()ed by backtrace_symbols
    return ost.str();
#else
    // Not implemented on this platform
    return {};
#endif
}

inline void forwardable_output(std::ostream* ost,
                               yarp::os::Log::LogType t,
                               const char* msg,
                               const char* file,
                               const unsigned int line,
                               const char* func,
                               double systemtime,
                               double networktime,
                               const char* comp_name)
{
    const char *level = logTypeToString(t);

    // This generates the same string a Property containing the following keys:
    // * level
    // * systemtime
    // * networktime
    // * filename (if YARP_FORWARD_CODEINFO_ENABLE is enabled)
    // * line (if YARP_FORWARD_CODEINFO_ENABLE is enabled)
    // * function (if YARP_FORWARD_CODEINFO_ENABLE is enabled)
    // * hostname (if YARP_FORWARD_HOSTNAME_ENABLE is enabled)
    // * cmd (if YARP_FORWARD_PROCESSINFO_ENABLE is enabled)
    // * args (if YARP_FORWARD_PROCESSINFO_ENABLE is enabled)
    // * pid (if YARP_FORWARD_PROCESSINFO_ENABLE is enabled)
    // * thread_id (if YARP_FORWARD_PROCESSINFO_ENABLE is enabled)
    // * component (if defined)
    // * message (if any)
    // * backtrace (for FATAL or if requested using YARP_FORWARD_BACKTRACE_ENABLE)

    *ost << "(level " << yarp::os::impl::StoreString::quotedString(level) << ")";
    *ost << " (systemtime " << yarp::os::NetType::toString(systemtime)  << ")";
    *ost << " (networktime " << yarp::os::NetType::toString(networktime)  << ")";
    if (yarp::os::impl::LogPrivate::forward_codeinfo.load()) {
        *ost << " (filename " << yarp::os::impl::StoreString::quotedString(file) << ")";
        *ost << " (line " << line << ")";
        *ost << " (function " << yarp::os::impl::StoreString::quotedString(func) << ")";
    }
    if (yarp::os::impl::LogPrivate::forward_hostname.load()) {
        static std::string hostname(yarp::os::gethostname());
        *ost << " (hostname " << yarp::os::impl::StoreString::quotedString(hostname) << ")";
    }
    if (yarp::os::impl::LogPrivate::forward_processinfo.load()) {
        static yarp::os::SystemInfo::ProcessInfo processInfo(yarp::os::SystemInfo::getProcessInfo());
        static std::string cmd(processInfo.name.substr(processInfo.name.find_last_of("\\/") + 1));
        thread_local long thread_id(yarp::os::impl::ThreadImpl::getKeyOfCaller());
        *ost << " (pid " << processInfo.pid << ")";
        *ost << " (cmd " << yarp::os::impl::StoreString::quotedString(cmd) << ")";
        *ost << " (args " << yarp::os::impl::StoreString::quotedString(processInfo.arguments) << ")";
        *ost << " (thread_id 0x" << std::setfill('0') << std::setw(8) << yarp::os::NetType::toHexString(thread_id) << ")";
    }
    if (comp_name) {
        *ost << " (component " << yarp::os::impl::StoreString::quotedString(comp_name) << ")";
    }
    if (msg[0]) {
        *ost << " (message " << yarp::os::impl::StoreString::quotedString(msg) << ")";
    }
    if (t == yarp::os::Log::FatalType || yarp::os::impl::LogPrivate::forward_backtrace.load()) {
        *ost << " (backtrace " << yarp::os::impl::StoreString::quotedString(backtrace()) << ")";
    }
}

inline void printable_output(std::ostream* ost,
                             yarp::os::Log::LogType t,
                             const char* msg,
                             const char* file,
                             const unsigned int line,
                             const char* func,
                             double systemtime,
                             double networktime,
                             const char* comp_name)
{
    YARP_UNUSED(file);
    YARP_UNUSED(line);
    YARP_UNUSED(systemtime);
    YARP_UNUSED(networktime);

#if !defined (_MSC_VER)
    static constexpr const char* level_char = u8"\u25CF";
#else
    static constexpr const char* level_char = "*";
#endif

    const char* level_string = logTypeToString(t);
    const char* color = logTypeToColor(t);
    const char* bgcolor = logTypeToBgColor(t);
    static const char *reserved_color = logTypeToColor(yarp::os::Log::LogTypeReserved);

    // Print Level
    if (yarp::os::impl::LogPrivate::colored_output.load() && yarp::os::impl::LogPrivate::compact_output.load()) {
        *ost << color << bgcolor << level_char << CLEAR << " ";
    } else {
        *ost << "[" << color << bgcolor << level_string << CLEAR << "] ";
    }

    // Print function information (trace only)
    if (t == yarp::os::Log::TraceType) {
        *ost << color << func << CLEAR << ((msg[0] || comp_name) ? ": " : "");
    }

    // Print component
    if (comp_name) {
        *ost << "|" << comp_name << "| ";
    }

    // Finally print the message
    if (yarp::os::impl::LogPrivate::debug_log.load()) {
        *ost << reserved_color << '^' << CLEAR;
    }
    *ost << msg;
    if (yarp::os::impl::LogPrivate::debug_log.load()) {
        *ost << reserved_color << '$' << CLEAR;
    }
}

inline void printable_output_verbose(std::ostream* ost,
                                     yarp::os::Log::LogType t,
                                     const char* msg,
                                     const char* file,
                                     const unsigned int line,
                                     const char* func,
                                     double systemtime,
                                     double networktime,
                                     const char* comp_name)
{
    YARP_UNUSED(systemtime);

    const char* level_string = logTypeToString(t);
    const char *color = logTypeToColor(t);
    const char *bgcolor = logTypeToBgColor(t);
    static const char *reserved_color = logTypeToColor(yarp::os::Log::LogTypeReserved);

    // Print time
    *ost << "[" << std::fixed << networktime << "] ";

    // Print level
    *ost << "[" << color << bgcolor << level_string << CLEAR << "] ";

    // Print file, line and function
    *ost << file << ":" << line << " " << color << bgcolor << func << CLEAR << " ";

    // Print thread id
    *ost << "(0x" << std::setfill('0') << std::setw(8) << yarp::os::NetType::toHexString(yarp::os::impl::ThreadImpl::getKeyOfCaller()) << ") ";

    // Print component
    if (comp_name) {
        *ost << "|" << comp_name << "| ";
    }

    // Finally print the message
    if (yarp::os::impl::LogPrivate::debug_log.load()) {
        *ost << reserved_color << '^' << CLEAR;
    }
    *ost << msg;
    if (yarp::os::impl::LogPrivate::debug_log.load()) {
        *ost << reserved_color << '$' << CLEAR;
    }
}


} // namespace

// END Utilities

// BEGIN LogPrivate static variables initialization

std::atomic<bool> yarp::os::impl::LogPrivate::yarprun_format(from_env("YARP_IS_YARPRUN", false) &&
                                                             from_env("YARPRUN_IS_FORWARDING_LOG", false));

#if defined(_WIN32) && !defined(YARP_HAS_WIN_VT_SUPPORT)
std::atomic<bool> yarp::os::impl::LogPrivate::colored_output(false);
#else
std::atomic<bool> yarp::os::impl::LogPrivate::colored_output(from_env("YARP_COLORED_OUTPUT", false) &&
                                                             !yarp::os::impl::LogPrivate::yarprun_format.load());
#endif
std::atomic<bool> yarp::os::impl::LogPrivate::verbose_output(from_env("YARP_VERBOSE_OUTPUT", false) &&
                                                             !yarp::os::impl::LogPrivate::yarprun_format.load());
std::atomic<bool> yarp::os::impl::LogPrivate::compact_output(from_env("YARP_COMPACT_OUTPUT", false) &&
                                                             !yarp::os::impl::LogPrivate::yarprun_format.load() &&
                                                             !yarp::os::impl::LogPrivate::verbose_output.load());
std::atomic<bool> yarp::os::impl::LogPrivate::forward_output(from_env("YARP_FORWARD_LOG_ENABLE", false) &&
                                                             !yarp::os::impl::LogPrivate::yarprun_format.load());

// The following 4 environment variables are to be considered experimental
// until we have a reason to believe that this extra traffic does not impact
// on the performances (and that all these info are actually useful).
std::atomic<bool> yarp::os::impl::LogPrivate::forward_codeinfo(from_env("YARP_FORWARD_CODEINFO_ENABLE", false));
std::atomic<bool> yarp::os::impl::LogPrivate::forward_hostname(from_env("YARP_FORWARD_HOSTNAME_ENABLE", false));
std::atomic<bool> yarp::os::impl::LogPrivate::forward_processinfo(from_env("YARP_FORWARD_PROCESSINFO_ENABLE", false));
std::atomic<bool> yarp::os::impl::LogPrivate::forward_backtrace(from_env("YARP_FORWARD_BACKTRACE_ENABLE", false));

std::atomic<bool> yarp::os::impl::LogPrivate::debug_output(from_env("YARP_DEBUG_ENABLE", true));
std::atomic<bool> yarp::os::impl::LogPrivate::trace_output(from_env("YARP_TRACE_ENABLE", false) &&
                                                           yarp::os::impl::LogPrivate::debug_output.load());

std::atomic<bool> yarp::os::impl::LogPrivate::debug_log(from_env("YARP_DEBUG_LOG_ENABLE", false));

std::atomic<bool> yarp::os::impl::LogPrivate::quiet(from_env("YARP_QUIET", false));
std::atomic<bool> yarp::os::impl::LogPrivate::verbose(from_env("YARP_VERBOSE", false) &&
                                                      !yarp::os::impl::LogPrivate::quiet.load());

#ifdef YARP_HAS_WIN_VT_SUPPORT
std::atomic<bool> yarp::os::impl::LogPrivate::vt_colors_enabled = false;
#endif

std::atomic<yarp::os::Log::LogType> yarp::os::impl::LogPrivate::minimumPrintLevel(
    (yarp::os::impl::LogPrivate::trace_output.load() ? yarp::os::Log::TraceType :
    (yarp::os::impl::LogPrivate::debug_output.load() ? yarp::os::Log::DebugType :
    (yarp::os::impl::LogPrivate::quiet.load() ? yarp::os::Log::WarningType :
    (yarp::os::impl::LogPrivate::verbose.load() ? yarp::os::Log::DebugType : yarp::os::Log::InfoType)))));
std::atomic<yarp::os::Log::LogType> yarp::os::impl::LogPrivate::minimumForwardLevel(
    (yarp::os::impl::LogPrivate::forward_output.load() ? yarp::os::impl::LogPrivate::minimumPrintLevel.load() : yarp::os::Log::LogTypeReserved));

std::atomic<yarp::os::Log::LogCallback> yarp::os::impl::LogPrivate::current_print_callback(
    yarp::os::impl::LogPrivate::print_callback);
std::atomic<yarp::os::Log::LogCallback> yarp::os::impl::LogPrivate::current_forward_callback(
    (yarp::os::impl::LogPrivate::forward_output ? yarp::os::impl::LogPrivate::forward_callback : nullptr));

// Log internal component never forwards the output and, if enabled, prints
// all the output using the default print callback
const yarp::os::LogComponent yarp::os::impl::LogPrivate::log_internal_component(
    "yarp.os.Log",
    yarp::os::impl::LogPrivate::debug_log.load() ? yarp::os::Log::TraceType : yarp::os::Log::LogTypeReserved,
    yarp::os::Log::LogTypeReserved,
    yarp::os::impl::LogPrivate::debug_log.load() ? yarp::os::impl::LogPrivate::default_print_callback : nullptr,
    nullptr);


// END LogPrivate static variables initialization

// BEGIN LogPrivate methods

yarp::os::impl::LogPrivate::LogPrivate(const char* file,
                                       unsigned int line,
                                       const char* func,
                                       const LogComponent& comp) :
        file(file),
        line(line),
        func(func),
        systemtime(yarp::os::SystemClock::nowSystem()),
        networktime(!yarp::os::NetworkBase::isNetworkInitialized() ? 0.0 : (yarp::os::Time::isSystemClock() ? systemtime : yarp::os::Time::now())),
        comp(comp)
{
#ifdef YARP_HAS_WIN_VT_SUPPORT
    if (colored_output.load() && !yarp::os::impl::LogPrivate::vt_colors_enabled.load()) {
        colored_output = enable_vt_colors();
    }
#endif
}

void yarp::os::impl::LogPrivate::print_callback(yarp::os::Log::LogType t,
                                                const char* msg,
                                                const char* file,
                                                const unsigned int line,
                                                const char* func,
                                                double systemtime,
                                                double networktime,
                                                const char* comp_name)
{
    std::ostream *ost;
    if(t == yarp::os::Log::TraceType ||
       t == yarp::os::Log::DebugType ||
       t == yarp::os::Log::InfoType) {
        ost = &std::cout;
    } else {
        ost = &std::cerr;
    }

    static std::mutex log_mutex;
    std::lock_guard<std::mutex> lock(log_mutex);

    if (yarprun_format.load()) {
        // Same output as forward_callback
        forwardable_output(ost, t, msg, file, line, func, systemtime, networktime, comp_name);
    } else if (verbose_output.load()) {
        printable_output_verbose(ost, t, msg, file, line, func, systemtime, networktime, comp_name);
    } else {
        printable_output(ost, t, msg, file, line, func, systemtime, networktime, comp_name);
    }
    *ost << std::endl;
}

void yarp::os::impl::LogPrivate::forward_callback(yarp::os::Log::LogType t,
                                                  const char* msg,
                                                  const char* file,
                                                  const unsigned int line,
                                                  const char* func,
                                                  double systemtime,
                                                  double networktime,
                                                  const char* comp_name)
{
    if (!yarp::os::NetworkBase::isNetworkInitialized()) {
        // Network is not initialized. Don't forward any log.
        // And avoid creating the LogForwarder!
        return;
    }
    std::stringstream stringstream_buffer;
    forwardable_output(&stringstream_buffer, t, msg, file, line, func, systemtime, networktime, comp_name);
    LogForwarder::getInstance().forward(stringstream_buffer.str());
}

void yarp::os::impl::LogPrivate::log(yarp::os::Log::LogType type,
                                     const char* msg,
                                     va_list args) const
{
    constexpr size_t YARP_MAX_LOG_MSG_SIZE = 1024;

    if (msg != nullptr) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        auto w = static_cast<size_t>(vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args));
        buf[YARP_MAX_LOG_MSG_SIZE - 1] = 0;
        auto p = std::min(w - 1, YARP_MAX_LOG_MSG_SIZE);
        if (w > 0 && p < YARP_MAX_LOG_MSG_SIZE && buf[p] == '\n' && msg[strlen(msg) - 1] == '\n') {
            yarp::os::Log(file, line, func, log_internal_component).warning("Removing extra '\\n' (c-style)");
            buf[p] = 0;
        }

        do_log(type, buf, file, line, func, systemtime, networktime, comp);

        if (w > YARP_MAX_LOG_MSG_SIZE - 1) {
            yarp::os::Log(file, line, func, log_internal_component).warning("Previous message was truncated");
        }
    } else {
        yarp::os::Log(file, line, func, log_internal_component).warning("Unexpected nullptr received");
    }
}

void yarp::os::impl::LogPrivate::do_log(yarp::os::Log::LogType type,
                                        const char* msg,
                                        const char* file,
                                        const unsigned int line,
                                        const char* func,
                                        double systemtime,
                                        double networktime,
                                        const LogComponent& comp)
{
    auto* print_cb = comp.printCallback(type);
    if (print_cb) {
        print_cb(type, msg, file, line, func, systemtime, networktime, comp.name());
    } else {
        if (comp != log_internal_component) {
            yarp::os::Log(file, line, func, log_internal_component).debug("Not printing [%s][%s]", comp.name(), msg);
        }
    }

    auto* forward_cb = comp.forwardCallback(type);
    if(forward_cb) {
        forward_cb(type, msg, file, line, func, systemtime, networktime, comp.name());
    } else {
        if (comp != log_internal_component) {
            yarp::os::Log(file, line, func, log_internal_component).debug("Not forwarding [%s][%s]", comp.name(), msg);
        }
    }
}

#ifdef YARP_HAS_WIN_VT_SUPPORT
bool yarp::os::impl::LogPrivate::enable_vt_colors()
{
    DWORD handleMode = 0;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    bool success = false;

    if (hStdout != INVALID_HANDLE_VALUE && GetConsoleMode(hStdout, &handleMode)) {
        handleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        success = SetConsoleMode(hStdout, handleMode);
    }

    yarp::os::impl::LogPrivate::vt_colors_enabled = true;
    return success;
}
#endif

// END LogPrivate methods


yarp::os::Log::LogType yarp::os::Log::defaultMinimumPrintLevel()
{
    return yarp::os::impl::LogPrivate::minimumPrintLevel.load();
}

yarp::os::Log::LogType yarp::os::Log::defaultMinimumForwardLevel()
{
    return yarp::os::impl::LogPrivate::forward_output.load() ? yarp::os::impl::LogPrivate::minimumForwardLevel.load() : yarp::os::Log::LogTypeReserved;
}

yarp::os::Log::LogCallback yarp::os::Log::defaultPrintCallback()
{
    return yarp::os::impl::LogPrivate::default_print_callback;
}

yarp::os::Log::LogCallback yarp::os::Log::defaultForwardCallback()
{
    return yarp::os::impl::LogPrivate::forward_output.load() ? yarp::os::impl::LogPrivate::default_forward_callback : nullptr;
};

const yarp::os::LogComponent& yarp::os::Log::defaultLogComponent()
{
    static const yarp::os::LogComponent component(nullptr,
                                                  defaultMinimumPrintLevel(),
                                                  defaultMinimumForwardLevel(),
                                                  defaultPrintCallback(),
                                                  defaultForwardCallback());
    return component;
}

const yarp::os::LogComponent& yarp::os::Log::logInternalComponent()
{
    return yarp::os::impl::LogPrivate::log_internal_component;
}

yarp::os::Log::Log(const char* file,
                   unsigned int line,
                   const char* func,
                   const LogComponent& comp) :
        mPriv(new yarp::os::impl::LogPrivate(file, line, func, comp))
{
}

yarp::os::Log::Log() :
        mPriv(new yarp::os::impl::LogPrivate(nullptr, 0, nullptr, nullptr))
{
}

yarp::os::Log::~Log()
{
    delete mPriv;
}

void yarp::os::Log::do_log(yarp::os::Log::LogType type,
                           const char* msg,
                           const char* file,
                           const unsigned int line,
                           const char* func,
                           double systemtime,
                           double networktime,
                           const LogComponent& comp)
{
    yarp::os::impl::LogPrivate::do_log(type, msg, file, line, func, systemtime, networktime, comp);
}


void yarp::os::Log::trace(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    mPriv->log(yarp::os::Log::TraceType, msg, args);
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::trace() const
{
    return yarp::os::LogStream(yarp::os::Log::TraceType,
                               mPriv->file,
                               mPriv->line,
                               mPriv->func,
                               mPriv->comp);
}


void yarp::os::Log::debug(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    mPriv->log(yarp::os::Log::DebugType, msg, args);
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::debug() const
{
    return yarp::os::LogStream(yarp::os::Log::DebugType,
                               mPriv->file,
                               mPriv->line,
                               mPriv->func,
                               mPriv->comp);
}


void yarp::os::Log::info(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    mPriv->log(yarp::os::Log::InfoType, msg, args);
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::info() const
{
    return yarp::os::LogStream(yarp::os::Log::InfoType,
                               mPriv->file,
                               mPriv->line,
                               mPriv->func,
                               mPriv->comp);
}


void yarp::os::Log::warning(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    mPriv->log(yarp::os::Log::WarningType, msg, args);
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::warning() const
{
    return yarp::os::LogStream(yarp::os::Log::WarningType,
                               mPriv->file,
                               mPriv->line,
                               mPriv->func,
                               mPriv->comp);
}


void yarp::os::Log::error(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    mPriv->log(yarp::os::Log::ErrorType, msg, args);
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::error() const
{
    return yarp::os::LogStream(yarp::os::Log::ErrorType,
                               mPriv->file,
                               mPriv->line,
                               mPriv->func,
                               mPriv->comp);
}

void yarp::os::Log::fatal(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    mPriv->log(yarp::os::Log::FatalType, msg, args);
    va_end(args);
    yarp_print_trace(stderr, mPriv->file, mPriv->line);
    std::exit(-1);
}

yarp::os::LogStream yarp::os::Log::fatal() const
{
    return yarp::os::LogStream(yarp::os::Log::FatalType,
                               mPriv->file,
                               mPriv->line,
                               mPriv->func,
                               mPriv->comp);
}

void yarp::os::Log::setPrintCallback(yarp::os::Log::LogCallback cb)
{
    yarp::os::impl::LogPrivate::current_print_callback = cb;
}

void yarp::os::Log::setForwardCallback(yarp::os::Log::LogCallback cb)
{
    yarp::os::impl::LogPrivate::current_forward_callback = cb;
}



void yarp_print_trace(FILE* out, const char* file, unsigned int line)
{
    fprintf(out, "Trace requested at %s:%u by code called from:\n", file, line);
    fprintf(out, "%s", backtrace().c_str());
    fflush(out);
}
