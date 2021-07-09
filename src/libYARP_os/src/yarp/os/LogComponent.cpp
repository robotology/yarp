/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>

using yarp::os::LogComponent;
using LogCallback = yarp::os::Log::LogCallback;

LogComponent::LogComponent(const char* name,
                           LogType minimumPrintLevel,
                           LogType minimumForwardLevel,
                           LogCallback printCallback,
                           LogCallback forwardCallback) :
        m_name(name),
        m_tracePrintEnabled(minimumPrintLevel <= yarp::os::Log::TraceType),
        m_debugPrintEnabled(minimumPrintLevel <= yarp::os::Log::DebugType),
        m_infoPrintEnabled(minimumPrintLevel <= yarp::os::Log::InfoType),
        m_warningPrintEnabled(minimumPrintLevel <= yarp::os::Log::WarningType),
        m_errorPrintEnabled(minimumPrintLevel <= yarp::os::Log::ErrorType),
        m_fatalPrintEnabled(minimumPrintLevel <= yarp::os::Log::FatalType),
        m_traceForwardEnabled(minimumForwardLevel <= yarp::os::Log::TraceType),
        m_debugForwardEnabled(minimumForwardLevel <= yarp::os::Log::DebugType),
        m_infoForwardEnabled(minimumForwardLevel <= yarp::os::Log::InfoType),
        m_warningForwardEnabled(minimumForwardLevel <= yarp::os::Log::WarningType),
        m_errorForwardEnabled(minimumForwardLevel <= yarp::os::Log::ErrorType),
        m_fatalForwardEnabled(minimumForwardLevel <= yarp::os::Log::FatalType),
        m_printCallback(printCallback),
        m_forwardCallback(forwardCallback)
{
    // FIXME Read enviroment variables, config files, etc to configure the component dynamically
}

LogCallback LogComponent::printCallback(LogType t) const
{
    switch (t) {
    case yarp::os::Log::TraceType:
        return (m_tracePrintEnabled.load() ? m_printCallback : nullptr);
    case yarp::os::Log::DebugType:
        return (m_debugPrintEnabled.load() ? m_printCallback : nullptr);
    case yarp::os::Log::InfoType:
        return (m_infoPrintEnabled.load() ? m_printCallback : nullptr);
    case yarp::os::Log::WarningType:
        return (m_warningPrintEnabled.load() ? m_printCallback : nullptr);
    case yarp::os::Log::ErrorType:
        return (m_errorPrintEnabled.load() ? m_printCallback : nullptr);
    case yarp::os::Log::FatalType:
        return (m_fatalPrintEnabled.load() ? m_printCallback : nullptr);
    default:
        return nullptr;
    }
}

LogCallback LogComponent::forwardCallback(LogType t) const
{
    switch (t) {
    case yarp::os::Log::TraceType:
        return (m_traceForwardEnabled.load() ? m_forwardCallback : nullptr);
    case yarp::os::Log::DebugType:
        return (m_debugForwardEnabled.load() ? m_forwardCallback : nullptr);
    case yarp::os::Log::InfoType:
        return (m_infoForwardEnabled.load() ? m_forwardCallback : nullptr);
    case yarp::os::Log::WarningType:
        return (m_warningForwardEnabled.load() ? m_forwardCallback : nullptr);
    case yarp::os::Log::ErrorType:
        return (m_errorForwardEnabled.load() ? m_forwardCallback : nullptr);
    case yarp::os::Log::FatalType:
        return (m_fatalForwardEnabled.load() ? m_forwardCallback : nullptr);
    default:
        return nullptr;
    }
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
void LogComponent::setMinimumPrintLevel(LogType minimumPrintLevel)
{
        m_tracePrintEnabled = (minimumPrintLevel <= yarp::os::Log::TraceType);
        m_debugPrintEnabled = (minimumPrintLevel <= yarp::os::Log::DebugType);
        m_infoPrintEnabled = (minimumPrintLevel <= yarp::os::Log::InfoType);
        m_warningPrintEnabled = (minimumPrintLevel <= yarp::os::Log::WarningType);
        m_errorPrintEnabled = (minimumPrintLevel <= yarp::os::Log::ErrorType);
        m_fatalPrintEnabled = (minimumPrintLevel <= yarp::os::Log::FatalType);
}

void LogComponent::setMinimumForwardLevel(LogType minimumForwardLevel)
{
        m_traceForwardEnabled = (minimumForwardLevel <= yarp::os::Log::TraceType);
        m_debugForwardEnabled = (minimumForwardLevel <= yarp::os::Log::DebugType);
        m_infoForwardEnabled = (minimumForwardLevel <= yarp::os::Log::InfoType);
        m_warningForwardEnabled = (minimumForwardLevel <= yarp::os::Log::WarningType);
        m_errorForwardEnabled = (minimumForwardLevel <= yarp::os::Log::ErrorType);
        m_fatalForwardEnabled = (minimumForwardLevel <= yarp::os::Log::FatalType);
}
#endif // YARP_NO_DEPRECATED
