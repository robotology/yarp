// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <signal.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Run.h>
#include <string>

#ifndef YARP_HAS_ACE
#ifndef __APPLE__
#include <wait.h>
#else
#include <sys/wait.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#endif

#define YARPRUN_ERROR -1

#if defined(WIN32) || defined(WIN64)
typedef DWORD PID;
typedef HANDLE FDESC;
#define SIGKILL 9
bool KILL(HANDLE handle)
{ 
    BOOL bRet=TerminateProcess(handle,0);
    CloseHandle(handle);
    fprintf(stdout,"brutally terminated by TerminateProcess\n");
    fflush(stdout);
    return bRet?true:false;
}
bool TERMINATE(PID pid); 
#define CLOSE(h) CloseHandle(h)
HANDLE  hZombieHunter=NULL;
HANDLE* aHandlesVector=NULL;
#ifndef __GNUC__
DWORD WINAPI ZombieHunter(__in  LPVOID lpParameter);
#else
DWORD WINAPI ZombieHunter(LPVOID lpParameter);
#endif
#else
#include <unistd.h>
#include <fcntl.h>
typedef int PID;
typedef int FDESC;
typedef void* HANDLE;
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

const double TIMEOUT=2.0;
inline bool attemptConnect(const yarp::os::ConstString &from,const yarp::os::ConstString &to,double timeout=TIMEOUT)
{
    return yarp::os::NetworkBase::connect(from.c_str(),to.c_str());
}

inline yarp::os::ConstString int2String(int x)
{
    char buff[16];
    sprintf(buff,"%d",x);
    return yarp::os::ConstString(buff);
}

#if defined(WIN32) || defined(WIN64)
inline yarp::os::ConstString lastError2String()
{
    int error=GetLastError();
    char buff[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,error,0,buff,1024,NULL);
    return yarp::os::ConstString(buff);
}
#endif

class YarpRunProcInfo
{
public:
	YarpRunProcInfo(yarp::os::ConstString& alias,yarp::os::ConstString& on,PID pidCmd,HANDLE handleCmd,bool hold)
    {
		mAlias=alias;
		mOn=on;
		mPidCmd=pidCmd;
		mHandleCmd=handleCmd;
		mHold=hold;
	}

	virtual ~YarpRunProcInfo(){}

	virtual bool Match(yarp::os::ConstString& alias){ return mAlias==alias; }

	virtual bool Signal(int signum)
	{	
        #if defined(WIN32) || defined(WIN64)
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

    virtual void Clean()
    { 
        mPidCmd=0;     
    }
    
    #if !defined(WIN32) && !defined(WIN64)
    virtual bool waitPid()
    {
        return waitpid(mPidCmd,0,WNOHANG)==mPidCmd;
    }
    #endif

	virtual bool IsActive()
	{
	    if (!mPidCmd) return false;
	    
		#if defined(WIN32) || defined(WIN64)
			DWORD status;
			return (::GetExitCodeProcess(mHandleCmd,&status) && status==STILL_ACTIVE);
		#else
			return !kill(mPidCmd,0);
		#endif
	}
	
protected:
	yarp::os::ConstString mAlias;
	yarp::os::ConstString mOn;
	PID mPidCmd;
	HANDLE mHandleCmd; // only windows
	bool mHold; // only linux

	friend class YarpRunInfoVector; 
};

class YarpRunInfoVector
{
public:
	YarpRunInfoVector()
	{
		m_nProcesses=0;
		
		for (int i=0; i<MAX_PROCESSES; ++i)
		{
			m_apList[i]=0;
		}
	}

	~YarpRunInfoVector()
	{
		for (int i=0; i<MAX_PROCESSES; ++i)
		{
			if (m_apList[i])
			{
				delete m_apList[i];
			}
		}
	}

	int Size(){ return m_nProcesses; }

	bool Add(YarpRunProcInfo *process)
	{
		mutex.wait();

		if (m_nProcesses>=MAX_PROCESSES)
		{
			fprintf(stderr,"ERROR: maximum process limit reached\n");
			mutex.post();		
			return false;
		}

		#if defined(WIN32) || defined(WIN64)
        if (hZombieHunter) TerminateThread(hZombieHunter,0);
		#endif

		m_apList[m_nProcesses++]=process;

		#if defined(WIN32) || defined(WIN64)
        hZombieHunter=CreateThread(0,0,ZombieHunter,0,0,0);
		#endif

		mutex.post();
		
		return true;
	}

	int Signal(yarp::os::ConstString& alias,int signum)
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

	int Killall(int signum)
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
	
	#if defined(WIN32) || defined(WIN64) 
	void GetHandles(HANDLE* &lpHandles,DWORD &nCount)
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
	void CleanZombies()
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

    yarp::os::Bottle PS()
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

	bool IsRunning(yarp::os::ConstString &alias)
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

	yarp::os::Semaphore mutex;

protected:
	
	void Pack()
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

