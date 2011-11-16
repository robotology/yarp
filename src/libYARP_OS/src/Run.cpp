// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <signal.h>
#include <string>
#include <yarp/os/impl/RunReadWrite.h>
#include <yarp/os/impl/RunProcManager.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Run.h>

#ifndef YARP_HAS_ACE
#ifndef __APPLE__
#include <wait.h>
#else
#include <sys/wait.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#endif

#if defined(WIN32) || defined(WIN64)
    HANDLE  yarp::os::Run::hZombieHunter=NULL;
    HANDLE* yarp::os::Run::aHandlesVector=NULL;
#endif

#if defined(WIN32) || defined(WIN64)
inline yarp::os::ConstString lastError2String()
{
    int error=GetLastError();
    char buff[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,error,0,buff,1024,NULL);
    return yarp::os::ConstString(buff);
}
#endif

///////////////////////////
// OS INDEPENDENT FUNCTIONS
///////////////////////////

/*
void dontexit(int sig)
{
    signal(SIGINT,dontexit);
	signal(SIGTERM,dontexit);
}
*/

YarpRunInfoVector yarp::os::Run::mProcessVector;
YarpRunInfoVector yarp::os::Run::mStdioVector;
yarp::os::ConstString yarp::os::Run::mPortName;
yarp::os::Port* yarp::os::Run::pServerPort=0;
int yarp::os::Run::mProcCNT=0;
yarp::os::Semaphore yarp::os::Run::serializer(1);

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

    /*
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
    */

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
	 || config.check("ps")
	 || config.check("env"))
	{ 
        return sendToServer(config);
    }

    if (config.check("readwrite"))
    {
        yarp::os::ConstString uuid=config.findGroup("readwrite").get(1).asString();
        RunReadWrite rw;
        return rw.loop(uuid);
    }

    if (config.check("write"))
    {
        yarp::os::ConstString uuid=config.findGroup("write").get(1).asString();
        RunWrite w;
        return w.loop(uuid);
    }

    if (config.check("read"))
    {
        yarp::os::ConstString uuid=config.findGroup("read").get(1).asString();
        RunRead r;
        return r.loop(uuid);
    }

	Help();
    return 0;
}

