// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007-2009 RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Port.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/RunProcManager.h>

#ifndef YARP_HAS_ACE
#ifndef __APPLE__
#include <wait.h>
#else
#include <sys/wait.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#endif

#if defined(WIN32)
    #define SIGKILL 9
    static bool KILL(HANDLE handle)
    { 
        BOOL bRet=TerminateProcess(handle,0);
        CloseHandle(handle);
        fprintf(stdout,"brutally terminated by TerminateProcess\n");
        fflush(stdout);
        return bRet?true:false;
    }
    static bool TERMINATE(PID pid); 
    #define CLOSE(h) CloseHandle(h)
    #ifndef __GNUC__
    static DWORD WINAPI ZombieHunter(__in LPVOID lpParameter)
    #else
    static DWORD WINAPI ZombieHunter(LPVOID lpParameter)
    #endif
    {
	    DWORD nCount;

	    while (true)
	    {
	        if (yarp::os::Run::aHandlesVector)
	        {
		        delete [] yarp::os::Run::aHandlesVector;
		        yarp::os::Run::aHandlesVector=0;
	        }
	        nCount=0;

	        yarp::os::Run::aHandlesVector=new HANDLE[yarp::os::Run::mProcessVector.Size()+yarp::os::Run::mStdioVector.Size()];

	        yarp::os::Run::mProcessVector.GetHandles(yarp::os::Run::aHandlesVector,nCount);
	        yarp::os::Run::mStdioVector.GetHandles(yarp::os::Run::aHandlesVector,nCount);

		    if (nCount)
		    {
			    WaitForMultipleObjects(nCount,yarp::os::Run::aHandlesVector,FALSE,INFINITE);
		    }
		    else
		    {
			    yarp::os::Run::hZombieHunter=NULL;
			    return 0;
		    }
	    }

	    return 0;
    }
#else
    #include <unistd.h>
    #include <fcntl.h>
    int CLOSE(int h){ return close(h)==0; }
    int SIGNAL(int pid,int signum,bool wait)
    { 
        int ret=!kill(pid,signum);
        if (wait) waitpid(pid,0,0);
        return ret;
    }
    void sigchild_handler(int sig)
    {   
        yarp::os::Run::CleanZombies();
    }
#endif

YarpRunProcInfo::YarpRunProcInfo(yarp::os::ConstString& alias,yarp::os::ConstString& on,PID pidCmd,HANDLE handleCmd,bool hold)
{
    mAlias=alias;
    mOn=on;
    mPidCmd=pidCmd;
    mHandleCmd=handleCmd;
    mHold=hold;
}

bool YarpRunProcInfo::Signal(int signum)
{	
#if defined(WIN32)
    if (signum==SIGKILL)
    {
        if (mHandleCmd) return KILL(mHandleCmd);
    }
    else
    {
        if (mPidCmd) return TERMINATE(mPidCmd);
    }
#else
    if (mPidCmd && !mHold)
    {
        return SIGNAL(mPidCmd,signum,false);
    }
#endif

    return true;
}

bool YarpRunProcInfo::IsActive()
{
    if (!mPidCmd) return false;

#if defined(WIN32)
    DWORD status;
    return (::GetExitCodeProcess(mHandleCmd,&status) && status==STILL_ACTIVE);
#else
    return !kill(mPidCmd,0);
#endif
}

YarpRunInfoVector::YarpRunInfoVector()
{
    m_nProcesses=0;

    for (int i=0; i<MAX_PROCESSES; ++i)
    {
        m_apList[i]=0;
    }
}

YarpRunInfoVector::~YarpRunInfoVector()
{
    for (int i=0; i<MAX_PROCESSES; ++i)
    {
        if (m_apList[i])
        {
            delete m_apList[i];
        }
    }
}

bool YarpRunInfoVector::Add(YarpRunProcInfo *process)
{
    mutex.wait();

    if (m_nProcesses>=MAX_PROCESSES)
    {
        fprintf(stderr,"ERROR: maximum process limit reached\n");
        mutex.post();		
        return false;
    }

#if defined(WIN32)
    if (yarp::os::Run::hZombieHunter) TerminateThread(yarp::os::Run::hZombieHunter,0);
#endif

    m_apList[m_nProcesses++]=process;

#if defined(WIN32)
    yarp::os::Run::hZombieHunter=CreateThread(0,0,ZombieHunter,0,0,0);
#endif

    mutex.post();

    return true;
}

int YarpRunInfoVector::Signal(yarp::os::ConstString& alias,int signum)
{
    mutex.wait();

    YarpRunProcInfo **aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;	

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias))
        {
            if (m_apList[i]->IsActive())
            {
                aKill[nKill++]=m_apList[i]; 
            }
        }
    }

    mutex.post();

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr,"SIGNAL  %s (%d)\n",m_apList[k]->mAlias.c_str(),m_apList[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    return nKill;
}