	static const int MAX_PROCESSES=1024;
	int m_nProcesses;
	YarpRunProcInfo* m_apList[MAX_PROCESSES];
	YarpRunInfoVector *m_pStdioMate;
};

class YarpRunCmdWithStdioInfo : public YarpRunProcInfo
{
public:
	YarpRunCmdWithStdioInfo(yarp::os::ConstString& alias,
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

	virtual ~YarpRunCmdWithStdioInfo(){}
	
	virtual void Clean()
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
	
        yarp::os::NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(mStdioUUID+"/stdio:i").c_str());
        yarp::os::NetworkBase::disconnect((mStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
		
	    mPidCmd=0;
	
		if (mPidStdin)
		{  
            #if defined(WIN32) || defined(WIN64)
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
            #if defined(WIN32) || defined(WIN64)
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
	
    #if !defined(WIN32) && !defined(WIN64)
	virtual bool waitPid()
    {
        bool r0=waitpid(mPidCmd,0,WNOHANG)==mPidCmd;
        bool r1=waitpid(mPidStdin,0,WNOHANG)==mPidStdin;
        bool r2=waitpid(mPidStdout,0,WNOHANG)==mPidStdout;
        
        return r0 || r1 || r2;
    }
    #endif

	void TerminateStdio()
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
            bool connected=attemptConnect(port.getName(), mStdio);
            if (connected)
            {
			    port.write(msg);
                yarp::os::NetworkBase::disconnect(port.getName().c_str(),mStdio.c_str());
            }
            port.close();
		}
	}

protected:
	PID mPidStdin;
    PID mPidStdout;

    yarp::os::ConstString mStdio;
    yarp::os::ConstString mStdioUUID;

	FDESC mWriteToPipeStdinToCmd;
    FDESC mReadFromPipeStdinToCmd;
	FDESC mWriteToPipeCmdToStdout;
    FDESC mReadFromPipeCmdToStdout;

	YarpRunInfoVector *mStdioVector;
};

///////////////////////////
// OS INDEPENDENT FUNCTIONS
///////////////////////////

void dontexit(int sig)
{
    signal(SIGINT,dontexit);
	signal(SIGTERM,dontexit);
}

YarpRunInfoVector yarp::os::Run::mProcessVector;
YarpRunInfoVector yarp::os::Run::mStdioVector;
yarp::os::ConstString yarp::os::Run::mPortName;
yarp::os::Port* yarp::os::Run::pServerPort=0;
int yarp::os::Run::mProcCNT=0;

int yarp::os::Run::main(int argc, char *argv[]) 
{
	mPortName="";

	if (!NetworkBase::getLocalMode())
    {
		if (!NetworkBase::checkNetwork())
		{
			fprintf(stderr,"ERROR: no yarp network found.\n");
			return YARPRUN_ERROR;
		}
    }

    Property config;
    config.fromCommand(argc,argv,false);

    if (config.check("block"))
    {
        signal(SIGINT,dontexit);
	    signal(SIGTERM,dontexit);
	    
	    for (unsigned int i=0; i<0xFFFFFFFF; ++i)
	    {
	        fprintf(stdout,"%d\n",i);
	        fflush(stdout);
	        yarp::os::Time::delay(1.0);
	    }
    }
    
	if (config.check("echo"))
	{
		char line[1024];
		fprintf(stderr,"Program echo started.\n");
        fflush(stderr);
        
		while(true)
		{
			int ret=scanf("%s",line);
            
            if (ret!=0)
            {
                fprintf(stderr,"%s\n",line);
                fflush(stderr);
            }
		}
		return 0;
	}

	if (config.check("segfault"))
	{
	    fprintf(stderr,"writing to forbidden location\n");
	    fflush(stderr);
	
	    int *zero=NULL;
	    
	    *zero=0;
	    
	    return 0;
	}

	if (config.check("wait"))
	{
	    yarp::os::Time::delay(config.find("wait").asDouble());
	
	    fprintf(stderr,"Done.\n");
	    fflush(stderr);
	    
	    return 0;
	}

	// HELP
	if (config.check("help"))
	{
		Help();
		return 0;
	}

	// SERVER
    if (config.check("server")) 
	{
		mPortName=yarp::os::ConstString(config.find("server").asString());
		return Server();
	} 

	// CLIENT (config is from keyboard)
	if (config.check("stdio")
	 || config.check("cmd") 
	 || config.check("kill") 
	 || config.check("sigterm")
	 || config.check("sigtermall") 
	 || config.check("exit") 
	 || config.check("isrunning")
	 || config.check("ps"))
	{ 
        return sendToServer(config);
    }

	Help();
    return 0;
}

yarp::os::Bottle yarp::os::Run::SendMsg(Bottle& msg,yarp::os::ConstString target)
{
	Port port;
    port.open("...");

    bool connected=attemptConnect(port.getName(), target);

    if (!connected)
    {
        Bottle response;
        response.addString("RESPONSE:\n=========\n");
        response.addString("Cannot connect to remote server, aborting...\n");
        fprintf(stderr,"%s\n",response.toString().c_str());
        port.close();
        
        return response;
    }

	Bottle response;
    port.write(msg,response);

    NetworkBase::disconnect(port.getName().c_str(),target.c_str());

    port.close();

	int size=response.size();
	fprintf(stderr,"RESPONSE:\n=========\n");
    for (int s=0; s<size; ++s)
    {
        //if (response.get(s).isString())
        {
            fprintf(stderr,"%s\n",response.get(s).toString().c_str());
        }
    }
    
	return response;
}

void sigint_handler(int sig)
{
	if (yarp::os::Run::pServerPort)
	{
		yarp::os::Port *pClose=yarp::os::Run::pServerPort;
		yarp::os::Run::pServerPort=0;
		pClose->close();
	}
}

int yarp::os::Run::Server()
{
	Port port;
	port.open(mPortName.c_str());
	pServerPort=&port;

	signal(SIGINT,sigint_handler);
	signal(SIGTERM,sigint_handler);

	#if !defined(WIN32) && !defined(WIN64)
	signal(SIGCHLD,sigchild_handler); 
	#endif

    while (pServerPort) 
	{
		Bottle msg;
        port.read(msg,true);

        fprintf(stderr,"%s\n",msg.toString().c_str());
        fflush(stdout);

		if (!pServerPort) break;

		// command with stdio management
		if (msg.check("stdio") && msg.check("cmd"))
		{
			yarp::os::ConstString strStdioPort=msg.find("stdio").asString();
			yarp::os::ConstString strOnPort=msg.find("on").asString();

			// AM I THE CMD OR/AND STDIO SERVER?
			if (mPortName==strStdioPort) // stdio
			{
			    Bottle botStdioResult;
                yarp::os::ConstString strStdioUUID;
                int pidStdio=UserStdio(msg,botStdioResult,strStdioUUID);

                Bottle botStdioUUID;
                botStdioUUID.addString("stdiouuid");
                botStdioUUID.addString(strStdioUUID.c_str());
                msg.addList()=botStdioUUID;

				if (pidStdio>0)
                {
                    Bottle cmdResult;
				    if (mPortName==strOnPort)
				    {
                        // execute command here
					    cmdResult=ExecuteCmdAndStdio(msg);
				    }
				    else
				    {
					    // execute command on cmd server
                        cmdResult=SendMsg(msg,strOnPort);
				    }
                    
                    cmdResult.append(botStdioResult);
                    //yarp::os::Time::delay(10.0);
                    port.reply(cmdResult);
				}
                else
                {
                    port.reply(botStdioResult);
                }
			}
			else if (mPortName==yarp::os::ConstString(strOnPort)) // cmd
			{
				Bottle cmdResult=ExecuteCmdAndStdio(msg);
                port.reply(cmdResult);
			}
			else // some error (should never happen)
			{			    
                Bottle botFailure;
                botFailure.addInt(-1);
                port.reply(botFailure);
            }
			continue;
		}

		// without stdio
		if (msg.check("cmd"))
		{			
            Bottle cmdResult=ExecuteCmd(msg);
            port.reply(cmdResult);
			continue;
		}

		if (msg.check("kill"))
		{
			yarp::os::ConstString alias(msg.findGroup("kill").get(1).asString());
			int sig=msg.findGroup("kill").get(2).asInt();
            Bottle result;
			result.addString(mProcessVector.Signal(alias,sig)?"kill OK":"kill FAILED");
			port.reply(result);
            continue;
		}
		
	    if (msg.check("sigterm"))
		{
		    yarp::os::ConstString alias(msg.find("sigterm").asString());
            Bottle result;
			result.addString(mProcessVector.Signal(alias,SIGTERM)?"sigterm OK":"sigterm FAILED");
			port.reply(result);
			continue;
		}

		if (msg.check("sigtermall"))
		{
			mProcessVector.Killall(SIGTERM);
			Bottle result;
            result.addString("sigtermall OK");
			port.reply(result);
			continue;
		}

		if (msg.check("ps"))
		{
            Bottle result;
			result.append(mProcessVector.PS());
			port.reply(result);
			continue;
		}

		if (msg.check("isrunning"))
		{
		    yarp::os::ConstString alias(msg.find("isrunning").asString());
			Bottle result;
            result.addString(mProcessVector.IsRunning(alias)?"running":"not running");
			port.reply(result);
			continue;
		}

		if (msg.check("killstdio"))
		{
		    fprintf(stderr,"Run::Server() killstdio(%s)\n",msg.find("killstdio").asString().c_str());
		    yarp::os::ConstString alias(msg.find("killstdio").asString());
			mStdioVector.Signal(alias,SIGTERM);
			continue;
		}

		if (msg.check("exit"))
		{
            Bottle result;
			result.addString("exit OK");
			port.reply(result);
			port.close();
			pServerPort=0;
		}
	}

	Run::mStdioVector.Killall(SIGTERM);
	Run::mProcessVector.Killall(SIGTERM);
	
	#if defined(WIN32) || defined(WIN64)
	Run::mProcessVector.mutex.wait();
	if (hZombieHunter) TerminateThread(hZombieHunter,0);
	Run::mProcessVector.mutex.post();
	if (aHandlesVector) delete [] aHandlesVector;
	#endif

	return 0;
}

// CLIENT
int yarp::os::Run::sendToServer(yarp::os::Property& config)
{
	yarp::os::Bottle msg;

	// USE A YARP RUN SERVER TO MANAGE STDIO
	//
	// client -> stdio server -> cmd server
	//
	if (config.check("cmd") && config.check("stdio"))
	{
		if (config.find("stdio")=="") { Help("SYNTAX ERROR: missing remote stdio server\n"); return YARPRUN_ERROR; }
		if (config.find("cmd")=="")   { Help("SYNTAX ERROR: missing command\n"); return YARPRUN_ERROR; }
		if (!config.check("as") || config.find("as")=="") { Help("SYNTAX ERROR: missing tag\n"); return YARPRUN_ERROR; }
		if (!config.check("on") || config.find("on")=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }

		msg.addList()=config.findGroup("stdio");
		msg.addList()=config.findGroup("cmd");
		msg.addList()=config.findGroup("as");
		msg.addList()=config.findGroup("on");
		
		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
		if (config.check("geometry")) msg.addList()=config.findGroup("geometry");
		if (config.check("hold")) msg.addList()=config.findGroup("hold");

		Bottle response=SendMsg(msg,config.find("stdio").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return response.get(0).asInt()>0?0:2;
	}
	
	// DON'T USE A RUN SERVER TO MANAGE STDIO
	//
	// client -> cmd server
	//
	if (config.check("cmd"))
	{                
		if (config.find("cmd").asString()=="")   { Help("SYNTAX ERROR: missing command\n"); return YARPRUN_ERROR; }
		if (!config.check("as") || config.find("as").asString()=="") { Help("SYNTAX ERROR: missing tag\n"); return YARPRUN_ERROR; }
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }

		msg.addList()=config.findGroup("cmd");
		msg.addList()=config.findGroup("as");

		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");

		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return response.get(0).asInt()>0?0:2;
	}
	
	// client -> cmd server
	if (config.check("kill")) 
	{ 
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }
		if (config.findGroup("kill").get(1).asString()=="")  { Help("SYNTAX ERROR: missing tag\n"); return YARPRUN_ERROR; }
		if (config.findGroup("kill").get(2).asInt()==0)	  { Help("SYNTAX ERROR: missing signum\n"); return YARPRUN_ERROR; }

		msg.addList()=config.findGroup("kill");
		
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return response.get(0).asString()=="kill OK"?0:2;
	}

