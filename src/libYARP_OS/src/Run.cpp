// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 Robotcub Consortium
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 */

#include <string>
#include <iostream>

#include <yarp/os/Run.h>

#if defined(WIN32) || defined(WIN64)
#define SIGKILL 0
BOOL KILL(HANDLE pid,int signum=SIGTERM) 
{ 
    BOOL bRet=TerminateProcess(pid,0);
    CloseHandle(pid);
    return bRet;
}
#define PID HANDLE
#define FDESC HANDLE
#define CLOSE(h) CloseHandle(h)
#else
#include <signal.h>
#define PID int
#define FDESC int
int CLOSE(int h){ return close(h)==0; }
int KILL(int pid,int signum=SIGTERM){ return kill(pid,signum)==0; }
#endif

class YarpRunProcInfo
{
public:
	YarpRunProcInfo(String& command,String& alias,String& on,PID pid_cmd,int ps_id)
	{ 
		m_command=command;
		m_alias=alias;
		m_on=on;
		m_pid_cmd=pid_cmd;

		m_ps_id=ps_id;
	}
	virtual ~YarpRunProcInfo(){ Kill(SIGKILL); }

	virtual bool Match(String& alias){ return m_alias==alias; }
	
	virtual bool Kill(int signum=SIGTERM)
	{
		bool bRet=true;
		if (m_pid_cmd)
		{ 
		    bRet=KILL(m_pid_cmd,signum);
		}
		m_pid_cmd=0;
		return bRet;
	}

	virtual bool IsActive()
	{
		#if defined(WIN32) || defined(WIN64)
			DWORD status;
			return (::GetExitCodeProcess(m_pid_cmd,&status)&&status==STILL_ACTIVE);
		#else
			return kill(m_pid_cmd,0)==0;
		#endif
	}
	
protected:
	PID m_pid_cmd;
	String m_command,m_alias,m_on;

	// windows only
	int m_ps_id;

	friend class YarpRunInfoVector; 
};

class YarpRunInfoVector
{
public:
	YarpRunInfoVector()
	{
		m_nProcesses=0;
		
		for (int i=0; i<MAX_PROCESSES; ++i) 
			m_apList[i]=0;
	}

	~YarpRunInfoVector()
	{
		for (int i=0; i<MAX_PROCESSES; ++i) 
			if (m_apList[i]) delete m_apList[i];
	}

	bool Add(YarpRunProcInfo *process)
	{
		if (m_nProcesses>=MAX_PROCESSES)
		{
			fprintf(stderr,"ERROR: maximum process limit reached\n");
			return false;
		}
		m_apList[m_nProcesses++]=process;
		return true;
	}

	bool Kill(String& alias,int signum=SIGTERM)
	{
		bool bRet=false;

		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apList[i] && m_apList[i]->Match(alias))
			{
				if (m_apList[i]->Kill()) bRet=true;
				delete m_apList[i];
				m_apList[i]=0;
			}
		}
		
		if (bRet)
		{
			Pack();
			return true;
		}
		
		return false;
	}

	void Killall(int signum=SIGTERM)
	{
		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apList[i])
			{
				m_apList[i]->Kill(signum);
				delete m_apList[i];
				m_apList[i]=0;
			}
		}

		m_nProcesses=0;
	}
	
	Bottle PS()
	{
		Bottle ps;

		for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
		{
			ps.addString("PID: ");
			ps.addInt(m_apList[i]->m_ps_id);
			ps.addString(" ALIAS: "); 
			ps.addString(m_apList[i]->m_alias.c_str());
			ps.addString(" CMD: ");
			ps.addString(m_apList[i]->m_command.c_str());
			if (m_apList[i]->IsActive())
				ps.addString(" STATUS: running\n");
			else
				ps.addString(" STATUS: not responding\n");
		}

		return ps;
	}

