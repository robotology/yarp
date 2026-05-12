/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MONITORPYTHON_H
#define MONITORPYTHON_H

#include <string>
#include <yarp/os/PeriodicThread.h>
#include "MonitorBinding.h"

#include "Python.h"

#include <mutex>

class MonitorTrigger;

class MonitorPython : public MonitorBinding
{

public:
    MonitorPython();
    ~MonitorPython() override;

    bool load(const yarp::os::Property& options) override;
    bool setParams(const yarp::os::Property& params) override;
    bool getParams(yarp::os::Property& params) override;

    bool acceptData(yarp::os::Things& thing) override;
    yarp::os::Things& updateData(yarp::os::Things& thing) override;
    yarp::os::Things& updateReply(yarp::os::Things& thing) override;

    bool peerTrigged() override;
    bool canAccept() override;

    bool setAcceptConstraint(const char* constraint) override
    {
        if(!constraint) {
            return false;
        }
        MonitorPython::constraint = constraint;
        trimString(MonitorPython::constraint);
        return true;
    }

    const char* getAcceptConstraint() override {
        return constraint.c_str();
    }

    bool hasAccept() override {
        return bHasAcceptCallback;
    }

    bool hasUpdate() override {
        return bHasUpdateCallback;
    }

    bool hasUpdateReply() override {
        return bHasUpdateReplyCallback;
    }

private:
    std::string constraint;
    bool bHasAcceptCallback;
    bool bHasUpdateCallback;
    bool bHasUpdateReplyCallback;

    std::string m_path;
    std::string m_pythonScriptName = "monitor.py"; // Name of the python file containing the functions/classes
    std::recursive_mutex m_monitor_mutex;

    bool classWrapper(PyObject* &pClassInstance, std::string methodName, PyObject* &pClassMethodArgs, PyObject* &pValue);
    bool functionWrapper(std::string moduleName, std::string functionName, PyObject* &pArgs, PyObject* &pValue);

    PyObject* m_classInstance=nullptr; // Python object of the created class

public:
    MonitorTrigger* trigger=nullptr;

private:
    void trimString(std::string& str);
    void searchReplace(std::string& str,
                       const std::string& oldStr, const std::string& newStr);
    bool isKeyword(const char* str);
};

class MonitorTrigger : public yarp::os::PeriodicThread {
public:
    MonitorTrigger(MonitorPython* monitor, double period)
        : yarp::os::PeriodicThread(period) {
        MonitorTrigger::monitor = monitor;
    }
    virtual ~MonitorTrigger() { }

    // inherited from the yarp::os::RateThread
    void run () override {
        monitor->peerTrigged();
    }

private:
    MonitorPython* monitor;
};


#endif //MONITORPYTHON_H