	// client -> cmd server
	if (config.check("sigterm")) 
	{ 
		if (config.find("sigterm").asString()=="") { Help("SYNTAX ERROR: missing tag"); return YARPRUN_ERROR; }
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }
		
		msg.addList()=config.findGroup("sigterm");
		
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return response.get(0).asString()=="sigterm OK"?0:2;
	}

	// client -> cmd server
	if (config.check("sigtermall")) 
	{ 
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }
		
		msg.addList()=config.findGroup("sigtermall");
       
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return 0;
	}

	if (config.check("ps"))
	{
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }
		
		msg.addList()=config.findGroup("ps");
       
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return 0;
	}

	if (config.check("isrunning"))
	{
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }
		if (config.find("isrunning").asString()=="") { Help("SYNTAX ERROR: missing tag\n"); return YARPRUN_ERROR; }

		msg.addList()=config.findGroup("isrunning");
		
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return response.get(0).asString()=="running"?0:2;
	}

	if (config.check("exit"))
	{
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return YARPRUN_ERROR; }
		
		msg.addList()=config.findGroup("exit");
        
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return 0;
	}

	return 0;
}

void yarp::os::Run::Help(const char *msg)
{
	fprintf(stderr,"%s",msg);
    fprintf(stderr,"\nUSAGE:\n\n");
    fprintf(stderr,"yarp run --server SERVERPORT\nrun a server on the local machine\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST] [--workdir WORKDIR]\nrun a command on SERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --stdio STDIOSERVERPORT [--hold] [--geometry WxH+X+Y] --cmd COMMAND [ARGLIST] [--workdir WORKDIR]\n");
    fprintf(stderr,"run a command on SERVERPORT server sending I/O to STDIOSERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --kill TAG SIGNUM\nsend SIGNUM signal to TAG command\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --sigterm TAG\nterminate TAG command\n\n");
	fprintf(stderr,"yarp run --on SERVERPORT --sigtermall\nterminate all commands\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --ps\nreport commands running on SERVERPORT\n\n");
	fprintf(stderr,"yarp run --on SERVERPORT --isrunning TAG\nTAG command is running?\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --exit\nstop SERVERPORT server\n\n");  
}

/////////////////////////
// OS DEPENDENT FUNCTIONS
/////////////////////////

// WINDOWS

//#define CREATE_NEW_PROCESS_GROUP 0

#if defined(WIN32) || defined(WIN64)

#ifndef __GNUC__
DWORD WINAPI ZombieHunter(__in LPVOID lpParameter)
#else
DWORD WINAPI ZombieHunter(LPVOID lpParameter)
#endif
{
	DWORD nCount;

	while (true)
	{
	    if (aHandlesVector)
	    {
		    delete [] aHandlesVector;
		    aHandlesVector=0;
	    }
	    nCount=0;

	    aHandlesVector=new HANDLE[yarp::os::Run::mProcessVector.Size()+yarp::os::Run::mStdioVector.Size()];

	    yarp::os::Run::mProcessVector.GetHandles(aHandlesVector,nCount);
	    yarp::os::Run::mStdioVector.GetHandles(aHandlesVector,nCount);

		if (nCount)
		{
			WaitForMultipleObjects(nCount,aHandlesVector,FALSE,INFINITE);
		}
		else
		{
			hZombieHunter=NULL;
			return 0;
		}
	}

	return 0;
}

