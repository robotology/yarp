// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium, Arjan Gijsberts
 * Authors: Paul Fitzpatrick, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
yarp::os::SearchReport::SearchReport() :
        key("?"),
        value(""),
        isFound(false),
        isGroup(false),
        isComment(false),
        isDefault(false) {
}

yarp::os::SearchMonitor::~SearchMonitor() {
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


yarp::os::Searchable::Searchable() :
        monitor(NULL) {
}

yarp::os::Searchable::~Searchable() {
}

bool yarp::os::Searchable::check(const char *txt,
                                 yarp::os::Value *& result,
                                 const char *comment) {
    if (getMonitor()!=NULL && comment!=NULL) {
        SearchReport report;
        report.key = txt;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    yarp::os::Value& bit = find(txt);
    bool ok = !(bit.isNull());
    if (ok) {
        result = &bit;
    }
    return ok;
}

yarp::os::Value yarp::os::Searchable::check(const char *txt,
                                  const yarp::os::Value& fallback,
                                  const char *comment) {
    if (getMonitor()!=NULL && comment!=NULL) {
        yarp::os::SearchReport report;
        report.key = txt;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    if (getMonitor()!=NULL) {
        yarp::os::SearchReport report;
        report.key = txt;
        report.value = fallback.toString();
        report.isDefault = true;
        reportToMonitor(report);
    }
    yarp::os::Value& bit = find(txt);
    bool ok = !(bit.isNull());
    if (ok) {
        return bit;
    }
    return fallback;
}

bool yarp::os::Searchable::check(const char *key,
                                 const char *comment) {
    if (getMonitor()!=NULL && comment!=NULL) {
        yarp::os::SearchReport report;
        report.key = key;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    return check(key);
}

yarp::os::Bottle& yarp::os::Searchable::findGroup(const char *key,
                                                  const char *comment) {
    if (getMonitor()!=NULL && comment!=NULL) {
        yarp::os::SearchReport report;
        report.key = key;
        report.value = comment;
        report.isComment = true;
        reportToMonitor(report);
    }
    return findGroup(key);
}

bool yarp::os::Searchable::isNull() const {
    return false;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

void yarp::os::Searchable::setMonitor(yarp::os::SearchMonitor *monitor, const char *context) {
    this->monitor = monitor;
    this->monitorContext = context;
}

yarp::os::SearchMonitor *yarp::os::Searchable::getMonitor() {
    return monitor;
}

yarp::os::ConstString yarp::os::Searchable::getContext() {
    return monitorContext;
}

void yarp::os::Searchable::reportToMonitor(const yarp::os::SearchReport& report) {
    if (monitor!=NULL) {
        monitor->report(report,monitorContext);
    }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
