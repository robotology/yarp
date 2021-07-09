/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Searchable.h>

#include <yarp/os/Value.h>

using namespace yarp::os;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
yarp::os::SearchReport::SearchReport() :
        key("?"),
        value(""),
        isFound(false),
        isGroup(false),
        isComment(false),
        isDefault(false)
{
}

yarp::os::SearchMonitor::~SearchMonitor() = default;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


yarp::os::Searchable::Searchable() :
        monitor(nullptr)
{
}

yarp::os::Searchable::~Searchable() = default;

bool yarp::os::Searchable::check(const std::string& key,
                                 yarp::os::Value*& result,
                                 const std::string& comment) const
{
    if (getMonitor() != nullptr && !comment.empty()) {
        SearchReport report;
        report.key = key;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    yarp::os::Value& bit = find(key);
    bool ok = !(bit.isNull());
    if (ok) {
        result = &bit;
    }
    return ok;
}

yarp::os::Value yarp::os::Searchable::check(const std::string& key,
                                            const yarp::os::Value& fallback,
                                            const std::string& comment) const
{
    if (getMonitor() != nullptr && !comment.empty()) {
        yarp::os::SearchReport report;
        report.key = key;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    if (getMonitor() != nullptr) {
        yarp::os::SearchReport report;
        report.key = key;
        report.value = fallback.toString();
        report.isDefault = true;
        reportToMonitor(report);
    }
    yarp::os::Value& bit = find(key);
    bool ok = !(bit.isNull());
    if (ok) {
        return bit;
    }
    return fallback;
}

bool yarp::os::Searchable::check(const std::string& key,
                                 const std::string& comment) const
{
    if (getMonitor() != nullptr && !comment.empty()) {
        yarp::os::SearchReport report;
        report.key = key;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    return check(key);
}

yarp::os::Bottle& yarp::os::Searchable::findGroup(const std::string& key,
                                                  const std::string& comment) const
{
    if (getMonitor() != nullptr && !comment.empty()) {
        yarp::os::SearchReport report;
        report.key = key;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    return findGroup(key);
}

bool yarp::os::Searchable::isNull() const
{
    return false;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

void yarp::os::Searchable::setMonitor(yarp::os::SearchMonitor* monitor, const char* context)
{
    this->monitor = monitor;
    this->monitorContext = context;
}

yarp::os::SearchMonitor* yarp::os::Searchable::getMonitor() const
{
    return monitor;
}

std::string yarp::os::Searchable::getMonitorContext() const
{
    return monitorContext;
}

void yarp::os::Searchable::reportToMonitor(const yarp::os::SearchReport& report) const
{
    if (monitor != nullptr) {
        monitor->report(report, monitorContext.c_str());
    }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