// CMD SERVER
yarp::os::Bottle yarp::os::Run::ExecuteCmdAndStdio(Bottle& msg)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strStdio=msg.find("stdio").asString();
    yarp::os::ConstString strStdioUUID=msg.find("stdiouuid").asString();
    yarp::os::ConstString strCmdUUID=mPortName+"/"+int2String(GetCurrentProcessId())+"/"+strAlias+"-"+int2String(mProcCNT++);

	// PIPES
	SECURITY_ATTRIBUTES pipe_sec_attr; 
    pipe_sec_attr.nLength=sizeof(SECURITY_ATTRIBUTES); 
    pipe_sec_attr.bInheritHandle=TRUE; 
    pipe_sec_attr.lpSecurityDescriptor=NULL;
	HANDLE read_from_pipe_stdin_to_cmd,write_to_pipe_stdin_to_cmd;
    CreatePipe(&read_from_pipe_stdin_to_cmd,&write_to_pipe_stdin_to_cmd,&pipe_sec_attr,0);
    HANDLE read_from_pipe_cmd_to_stdout,write_to_pipe_cmd_to_stdout;
    CreatePipe(&read_from_pipe_cmd_to_stdout,&write_to_pipe_cmd_to_stdout,&pipe_sec_attr,0);

	// RUN STDOUT
	PROCESS_INFORMATION stdout_process_info;
	ZeroMemory(&stdout_process_info,sizeof(PROCESS_INFORMATION));
	STARTUPINFO stdout_startup_info;
	ZeroMemory(&stdout_startup_info,sizeof(STARTUPINFO));

	stdout_startup_info.cb=sizeof(STARTUPINFO); 
	stdout_startup_info.hStdError=stderr;
	stdout_startup_info.hStdOutput=stdout;
	stdout_startup_info.hStdInput=read_from_pipe_cmd_to_stdout;
	stdout_startup_info.dwFlags|=STARTF_USESTDHANDLES;

	BOOL bSuccess=CreateProcess(NULL,	// command name
								(char*)(yarp::os::ConstString("yarp quiet write ")+strCmdUUID+"/stdout verbatim").c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								CREATE_NEW_PROCESS_GROUP, // creation flags 
								NULL,          // use parent's environment 
								NULL,          // use parent's current directory 
								&stdout_startup_info,   // STARTUPINFO pointer 
								&stdout_process_info);  // receives PROCESS_INFORMATION 

    if (!bSuccess)
	{
        yarp::os::ConstString errorMsg=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +" alias="+strAlias+" cmd=stdout\n"
                                      +yarp::os::ConstString("Can't execute stdout because ")+lastError2String()+"\n";

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(errorMsg.c_str());
        fprintf(stderr,"%s",errorMsg.c_str());
        fflush(stderr);

		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);

	    return result;
	}

	// RUN STDIN
	PROCESS_INFORMATION stdin_process_info;
	ZeroMemory(&stdin_process_info,sizeof(PROCESS_INFORMATION));
	STARTUPINFO stdin_startup_info;
	ZeroMemory(&stdin_startup_info,sizeof(STARTUPINFO));

	stdin_startup_info.cb=sizeof(STARTUPINFO); 
	stdin_startup_info.hStdError=write_to_pipe_stdin_to_cmd;
	stdin_startup_info.hStdOutput=write_to_pipe_stdin_to_cmd;
	stdin_startup_info.hStdInput=stdin;
	stdin_startup_info.dwFlags|=STARTF_USESTDHANDLES;

	bSuccess=CreateProcess(NULL,	// command name
                           (char*)(yarp::os::ConstString("yarp quiet read ")+strCmdUUID+"/stdin").c_str(), // command line 
                           NULL,          // process security attributes 
                           NULL,          // primary thread security attributes 
                           TRUE,          // handles are inherited 
                           CREATE_NEW_PROCESS_GROUP, // creation flags 
                           NULL,          // use parent's environment 
                           NULL,          // use parent's current directory 
                           &stdin_startup_info,   // STARTUPINFO pointer 
                           &stdin_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess)
	{
        yarp::os::ConstString errorMsg=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +" alias="+strAlias+" cmd=stdin\n"
                                      +yarp::os::ConstString("Can't execute stdin because ")+lastError2String()+"\n";

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(errorMsg.c_str());
        fprintf(stderr,"%s",errorMsg.c_str());
        fflush(stderr);

		TerminateProcess(stdout_process_info.hProcess,YARPRUN_ERROR);
		CloseHandle(stdout_process_info.hProcess);

		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);

	    return result;
	}

    yarp::os::ConstString sPidStdin=int2String(stdin_process_info.dwProcessId);
    yarp::os::ConstString sPidStdout=int2String(stdout_process_info.dwProcessId);

	// connect yarp read and write
	bool bConnR=false,bConnW=false;
	for (int i=0; i<8 && !(bConnR&&bConnW); ++i)
	{ 	
        if (!bConnW && NetworkBase::connect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str())) bConnW=true;

        if (!bConnR && NetworkBase::connect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str())) bConnR=true;

	    if (!bConnW || !bConnR) yarp::os::Time::delay(0.05);
	}        
		    
    if (!(bConnR&&bConnW))
    {
	    Bottle result;
        result.addInt(YARPRUN_ERROR);

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=connect\nCan't connect stdio\n";
		
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        if (bConnW) NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());
		if (bConnR) NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());

		TerminateProcess(stdout_process_info.hProcess,YARPRUN_ERROR);
		CloseHandle(stdout_process_info.hProcess);
		TerminateProcess(stdin_process_info.hProcess,YARPRUN_ERROR);
		CloseHandle(stdin_process_info.hProcess);

		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);

	    return result;
	}

	// RUN COMMAND
	PROCESS_INFORMATION cmd_process_info;
	ZeroMemory(&cmd_process_info,sizeof(PROCESS_INFORMATION));
	STARTUPINFO cmd_startup_info;
	ZeroMemory(&cmd_startup_info,sizeof(STARTUPINFO));

	cmd_startup_info.cb=sizeof(STARTUPINFO); 
	cmd_startup_info.hStdError=write_to_pipe_cmd_to_stdout;
	cmd_startup_info.hStdOutput=write_to_pipe_cmd_to_stdout;
	cmd_startup_info.hStdInput=read_from_pipe_stdin_to_cmd;
	cmd_startup_info.dwFlags|=STARTF_USESTDHANDLES;

	Bottle command_bottle=msg.findGroup("cmd").tail();

    std::string tmpStr;
	for (int s=0; s<command_bottle.size(); ++s)
    {
        tmpStr+=command_bottle.get(s).toString()+yarp::os::ConstString(" ");
    }

    yarp::os::ConstString strCommand(tmpStr.c_str());

	bool bWorkdir=msg.check("workdir");
	yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

	bSuccess=CreateProcess(NULL,	// command name
								(char*)(strWorkdir+strCommand).c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								CREATE_NEW_PROCESS_GROUP, // creation flags 
								NULL, // use parent's environment 
								bWorkdir?strWorkdir.c_str():NULL, // working directory
								&cmd_startup_info,   // STARTUPINFO pointer 
								&cmd_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess && bWorkdir)
	{
			bSuccess=CreateProcess(NULL,	// command name
									(char*)(strCommand.c_str()), // command line 
									NULL,          // process security attributes 
									NULL,          // primary thread security attributes 
									TRUE,          // handles are inherited 
									CREATE_NEW_PROCESS_GROUP, // creation flags 
									NULL,          // use parent's environment 
									strWorkdir.c_str(), // working directory 
									&cmd_startup_info,   // STARTUPINFO pointer 
									&cmd_process_info);  // receives PROCESS_INFORMATION 
	}

	if (!bSuccess)
	{
        Bottle result;
        result.addInt(YARPRUN_ERROR);

        DWORD nBytes;
        yarp::os::ConstString line1=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd="+strCommand+"pid="+int2String(cmd_process_info.dwProcessId)+"\n";
        WriteFile(write_to_pipe_cmd_to_stdout,line1.c_str(),line1.length(),&nBytes,0);
        yarp::os::ConstString line2=yarp::os::ConstString("Can't execute command because ")+lastError2String()+"\n";
        WriteFile(write_to_pipe_cmd_to_stdout,line1.c_str(),line2.length(),&nBytes,0);
        FlushFileBuffers(write_to_pipe_cmd_to_stdout);

        yarp::os::ConstString out=line1+line2;
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        if (bConnW) NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());
		if (bConnR) NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());

        CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);

        TerminateProcess(stdout_process_info.hProcess,YARPRUN_ERROR);
		CloseHandle(stdout_process_info.hProcess);
		TerminateProcess(stdin_process_info.hProcess,YARPRUN_ERROR);
		CloseHandle(stdin_process_info.hProcess);

	    return result;
	}

    FlushFileBuffers(write_to_pipe_cmd_to_stdout);

	// EVERYTHING IS ALL RIGHT

	mProcessVector.Add(new YarpRunCmdWithStdioInfo(strAlias,
                                                    mPortName,
                                                    strStdio,
						                            cmd_process_info.dwProcessId,
                                                    strStdioUUID,
                                                    &mStdioVector,
                                                    stdin_process_info.dwProcessId,
                                                    stdout_process_info.dwProcessId,
						                            read_from_pipe_stdin_to_cmd,
                                                    write_to_pipe_stdin_to_cmd,
						                            read_from_pipe_cmd_to_stdout,
                                                    write_to_pipe_cmd_to_stdout,
						                            cmd_process_info.hProcess,
                                                    false));

    Bottle result;
    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName+" alias="+strAlias+" cmd="+strCommand+"pid="+int2String(cmd_process_info.dwProcessId)+"\n";
    result.addString(out.c_str());
    fprintf(stderr,"%s",out.c_str());

	return result;
}