protected:
	
	void Pack()
	{
		int tot=0;

		for (int i=0; i<m_nProcesses; ++i)
			if (m_apList[i]) m_apList[tot++]=m_apList[i];
		
		for (int i=tot; i<m_nProcesses; ++i)
			m_apList[i]=0;

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
	YarpRunCmdWithStdioInfo(String& command,String& alias,String& on,PID pid_cmd,String& stdio_server,YarpRunInfoVector* pStdioVector,
					   PID pid_stdin,PID pid_stdout,
		               FDESC read_from_pipe_stdin_to_cmd,FDESC write_to_pipe_stdin_to_cmd,
					   FDESC read_from_pipe_cmd_to_stdout,FDESC write_to_pipe_cmd_to_stdout,int ps_id)
		: YarpRunProcInfo(command,alias,on,pid_cmd,ps_id)
	{
		m_pid_stdin=pid_stdin;
		m_pid_stdout=pid_stdout;
		m_stdio=stdio_server;

		m_pStdioVector=pStdioVector;

		m_read_from_pipe_stdin_to_cmd=read_from_pipe_stdin_to_cmd;
		m_write_to_pipe_stdin_to_cmd=write_to_pipe_stdin_to_cmd;
		m_read_from_pipe_cmd_to_stdout=read_from_pipe_cmd_to_stdout;
		m_write_to_pipe_cmd_to_stdout=write_to_pipe_cmd_to_stdout;
	}

	virtual ~YarpRunCmdWithStdioInfo(){ Kill(SIGKILL); }

	virtual bool Kill(int signum=SIGTERM)
	{
		bool bRet=true;

		if (m_pid_cmd) bRet=bRet&&KILL(m_pid_cmd,signum);
		if (m_pid_stdin) bRet=bRet&&KILL(m_pid_stdin,signum);
		if (m_pid_stdin) bRet=bRet&&KILL(m_pid_stdout,signum);
		m_pid_cmd=m_pid_stdin=m_pid_stdout=0;

		if (m_write_to_pipe_stdin_to_cmd) bRet=bRet&&CLOSE(m_write_to_pipe_stdin_to_cmd);
		if (m_read_from_pipe_stdin_to_cmd) bRet=bRet&&CLOSE(m_read_from_pipe_stdin_to_cmd);
		if (m_write_to_pipe_cmd_to_stdout) bRet=bRet&&CLOSE(m_write_to_pipe_cmd_to_stdout);
		if (m_read_from_pipe_cmd_to_stdout) bRet=bRet&&CLOSE(m_read_from_pipe_cmd_to_stdout);
		m_write_to_pipe_stdin_to_cmd=m_read_from_pipe_stdin_to_cmd=0;
		m_write_to_pipe_cmd_to_stdout=m_read_from_pipe_cmd_to_stdout=0;

		TerminateRemoteStdio();

		return bRet;
	}

	void TerminateRemoteStdio()
	{
		if (m_on==m_stdio)
		{
			m_pStdioVector->Kill(m_alias);
		}
		else
		{
			Bottle msg;
			msg.fromString((String("(killstdio ")+m_alias+")").c_str());

			Port port;
			port.open("...");
			for (int i=0; i<20; ++i)
			{
			    if (Network::connect(port.getName().c_str(),m_stdio.c_str())) break;
			    yarp::os::Time::delay(1.0);
			}
			port.write(msg);
			Network::disconnect(port.getName().c_str(),m_stdio.c_str());
			port.close();
		}
	}

protected:
	PID m_pid_stdin,m_pid_stdout;
	String m_stdio;
	FDESC m_write_to_pipe_stdin_to_cmd,m_read_from_pipe_stdin_to_cmd;
	FDESC m_write_to_pipe_cmd_to_stdout,m_read_from_pipe_cmd_to_stdout;

	YarpRunInfoVector *m_pStdioVector;
};

///////////////////////////
// OS INDEPENDENT FUNCTIONS
///////////////////////////

YarpRunInfoVector Run::m_ProcessVector;
YarpRunInfoVector Run::m_StdioVector;
String Run::m_PortName;

int Run::main(int argc, char *argv[]) 
{
	m_PortName="";

    if (!Network::checkNetwork())
    {
		fprintf(stderr,"ERROR: no yarp network found.\n");
        return -1;
    }

    Property config;
    config.fromCommand(argc,argv,false);

	if (config.check("echo"))
	{
		std::string line;
		std::cout<<"Program echo starting...\n";
		while(true)
		{

			std::cin>>line;
			std::cout<<line<<"\n";
		}
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
		m_PortName=String(config.find("server").asString());
		return Server();
	} 

	// CLIENT (config is from keyboard)
	if (config.check("stdio")
	 || config.check("cmd") 
	 || config.check("kill") 
	 || config.check("sigterm")
	 || config.check("killall") 
	 || config.check("exit") 
	 || config.check("ps"))
	{ 
        return SendToServer(config);
    }

	Help();
    return 0;
}

