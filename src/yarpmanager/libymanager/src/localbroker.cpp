/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "localbroker.h"

#include <signal.h>
#include <string.h>

#define RUN_TIMEOUT             10.0        //seconds
#define STOP_TIMEOUT            30.0
#define KILL_TIMEOUT            10.0

#define WRITE_TO_PIPE           1
#define READ_FROM_PIPE          0

#if defined(WIN32) || defined(WIN64)
    #define SIGKILL 9
#else
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>

    #define PIPE_TIMEOUT    0
    #define PIPE_EVENT      1
    #define PIPE_SIGNALED   2
#endif 

using namespace yarp::os;

LocalBroker::LocalBroker()
{
    bOnlyConnector = bInitialized = false;  
    ID = 0;
    fd_stdout = NULL;
}


LocalBroker::~LocalBroker()
{
	fini();
}

void LocalBroker::fini(void)
{
	if(Thread::isRunning())
	    Thread::stop();
}

bool LocalBroker::init(void)
{
    /*
    if(!NetworkBase::checkNetwork(5.0))
    {
        strError = "Yarp network server is not up.";
        return false;
    }
    */
    bInitialized = true;
    bOnlyConnector = true;
    return true;
}

bool LocalBroker::init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv )
{

    strCmd.clear();
    strParam.clear();
    strHost.clear();
    strStdio.clear();
    strWorkdir.clear();
    strTag.clear();
    strEnv.clear();

    if(!szcmd)
    {
        strError = "command is not specified.";
        return false;
    }
    strCmd = szcmd; 
    if(szparam && strlen(szparam))
        strParam = szparam;

    if(szhost && strlen(szhost))
        strHost = szhost;
    if(szworkdir && strlen(szworkdir))
        strWorkdir = szworkdir;

    if(szstdio && strlen(szstdio))
    {
        if(szstdio[0] != '/')
            strStdio = string("/") + string(szstdio);
        else
            strStdio = szstdio;
    }
        
    if(szenv && strlen(szenv))
        strEnv = szenv;
    
    /*
    ostringstream sstrID;
    sstrID<<ID;
    strTag = strHost + strCmd + sstrID.str();

    if(!NetworkBase::checkNetwork(5.0))
    {
        strError = "Yarp network server is not up.";
        semParam.post();
        return false;
    }
    */

#if defined(WIN32) || defined(WIN64)
    // do nothing
    strError = "Local broker is currently supported only on Unix.";
    bInitialized = false;
    return false;
#else   
    /* avoiding zombie */
    struct sigaction new_action; 
    new_action.sa_handler = SIG_IGN;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGCHLD, &new_action, NULL);
    bInitialized = true;
    return true;
#endif

}


bool LocalBroker::start()
{   
    if(!bInitialized) return false;
    if(bOnlyConnector) return false; 

    if(running())
        return true;

    strError.clear();
    ID = ExecuteCmd();
    if(!ID)
        return false;

   if(running())
   {
        return true;
   }    
   return false;
}

bool LocalBroker::stop()
{
    if(!bInitialized) return true;
    if(bOnlyConnector) return false;

    stopStdout();

    strError.clear();
    stopCmd(ID);
    double base = Time::now();
    while(!timeout(base, STOP_TIMEOUT))
    {
        if(!running())
            return true;
    }

    strError = "Timeout! cannot stop ";
    strError += strCmd;
    strError += " on ";
    strError += strHost;
    return false;
}

bool LocalBroker::kill()
{
    if(!bInitialized) return true;
    if(bOnlyConnector) return false;
    
    stopStdout();

    strError.clear();
    killCmd(ID);
    double base = Time::now();
    while(!timeout(base, KILL_TIMEOUT))
    {
        if(!running())
            return true;
    }

    strError = "Timeout! cannot kill ";
    strError += strCmd;
    strError += " on ";
    strError += strHost;
    return false;   
}


bool LocalBroker::running(void)
{
    if(!bInitialized) return false;
    if(bOnlyConnector) return false;
    return psCmd(ID);
}


/**
 *  connecttion broker
 */ 
bool LocalBroker::connect(const char* from, const char* to, 
            const char* carrier)
{
    
    if(!from)
    {
        strError = "no source port is introduced.";
        return false;
    }

    if(!to)
    {
        strError = "no destination port is introduced.";
        return false;
    }

    if(!exists(from))
    {
        strError = from;
        strError += " does not exist.";
        return false;
    }

    if(!exists(to))
    {
        strError = to;
        strError += " does not exist.";
        return false;
    }
    
    NetworkBase::connect(from, to, carrier);
    if(!connected(from, to))
    {
        strError = "cannot connect ";
        strError +=from;
        strError += " to " + string(to);
        return false;
    }
    return true;     
}