int YarpRunInfoVector::Killall(int signum)
{
    mutex.wait();

    YarpRunProcInfo **aKill=new YarpRunProcInfo*[m_nProcesses];
    int nKill=0;		

    for (int i=0; i<m_nProcesses; ++i)
    {    
        if (m_apList[i])
        {
            if (m_apList[i]->IsActive())
            {
                aKill[nKill++]=m_apList[i]; 
            }
        }
    }

    mutex.post();

    for (int k=0; k<nKill; ++k)
    {
        fprintf(stderr,"SIGNAL %s (%d)\n",m_apList[k]->mAlias.c_str(),m_apList[k]->mPidCmd);
        aKill[k]->Signal(signum);
    }

    delete [] aKill;

    return nKill;
}

#if defined(WIN32) 
void YarpRunInfoVector::GetHandles(HANDLE* &lpHandles,DWORD &nCount)
{
    mutex.wait();

    for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
    {
        if (!m_apList[i]->IsActive())
        {
            fprintf(stderr,"CLEAN-UP %s (%d)\n",m_apList[i]->mAlias.c_str(),m_apList[i]->mPidCmd);
            fflush(stderr);

            m_apList[i]->Clean();
            delete m_apList[i];
            m_apList[i]=0;
        }
    }

    Pack();

    for (int i=0; i<m_nProcesses; ++i)
    {
        lpHandles[nCount+i]=m_apList[i]->mHandleCmd;
    }

    nCount+=m_nProcesses;

    mutex.post();
}
#else
void YarpRunInfoVector::CleanZombies()
{	
    //yarp::os::Time::delay(1.0); 

    mutex.wait();

    YarpRunProcInfo **apZombie=new YarpRunProcInfo*[m_nProcesses];
    int nZombies=0;

    for (int i=0; i<m_nProcesses; ++i)
    {
        //if (m_apList[i] && waitpid(m_apList[i]->m_pid_cmd,0,WNOHANG)==m_apList[i]->m_pid_cmd)
        if (m_apList[i] && m_apList[i]->waitPid())
        {	            
            apZombie[nZombies++]=m_apList[i];
            m_apList[i]=NULL;
        }
    }

    Pack();

    mutex.post();

    for (int z=0; z<nZombies; ++z)
    {
        fprintf(stderr,"CLEAN-UP %s (%d)\n",apZombie[z]->mAlias.c_str(),apZombie[z]->mPidCmd);
        fflush(stderr);

        apZombie[z]->Clean();
        delete apZombie[z];
    }

    delete [] apZombie;
}
#endif

yarp::os::Bottle YarpRunInfoVector::PS()
{
    mutex.wait();

    yarp::os::Bottle ps,line,grp;

    for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
    {
        line.clear();

        grp.clear();
        grp.addString("pid");
        grp.addInt(m_apList[i]->mPidCmd);
        line.addList()=grp;

        grp.clear();
        grp.addString("tag"); 
        grp.addString(m_apList[i]->mAlias.c_str());
        line.addList()=grp;

        grp.clear();
        grp.addString("status");
        grp.addString(m_apList[i]->IsActive()?"running":"zombie");
        line.addList()=grp;

        ps.addList()=line;
    }

    mutex.post();

    return ps;
}

bool YarpRunInfoVector::IsRunning(yarp::os::ConstString &alias)
{
    mutex.wait();

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i] && m_apList[i]->Match(alias))
        {
            if (m_apList[i]->IsActive())
            {
                mutex.post();
                return true;
            }
            else
            {
                mutex.post();
                return false;
            }
        }
    }

    mutex.post();

    return false;
}

void YarpRunInfoVector::Pack()
{
    int tot=0;

    for (int i=0; i<m_nProcesses; ++i)
    {
        if (m_apList[i])
        {
            m_apList[tot++]=m_apList[i];
        }
    }

    for (int i=tot; i<m_nProcesses; ++i)
    {
        m_apList[i]=0;
    }

    m_nProcesses=tot;
}

YarpRunCmdWithStdioInfo::YarpRunCmdWithStdioInfo(yarp::os::ConstString& alias,
                                                 yarp::os::ConstString& on,
                                                 yarp::os::ConstString& stdio,
                                                 PID pidCmd,
                                                 yarp::os::ConstString& stdioUUID,
                                                 YarpRunInfoVector* stdioVector,
                                                 PID pidStdin,
                                                 PID pidStdout,
                                                 FDESC readFromPipeStdinToCmd,
                                                 FDESC writeToPipeStdinToCmd,
                                                 FDESC readFromPipeCmdToStdout,
                                                 FDESC writeToPipeCmdToStdout,
                                                 HANDLE handleCmd,
                                                 bool hold)
                                                 : 
YarpRunProcInfo(alias,on,pidCmd,handleCmd,hold)
{
    mPidStdin=pidStdin;
    mPidStdout=pidStdout;
    mStdio=stdio;
    mStdioUUID=stdioUUID;

    mStdioVector=stdioVector;

    mReadFromPipeStdinToCmd=readFromPipeStdinToCmd;
    mWriteToPipeStdinToCmd=writeToPipeStdinToCmd;
    mReadFromPipeCmdToStdout=readFromPipeCmdToStdout;
    mWriteToPipeCmdToStdout=writeToPipeCmdToStdout;
}