void Run::SendMsg(Bottle& msg,ConstString target)
{
	Port port;
    port.open("...");
    Network::connect(port.getName().c_str(),target.c_str());
	Bottle response;
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),target.c_str());
    port.close();

    int size=response.size();
	printf("\nRESPONSE:\n=========\n");
    for (int s=0; s<size; ++s)
        printf("%s",response.get(s).toString().c_str());
    printf("\n");
}
/*
void sigchild_handler(int sig)
{
	printf("SIGCHLD received %d\n",sig);
}
*/
int Run::Server()
{
	Port port;
	port.open(m_PortName.c_str());

	bool bRun=true;

	// yarp read | command | yarp write

	#if !defined(WIN32) && !defined(WIN64)
	signal(SIGCHLD,SIG_IGN); // avoid zombies
	//signal(SIGCHLD,sigchild_handler);
	#endif

    while (bRun) 
	{
		Bottle msg,output;
        port.read(msg,true);

		// command with stdio management
		if (msg.check("stdio") && msg.check("cmd"))
		{
			ConstString stdio_port=msg.find("stdio").toString();
			ConstString on_port=msg.find("on").toString();

			// AM I THE CMD OR/AND STDIO SERVER?
			if (m_PortName==String(stdio_port)) // stdio
			{
				output.addString(UserStdio(msg)?"stdio OK":"stdio FAILED");
				//yarp::os::Time::delay(1.0);

				if (m_PortName==String(on_port))
				{
					// execute command here
					output.addString(ExecuteCmdAndStdio(msg)?"cmd OK":"cmd FAILED");
				}
				else
				{
					// execute command on cmd server
					SendMsg(msg,on_port);
				}
			}
			else if (m_PortName==String(on_port)) // cmd
				output.addString(ExecuteCmdAndStdio(msg)?"cmd OK":"cmd FAILED");
			else // some error
				output.addString("SOME ERROR");

			port.reply(output);
			continue;
		}

		// without stdio
		if (msg.check("cmd"))
		{
			output.addString(ExecuteCmd(msg)?"cmd OK":"cmd FAILED");
			port.reply(output);
			continue;
		}

		if (msg.check("kill"))
		{
		    String kill_cmd(msg.find("kill").toString());
			output.addString(m_ProcessVector.Kill(kill_cmd,SIGKILL)?"kill OK":"kill FAILED");
			port.reply(output);
			continue;
		}
		
	    if (msg.check("sigterm"))
		{
		    String kill_cmd(msg.find("sigterm").toString());
			output.addString(m_ProcessVector.Kill(kill_cmd)?"sigterm OK":"sigterm FAILED");
			port.reply(output);
			continue;
		}

		if (msg.check("killall"))
		{
			m_ProcessVector.Killall();
			output.addString("killall OK");
			port.reply(output);
			continue;
		}

		if (msg.check("ps"))
		{
			output.append(m_ProcessVector.PS());
			port.reply(output);
			continue;
		}

		if (msg.check("killstdio"))
		{
		    String killstdio_cmd(msg.find("killstdio").toString());
		    //printf("TERMINATION COMMAND %s",msg.find("killstdio").toString().c_str());
			m_StdioVector.Kill(killstdio_cmd);
			continue;
		}

		if (msg.check("exit"))
		{
			output.addString("exit OK");

			m_StdioVector.Killall();
			m_ProcessVector.Killall();
			port.reply(output);
			return 0;
		}
	}

	return 0;
}

