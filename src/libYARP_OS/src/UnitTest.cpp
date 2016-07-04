/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/Network.h>
#include <math.h>

#ifdef YARP_TEST_HEAP
#include <yarp/os/Mutex.h>
#endif

using namespace yarp::os::impl;
using namespace yarp::os;

UnitTest *UnitTest::theRoot = NULL;

#ifdef YARP_TEST_HEAP

static bool heap_count_active = false;
static int heap_count_ops = 0;
static bool heap_expect_ops = false;
static yarp::os::Mutex *heap_count_mutex = NULL;

void addHeapOperation(const char *act) {
    if (heap_count_active) {
        heap_count_mutex->lock();
        if (!heap_expect_ops) {
            heap_count_mutex->unlock();
            fprintf(stderr,"Unexpected '%s' heap operation detected.\n", act);
            yarp_print_trace(stderr,__FILE__,__LINE__);
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

class RootUnitTest : public UnitTest {
public:
    RootUnitTest(UnitTest *parent) : UnitTest(parent) {
        // no parent
    }

    virtual ConstString getName() {
        return "root";
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



UnitTest::UnitTest() {
    parent = &UnitTest::getRoot();
    hasProblem = false;
}

UnitTest::UnitTest(UnitTest *parent) {
    this->parent = parent;
    if (parent!=NULL) {
        parent->add(*this);
    }
    hasProblem = false;
}

void UnitTest::add(UnitTest& unit) {
    for (unsigned int i=0; i<subTests.size(); i++) {
        if (subTests[i]==&unit) {
            return; // already present, no need to add
        }
    }
    subTests.push_back(&unit);
}

void UnitTest::clear() {
    subTests.clear();
}

void UnitTest::report(int severity, const ConstString& problem) {
    if (parent!=NULL) {
        parent->report(severity, getName() + ": " + problem);
    } else {
        YARP_SPRINTF2(Logger::get(),info,
                      "%d | %s", severity, problem.c_str());
    }
    count(severity);
}

void UnitTest::count(int severity) {
    if (severity>0) {
        // could do something more sophisticated with the reports than this...
        hasProblem = true;
    }
}


void UnitTest::runSubTests(int argc, char *argv[]) {
    for (unsigned int i=0; i<subTests.size(); i++) {
        subTests[i]->run(argc,argv);
    }
}


int UnitTest::run() {
    run(0,NULL);
    return hasProblem;
}


int UnitTest::run(int argc, char *argv[]) {
    bool ran = false;
    if (argc==0) {
        runTests();
        ran = true;
    } else {
        ConstString name = getName();
        bool onList = false;
        for (int i=0; i<argc; i++) {
            if (name == ConstString(argv[i])) {
                onList = true;
                break;
            }
        }
        if (onList) {
            runTests();
            ran = true;
        }
    }
    runSubTests(argc,argv);
    if (hasProblem) {
        report(0,"A PROBLEM WAS ENCOUNTERED");
    } 
    else {
        if (ran) {
            report(0,"no problems reported");
        }
    }
    return hasProblem;
}



void UnitTest::startTestSystem() {
    if (theRoot==NULL) {
        theRoot = new RootUnitTest(NULL);
    }
}

// system starts on first call, probably from a static object - this
// is to avoid link order dependency problems
UnitTest& UnitTest::getRoot() {
    startTestSystem();
    yAssert(theRoot!=NULL);
    return *theRoot;
}

// this is the important one to call
void UnitTest::stopTestSystem() {
    if (theRoot!=NULL) {
        delete theRoot;
        theRoot = NULL;
    }
}


bool UnitTest::checkEqualImpl(int x, int y, 
                              const char *desc,
                              const char *txt1,
                              const char *txt2,
                              const char *fname,
                              int fline) {
    char buf[1000];
    ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%d) == %s (%d)",
                    fname, fline, desc, txt1, x, txt2, y);
    if (x==y) {
        report(0,ConstString("  [") + desc + "] passed ok");
    } else {
        report(1,ConstString("  FAILURE ") + buf);
    }
    return x==y;
}

bool UnitTest::checkEqualishImpl(double x, double y, 
                                 const char *desc,
                                 const char *txt1,
                                 const char *txt2,
                                 const char *fname,
                                 int fline) {
    char buf[1000];
    ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%g) == %s (%g)",
                    fname, fline, desc, txt1, x, txt2, y);
    bool ok = (fabs(x-y)<0.0001);
    if (ok) {
        report(0,ConstString("  [") + desc + "] passed ok");
    } else {
        report(1,ConstString("  FAILURE ") + buf);
    }
    return ok;
}


bool UnitTest::checkEqualImpl(const ConstString& x, const ConstString& y,
                              const char *desc,
                              const char *txt1,
                              const char *txt2,
                              const char *fname,
                              int fline) {
    char buf[1000];
    ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%s) == %s (%s)",
                    fname, fline, desc, txt1, humanize(x).c_str(), txt2, humanize(y).c_str());
    bool ok = (x==y);
    if (ok) {
        report(0,ConstString("  [") + desc + "] passed ok");
    } else {
        report(1,ConstString("  FAILURE ") + buf);
    }
    return ok;
}


ConstString UnitTest::humanize(const ConstString& txt) {
    ConstString result("");
    for (unsigned int i=0; i<txt.length(); i++) {
        char ch = txt[i];
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


void UnitTest::saveEnvironment(const char *key) {
    bool found = false;
    ConstString val = NetworkBase::getEnvironment(key,&found);
    Bottle& lst = env.addList();
    lst.addString(key);
    lst.addString(val);
    lst.addInt(found?1:0);
}

void UnitTest::restoreEnvironment() {
    for (int i=0; i<env.size(); i++) {
        Bottle *lst = env.get(i).asList();
        if (lst==NULL) continue;
        ConstString key = lst->get(0).asString();
        ConstString val = lst->get(1).asString();
        bool found = lst->get(2).asInt()?true:false;
        if (!found) {
            NetworkBase::unsetEnvironment(key);
        } else {
            NetworkBase::setEnvironment(key,val);
        }
    }
    env.clear();
}

 
bool UnitTest::heapMonitorSupported() {
#ifdef YARP_TEST_HEAP
    return true;
#else
    return false;
#endif
}

void UnitTest::heapMonitorBegin(bool expectAllocations) {
#ifdef YARP_TEST_HEAP
    heapMonitorEnd();
    heap_count_ops = 0;
    heap_count_mutex = new Mutex();
    heap_expect_ops = expectAllocations;
    heap_count_active = true;
#endif
}

int UnitTest::heapMonitorOps() {
#ifdef YARP_TEST_HEAP
    heap_count_mutex->lock();
    int diff = heap_count_ops;
    heap_count_ops = 0;
    heap_count_mutex->unlock();
    if (!heap_expect_ops) {
        checkEqual(0,diff,"heap operator count");
    }
    return diff;
#else
    return -1;
#endif
}

int UnitTest::heapMonitorEnd() {
#ifdef YARP_TEST_HEAP
    if (!heap_count_mutex) return 0;
    heap_count_mutex->lock();
    heap_count_active = false;
    int diff = heap_count_ops;
    heap_count_ops = 0;
    heap_count_mutex->unlock();
    if (!heap_expect_ops) {
        checkEqual(0,diff,"heap operator final count");
    }
    delete heap_count_mutex;
    heap_count_mutex = NULL;
    return diff;
#else
    return -1;
#endif
}