yarp::os::Bottle yarp::os::Run::ExecuteCmd(yarp::os::Bottle& msg)
{
	yarp::os::ConstString alias=msg.find("as").asString().c_str();

	// RUN COMMAND
	PROCESS_INFORMATION cmd_process_info;
	ZeroMemory(&cmd_process_info,sizeof(PROCESS_INFORMATION));
	STARTUPINFO cmd_startup_info;
	ZeroMemory(&cmd_startup_info,sizeof(STARTUPINFO));

	cmd_startup_info.cb=sizeof(STARTUPINFO); 

	Bottle command_bottle=msg.findGroup("cmd").tail();
    
    yarp::os::impl::String tmpStr;
	for (int s=0; s<command_bottle.size(); ++s)
    {
        tmpStr+=command_bottle.get(s).toString()+yarp::os::ConstString(" ");
    }

    yarp::os::ConstString commandText(tmpStr.c_str());

	bool bWorkdir=msg.check("workdir");
	yarp::os::ConstString sWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

	BOOL bSuccess=CreateProcess(NULL,	// command name
								(char*)(sWorkdir+commandText).c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								CREATE_NEW_PROCESS_GROUP, // creation flags 
								NULL,          // use parent's environment 
								bWorkdir?sWorkdir.c_str():NULL, // working directory 
								&cmd_startup_info,   // STARTUPINFO pointer 
								&cmd_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess && bWorkdir)
	{
			bSuccess=CreateProcess(NULL,	// command name
									(char*)(commandText.c_str()), // command line 
									NULL,          // process security attributes 
									NULL,          // primary thread security attributes 
									TRUE,          // handles are inherited 
									CREATE_NEW_PROCESS_GROUP, // creation flags 
									NULL,          // use parent's environment 
									sWorkdir.c_str(), // working directory 
									&cmd_startup_info,   // STARTUPINFO pointer 
									&cmd_process_info);  // receives PROCESS_INFORMATION 
	}

	if (!bSuccess)
	{
        DWORD error=GetLastError();
        char errorMsg[1024];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,error,0,errorMsg,1024,NULL);
        
	    Bottle result;
        char pidstr[16];
	    sprintf(pidstr,"%d",cmd_process_info.dwProcessId);

        result.addInt(YARPRUN_ERROR);

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+alias+" cmd="+commandText+" pid="+pidstr+"\n"
                       +yarp::os::ConstString("Can't execute command because ")+errorMsg+"\n";

        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

	    return result;
	}

	// EVERYTHING IS ALL RIGHT

	mProcessVector.Add(new YarpRunProcInfo(alias,
                                           mPortName,
                                           cmd_process_info.dwProcessId,
                                           cmd_process_info.hProcess,
                                           false));

    Bottle result;
    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName+" alias="+alias+" cmd="+commandText+"pid="+int2String(cmd_process_info.dwProcessId)+"\n";
    fprintf(stderr,"%s",out.c_str());
	return result;
}

// STDIO SERVER
int yarp::os::Run::UserStdio(yarp::os::Bottle& msg,yarp::os::Bottle& result,yarp::os::ConstString& strStdioPortUUID)
{
	PROCESS_INFORMATION stdio_process_info;
	ZeroMemory(&stdio_process_info,sizeof(PROCESS_INFORMATION));

	STARTUPINFO stdio_startup_info;
	ZeroMemory(&stdio_startup_info,sizeof(STARTUPINFO));
	stdio_startup_info.cb=sizeof(STARTUPINFO); 
	stdio_startup_info.wShowWindow=SW_SHOWNOACTIVATE;
	stdio_startup_info.dwFlags=STARTF_USESHOWWINDOW;

    yarp::os::ConstString strAlias=msg.find("as").asString();
    strStdioPortUUID=mPortName+"/"+int2String(GetCurrentProcessId())+"/"+strAlias+"-"+int2String(mProcCNT++);
    yarp::os::ConstString strCommand=yarp::os::ConstString("yarp quiet readwrite ")+strStdioPortUUID+"/stdio:i "+strStdioPortUUID+"/stdio:o";

	BOOL bSuccess=CreateProcess(NULL,	// command name
								(char*)strCommand.c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								CREATE_NEW_CONSOLE, // creation flags 
								NULL,          // use parent's environment 
								NULL,          // use parent's current directory 
								&stdio_startup_info,   // STARTUPINFO pointer 
								&stdio_process_info);  // receives PROCESS_INFORMATION 
	
	yarp::os::ConstString out;

    if (bSuccess)
    {
        mStdioVector.Add(new YarpRunProcInfo(strAlias,
                                             mPortName,
                                             stdio_process_info.dwProcessId,
                                             stdio_process_info.hProcess,
                                             false));

        out=yarp::os::ConstString("STARTED: server=")+mPortName+" alias="+strAlias+" cmd=stdio pid="+int2String(stdio_process_info.dwProcessId)+"\n";
    }
    else
	{
        stdio_process_info.dwProcessId=YARPRUN_ERROR;
      
        out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdio\n"
           +yarp::os::ConstString("Can't open stdio window because ")+lastError2String()+"\n";
	}

    result.clear();
    result.addInt(stdio_process_info.dwProcessId);
    result.addString(out.c_str());
    fprintf(stderr,"%s",out.c_str());
    fflush(stderr);

	return stdio_process_info.dwProcessId;	
}

////////////////
#else // LINUX
////////////////

#define READ_FROM_PIPE 0
#define WRITE_TO_PIPE  1
#define REDIRECT_TO(from,to) dup2(to,from)

int CountArgs(char *str)
{
    int nargs=0;
   
    for (bool bSpace=true; *str; ++str)
    {
        if (bSpace)
        {
            if (*str!=' ')
            {
                ++nargs;
                bSpace=false;
            }
        }
        else
        {
            if (*str==' ')
            {
                bSpace=true;
            }
        }
    }
    
    return nargs;
}

void ParseCmd(char* cmd_str,char** arg_str)
{
    int nargs=0;
    
    for (bool bSpace=true; *cmd_str; ++cmd_str)
    {
        if (*cmd_str!=' ')
        {
            if (bSpace) arg_str[nargs++]=cmd_str;
            bSpace=false;
        }
        else
        {
            *cmd_str=0;
            bSpace=true;
        }
    }
}

void yarp::os::Run::CleanZombies()
{
	mProcessVector.CleanZombies();
	mStdioVector.CleanZombies();
}

//////////////////////////////////////////////////////////////////////////////////////////