yarp::os::Bottle yarp::os::Run::SendMsg(Bottle& msg,yarp::os::ConstString target)
{
	Port port;
    port.open("...");

    bool connected=yarp::os::NetworkBase::connect(port.getName(), target);

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
                
                serializer.wait();
                int pidStdio=UserStdio(msg,botStdioResult,strStdioUUID);
                serializer.post();

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
                        serializer.wait();
					    cmdResult=ExecuteCmdAndStdio(msg);
					    serializer.post();
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
			    serializer.wait();
				Bottle cmdResult=ExecuteCmdAndStdio(msg);
				serializer.post();
				
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
		    serializer.wait();			
            Bottle cmdResult=ExecuteCmd(msg);
            serializer.post();
            
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
	if (yarp::os::Run::hZombieHunter) TerminateThread(yarp::os::Run::hZombieHunter,0);
	Run::mProcessVector.mutex.post();
    if (yarp::os::Run::aHandlesVector) delete [] yarp::os::Run::aHandlesVector;
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
		if (config.check("env")) msg.addList()=config.findGroup("env");


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
		if (config.check("env")) msg.addList()=config.findGroup("env");

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
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\nrun a command on SERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --stdio STDIOSERVERPORT [--hold] [--geometry WxH+X+Y] --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\n");
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

#if defined(WIN32) || defined(WIN64)

// CMD SERVER
yarp::os::Bottle yarp::os::Run::ExecuteCmdAndStdio(Bottle& msg)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strStdio=msg.find("stdio").asString();
    yarp::os::ConstString strStdioUUID=msg.find("stdiouuid").asString();
    //yarp::os::ConstString strCmdUUID=mPortName+"/"+int2String(GetCurrentProcessId())+"/"+strAlias+"-"+int2String(mProcCNT++);

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
								(char*)(yarp::os::ConstString("yarprun --write ")+strStdioUUID).c_str(), // command line 
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
        yarp::os::ConstString strError=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +yarp::os::ConstString(" alias=")+strAlias
                                      +yarp::os::ConstString(" cmd=stdout\n")
                                      +yarp::os::ConstString("Can't execute stdout because ")+lastError2String()
                                      +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(strError.c_str());
        fprintf(stderr,"%s",strError.c_str());
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
                           (char*)(yarp::os::ConstString("yarprun --read ")+strStdioUUID).c_str(), // command line 
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
        yarp::os::ConstString strError=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +yarp::os::ConstString(" alias=")+strAlias
                                      +yarp::os::ConstString(" cmd=stdin\n")
                                      +yarp::os::ConstString("Can't execute stdin because ")+lastError2String()
                                      +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(strError.c_str());
        fprintf(stderr,"%s",strError.c_str());
        fflush(stderr);

		TerminateProcess(stdout_process_info.hProcess,YARPRUN_ERROR);
		CloseHandle(stdout_process_info.hProcess);

		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);

	    return result;
	}

    /*
	// connect yarp read and write
	bool bConnR=false,bConnW=false;
	for (int i=0; i<100 && !(bConnR&&bConnW); ++i)
	{ 	
        if (!bConnW && NetworkBase::connect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str())) bConnW=true;

        if (!bConnR && NetworkBase::connect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str())) bConnR=true;

	    if (!bConnW || !bConnR) yarp::os::Time::delay(0.05);
	}        
		    
    if (!(bConnR&&bConnW))
    {
	    Bottle result;
        result.addInt(YARPRUN_ERROR);

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=connect\nCan't connect stdio\n");
		
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
    */

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

	Bottle botCmd=msg.findGroup("cmd").tail();

    yarp::os::ConstString strCmd;
	for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+yarp::os::ConstString(" ");
    }

	
	if(msg.check("env"))
	{ 
		int pos = msg.find("env").asString().find("=");
		if(pos)
		{
			yarp::os::ConstString cstrName = msg.find("env").asString().substr(0,pos);
			int nValue = msg.find("env").asString().length() - 
						 cstrName.length() - 1;
			yarp::os::ConstString cstrValue = msg.find("env").asString().substr(pos+1,nValue);

			SetEnvironmentVariable(cstrName.c_str(), cstrValue.c_str());
		}
	}

	bool bWorkdir=msg.check("workdir");
	yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

	bSuccess=CreateProcess(NULL,	// command name
								(char*)(strWorkdir+strCmd).c_str(), // command line 
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
									(char*)(strCmd.c_str()), // command line 
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
        yarp::os::ConstString line1=yarp::os::ConstString("ABORTED: server=")+mPortName
                                   +yarp::os::ConstString(" alias=")+strAlias
                                   +yarp::os::ConstString(" cmd=")+strCmd
                                   +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                                   +yarp::os::ConstString("\n");

        WriteFile(write_to_pipe_cmd_to_stdout,line1.c_str(),line1.length(),&nBytes,0);
        
        yarp::os::ConstString line2=yarp::os::ConstString("Can't execute command because ")+lastError2String()+yarp::os::ConstString("\n");
        WriteFile(write_to_pipe_cmd_to_stdout,line1.c_str(),line2.length(),&nBytes,0);
        FlushFileBuffers(write_to_pipe_cmd_to_stdout);

        yarp::os::ConstString out=line1+line2;
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        //if (bConnW) NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());
		//if (bConnR) NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());

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
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName
                             +yarp::os::ConstString(" alias=")+strAlias
                             +yarp::os::ConstString(" cmd=")+strCmd
                             +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                             +yarp::os::ConstString("\n");

    result.addString(out.c_str());
    result.addString(strStdioUUID.c_str());
    fprintf(stderr,"%s",out.c_str());

	return result;
}

