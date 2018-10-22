/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Logger.h>

#include <cstdio>
#include <yarp/os/Network.h>
#include <cmath>

#ifdef YARP_TEST_HEAP
#include <yarp/os/Mutex.h>
#endif

using namespace yarp::os::impl;
using namespace yarp::os;

UnitTest *UnitTest::theRoot = nullptr;

#ifdef YARP_TEST_HEAP

static bool heap_count_active = false;
static int heap_count_ops = 0;
static bool heap_expect_ops = false;
static yarp::os::Mutex *heap_count_mutex = nullptr;

void addHeapOperation(const char *act) {
    if (heap_count_active) {
        heap_count_mutex->lock();
        if (!heap_expect_ops) {
            heap_count_mutex->unlock();
            fprintf(stderr, "Unexpected '%s' heap operation detected.\n", act);
            yarp_print_trace(stderr, __FILE__, __LINE__);
            heap_count_mutex->lock();
        }
        heap_count_ops++;
        heap_count_mutex->unlock();
    }
}

void *operator new(std::size_t size) {
    addHeapOperation("new");
    return malloc(size);
}

void operator delete(void *ptr) {
    addHeapOperation("delete");
    free(ptr);
}

#endif



#ifndef DOXYGEN_SHOULD_SKIP_THIS

class RootUnitTest : public UnitTest
{
public:
    RootUnitTest(UnitTest *parent) : UnitTest(parent)
    {
        // no parent
    }