yarp::os::Bottle yarp::os::Run::ExecuteCmdAndStdio(yarp::os::Bottle& msg)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strCmd=msg.find("cmd").asString();
    yarp::os::ConstString strStdio=msg.find("stdio").asString();
    yarp::os::ConstString strStdioUUID=msg.find("stdiouuid").asString();
    yarp::os::ConstString strCmdUUID=mPortName+"/"+int2String(getpid())+"/"+strAlias+"-"+int2String(mProcCNT++);

	int  pipe_stdin_to_cmd[2];
    int ret_stdin_to_cmd=0;
	ret_stdin_to_cmd=pipe(pipe_stdin_to_cmd);
    
	int  pipe_cmd_to_stdout[2];
	int  ret_cmd_to_stdout=pipe(pipe_cmd_to_stdout);
	
    int  pipe_child_to_parent[2];
	int  ret_child_to_parent=pipe(pipe_child_to_parent);
    
    if (ret_child_to_parent!=0 || ret_cmd_to_stdout!=0 || ret_stdin_to_cmd!=0)
    {
        int error=errno;
        
        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdout\n"
                                 +yarp::os::ConstString("Can't create pipes ")+strerror(error)+"\n";
        
        Bottle result;
        result.addInt(YARPRUN_ERROR);       
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);
        
        return result;        
    }

	int pid_stdout=fork();

	if (IS_INVALID(pid_stdout))
	{
	    int error=errno;
	    
		yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdout\n"
		                         +yarp::os::ConstString("Can't fork stdout process because ")+strerror(error)+"\n";
        
        Bottle result;
        result.addInt(YARPRUN_ERROR);		
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

		CLOSE(pipe_stdin_to_cmd[0]);
		CLOSE(pipe_stdin_to_cmd[1]);
		CLOSE(pipe_cmd_to_stdout[0]);
		CLOSE(pipe_cmd_to_stdout[1]);
		
		return result;
	}

	if (IS_NEW_PROCESS(pid_stdout)) // STDOUT IMPLEMENTED HERE
	{        
		REDIRECT_TO(STDIN_FILENO,pipe_cmd_to_stdout[READ_FROM_PIPE]);
		int ret=execlp("yarp","yarp","quiet","write",(strCmdUUID+"/stdout").c_str(),"verbatim",NULL);
	    
	    if (ret==YARPRUN_ERROR)
	    {
	        int error=errno;
	        
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdout\n"
                                     +yarp::os::ConstString("Can't execute stdout because ")+strerror(error)+"\n";
	        
	        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
	        fprintf(out_to_parent,"%s",out.c_str());
	        fflush(out_to_parent);
	        fclose(out_to_parent);
	        fprintf(stderr,"%s",out.c_str());
            fflush(stderr);
	    }
	    
		exit(ret);
	}

	if (IS_PARENT_OF(pid_stdout))
	{
	    fprintf(stderr,"STARTED: server=%s alias=%s cmd=stdout pid=%d\n",mPortName.c_str(),strAlias.c_str(),pid_stdout);
	    fflush(stderr);
	
		int pid_stdin=fork();

		if (IS_INVALID(pid_stdin))
		{		    
            int error=errno;
                        
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdin\n"
                                     +yarp::os::ConstString("Can't fork stdin process because ")+strerror(error)+"\n";

		    Bottle result;
            result.addInt(YARPRUN_ERROR);            
            result.addString(out.c_str());
	        fprintf(stderr,"%s",out.c_str());
		    fflush(stderr);
		    
		    SIGNAL(pid_stdout,SIGTERM,true);
			fprintf(stderr,"TERMINATING stdout (%d)\n",pid_stdout);
			fflush(stderr);
		    
			CLOSE(pipe_stdin_to_cmd[0]);
			CLOSE(pipe_stdin_to_cmd[1]);
			CLOSE(pipe_cmd_to_stdout[0]);
			CLOSE(pipe_cmd_to_stdout[1]);
		    
		    return result;
		}

		if (IS_NEW_PROCESS(pid_stdin)) // STDIN IMPLEMENTED HERE
		{            
			REDIRECT_TO(STDOUT_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);
			REDIRECT_TO(STDERR_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);
			int ret=execlp("yarp","yarp","quiet","read",(strCmdUUID+"/stdin").c_str(),NULL);
			
		    if (ret==YARPRUN_ERROR)
	        {
	            int error=errno;
	                
                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdin\n"
                                         +yarp::os::ConstString("Can't execute stdin because ")+strerror(error)+"\n";
                
                FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
	            fprintf(out_to_parent,"%s",out.c_str());
	            fflush(out_to_parent);
	            fclose(out_to_parent);
	            fprintf(stderr,"%s",out.c_str());
                fflush(stderr);
	        }
			
			exit(ret);
		}

		if (IS_PARENT_OF(pid_stdin))
		{
			// connect yarp read and write
			
			fprintf(stderr,"STARTED: server=%s alias=%s cmd=stdin pid=%d\n",mPortName.c_str(),strAlias.c_str(),pid_stdin);
			fflush(stderr);
			
			bool bConnR=false,bConnW=false;
		    for (int i=0; i<8 && !(bConnR&&bConnW); ++i)
		    { 
                if (!bConnW && NetworkBase::connect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str())) bConnW=true;
			  
                if (!bConnR && NetworkBase::connect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str())) bConnR=true;
		        
		        if (!bConnW || !bConnR) yarp::os::Time::delay(0.05);
		    }
		    
		    if (!(bConnR&&bConnW))
		    {
                if (bConnW) NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());
                    
                if (bConnR) NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
                
	            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=connect\nCan't connect stdio\n";
	           
	           	Bottle result;
				result.addInt(YARPRUN_ERROR);
	            result.addString(out.c_str());
	            fprintf(stderr,"%s",out.c_str());
	            fflush(stderr); 
	             
	            SIGNAL(pid_stdout,SIGTERM,true);
			    fprintf(stderr,"TERMINATING stdout (%d)\n",pid_stdout);
				SIGNAL(pid_stdin,SIGTERM,true);
				fprintf(stderr,"TERMINATING stdin (%d)\n",pid_stdin);
				fflush(stderr);
				
				CLOSE(pipe_stdin_to_cmd[0]);
				CLOSE(pipe_stdin_to_cmd[1]);
				CLOSE(pipe_cmd_to_stdout[0]);
				CLOSE(pipe_cmd_to_stdout[1]);
				
				return result;
		    }

			int pid_cmd=fork();

			if (IS_INVALID(pid_cmd))
			{				
                int error=errno;
		     	
		        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd="+strCmd+"\n"
                                         +yarp::os::ConstString("Can't fork command process because ")+strerror(error)+"\n";
	            
			    Bottle result;
                result.addInt(YARPRUN_ERROR);	            
	            result.addString(out.c_str());
	            fprintf(stderr,"%s",out.c_str());
	            fflush(stderr);

	            FILE* to_yarp_stdout=fdopen(pipe_cmd_to_stdout[WRITE_TO_PIPE],"w");
	            fprintf(to_yarp_stdout,"%s",out.c_str());
	            fflush(to_yarp_stdout);
	            fclose(to_yarp_stdout);

                NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());

			    SIGNAL(pid_stdout,SIGTERM,true);
			    fprintf(stderr,"TERMINATING stdout (%d)\n",pid_stdout);
				SIGNAL(pid_stdin,SIGTERM,true);
				fprintf(stderr,"TERMINATING stdin (%d)\n",pid_stdin);
				fflush(stderr);
				
				CLOSE(pipe_stdin_to_cmd[0]);
				CLOSE(pipe_stdin_to_cmd[1]);
				CLOSE(pipe_cmd_to_stdout[0]);
				CLOSE(pipe_cmd_to_stdout[1]);
		
		        return result;
			}

			if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
			{                        
                char *cmd_str=new char[strCmd.length()+1];
                strcpy(cmd_str,strCmd.c_str());
                int nargs=CountArgs(cmd_str);
                char **arg_str=new char*[nargs+1];
                ParseCmd(cmd_str,arg_str);
                arg_str[nargs]=0;
                
			    setvbuf(stdout,NULL,_IONBF,0);

				REDIRECT_TO(STDIN_FILENO, pipe_stdin_to_cmd[READ_FROM_PIPE]);
				REDIRECT_TO(STDOUT_FILENO,pipe_cmd_to_stdout[WRITE_TO_PIPE]);
				REDIRECT_TO(STDERR_FILENO,pipe_cmd_to_stdout[WRITE_TO_PIPE]);
	           
				if (msg.check("workdir"))
			    {
			        int ret=chdir(msg.find("workdir").asString().c_str());
                    if (ret!=0)
                    {
                        int error=errno;
                                            
                        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd="+strCmd+"\n"
                                                 +yarp::os::ConstString("Can't execute command, cannot set working directory")+strerror(error)+"\n";
                    
                        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                        fprintf(out_to_parent,"%s",out.c_str());
                        fflush(out_to_parent);
                        fclose(out_to_parent);
                        fprintf(stderr,"%s",out.c_str());
                        fflush(stderr);
                        
                        NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                        NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
 
                        exit(ret);
                    }
                }

				int ret=execvp(arg_str[0],arg_str);   

                fflush(stdout);
                fflush(stderr);

                if (ret==YARPRUN_ERROR)
	            {
	                int error=errno;
	                
                    yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd="+strCmd+"\n"
                                             +yarp::os::ConstString("Can't execute command because ")+strerror(error)+"\n";
                    
                	FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
	                fprintf(out_to_parent,"%s",out.c_str());
	                fflush(out_to_parent);
	                fclose(out_to_parent);
	                fprintf(stderr,"%s",out.c_str());
                    fflush(stderr);
                    
                    NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                    NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
	            }

        		delete [] cmd_str;
        		delete [] arg_str;

				exit(ret);
			}

			if (IS_PARENT_OF(pid_cmd))
			{
				mProcessVector.Add(
				    new YarpRunCmdWithStdioInfo(
				        strAlias,
				        mPortName,
				        strStdio,
				        pid_cmd,
				        strStdioUUID,
				        &mStdioVector,
					    pid_stdin,
					    pid_stdout,
					    pipe_stdin_to_cmd[READ_FROM_PIPE],
					    pipe_stdin_to_cmd[WRITE_TO_PIPE],
					    pipe_cmd_to_stdout[READ_FROM_PIPE],
					    pipe_cmd_to_stdout[WRITE_TO_PIPE],
					    NULL,
					    false
					)
				);
				
	            yarp::os::ConstString out;
	            Bottle result;
	            
	            FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
	            int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
	            fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK); 
                for (char buff[1024]; fgets(buff,1024,in_from_child);)
	            {
	                out+=yarp::os::ConstString(buff);
	            }
	            fclose(in_from_child);
	             
	            if (out.length()>0)
	            {
	                result.addInt(YARPRUN_ERROR);
	                NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                    NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
	            }
	            else
	            {	                
	                result.addInt(pid_cmd);

	                out=yarp::os::ConstString("STARTED: server=")+mPortName
                       +yarp::os::ConstString(" alias=")+strAlias
                       +yarp::os::ConstString(" cmd=")+strCmd
                       +yarp::os::ConstString(" pid=")+int2String(pid_cmd)
                       +yarp::os::ConstString("\n");
	            }
	            
	            result.addString(out.c_str());
	            fprintf(stderr,"%s",out.c_str());
                fflush(stderr);
  
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                
		        return result;
			}
		}
	}

    yarp::os::Bottle result;
    result.addInt(YARPRUN_ERROR);
    result.addString("I should never reach this point!!!\n");
	return result;
}