yarp::os::Bottle yarp::os::Run::ExecuteCmd(yarp::os::Bottle& msg)
{
	yarp::os::ConstString strAlias=msg.find("as").asString().c_str();

	// RUN COMMAND
	PROCESS_INFORMATION cmd_process_info;
	ZeroMemory(&cmd_process_info,sizeof(PROCESS_INFORMATION));
	STARTUPINFO cmd_startup_info;
	ZeroMemory(&cmd_startup_info,sizeof(STARTUPINFO));

	cmd_startup_info.cb=sizeof(STARTUPINFO); 

	Bottle botCmd=msg.findGroup("cmd").tail();
    
    yarp::os::ConstString strCmd;
	for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+yarp::os::ConstString(" ");
    }

	
	if(msg.check("env"))
	{
		
		int pos = msg.find("env").asString().find("=");
		if(pos)
		{
			yarp::os::ConstString cstrName = msg.find("env").asString().substr(0,pos);
			int nValue = msg.find("env").asString().length() - 
						 cstrName.length() - 1;
			yarp::os::ConstString cstrValue = msg.find("env").asString().substr(pos+1,nValue);

			SetEnvironmentVariable(cstrName.c_str(), cstrValue.c_str());
		}
		
	}
	

	bool bWorkdir=msg.check("workdir");
	yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

	BOOL bSuccess=CreateProcess(NULL,	// command name
								(char*)(strWorkdir+strCmd).c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								CREATE_NEW_PROCESS_GROUP, // creation flags 
								NULL,          // use parent's environment 
								bWorkdir?strWorkdir.c_str():NULL, // working directory 
								&cmd_startup_info,   // STARTUPINFO pointer 
								&cmd_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess && bWorkdir)
	{
			bSuccess=CreateProcess(NULL,	// command name
									(char*)(strCmd.c_str()), // command line 
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

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=")+strCmd
                                 +yarp::os::ConstString(" pid=")+int2String(cmd_process_info.dwProcessId)
                                 +yarp::os::ConstString("\nCan't execute command because ")+lastError2String()
                                 +yarp::os::ConstString("\n");

        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

	    return result;
	}

	// EVERYTHING IS ALL RIGHT

	mProcessVector.Add(new YarpRunProcInfo(strAlias,
                                           mPortName,
                                           cmd_process_info.dwProcessId,
                                           cmd_process_info.hProcess,
                                           false));

    Bottle result;
    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName
                             +yarp::os::ConstString(" alias=")+strAlias
                             +yarp::os::ConstString(" cmd=")+strCmd
                             +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                             +yarp::os::ConstString("\n");

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
    yarp::os::ConstString strCmd=yarp::os::ConstString("yarprun --readwrite ")+strStdioPortUUID;

	BOOL bSuccess=CreateProcess(NULL,	// command name
								(char*)strCmd.c_str(), // command line 
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

        out=yarp::os::ConstString("STARTED: server=")+mPortName
           +yarp::os::ConstString(" alias=")+strAlias
           +yarp::os::ConstString(" cmd=stdio pid=")+int2String(stdio_process_info.dwProcessId)
           +yarp::os::ConstString("\n");
    }
    else
	{
        stdio_process_info.dwProcessId=YARPRUN_ERROR;
      
        out=yarp::os::ConstString("ABORTED: server=")+mPortName
           +yarp::os::ConstString(" alias=")+strAlias
           +yarp::os::ConstString(" cmd=stdio\n")
           +yarp::os::ConstString("Can't open stdio window because ")+lastError2String()
           +yarp::os::ConstString("\n");
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
    //yarp::os::ConstString strCmdUUID=mPortName+"/"+int2String(getpid())+"/"+strAlias+"-"+int2String(mProcCNT++);

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
        
        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
                                 +yarp::os::ConstString("Can't create pipes ")+strerror(error)
                                 +yarp::os::ConstString("\n");
        
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
	    
		yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
		                         +yarp::os::ConstString("Can't fork stdout process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");
        
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
		int ret=execlp("yarprun","yarprun","--write",strStdioUUID.c_str(),(char*)NULL);
	    
	    if (ret==YARPRUN_ERROR)
	    {
	        int error=errno;
	        
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=stdout\n")
                                     +yarp::os::ConstString("Can't execute stdout because ")+strerror(error)
                                     +yarp::os::ConstString("\n");
	        
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
                        
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=stdin\n")
                                     +yarp::os::ConstString("Can't fork stdin process because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

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
			int ret=execlp("yarprun","yarprun","--read",strStdioUUID.c_str(),(char*)NULL);
			
		    if (ret==YARPRUN_ERROR)
	        {
	            int error=errno;
	                
                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=stdin\n")
                                         +yarp::os::ConstString("Can't execute stdin because ")+strerror(error)
                                         +yarp::os::ConstString("\n");
                
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
			
            /*
			bool bConnR=false,bConnW=false;
		    for (int i=0; i<100 && !(bConnR&&bConnW); ++i)
		    { 
                if (!bConnW && NetworkBase::connect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str())) bConnW=true;
			  
                if (!bConnR && NetworkBase::connect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str())) bConnR=true;
		        
		        if (!bConnW || !bConnR) yarp::os::Time::delay(0.05);
		    }
		    */

            /*
		    if (!(bConnR&&bConnW))
		    {
                if (bConnW) NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());
                    
                if (bConnR) NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
                
	            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=connect\nCan't connect stdio\n");
	           
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
            */

			int pid_cmd=fork();

			if (IS_INVALID(pid_cmd))
			{				
                int error=errno;
		     	
		        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=")+strCmd
                                         +yarp::os::ConstString("\nCan't fork command process because ")+strerror(error)
                                         +yarp::os::ConstString("\n");
	            
			    Bottle result;
                result.addInt(YARPRUN_ERROR);	            
	            result.addString(out.c_str());
	            fprintf(stderr,"%s",out.c_str());
	            fflush(stderr);

	            FILE* to_yarp_stdout=fdopen(pipe_cmd_to_stdout[WRITE_TO_PIPE],"w");
	            fprintf(to_yarp_stdout,"%s",out.c_str());
	            fflush(to_yarp_stdout);
	            fclose(to_yarp_stdout);

                //NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                //NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());

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
				
				if(msg.check("env"))
				{
					char* szenv = new char[msg.find("env").asString().length()+1];
					strcpy(szenv,msg.find("env").asString().c_str()); 
					putenv(szenv);
					//delete szenv;
				}
           
				if (msg.check("workdir"))
			    {
			        int ret=chdir(msg.find("workdir").asString().c_str());
                    if (ret!=0)
                    {
                        int error=errno;
                                            
                        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                                 +yarp::os::ConstString(" alias=")+strAlias
                                                 +yarp::os::ConstString(" cmd=")+strCmd
                                                 +yarp::os::ConstString("\nCan't execute command, cannot set working directory ")+strerror(error)
                                                 +yarp::os::ConstString("\n");
                    
                        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                        fprintf(out_to_parent,"%s",out.c_str());
                        fflush(out_to_parent);
                        fclose(out_to_parent);
                        fprintf(stderr,"%s",out.c_str());
                        fflush(stderr);
                        
                        //NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                        //NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
 
                        exit(ret);
                    }
                }

                int ret=YARPRUN_ERROR;

                char currWorkDirBuff[1024];
                char *currWorkDir=getcwd(currWorkDirBuff,1024);

                if (currWorkDir)
                {
                    char **cwd_arg_str=new char*[nargs+1];
                    for (int i=1; i<nargs; ++i) cwd_arg_str[i]=arg_str[i];
                    cwd_arg_str[nargs]=0;
                    cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

                    strcpy(cwd_arg_str[0],currWorkDir);
                    strcat(cwd_arg_str[0],"/");
                    strcat(cwd_arg_str[0],arg_str[0]);

                    ret=execvp(cwd_arg_str[0],cwd_arg_str);  

                    delete [] cwd_arg_str[0];
                    delete [] cwd_arg_str;
                }

                if (ret==YARPRUN_ERROR)
                {
				    ret=execvp(arg_str[0],arg_str);
                }

                fflush(stdout);
                fflush(stderr);

                if (ret==YARPRUN_ERROR)
	            {
	                int error=errno;
	                
                    yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                             +yarp::os::ConstString(" alias=")+strAlias
                                             +yarp::os::ConstString(" cmd=")+strCmd
                                             +yarp::os::ConstString("\nCan't execute command because ")+strerror(error)
                                             +yarp::os::ConstString("\n");
                    
                	FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
	                fprintf(out_to_parent,"%s",out.c_str());
	                fflush(out_to_parent);
	                fclose(out_to_parent);
	                fprintf(stderr,"%s",out.c_str());
                    fflush(stderr);
                    
                    //NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                    //NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
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
				
	            FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
	            int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
	            fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK); 
                
                yarp::os::ConstString out;
                for (char buff[1024]; fgets(buff,1024,in_from_child);)

	            {
	                out+=yarp::os::ConstString(buff);
	            }
	            fclose(in_from_child);
	             
	            Bottle result;
                if (out.length()>0)
	            {
	                result.addInt(YARPRUN_ERROR);
	                //NetworkBase::disconnect((strCmdUUID+"/stdout").c_str(),(strStdioUUID+"/stdio:i").c_str());    
                    //NetworkBase::disconnect((strStdioUUID+"/stdio:o").c_str(),(strCmdUUID+"/stdin").c_str());
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
                result.addString(strStdioUUID.c_str());

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

    if (pipe(pipe_child_to_parent))
    {
        int error=errno;
        
        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +yarp::os::ConstString("\n");
        
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
	    
		yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't fork stdout process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");
        
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
        yarp::os::ConstString strCmd=yarp::os::ConstString("/bin/bash -l -c \"yarprun --readwrite ")+strStdioPortUUID+"\"";
        const char *hold=msg.check("hold")?"-hold":"+hold";

        setvbuf(stdout,NULL,_IONBF,0);
        
        //REDIRECT_TO(STDOUT_FILENO,pipe_child_to_parent[WRITE_TO_PIPE]);
        REDIRECT_TO(STDERR_FILENO,pipe_child_to_parent[WRITE_TO_PIPE]);

		if (msg.check("geometry"))
		{
	        yarp::os::ConstString geometry(msg.find("geometry").asString());
			ret=execlp("xterm","xterm",hold,"-geometry",geometry.c_str(),"-title",strAlias.c_str(),"-e",strCmd.c_str(),(char*)NULL);
		}
		else
		{
			ret=execlp("xterm","xterm",hold,"-title",strAlias.c_str(),"-e",strCmd.c_str(),(char*)NULL);
		}
		
		fflush(stdout);
		fflush(stderr);
		
		if (ret==YARPRUN_ERROR)
		{
		    int error=errno;
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=xterm\nCan't execute command because ")+strerror(error)
                                     +yarp::os::ConstString("\n");
	     
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
	 
	    //yarp::os::Time::delay(0.5);
	    
	    FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
	    int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
	    fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK); 
        
        yarp::os::ConstString out;
        for (char buff[1024]; fgets(buff,1024,in_from_child);)
	    {
	        out+=yarp::os::ConstString(buff);
	    }
	    fclose(in_from_child);
	    
        result.clear();
	    int ret;
	    //if (out.length()>0)
	    if (out.substr(0,14)=="xterm Xt error" || out.substr(0,7)=="ABORTED")
	    {
	        result.addInt(ret=YARPRUN_ERROR);
	    }
	    else
	    {
	        result.addInt(ret=pid_cmd);
	        out=yarp::os::ConstString("STARTED: server=")+mPortName
               +yarp::os::ConstString(" alias=")+strAlias
               +yarp::os::ConstString(" cmd=xterm pid=")+int2String(pid_cmd)
               +yarp::os::ConstString("\n");
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

	yarp::os::ConstString strAlias(msg.find("as").asString());
	yarp::os::ConstString strCmd(msg.find("cmd").toString());

	int  pipe_child_to_parent[2];
	int ret_pipe_child_to_parent=pipe(pipe_child_to_parent);
    
    if (ret_pipe_child_to_parent!=0)
    {
        int error=errno;
        
        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +yarp::os::ConstString("\n");
        
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
	    	     	
	    yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=")+strCmd
                                 +yarp::os::ConstString("\nCan't fork command process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");
	            
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
        
        char *cmd_str=new char[strCmd.length()+1];
        strcpy(cmd_str,strCmd.c_str());
        int nargs=CountArgs(cmd_str);
        char **arg_str=new char*[nargs+1];
        ParseCmd(cmd_str,arg_str);
        arg_str[nargs]=0;
       
		if(msg.check("env"))
		{
			char* szenv = new char[msg.find("env").asString().length()+1];
			strcpy(szenv,msg.find("env").asString().c_str()); 
            putenv(szenv);
			//delete szenv;
		}

		if (msg.check("workdir"))
		{
            int ret=chdir(msg.find("workdir").asString().c_str());
            if (ret!=0)
            {
                int error=errno;
                                            
                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=")+strCmd
                                         +yarp::os::ConstString("\nCan't execute command, cannot set working directory ")+strerror(error)
                                         +yarp::os::ConstString("\n");

                FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                fprintf(out_to_parent,"%s",out.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);

                REDIRECT_TO(STDERR_FILENO,saved_stderr);
                fprintf(stderr,"%s",out.c_str());
                fflush(stderr);
            }
        }
        
        int ret=YARPRUN_ERROR;

        char currWorkDirBuff[1024];
        char *currWorkDir=getcwd(currWorkDirBuff,1024);

        if (currWorkDir)
        {
            char **cwd_arg_str=new char*[nargs+1];
            for (int i=1; i<nargs; ++i) cwd_arg_str[i]=arg_str[i];
            cwd_arg_str[nargs]=0;
            cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

            strcpy(cwd_arg_str[0],currWorkDir);
            strcat(cwd_arg_str[0],"/");
            strcat(cwd_arg_str[0],arg_str[0]);
        	
            ret=execvp(cwd_arg_str[0],cwd_arg_str);  

            delete [] cwd_arg_str[0];
            delete [] cwd_arg_str;
        }

        if (ret==YARPRUN_ERROR)
        {
		    ret=execvp(arg_str[0],arg_str);
        }

        if (ret==YARPRUN_ERROR)
	    {
	        int error=errno;
            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=")+strCmd
                                     +yarp::os::ConstString("\nCan't execute command because ")+strerror(error)
                                     +yarp::os::ConstString("\n");
	     
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
		mProcessVector.Add(new YarpRunProcInfo(strAlias,mPortName,pid_cmd,NULL,false));
		
	    char pidstr[16];
	    sprintf(pidstr,"%d",pid_cmd);
         
        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
	    int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
	    fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK); 
        
        yarp::os::ConstString out;
        for (char buff[1024]; fgets(buff,1024,in_from_child);)
	    {
	        out+=yarp::os::ConstString(buff);
	    }
	    fclose(in_from_child);
	    
        Bottle result;
	    if (out.length()>0)
	    {
	        result.addInt(YARPRUN_ERROR);
	    }
	    else
        {
	        result.addInt(pid_cmd);
	        out=yarp::os::ConstString("STARTED: server=")+mPortName
               +yarp::os::ConstString(" alias=")+strAlias
               +yarp::os::ConstString(" cmd=")+strCmd
               +yarp::os::ConstString(" pid=")+int2String(pid_cmd)
               +yarp::os::ConstString("\n");

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

    bool connected=yarp::os::NetworkBase::connect(port.getName(), node);
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