void YarpRunCmdWithStdioInfo::Clean()
{
    fflush(stdout);
    fflush(stderr);

    if (mWriteToPipeStdinToCmd)   CLOSE(mWriteToPipeStdinToCmd);
    if (mReadFromPipeStdinToCmd)  CLOSE(mReadFromPipeStdinToCmd);
    if (mWriteToPipeCmdToStdout)  CLOSE(mWriteToPipeCmdToStdout);
    if (mReadFromPipeCmdToStdout) CLOSE(mReadFromPipeCmdToStdout);
    mWriteToPipeStdinToCmd=mReadFromPipeStdinToCmd=0;
    mWriteToPipeCmdToStdout=mReadFromPipeCmdToStdout=0;

    yarp::os::ConstString strCmdUUID=yarp::os::ConstString("/")+mOn+"/"+int2String(getpid())+"/"+mAlias+"-"+int2String(mPidCmd);

    //yarp::os::NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(mStdioUUID+"/stdio:i").c_str());
    //yarp::os::NetworkBase::disconnect((mStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());

    mPidCmd=0;

    if (mPidStdin)
    {  
#if defined(WIN32)
        TERMINATE(mPidStdin);
#else
        SIGNAL(mPidStdin,SIGTERM,true);
#endif
        fprintf(stderr,"TERMINATING stdin (%d)\n",mPidStdin);
        mPidStdin=0;
        fflush(stderr);
    }
    if (mPidStdout)
    {
#if defined(WIN32)
        TERMINATE(mPidStdout);
#else
        SIGNAL(mPidStdout,SIGTERM,true);
#endif
        fprintf(stderr,"TERMINATING stdout (%d)\n",mPidStdout);
        mPidStdout=0;
        fflush(stderr);
    }

    TerminateStdio();
}

#if !defined(WIN32)
bool YarpRunCmdWithStdioInfo::waitPid()
{
    bool r0=waitpid(mPidCmd,0,WNOHANG)==mPidCmd;
    bool r1=waitpid(mPidStdin,0,WNOHANG)==mPidStdin;
    bool r2=waitpid(mPidStdout,0,WNOHANG)==mPidStdout;

    return r0 || r1 || r2;
}
#endif

void YarpRunCmdWithStdioInfo::TerminateStdio()
{
    if (mOn==mStdio)
    {
        mStdioVector->Signal(mAlias,SIGTERM);
    }
    else
    {
        yarp::os::Bottle msg;
        msg.fromString((yarp::os::ConstString("(killstdio ")+mAlias+")").c_str());

        yarp::os::Port port;
        port.open("...");
        bool connected=yarp::os::NetworkBase::connect(port.getName(), mStdio);
        if (connected)
        {
            port.write(msg);
            yarp::os::NetworkBase::disconnect(port.getName().c_str(),mStdio.c_str());
        }
        port.close();
    }
}

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////

#if defined(WIN32)

#define TA_FAILED        0
#define TA_SUCCESS_CLEAN 1
#define TA_SUCCESS_KILL  2

class TerminateParams
{
public:
    TerminateParams(DWORD id)
    {
        nWin=0;
        dwID=id;
    }

    ~TerminateParams(){}

    int nWin;
    DWORD dwID;
};

BOOL CALLBACK TerminateAppEnum(HWND hwnd,LPARAM lParam)
{
    TerminateParams* params=(TerminateParams*)lParam;

    DWORD dwID;
    GetWindowThreadProcessId(hwnd,&dwID) ;

    if (dwID==params->dwID)
    {
        params->nWin++;
        PostMessage(hwnd,WM_CLOSE,0,0);
    }

    return TRUE ;
}

/*----------------------------------------------------------------
Purpose:
Shut down a 32-Bit Process

Parameters:
dwPID
Process ID of the process to shut down.
----------------------------------------------------------------*/ 
bool TERMINATE(PID dwPID) 
{
    HANDLE hProc;

    // If we can't open the process with PROCESS_TERMINATE rights,
    // then we give up immediately.
    hProc=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,FALSE,dwPID);

    if (hProc==NULL)
    {
        return false;
    }

    // TerminateAppEnum() posts WM_CLOSE to all windows whose PID
    // matches your process's.

    TerminateParams params(dwPID);

    EnumWindows((WNDENUMPROC)TerminateAppEnum,(LPARAM)&params);

    if (params.nWin)
    {
        fprintf(stdout,"%d terminated by WM_CLOSE\n",dwPID);
    }
    else
    {
        GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,dwPID);
        fprintf(stdout,"%d terminated by CTRL_BREAK_EVENT\n",dwPID);    
    }

    fflush(stdout);

    CloseHandle(hProc);

    return true;
}

#endif