// CLIENT
int Run::SendToServer(Property& config)
{
	Bottle msg;

	// USE A YARP RUN SERVER TO MANAGE STDIO
	//
	// client -> stdio server -> cmd server
	//
	if (config.check("cmd") && config.check("stdio"))
	{
		msg.addList()=config.findGroup("stdio");
		msg.addList()=config.findGroup("cmd");
		if (!config.check("as")) { Help(); return -1; }
		msg.addList()=config.findGroup("as");
		if (!config.check("on")) { Help(); return -1; }
		msg.addList()=config.findGroup("on");
		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");

		SendMsg(msg,config.find("stdio").asString());

		return 0;
	}
	
	// DON'T USE A RUN SERVER TO MANAGE STDIO
	//
	// client -> cmd server
	//
	if (config.check("cmd"))
	{                
		msg.addList()=config.findGroup("cmd");
		if (!config.check("as")) { Help(); return -1; }
		msg.addList()=config.findGroup("as");
		if (!config.check("on")) { Help(); return -1; }
		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");

		SendMsg(msg,config.find("on").asString());
		
		return 0;
	}
	
	// client -> cmd server
	if (config.check("kill")) 
	{ 
		msg.addList()=config.findGroup("kill");
        if (!config.check("on")) { Help(); return -1; }
		SendMsg(msg,config.find("on").asString());

		return 0;
	}

	// client -> cmd server
	if (config.check("sigterm")) 
	{ 
		msg.addList()=config.findGroup("sigterm");
        if (!config.check("on")) { Help(); return -1; }
		SendMsg(msg,config.find("on").asString());

		return 0;
	}

	// client -> cmd server
	if (config.check("killall")) 
	{ 
		msg.addList()=config.findGroup("killall");
        if (!config.check("on")) { Help(); return -1; }
		SendMsg(msg,config.find("on").asString());

		return 0;
	}

	if (config.check("ps"))
	{
		msg.addList()=config.findGroup("ps");
        if (!config.check("on")) { Help(); return -1; }
		SendMsg(msg,config.find("on").asString());

		return 0;
	}

	if (config.check("exit"))
	{
		msg.addList()=config.findGroup("exit");
        if (!config.check("on")) { Help(); return -1; }
		SendMsg(msg,config.find("on").asString());

		return 0;
	}

	return 0;
}

void Run::Help()
{
    fprintf(stderr,"USAGE:\n\n");
    fprintf(stderr,"yarp run --server SERVERPORT\nrun a server on the local machine\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST] [--workdir WORKDIR]\nrun a command on SERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --stdio STDIOSERVERPORT --cmd COMMAND [ARGLIST] [--workdir WORKDIR]\n");
    fprintf(stderr,"run a command on SERVERPORT server sending I/O to STDIOSERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --kill TAG\nkill TAG command\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --sigterm TAG\nterminate TAG command\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --ps\nreport commands running on SERVERPORT\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --exit\nstop SERVERPORT server\n\n");              
}

/////////////////////////
// OS DEPENDENT FUNCTIONS
/////////////////////////

// WINDOWS

#if defined(WIN32) || defined(WIN64)

// CMD SERVER
bool Run::ExecuteCmdAndStdio(Bottle& msg)
{
	String alias=msg.find("as").asString();

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
								(char*)(String("yarp write /")+alias+"/stdout").c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								0,             // creation flags 
								NULL,          // use parent's environment 
								NULL,          // use parent's current directory 
								&stdout_startup_info,   // STARTUPINFO pointer 
								&stdout_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess)
	{
		fprintf(stderr,"ERROR: can't fork stdout\n");
		
		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);
		return false;
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
								(char*)(String("yarp read /")+alias+"/stdin").c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								0,             // creation flags 
								NULL,          // use parent's environment 
								NULL,          // use parent's current directory 
								&stdin_startup_info,   // STARTUPINFO pointer 
								&stdin_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess)
	{
		fprintf(stderr,"ERROR: can't fork stdin\n");

		TerminateProcess(stdout_process_info.hProcess,-1);
		CloseHandle(stdout_process_info.hProcess);

		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);
		return false;
	}

	// connect yarp read and write
	bool bConnR=false,bConnW=false;
	for (int i=0; i<20 && !(bConnR&&bConnW); ++i)
	{ 
	    yarp::os::Time::delay(1.0);
			    
	    if (!bConnW && Network::connect((String("/")+alias+"/stdout").c_str(),(String("/")+alias+"/user/stdout").c_str()))
	        bConnW=true;
			        
	    if (!bConnR && Network::connect((String("/")+alias+"/user/stdin").c_str(),(String("/")+alias+"/stdin").c_str()))
	        bConnR=true;
	}        
		    
    if (!(bConnR&&bConnW))
    {
		fprintf(stderr,"ERROR: can't connect yarp ports\n");
		TerminateProcess(stdout_process_info.hProcess,-1);
		CloseHandle(stdout_process_info.hProcess);
		TerminateProcess(stdin_process_info.hProcess,-1);
		CloseHandle(stdin_process_info.hProcess);

		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);
		return false;
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
    String command_text;

	for (int s=0; s<command_bottle.size(); ++s)
		command_text+=String(command_bottle.get(s).toString().c_str())+" ";

	printf("\nSTARTING: %s\n\n",command_text.c_str());

	bSuccess=CreateProcess(NULL,	// command name
								(char*)command_text.c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								0,             // creation flags 
								NULL,          // use parent's environment 
								msg.check("workdir")?msg.find("workdir").asString().c_str():NULL, // working directory
								&cmd_startup_info,   // STARTUPINFO pointer 
								&cmd_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess)
	{
		fprintf(stderr,"ERROR: can't fork cmd\n");

		TerminateProcess(stdout_process_info.hProcess,-1);
		CloseHandle(stdout_process_info.hProcess);
		TerminateProcess(stdin_process_info.hProcess,-1);
		CloseHandle(stdin_process_info.hProcess);

		CloseHandle(write_to_pipe_stdin_to_cmd);
		CloseHandle(read_from_pipe_stdin_to_cmd);
		CloseHandle(write_to_pipe_cmd_to_stdout);
		CloseHandle(read_from_pipe_cmd_to_stdout);
		return false;
	}

	// EVERYTHING IS ALL RIGHT

	fprintf(stderr,"executed\n");
	m_ProcessVector.Add(new YarpRunCmdWithStdioInfo(command_text,alias,m_PortName,
						cmd_process_info.hProcess,String(msg.find("stdio").toString()),&m_StdioVector,
						stdin_process_info.hProcess,stdout_process_info.hProcess,
						read_from_pipe_stdin_to_cmd,write_to_pipe_stdin_to_cmd,
						read_from_pipe_cmd_to_stdout,write_to_pipe_cmd_to_stdout,
						cmd_process_info.dwProcessId));

	return true;
}