bool LocalBroker::disconnect(const char* from, const char* to)
{
    
    if(!from)
    {
        strError = "no source port is introduced.";
        return false;
    }

    if(!to)
    {
        strError = "no destination port is introduced.";
        return false;
    }

    if(!exists(from))
    {
        strError = from;
        strError += " does not exist.";
        return true;
    }

    if(!exists(to))
    {
        strError = to;
        strError += " does not exist.";
        return true;
    }
    
    if(!connected(from, to))
        return true;

    if(!NetworkBase::disconnect(from, to))
    {
        strError = "cannot disconnect ";
        strError +=from;
        strError += " from " + string(to);      
        return false;
    }
    return true;
    
}

bool LocalBroker::exists(const char* port)
{
    return NetworkBase::exists(port);
}

bool LocalBroker::connected(const char* from, const char* to)
{
    if(!exists(from) || !exists(to))
        return false;
    return NetworkBase::isConnected(from, to);
}


const char* LocalBroker::error(void)
{
    return strError.c_str();
}

bool LocalBroker::attachStdout(void)
{
    if(Thread::isRunning())
        return true;
    if(!running())
    {
        strError = "Module is not running";
        return false;
    }
    return startStdout();
}

void LocalBroker::detachStdout(void)
{
   stopStdout();
}


bool LocalBroker::timeout(double base, double timeout)
{
    Time::delay(1.0);
    if((Time::now()-base) > timeout)
        return true;
    return false; 
}

bool LocalBroker::threadInit() 
{
   return true;
}


void LocalBroker::run() 
{

#if defined(WIN32) || defined(WIN64)
    //windows implementaion
#else
    while(!Thread::isStopping())
    {
        if(waitPipeSignal(pipe_to_stdout[READ_FROM_PIPE]) == PIPE_EVENT)
        {
           if(fd_stdout)
           {
                string strmsg;
                char buff[1024];
                while(fgets(buff, 1024, fd_stdout))
                    strmsg += string(buff);
                if(eventSink && strmsg.size())           
                    eventSink->onBrokerStdout(strmsg.c_str());
                yarp::os::Time::delay(1.0); // this prevents event flooding
           }
        }
    }
#endif
}


void LocalBroker::threadRelease()
{
}


void LocalBroker::ParseCmd(char* cmd_str,char** szarg)
{
    int nargs=0;
    for (bool bSpace=true; *cmd_str; ++cmd_str)
    {
        if (*cmd_str!=' ')
        {
            if (bSpace) szarg[nargs++]=cmd_str;
            bSpace=false;
        }
        else
        {
            *cmd_str=0;
            bSpace=true;
        }
    }
}


int LocalBroker::CountArgs(char *str)
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


#if defined(WIN32) || defined(WIN64)
bool LocalBroker::startStdout(void)
{
    //LocalStdout::Instance().registerBroker(this);
    return false;
}

void LocalBroker::stopStdout(void)
{ 
    //LocalStdout::Instance().unregisterBroker(this);
}

int LocalBroker::ExecuteCmd(void)
{
    return 0;
}

bool LocalBroker::psCmd(int pid)
{
    return false;
}

bool LocalBroker::killCmd(int pid)
{
    return false;
}

bool LocalBroker::stopCmd(int pid)
{
    return false;
}

#else   //for UNIX

bool LocalBroker::psCmd(int pid)
{
    if(!pid) 
        return false;
    return !::kill(pid, 0);
}


bool LocalBroker::killCmd(int pid)
{
    if(!pid) 
        return false;
    return !::kill(pid, SIGKILL);
}


bool LocalBroker::stopCmd(int pid)
{
    if(!pid) 
        return false;
   return !::kill(pid, SIGTERM);
}

int LocalBroker::waitPipe(int pipe_fd)
{
    struct timeval timeout;
    int rc;
    fd_set fd;

    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    FD_ZERO(&fd);
    FD_SET(pipe_fd, &fd);
    rc = select(pipe_fd + 1, &fd, NULL, NULL, &timeout);
    return rc;
}


int LocalBroker::waitPipeSignal(int pipe_fd)
{
    struct timespec timeout;
    fd_set fd;

    timeout.tv_sec = 2;
    timeout.tv_nsec = 0;
    FD_ZERO(&fd);
    FD_SET(pipe_fd, &fd);

    /*
#if (_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600) 
    struct sigaction new_action; 
    new_action.sa_handler = SIG_IGN;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGUSR1, &new_action, NULL);
    sigset_t sset, orgmask;
    sigemptyset(&sset);
    sigaddset(&sset, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &sset, &orgmask); 
    if(pselect(pipe_fd + 1, &fd, NULL, NULL, &timeout, &orgmask))
        return PIPE_EVENT;
#endif 
*/
    if(pselect(pipe_fd + 1, &fd, NULL, NULL, &timeout, NULL))
        return PIPE_EVENT;
    return PIPE_TIMEOUT;
}


