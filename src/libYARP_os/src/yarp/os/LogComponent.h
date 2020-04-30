/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_LOGCOMPONENT_H
#define YARP_OS_LOGCOMPONENT_H

#include <yarp/os/api.h>
#include <yarp/os/Log.h>

#include <atomic>

namespace yarp {
namespace os {

class YARP_os_API LogComponent
{
public:
    using LogCallback = yarp::os::Log::LogCallback;
    using LogType = yarp::os::Log::LogType;

    LogComponent(const char* name,
                 LogType minimumPrintLevel = yarp::os::Log::defaultMinimumPrintLevel(),
                 LogType minimumForwardLevel = yarp::os::Log::defaultMinimumForwardLevel(),
                 LogCallback printCallback = yarp::os::Log::defaultPrintCallback(),
                 LogCallback forwardCallback = yarp::os::Log::defaultForwardCallback());

    LogComponent(const LogComponent&) = delete;
    LogComponent(LogComponent&&) noexcept = delete;
    LogComponent& operator=(const LogComponent&) = delete;
    LogComponent& operator=(LogComponent&&) noexcept = delete;

    ~LogComponent() = default;

    LogCallback printCallback(yarp::os::Log::LogType t) const;
    LogCallback forwardCallback(yarp::os::Log::LogType t) const;

    constexpr const char* name() const { return m_name; }
    constexpr operator bool() const { return m_name != nullptr; }

private:
    const char* m_name { nullptr };

    std::atomic<bool> m_tracePrintEnabled;
    std::atomic<bool> m_debugPrintEnabled;
    std::atomic<bool> m_infoPrintEnabled;
    std::atomic<bool> m_warningPrintEnabled;
    std::atomic<bool> m_errorPrintEnabled;
    std::atomic<bool> m_fatalPrintEnabled;
    std::atomic<bool> m_traceForwardEnabled;
    std::atomic<bool> m_debugForwardEnabled;
    std::atomic<bool> m_infoForwardEnabled;
    std::atomic<bool> m_warningForwardEnabled;
    std::atomic<bool> m_errorForwardEnabled;
    std::atomic<bool> m_fatalForwardEnabled;

    LogCallback m_printCallback { nullptr };
    LogCallback m_forwardCallback { nullptr };
};

#define YARP_DECLARE_LOG_COMPONENT(name) \
    extern const yarp::os::LogComponent& name();

#define YARP_LOG_COMPONENT(name, ...) \
    const yarp::os::LogComponent& name() \
    { \
        static const yarp::os::LogComponent component(__VA_ARGS__); \
        return component; \
    }

#define yCTrace(component, ...)   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__, component()).trace(__VA_ARGS__)
#define yCDebug(component, ...)   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__, component()).debug(__VA_ARGS__)
#define yCInfo(component, ...)    yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__, component()).info(__VA_ARGS__)
#define yCWarning(component, ...) yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__, component()).warning(__VA_ARGS__)
#define yCError(component, ...)   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__, component()).error(__VA_ARGS__)
#define yCFatal(component, ...)   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__, component()).fatal(__VA_ARGS__)

#ifndef NDEBUG
#  define yCAssert(component, x)                                                       \
    if (!(x)) {                                                                        \
        yCFatal(component, "Assertion failure at %s:%d (%s)", __FILE__, __LINE__, #x); \
    }
#else
#  define yCAssert(x)
#endif


} // namespace yarp
} // namespace os

#endif // YARP_OS_LOGCOMPONENT_H