bool Run::ExecuteCmd(Bottle& msg)
{
	String alias=msg.find("as").asString();

	// RUN COMMAND
	PROCESS_INFORMATION cmd_process_info;
	ZeroMemory(&cmd_process_info,sizeof(PROCESS_INFORMATION));
	STARTUPINFO cmd_startup_info;
	ZeroMemory(&cmd_startup_info,sizeof(STARTUPINFO));

	cmd_startup_info.cb=sizeof(STARTUPINFO); 

	Bottle command_bottle=msg.findGroup("cmd").tail();
    String command_text;

	for (int s=0; s<command_bottle.size(); ++s)
		command_text+=String(command_bottle.get(s).toString().c_str())+" ";

	printf("\nSTARTING: %s\n\n",command_text.c_str());

	bool bSuccess=CreateProcess(NULL,	// command name
								(char*)command_text.c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								0,             // creation flags 
								NULL,          // use parent's environment 
								msg.check("workdir")?msg.find("workdir").asString().c_str():NULL, // working directory 
								&cmd_startup_info,   // STARTUPINFO pointer 
								&cmd_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess)
	{
		fprintf(stderr,"ERROR: can't fork cmd\n");
		return false;
	}

	// EVERYTHING IS ALL RIGHT

	fprintf(stderr,"executed\n");
	m_ProcessVector.Add(new YarpRunProcInfo(command_text,alias,m_PortName,cmd_process_info.hProcess,cmd_process_info.dwProcessId));

	return true;
}

