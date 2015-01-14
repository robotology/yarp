// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __YARP_MANAGER_LOCALBROKER__
#define __YARP_MANAGER_LOCALBROKER__

#include <cstdio>
#include <string>
#include <iostream>

#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Thread.h>

#include <yarp/manager/broker.h>

#if defined(WIN32)
    #include<Windows.h>
#endif

namespace yarp {
namespace manager {

/**
 * Class LocalBroker
 */
class LocalBroker: public Broker, public yarp::os::Thread {

public:
    LocalBroker();
    virtual ~LocalBroker();
    bool init();
    bool init(const char* szcmd, const char* szparam,
              const char* szhost, const char* szstdio,
              const char* szworkdir, const char* szenv );
    void fini(void);
    bool start();
    bool stop();
    bool kill();
    bool connect(const char* from, const char* to,
                 const char* carrier, bool persist=false);
    bool disconnect(const char* from, const char* to);
    int running(void);
    bool exists(const char* port);
    bool connected(const char* from, const char* to);
    const char* error(void);
    bool initialized(void) { return bInitialized;}
    bool attachStdout(void);
    void detachStdout(void);

    inline void showConsole(bool show) { bShowConsole = show; }

public: // for rate thread
    void run();
    bool threadInit();
    void threadRelease();

protected:

private:
    string strCmd;
    string strParam;
    string strHost;
    string strStdio;
    string strWorkdir;
    string strTag;
    string strEnv;
    int ID;
    string strError;
    bool bOnlyConnector;
    bool bInitialized;
    int  pipe_to_stdout[2];
    FILE* fd_stdout;
    bool bShowConsole;
    bool timeout(double base, double timeout);
    int ExecuteCmd(void);
    bool psCmd(int pid);
    bool killCmd(int pid);
    bool stopCmd(int pid);
#if defined(WIN32)
    HANDLE read_from_pipe_cmd_to_stdout;
    HANDLE write_to_pipe_cmd_to_stdout;
    string lastError2String();
#else
    int waitPipe(int pipe_fd);
    int waitPipeSignal(int pipe_fd);
    void splitLine(char *pLine, char **pArgs);
    void parseArguments(char *io_pLine, int *o_pArgc, char **o_pArgv);
#endif
    bool startStdout(void);
    void stopStdout(void);

    inline bool IS_PARENT_OF(int pid){ return pid>0; }
    inline bool IS_NEW_PROCESS(int pid){ return !pid; }
    inline bool IS_INVALID(int pid){ return pid<0; }
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_LOCALBROKER__