int yarp::os::Run::UserStdio(yarp::os::Bottle& msg,yarp::os::Bottle& result,yarp::os::ConstString& strStdioPortUUID)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    strStdioPortUUID=mPortName+"/"+int2String(getpid())+"/"+strAlias+"-"+int2String(mProcCNT++);

	int pipe_child_to_parent[2];
    int ret_pipe_child_to_parent;
	ret_pipe_child_to_parent=pipe(pipe_child_to_parent);
    
    if (ret_pipe_child_to_parent!=0)
    {
        int error=errno;
        
        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdio\n"
                                 +yarp::os::ConstString("Can't fork stdout cannot create pipe ")+strerror(error)+"\n";
        
        result.clear();
        result.addInt(YARPRUN_ERROR);       
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);
    
        return YARPRUN_ERROR;
    }

	int pid_cmd=fork();

	if (IS_INVALID(pid_cmd))
	{
		int error=errno;
	    
		yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=stdio\n"
                                 +yarp::os::ConstString("Can't fork stdout process because ")+strerror(error)+"\n";
        
        result.clear();
        result.addInt(YARPRUN_ERROR);		
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);
	
		return YARPRUN_ERROR;
	}

	if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
	{        
		int ret;
        yarp::os::ConstString strCmd=yarp::os::ConstString("/bin/bash -l -c \"yarp quiet readwrite ")+strStdioPortUUID+"/stdio:i "+strStdioPortUUID+"/stdio:o\"";
        const char *hold=msg.check("hold")?"-hold":"+hold";

        setvbuf(stdout,NULL,_IONBF,0);
        
        //REDIRECT_TO(STDOUT_FILENO,pipe_child_to_parent[WRITE_TO_PIPE]);
        REDIRECT_TO(STDERR_FILENO,pipe_child_to_parent[WRITE_TO_PIPE]);

		if (msg.check("geometry"))
		{
	        yarp::os::ConstString geometry(msg.find("geometry").asString());
			ret=execlp("xterm","xterm",hold,"-geometry",geometry.c_str(),"-title",strAlias.c_str(),"-e",strCmd.c_str(),NULL);
		}
		else
		{
			ret=execlp("xterm","xterm",hold,"-title",strAlias.c_str(),"-e",strCmd.c_str(),NULL);
		}
		
		fflush(stdout);
		fflush(stderr);
		
		if (ret==YARPRUN_ERROR)
		{
		    int error=errno;
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+strAlias+" cmd=xterm\n"
                                     +yarp::os::ConstString("Can't execute command because ")+strerror(error)+"\n";
	     
	        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
	        
	        fprintf(out_to_parent,"%s",out.c_str());
	        fflush(out_to_parent);
	        fclose(out_to_parent);
	        
	        fprintf(stdout,"%s",out.c_str());
            fflush(stdout);
		}
		
		exit(ret);
	}

	if (IS_PARENT_OF(pid_cmd))
	{
		mStdioVector.Add(new YarpRunProcInfo(strAlias,mPortName,pid_cmd,NULL,msg.check("hold")));
	    
	    result.clear();
	    yarp::os::ConstString out;
	    
	    //yarp::os::Time::delay(0.5);
	    
	    FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
	    int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
	    fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK); 
        for (char buff[1024]; fgets(buff,1024,in_from_child);)
	    {
	        out+=yarp::os::ConstString(buff);
	    }
	    fclose(in_from_child);
	    
	    int ret;
	    //if (out.length()>0)
	    if (out.substr(0,14)=="xterm Xt error" || out.substr(0,7)=="ABORTED")
	    {
	        result.addInt(ret=YARPRUN_ERROR);
	    }
	    else
	    {
	        result.addInt(ret=pid_cmd);
	        out=yarp::os::ConstString("STARTED: server=")+mPortName+" alias="+strAlias+" cmd=xterm pid="+int2String(pid_cmd)+"\n";
	    }
	    
	    fprintf(stderr,"%s",out.c_str());
        fflush(stderr);
	    
	    result.addString(out.c_str());
 
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                
		return ret;
	}
	
	result.clear();
    result.addInt(YARPRUN_ERROR);
	return YARPRUN_ERROR;
}