// STDIO SERVER
bool Run::UserStdio(Bottle& msg)
{
	String alias=msg.find("as").asString();

	// create yarp read and yarp write client processes
	// RUN STDOUT
	PROCESS_INFORMATION stdio_process_info;
	ZeroMemory(&stdio_process_info,sizeof(PROCESS_INFORMATION));
	STARTUPINFO stdio_startup_info;
	ZeroMemory(&stdio_startup_info,sizeof(STARTUPINFO));

	stdio_startup_info.cb=sizeof(STARTUPINFO); 
	stdio_startup_info.wShowWindow=SW_SHOWNOACTIVATE;
	stdio_startup_info.dwFlags=STARTF_USESHOWWINDOW;

	String command_line=String("yarp readwrite /")+alias+"/user/stdout /"+alias+"/user/stdin";

	BOOL bSuccess=CreateProcess(NULL,	// command name
								(char*)command_line.c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								CREATE_NEW_CONSOLE, // creation flags 
								NULL,          // use parent's environment 
								NULL,          // use parent's current directory 
								&stdio_startup_info,   // STARTUPINFO pointer 
								&stdio_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess)
	{
		fprintf(stderr,"ERROR: can't fork stdio\n");
		
		return false;
	}

	m_StdioVector.Add(new YarpRunProcInfo(command_line,alias,m_PortName,stdio_process_info.hProcess,stdio_process_info.dwProcessId));

	return true;	
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

void ParseCmd(char* str,String* args)
{
    int nargs=0;
    
    for (bool bSpace=*str==' '; *str; ++str)
    {
        if (*str!=' ')
        {
            if (bSpace) ++nargs;
            bSpace=false;
            args[nargs]+=*str;
        }
        else
        {
            bSpace=true;
        }
    }
}

bool Run::ExecuteCmdAndStdio(Bottle& msg)
{
	String alias(msg.find("as").asString());

	int  pipe_stdin_to_cmd[2];
	pipe(pipe_stdin_to_cmd);
	int  pipe_cmd_to_stdout[2];
	pipe(pipe_cmd_to_stdout);

	int pid_stdout=fork();

	if (IS_INVALID(pid_stdout))
	{
		fprintf(stderr,"ERROR: can't fork stdout\n");
		CLOSE(pipe_stdin_to_cmd[0]);
		CLOSE(pipe_stdin_to_cmd[1]);
		CLOSE(pipe_cmd_to_stdout[0]);
		CLOSE(pipe_cmd_to_stdout[1]);
		return false;
	}

	if (IS_NEW_PROCESS(pid_stdout)) // STDOUT IMPLEMENTED HERE
	{
		REDIRECT_TO(STDIN_FILENO,pipe_cmd_to_stdout[READ_FROM_PIPE]);
		execlp("yarp","yarp","write",(String("/")+alias+"/"+"stdout").c_str(),NULL);
		exit(0);
	}

	if (IS_PARENT_OF(pid_stdout))
	{
		int pid_stdin=fork();

		if (IS_INVALID(pid_stdin))
		{
			fprintf(stderr,"ERROR: can't fork stdin\n");
			KILL(pid_stdout);
			CLOSE(pipe_stdin_to_cmd[0]);
			CLOSE(pipe_stdin_to_cmd[1]);
			CLOSE(pipe_cmd_to_stdout[0]);
			CLOSE(pipe_cmd_to_stdout[1]);
			return false;
		}

		if (IS_NEW_PROCESS(pid_stdin)) // STDIN IMPLEMENTED HERE
		{
			REDIRECT_TO(STDOUT_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);
			REDIRECT_TO(STDERR_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);
			execlp("yarp","yarp","read",(String("/")+alias+"/"+"stdin").c_str(),NULL);
			exit(0);
		}

		if (IS_PARENT_OF(pid_stdin))
		{
			// connect yarp read and write
			bool bConnR=false,bConnW=false;
		    for (int i=0; i<20 && !(bConnR&&bConnW); ++i)
		    { 
		        yarp::os::Time::delay(1.0);
			    
			    if (!bConnW && Network::connect((String("/")+alias+"/stdout").c_str(),(String("/")+alias+"/user/stdout").c_str()))
			        bConnW=true;
			        
		        if (!bConnR && Network::connect((String("/")+alias+"/user/stdin").c_str(),(String("/")+alias+"/stdin").c_str()))
		            bConnR=true;
		    }        
		    
		    if (!(bConnR&&bConnW))
		    {
				fprintf(stderr,"ERROR: can't connect yarp ports\n");
				KILL(pid_stdout);
				KILL(pid_stdin);
				CLOSE(pipe_stdin_to_cmd[0]);
				CLOSE(pipe_stdin_to_cmd[1]);
				CLOSE(pipe_cmd_to_stdout[0]);
				CLOSE(pipe_cmd_to_stdout[1]);
				return false;
		    }

			int pid_cmd=fork();

			if (IS_INVALID(pid_cmd))
			{
				fprintf(stderr,"ERROR: can't fork cmd\n");
				KILL(pid_stdout);
				KILL(pid_stdin);
				CLOSE(pipe_stdin_to_cmd[0]);
				CLOSE(pipe_stdin_to_cmd[1]);
				CLOSE(pipe_cmd_to_stdout[0]);
				CLOSE(pipe_cmd_to_stdout[1]);
				return false;
			}

			if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
			{
		        Bottle command=msg.findGroup("cmd");
		        command=command.tail();
        
                char *cmd_str=new char[command.get(0).toString().length()+1];
                strcpy(cmd_str,command.get(0).toString().c_str());
                int nargs=CountArgs(cmd_str);
                String *Args=new String[nargs];
                ParseCmd(cmd_str,Args);
        
		        char **arg_str=new char*[nargs+1];
		        for (int s=0; s<nargs; ++s)
		        {
		        	arg_str[s]=new char[Args[s].length()+1];
		        	strcpy(arg_str[s],Args[s].c_str());
		        }
		        arg_str[nargs]=0;
        
				REDIRECT_TO(STDIN_FILENO, pipe_stdin_to_cmd[READ_FROM_PIPE]);
				REDIRECT_TO(STDOUT_FILENO,pipe_cmd_to_stdout[WRITE_TO_PIPE]);
				REDIRECT_TO(STDERR_FILENO,pipe_cmd_to_stdout[WRITE_TO_PIPE]);
				
				if (msg.check("workdir"))
			    {
			        chdir(msg.find("workdir").asString().c_str());
			    }

				execvp(arg_str[0],arg_str);

        		for (int s=0; s<command.size(); ++s)
        			delete [] arg_str[s];
        		delete [] arg_str;
        		delete [] Args;
        		delete [] cmd_str;		        

				exit(0);
			}

			if (IS_PARENT_OF(pid_cmd))
			{
				fprintf(stderr,"executed\n");

                String cmd_str(msg.findGroup("cmd").toString());
                String stdio_str(msg.find("stdio").toString());

				m_ProcessVector.Add(new YarpRunCmdWithStdioInfo(cmd_str,alias,m_PortName,
						pid_cmd,stdio_str,&m_StdioVector,
						pid_stdin,pid_stdout,
						pipe_stdin_to_cmd[READ_FROM_PIPE],pipe_stdin_to_cmd[WRITE_TO_PIPE],
						pipe_cmd_to_stdout[READ_FROM_PIPE],pipe_cmd_to_stdout[WRITE_TO_PIPE],
						pid_cmd));

				return true;
			}
		}
	}

	return false;
}

bool Run::ExecuteCmd(Bottle& msg)
{
	String alias(msg.find("as").asString());

	int pid_cmd=fork();

	if (IS_INVALID(pid_cmd))
	{
		fprintf(stderr,"ERROR: can't fork stdout\n");

		return false;
	}

	if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
	{
		Bottle command=msg.findGroup("cmd");
		command=command.tail();
        
        char *cmd_str=new char[command.get(0).toString().length()+1];
        strcpy(cmd_str,command.get(0).toString().c_str());
        int nargs=CountArgs(cmd_str);
        String *Args=new String[nargs];
        ParseCmd(cmd_str,Args);
        
		char **arg_str=new char*[nargs+1];
		for (int s=0; s<nargs; ++s)
		{
			arg_str[s]=new char[Args[s].length()+1];
			strcpy(arg_str[s],Args[s].c_str());
		}
		arg_str[nargs]=0;
        
        if (msg.check("workdir"))
		{
		    chdir(msg.find("workdir").asString().c_str());
		}
        
		execvp(arg_str[0],arg_str);	
		
		for (int s=0; s<command.size(); ++s)
			delete [] arg_str[s];
		delete [] arg_str;
		delete [] Args;
		delete [] cmd_str;

		exit(0);
	}

	if (IS_PARENT_OF(pid_cmd))
	{
		fprintf(stderr,"executed\n");

        String cmd_str(msg.findGroup("cmd").toString());
		m_ProcessVector.Add(new YarpRunProcInfo(cmd_str,alias,m_PortName,pid_cmd,pid_cmd));

		return true;
	}

	return false;
}

bool Run::UserStdio(Bottle& msg)
{
	String alias(msg.find("as").asString());

	int pid_cmd=fork();

	if (IS_INVALID(pid_cmd))
	{
		fprintf(stderr,"ERROR: can't fork stdout\n");

		return false;
	}

	if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
	{
		execlp("xterm","xterm","-title",alias.c_str(),"-e","yarp","readwrite",(String("/")+alias+"/user/stdout").c_str(),(String("/")+alias+"/user/stdin").c_str(),NULL);

		exit(0);
	}

	if (IS_PARENT_OF(pid_cmd))
	{
		fprintf(stderr,"executed\n");

        String rw_cmd(String("xterm -e yarp readwrite /")+alias+"/user/stdout /"+alias+"/user/stdin");
		m_StdioVector.Add(new YarpRunProcInfo(rw_cmd,alias,m_PortName,pid_cmd,pid_cmd));

		return true;
	}

	return false;
}

#endif