    virtual std::string getName() const override
    {
        return "root";
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



UnitTest::UnitTest() :
        parent(&UnitTest::getRoot()),
        hasProblem(0)
{
}

UnitTest::UnitTest(UnitTest *parent) :
        parent(parent),
        hasProblem(0)
{
    if (parent!=nullptr) {
        parent->add(*this);
    }
}

void UnitTest::add(UnitTest& unit)
{
    for (auto& subTest : subTests) {
        if (subTest == &unit) {
            return; // already present, no need to add
        }
    }
    subTests.push_back(&unit);
}

void UnitTest::clear()
{
    subTests.clear();
}

void UnitTest::report(int severity, const std::string& problem)
{
    if (parent!=nullptr) {
        parent->report(severity, getName() + ": " + problem);
    } else {
        YARP_SPRINTF2(Logger::get(), info,
                      "%d | %s", severity, problem.c_str());
    }
    count(severity);
}

void UnitTest::count(int severity)
{
    if (severity>0) {
        // could do something more sophisticated with the reports than this...
        hasProblem++;
    }
}

int UnitTest::run()
{
    run(0, nullptr);
    return hasProblem;
}

int UnitTest::run(int argc, char *argv[])
{
    static bool ran = false;

    if (argc==0) {
        runTests();
        ran = true;
    } else {
        std::string name = getName();
        bool onList = false;
        for (int i=0; i<argc; i++) {
            if (name == std::string(argv[i])) {
                onList = true;
                break;
            }
        }
        if (onList) {
            runTests();
            ran = true;
        }
    }

    // Run sub tests
    for (auto& subTest : subTests) {
        hasProblem += subTest->run(argc, argv);
    }

    if (parent != nullptr) {
        return hasProblem;
    }

    if (hasProblem) {
        report(0, "A PROBLEM WAS ENCOUNTERED");
        return 1;
    }

    if (!ran) {
        report(0, "no test found");
        return 2;
    }

    report(0, "no problems reported");
    return 0;
}



void UnitTest::startTestSystem()
{
    if (theRoot==nullptr) {
        theRoot = new RootUnitTest(nullptr);
    }
}

// system starts on first call, probably from a static object - this
// is to avoid link order dependency problems
UnitTest& UnitTest::getRoot()
{
    startTestSystem();
    yAssert(theRoot!=nullptr);
    return *theRoot;
}

// this is the important one to call
void UnitTest::stopTestSystem()
{
    if (theRoot!=nullptr) {
        delete theRoot;
        theRoot = nullptr;
    }
}

bool UnitTest::checkEqualishImpl(double x, double y,
                                 const std::string& desc,
                                 const char *txt1,
                                 const char *txt2,
                                 const char *fname,
                                 int fline)
{
    std::ostringstream ost;
    if (fabs(x-y)<0.0001) {
        ost << "  [" << desc << "] passed ok";
        report(0, ost.str());
        return true;
    } else {
        ost << "  FAILURE in file " << fname << ":" << fline << " [" << desc << "] " << txt1 << " (" << x << ") == " << txt2 << " (" << y << ")";
        report(1, ost.str());
        return false;
    }
}


bool UnitTest::checkEqualImpl(const std::string& x, const std::string& y,
                              const std::string& desc,
                              const char *txt1,
                              const char *txt2,
                              const char *fname,
                              int fline)
{
    std::ostringstream ost;
    if (x == y) {
        ost << "  [" << desc << "] passed ok";
        report(0, ost.str());
        return true;
    } else {
        ost << "  FAILURE in file " << fname << ":" << fline << " [" << desc << "] " << txt1 << " (" << humanize(x) << ") == " << txt2 << " (" << humanize(y) << ")";
        report(1, ost.str());
        return false;
    }
}


std::string UnitTest::humanize(const std::string& txt)
{
    std::string result;
    for (char ch : txt) {
        if (ch == '\n') {
            result += "\\n";
        } else if (ch == '\r') {
            result += "\\r";
        } else if (ch == '\0') {
            result += "\\0";
        } else {
            result += ch;
        }
    }
    return result;
}


void UnitTest::saveEnvironment(const char *key)
{
    bool found = false;
    std::string val = NetworkBase::getEnvironment(key, &found);
    Bottle& lst = env.addList();
    lst.addString(key);
    lst.addString(val);
    lst.addInt32(found?1:0);
}

void UnitTest::restoreEnvironment()
{
    for (size_t i=0; i<env.size(); i++) {
        Bottle *lst = env.get(i).asList();
        if (lst==nullptr) continue;
        std::string key = lst->get(0).asString();
        std::string val = lst->get(1).asString();
        bool found = lst->get(2).asInt32()?true:false;
        if (!found) {
            NetworkBase::unsetEnvironment(key);
        } else {
            NetworkBase::setEnvironment(key, val);
        }
    }
    env.clear();
}


bool UnitTest::heapMonitorSupported()
{
#ifdef YARP_TEST_HEAP
    return true;
#else
    return false;
#endif
}

void UnitTest::heapMonitorBegin(bool expectAllocations)
{
#ifdef YARP_TEST_HEAP
    heapMonitorEnd();
    heap_count_ops = 0;
    heap_count_mutex = new Mutex();
    heap_expect_ops = expectAllocations;
    heap_count_active = true;
#else
    YARP_UNUSED(expectAllocations);
#endif
}

int UnitTest::heapMonitorOps()
{
#ifdef YARP_TEST_HEAP
    heap_count_mutex->lock();
    int diff = heap_count_ops;
    heap_count_ops = 0;
    heap_count_mutex->unlock();
    if (!heap_expect_ops) {
        checkEqual(0, diff, "heap operator count");
    }
    return diff;
#else
    return -1;
#endif
}

int UnitTest::heapMonitorEnd()
{
#ifdef YARP_TEST_HEAP
    if (!heap_count_mutex) return 0;
    heap_count_mutex->lock();
    heap_count_active = false;
    int diff = heap_count_ops;
    heap_count_ops = 0;
    heap_count_mutex->unlock();
    if (!heap_expect_ops) {
        checkEqual(0, diff, "heap operator final count");
    }
    delete heap_count_mutex;
    heap_count_mutex = nullptr;
    return diff;
#else
    return -1;
#endif
}