bool LocalBroker::startStdout(void)
{
    fd_stdout = fdopen(pipe_to_stdout[READ_FROM_PIPE], "r");
    if(!fd_stdout)
    {
        close(pipe_to_stdout[READ_FROM_PIPE]);
        return false;
    }

    int oflags = fcntl(pipe_to_stdout[READ_FROM_PIPE], F_GETFL);
    fcntl(pipe_to_stdout[READ_FROM_PIPE], F_SETFL, oflags|O_NONBLOCK);
    
    Thread::start();
    return true;
}

void LocalBroker::stopStdout(void)
{ 
    //LocalStdout::Instance().unregisterBroker(this);
    Thread::stop();
    if(fd_stdout)
        fclose(fd_stdout);
    fd_stdout = NULL;
}



int LocalBroker::ExecuteCmd(void)
{
	int  pipe_child_to_parent[2];
	int ret = pipe(pipe_child_to_parent);    
    if (ret!=0)
    {
        strError = string("Can't create child pipe because") + string(strerror(errno));
        return 0;
    }
  
    ret = pipe(pipe_to_stdout);
    if (ret!=0)
    {
        strError = string("Can't create stdout pipe because") + string(strerror(errno));
        return 0;
    }
    
	int pid_cmd = fork();

	if(IS_INVALID(pid_cmd))
	{
	    strError = string("Can't fork command because ") + string(strerror(errno)); 
		return 0;
	}

	if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
	{
        close(pipe_child_to_parent[READ_FROM_PIPE]);
		//int saved_stderr = dup(STDERR_FILENO);
        dup2(pipe_to_stdout[WRITE_TO_PIPE], STDOUT_FILENO);
		dup2(pipe_to_stdout[WRITE_TO_PIPE], STDERR_FILENO);
        close(pipe_to_stdout[WRITE_TO_PIPE]);
        close(pipe_to_stdout[READ_FROM_PIPE]);

        strCmd = strCmd + string(" ") + strParam;
        char *szcmd = new char[strCmd.size()+1];
        strcpy(szcmd,strCmd.c_str());
        int nargs = CountArgs(szcmd);
        char **szarg = new char*[nargs+1];
        ParseCmd(szcmd, szarg);
        szarg[nargs]=0;
       
		if(strEnv.size())
		{
			char* szenv = new char[strEnv.size()+1];
			strcpy(szenv,strEnv.c_str()); 
            putenv(szenv);
			//delete szenv;
		}

		if(strWorkdir.size())
		{
            int ret = chdir(strWorkdir.c_str());
            if (ret!=0)
            {
                strError = string("Can't set working directory because ") + string(strerror(errno)); 
                FILE* out_to_parent = fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                fprintf(out_to_parent,"%s", strError.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);
                close(pipe_child_to_parent[WRITE_TO_PIPE]);
                delete [] szcmd;
                delete [] szarg;
                exit(ret);
            }
        }

        char currWorkDirBuff[1024];
        char *currWorkDir = getcwd(currWorkDirBuff,1024);

        ret = 0;
        if (currWorkDir)
        {
            char **cwd_szarg=new char*[nargs+1];
            for (int i=1; i<nargs; ++i) cwd_szarg[i]=szarg[i];
            cwd_szarg[nargs]=0;
            cwd_szarg[0]=new char[strlen(currWorkDir)+strlen(szarg[0])+16];

            strcpy(cwd_szarg[0],currWorkDir);
            strcat(cwd_szarg[0],"/");
            strcat(cwd_szarg[0],szarg[0]);	
            ret=execvp(cwd_szarg[0],cwd_szarg);  
            delete [] cwd_szarg[0];
            delete [] cwd_szarg;
        }
     
        if (ret==-1)
        {
		    ret=execvp(szarg[0],szarg);
        }

        if (ret==-1)
	    {
            strError = string("Can't execute command because ") + string(strerror(errno));
            FILE* out_to_parent = fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
            fprintf(out_to_parent,"%s", strError.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);
	    }
        close(pipe_child_to_parent[WRITE_TO_PIPE]);
		delete [] szcmd;
		delete [] szarg;
		exit(ret);
	}
	
	if (IS_PARENT_OF(pid_cmd))
	{
        close(pipe_child_to_parent[WRITE_TO_PIPE]);
        FILE* in_from_child = fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
	    int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
	    fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK); 
      
        string retError;
        waitPipe(pipe_child_to_parent[READ_FROM_PIPE]);

        for (char buff[1024]; fgets(buff,1024,in_from_child);)
	        retError += string(buff);
	    fclose(in_from_child);

	    if(retError.size())
	    {
            strError = retError;
            close(pipe_child_to_parent[READ_FROM_PIPE]);
            return 0;
	    }

        close(pipe_to_stdout[WRITE_TO_PIPE]);
        close(pipe_child_to_parent[READ_FROM_PIPE]);
        return pid_cmd;
	}

	return 0;
}

#endif