yarp::os::Bottle yarp::os::Run::ExecuteCmd(yarp::os::Bottle& msg)
{
	yarp::os::ConstString alias(msg.find("as").asString());
	yarp::os::ConstString commandString(msg.find("cmd").toString());

	int  pipe_child_to_parent[2];
	int ret_pipe_child_to_parent=pipe(pipe_child_to_parent);
    
    if (ret_pipe_child_to_parent!=0)
    {
        int error=errno;
        
        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+alias+" cmd=stdio\n"
                                 +yarp::os::ConstString("Can't create pipe ")+strerror(error)+"\n";
        
        Bottle result;
        result.addInt(YARPRUN_ERROR);       
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);
    
        return result;
    }
    
	int pid_cmd=fork();

	if (IS_INVALID(pid_cmd))
	{
	    int error=errno;
	    	     	
	    yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+alias+" cmd="+commandString+"\n"
                                 +yarp::os::ConstString("Can't fork command process because ")+strerror(error)+"\n";
	            
		Bottle result;
        result.addInt(YARPRUN_ERROR);	            
	    result.addString(out.c_str());
	    fprintf(stderr,"%s",out.c_str());
	    fflush(stderr);
		
		return result;
	}

	if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
	{
        int saved_stderr=dup(STDERR_FILENO);
		int null_file=open("/dev/null",O_WRONLY); 
        REDIRECT_TO(STDOUT_FILENO,null_file);
		REDIRECT_TO(STDERR_FILENO,null_file);
		close(null_file);
        
        char *cmd_str=new char[commandString.length()+1];
        strcpy(cmd_str,commandString.c_str());
        int nargs=CountArgs(cmd_str);
        char **arg_str=new char*[nargs+1];
        ParseCmd(cmd_str,arg_str);
        arg_str[nargs]=0;
        
        if (msg.check("workdir"))
		{
            int ret=chdir(msg.find("workdir").asString().c_str());
            if (ret!=0)
            {
                int error=errno;
                                            
                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+alias+" cmd="+commandString+"\n"
                                         +yarp::os::ConstString("Can't execute command, cannot set working directory")+strerror(error)+"\n";
                    

                FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                fprintf(out_to_parent,"%s",out.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);

                REDIRECT_TO(STDERR_FILENO,saved_stderr);
                fprintf(stderr,"%s",out.c_str());
                fflush(stderr);
            }
        }
        
		int ret=execvp(arg_str[0],arg_str);

        if (ret==YARPRUN_ERROR)
	    {
	        int error=errno;
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName+" alias="+alias+" cmd="+commandString+"\n"
                                     +yarp::os::ConstString("Can't execute command because ")+strerror(error)+"\n";
	     
	        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
	        fprintf(out_to_parent,"%s",out.c_str());
	        fflush(out_to_parent);
	        fclose(out_to_parent);

		    REDIRECT_TO(STDERR_FILENO,saved_stderr);
	        fprintf(stderr,"%s",out.c_str());
            fflush(stderr);
	    }

		delete [] cmd_str;
		delete [] arg_str;

		exit(ret);
	}

	if (IS_PARENT_OF(pid_cmd))
	{
		mProcessVector.Add(new YarpRunProcInfo(alias,mPortName,pid_cmd,NULL,false));
		
	    char pidstr[16];
	    sprintf(pidstr,"%d",pid_cmd);

        Bottle result;
        yarp::os::impl::String out;
         
        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
	    int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
	    fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK); 
        for (char buff[1024]; fgets(buff,1024,in_from_child);)
	    {
	        out+=yarp::os::impl::String(buff);
	    }
	    fclose(in_from_child);
	    
	    if (out.length()>0)
	    {
	        result.addInt(YARPRUN_ERROR);
	    }
	    else
	    {
	        //mProcessVector.Add(new YarpRunProcInfo(alias,mPortName,pid_cmd,false));
	        result.addInt(pid_cmd);
	        out=yarp::os::impl::String("STARTED: server=")+mPortName.c_str()+" alias="+alias.c_str()+" cmd="+commandString.c_str()+" pid="+pidstr+"\n";
	        fprintf(stderr,"%s",out.c_str());
            fflush(stderr);
	    }
	    
	    result.addString(out.c_str());
 
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                
		return result;
	}

	Bottle result;
    result.addInt(YARPRUN_ERROR);
	return result;
}

#endif

/////////////////////////////////////////////////////////////////
// API
/////////////////////////////////////////////////////////////////

int yarp::os::Run::start(const yarp::os::ConstString &node,Property &command,yarp::os::ConstString &keyv)
{
	yarp::os::Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	yarp::os::ConstString dest_srv=node;

	if (command.check("stdio"))
	{
		dest_srv=yarp::os::ConstString(command.find("stdio").asString());

		grp.clear(); 
		grp.addString("stdio");
		grp.addString(dest_srv.c_str());
		msg.addList()=grp;

		if (command.check("geometry"))
		{
			grp.clear();
			grp.addString("geometry");
			grp.addString(command.find("geometry").asString().c_str());
			msg.addList()=grp;
		}
		
		if (command.check("hold"))
		{
			grp.clear();
			grp.addString("hold");
			msg.addList()=grp;
		}
	}

	grp.clear();
	grp.addString("as");
	grp.addString(keyv.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("cmd");
	grp.addString(command.find("name").asString().c_str());
	grp.addString(command.find("parameters").asString().c_str());
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

    response=SendMsg(msg,dest_srv.c_str());

	char buff[16];
	sprintf(buff,"%d",response.get(0).asInt());
    keyv=yarp::os::ConstString(buff);

	return response.get(0).asInt()>0?0:YARPRUN_ERROR;
}

int yarp::os::Run::sigterm(const yarp::os::ConstString &node, const yarp::os::ConstString &keyv)
{
	yarp::os::Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("sigterm");
	grp.addString(keyv.c_str());
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

    response=SendMsg(msg,node.c_str());

	return response.get(0).asString()=="sigterm OK"?0:YARPRUN_ERROR;
}

int yarp::os::Run::sigterm(const yarp::os::ConstString &node)
{
	yarp::os::Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("sigtermall");
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

    response=SendMsg(msg,node.c_str());

	return response.get(0).asString()=="sigtermall OK"?0:YARPRUN_ERROR;
}

int yarp::os::Run::kill(const yarp::os::ConstString &node, const yarp::os::ConstString &keyv,int s)
{
	yarp::os::Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("kill");
	grp.addString(keyv.c_str());
	grp.addInt(s);
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

    response=SendMsg(msg,node.c_str());

	return response.get(0).asString()=="kill OK"?0:YARPRUN_ERROR;
}

bool yarp::os::Run::isRunning(const yarp::os::ConstString &node, yarp::os::ConstString &keyv)
{
	yarp::os::Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("isrunning");
	grp.addString(keyv.c_str());
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

	Port port;
    port.open("...");

    bool connected=attemptConnect(port.getName(), node);
    if (!connected)
    {
        port.close();
        return 0;
    }

    port.write(msg,response);
    NetworkBase::disconnect(port.getName().c_str(),node.c_str());
    port.close();

	if (!response.size()) return false;

	return response.get(0).asString()=="running";
}

// end API


#if defined(WIN32) || defined(WIN64)

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
