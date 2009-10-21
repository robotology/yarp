// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
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
HANDLE Run::hZombieHunter=NULL;
HANDLE* Run::aHandlesVector=NULL;
#ifndef __GNUC__
DWORD WINAPI ZombieHunter(__in  LPVOID lpParameter)
#else
DWORD WINAPI ZombieHunter(LPVOID lpParameter)
#endif
{
	DWORD nCount;

	while (true)
	{
		Run::GetHandles(Run::aHandlesVector,nCount);

		if (nCount)
		{
			WaitForMultipleObjects(nCount,Run::aHandlesVector,FALSE,INFINITE);
		}
		else
		{
			Run::hZombieHunter=NULL;
			return 0;
		}
	}

	return 0;
}
#else
#include <signal.h>
#define PID int
#define FDESC int
int CLOSE(int h){ return close(h)==0; }
int KILL(int pid,int signum=SIGTERM){ return kill(pid,signum)==0; }
void sigchild_handler(int sig)
{
    const int SZINC=16;
    int ret_status,child_pid;
    int nZombies=0,nLen=SZINC,*pZombies=new int[nLen];
    
    while ((child_pid=waitpid(WAIT_ANY,&ret_status,WNOHANG))>0)
    {
        if (nZombies>=nLen)
        {
            int *pNewZombies=new int[nLen+=SZINC];
            for (int z=0; z<nZombies; ++z) pNewZombies[z]=pZombies[z];
            delete [] pZombies;
            pZombies=pNewZombies;
        }
        
        pZombies[nZombies++]=child_pid;
        printf("SIGNALED %d\n",child_pid);
        fflush(stdout);
    }
    
    if (nZombies) Run::CleanZombies(pZombies,nZombies);
    
    delete [] pZombies;
}
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
	virtual ~YarpRunProcInfo(){ /*Kill(SIGKILL);*/ }

	virtual bool Match(String& alias){ return m_alias==alias; }

	virtual bool Kill(int signum=SIGTERM)
	{
		if (m_pid_cmd) return KILL(m_pid_cmd,signum); 
		       
		return true;
	}

    virtual void Clean()
    { 
        m_pid_cmd=0; 
    }

	virtual bool IsActive()
	{
	    if (!m_pid_cmd) return false;
	    
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

	int Size(){ return m_nProcesses; }

	bool Add(YarpRunProcInfo *process)
	{
	    //printf("Add mutex.wait()\n");
		mutex.wait();

		if (m_nProcesses>=MAX_PROCESSES)
		{
			fprintf(stderr,"ERROR: maximum process limit reached\n");
			//printf("Add mutex.post()\n");
			mutex.post();		
			return false;
		}

		#if defined(WIN32) || defined(WIN64)
		if (Run::hZombieHunter) TerminateThread(Run::hZombieHunter,0);
		#endif

		m_apList[m_nProcesses++]=process;

		#if defined(WIN32) || defined(WIN64)
		Run::hZombieHunter=CreateThread(0,0,ZombieHunter,0,0,0);
		#endif
        //printf("Add mutex.post()\n");
		mutex.post();
		
		return true;
	}

	bool Kill(String& alias,int signum=SIGTERM)
	{
	    //printf("Kill mutex.wait()\n");
	    //fflush(stdout);
		mutex.wait();

        YarpRunProcInfo *pKill=0;

		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apList[i] && m_apList[i]->Match(alias))
			{
				if (m_apList[i]->IsActive())
				{
				    pKill=m_apList[i];
				    break;
				}
			}
		}
		
		//printf("Kill mutex.post()\n");
		//fflush(stdout);
		mutex.post();

        if (pKill)
        {
            pKill->Kill(signum);
            return true;
        }
        
		return false;
	}

	void Killall(int signum=SIGTERM)
	{
	    //printf("Killall mutex.wait()\n");
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
		
	    //printf("Killall mutex.post()\n");
		mutex.post();
		
		for (int k=0; k<nKill; ++k)
		{
		    aKill[k]->Kill(signum);
		}
		
		delete [] aKill;
	}
	
	#if defined(WIN32) || defined(WIN64) 
	void GetHandles(HANDLE* &lpHandles,DWORD &nCount)
	{
	    //printf("GetHandles mutex.wait()\n");
		mutex.wait();

		for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
		{
			if (!m_apList[i]->IsActive())
			{
				printf("CLEAN-UP %d\n",m_apList[i]->m_ps_id);
				m_apList[i]->Clean();
				delete m_apList[i];
				m_apList[i]=0;
			}
		}

		Pack();
		
		for (int i=0; i<m_nProcesses; ++i)
			lpHandles[nCount+i]=m_apList[i]->m_pid_cmd;

		nCount+=m_nProcesses;

        //printf("GetHandles mutex.post()\n");
		mutex.post();
	}
	#else
	int CleanZombies(int *aZombies,int nZombies)
	{	
	    //printf("CleanZombies mutex.wait()\n");
	    //fflush(stdout);
		mutex.wait();

        YarpRunProcInfo **apClean=new YarpRunProcInfo*[nZombies];
        int nClean=0;

        for (int z=0; z<nZombies; ++z)
        {
		    for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
		    {
			    if (m_apList[i]->m_pid_cmd==aZombies[z])
			    {
			        aZombies[z]=-1;
			        apClean[nClean++]=m_apList[i];
				    m_apList[i]=0;
				    break;
			    }
			}
		}

		Pack();

        //printf("CleanZombies mutex.post()\n");
        //fflush(stdout);
		mutex.post();
		
		for (int z=0; z<nClean; ++z)
	    {
	        printf("CLEAN-UP %d\n",apClean[z]->m_pid_cmd);
	        fflush(stdout);
		    apClean[z]->Clean();
			delete apClean[z];
		}
		
		delete [] apClean;
		
		return nClean;
	}
	#endif

	Bottle PS()
	{
	    //printf("PS mutex.wait()\n");
		mutex.wait();

		Bottle ps,line,grp;

		for (int i=0; i<m_nProcesses; ++i) if (m_apList[i])
		{
			line.clear();

			grp.clear();
			grp.addString("pid");
			grp.addInt(m_apList[i]->m_ps_id);
			line.addList()=grp;

			grp.clear();
			grp.addString("tag"); 
			grp.addString(m_apList[i]->m_alias.c_str());
			line.addList()=grp;

			grp.clear();
			grp.addString("cmd");
			grp.addString(m_apList[i]->m_command.c_str());
			line.addList()=grp;

			grp.clear();
			grp.addString("status");
			grp.addString(m_apList[i]->IsActive()?"running":"zombie");
			line.addList()=grp;

			ps.addList()=line;
		}

        //printf("PS mutex.post()\n");
		mutex.post();

		return ps;
	}

	bool IsRunning(String &alias)
	{
	    //printf("IsRunning mutex.wait()\n");
		mutex.wait();

		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apList[i] && m_apList[i]->Match(alias))
			{
				if (m_apList[i]->IsActive())
				{
				    //printf("IsRunning mutex.post()\n");
					mutex.post();
					return true;
				}
				else
				{
				    //printf("IsRunning mutex.post()\n");
					mutex.post();
					return false;
				}
			}
		}
		
		//printf("IsRunning mutex.post()\n");
		mutex.post();

		return false;
	}

	yarp::os::Semaphore mutex;

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

	virtual ~YarpRunCmdWithStdioInfo(){ /*Kill(SIGKILL);*/ }
	
	virtual void Clean()
	{
		if (m_pid_stdin)  KILL(m_pid_stdin);
		if (m_pid_stdout) KILL(m_pid_stdout);
		m_pid_cmd=m_pid_stdin=m_pid_stdout=0;
		
		if (m_write_to_pipe_stdin_to_cmd)   CLOSE(m_write_to_pipe_stdin_to_cmd);
		if (m_read_from_pipe_stdin_to_cmd)  CLOSE(m_read_from_pipe_stdin_to_cmd);
		if (m_write_to_pipe_cmd_to_stdout)  CLOSE(m_write_to_pipe_cmd_to_stdout);
		if (m_read_from_pipe_cmd_to_stdout) CLOSE(m_read_from_pipe_cmd_to_stdout);
		m_write_to_pipe_stdin_to_cmd=m_read_from_pipe_stdin_to_cmd=0;
		m_write_to_pipe_cmd_to_stdout=m_read_from_pipe_cmd_to_stdout=0;

		TerminateRemoteStdio();
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
Port* Run::pServerPort=0;

// API

int Run::start(const String &node,Property &command,String &keyv)
{
	Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	String dest_srv=node;

	if (command.check("stdio"))
	{
		dest_srv=String(command.find("stdio").asString());

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

	Port port;
    port.open("...");
    Network::connect(port.getName().c_str(),dest_srv.c_str());
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),dest_srv.c_str());
    port.close();

	char buff[16];
	sprintf(buff,"%d",response.get(0).asInt());
	keyv=String(buff);
	return response.get(0).asInt()>0?0:-1;
}

int Run::sigterm(const String &node, const String &keyv)
{
	Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("sigterm");
	grp.addString(keyv.c_str());
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

	Port port;
    port.open("...");
    Network::connect(port.getName().c_str(),node.c_str());
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),node.c_str());
    port.close();

	return response.get(0).asString()=="sigterm OK"?0:-1;
}

int Run::sigterm(const String &node)
{
	Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("sigtermall");
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

	Port port;
    port.open("...");
    Network::connect(port.getName().c_str(),node.c_str());
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),node.c_str());
    port.close();

	return response.get(0).asString()=="sigtermall OK"?0:-1;
}

int Run::kill(const String &node, const String &keyv,int s)
{
	Bottle msg,grp,response;
	
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

	Port port;
    port.open("...");
    Network::connect(port.getName().c_str(),node.c_str());
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),node.c_str());
    port.close();

	return response.get(0).asString()=="kill OK"?0:-1;
}

int Run::ps(const String &node,std::list<std::string> &processes)
{
	Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(node.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("ps");
	msg.addList()=grp;

	printf(":: %s\n",msg.toString().c_str());

	Port port;
    port.open("...");
    Network::connect(port.getName().c_str(),node.c_str());
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),node.c_str());
    port.close();

	processes.clear();
	char buff[16];
	for (int i=0; i<response.size(); ++i)
	{
		sprintf(buff,"%d",response.get(i).find("pid").asInt());
		processes.push_back(std::string(buff));
	}

	return 0;
}

bool Run::isRunning(const String &node, String &keyv)
{
	Bottle msg,grp,response;
	
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
    Network::connect(port.getName().c_str(),node.c_str());
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),node.c_str());
    port.close();

	if (!response.size()) return false;

	return response.get(0).asString()=="running";
}

// end API

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

	#ifdef notdef // test API
	if (config.check("name"))
	{
		String keyv(config.find("tag").asString());

		Run::start(String(config.find("node").asString()),config,keyv);
		
		printf("KEYV=%s\n",keyv.c_str());

		return 0;
	}

	if (config.check("tkill"))
	{
		String keyv(config.findGroup("tkill").get(1).asString());
		int sig=config.findGroup("tkill").get(2).asInt();
		Run::kill(String(config.find("node").asString()),keyv,sig);
		return 0;
	}

	if (config.check("tsigterm"))
	{
		String keyv(config.find("tsigterm").asString());
		Run::sigterm(String(config.find("node").asString()),keyv);
		return 0;
	}

	if (config.check("tsigtermall"))
	{
		String keyv(config.find("tsigtermall").asString());
		Run::sigterm(String(config.find("node").asString()),keyv);
		return 0;
	}

	if (config.check("tps"))
	{
		std::list<std::string> processes;
		Run::ps(String(config.find("node").asString()),processes);
		std::list<std::string>::iterator iter;
		for (iter=processes.begin(); iter!=processes.end(); ++iter)
		{
			printf("%s\n",iter->c_str());
		}
		
		return 0;
	}

	if (config.check("isrunning"))
	{
		String keyv(config.find("isrunning").asString());
		bool isRunning=Run::isRunning(String(config.find("node").asString()),keyv);
		printf("%s is %s running\n",keyv.c_str(),isRunning?"":"NOT");
		return 0;
	}
	#endif //test API

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
	 || config.check("sigtermall") 
	 || config.check("exit") 
	 || config.check("isrunning")
	 || config.check("ps"))
	{ 
        return SendToServer(config);
    }

	Help();
    return 0;
}

Bottle Run::SendMsg(Bottle& msg,ConstString target)
{
	Port port;
    port.open("...");
    Network::connect(port.getName().c_str(),target.c_str());
	Bottle response;
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),target.c_str());
    port.close();
	
    int size=response.size();
	printf("RESPONSE:\n=========\n");
    for (int s=0; s<size; ++s)
        printf("%s\n",response.get(s).toString().c_str());
    printf("\n");

	return response;
}

void sigint_handler(int sig)
{
	if (Run::pServerPort)
	{
		Port *pClose=Run::pServerPort;
		Run::pServerPort=0;
		pClose->close();
	}
}

int Run::Server()
{
	Port port;
	port.open(m_PortName.c_str());
	pServerPort=&port;

	signal(SIGINT,sigint_handler);

	#if !defined(WIN32) && !defined(WIN64)
	signal(SIGCHLD,sigchild_handler); 
	#endif

    while (pServerPort) 
	{
		Bottle msg,output;
        port.read(msg,true);

		if (!pServerPort) break;

		// command with stdio management
		if (msg.check("stdio") && msg.check("cmd"))
		{
			ConstString stdio_port=msg.find("stdio").asString();
			ConstString on_port=msg.find("on").asString();

			// AM I THE CMD OR/AND STDIO SERVER?
			if (m_PortName==String(stdio_port)) // stdio
			{
				UserStdio(msg);

				//yarp::os::Time::delay(1.0);

				if (m_PortName==String(on_port))
				{
					// execute command here
					output.addInt(ExecuteCmdAndStdio(msg));
				}
				else
				{
					// execute command on cmd server
					output.addString(SendMsg(msg,on_port).toString());
				}
			}
			else if (m_PortName==String(on_port)) // cmd
			{
				output.addInt(ExecuteCmdAndStdio(msg));				
			}
			else // some error
				output.addString("SOME ERROR");

			port.reply(output);
			continue;
		}

		// without stdio
		if (msg.check("cmd"))
		{
			output.addInt(ExecuteCmd(msg));
			port.reply(output);
			continue;
		}

		if (msg.check("kill"))
		{
			String alias(msg.findGroup("kill").get(1).asString());
			int sig=msg.findGroup("kill").get(2).asInt();
			output.addString(m_ProcessVector.Kill(alias,sig)?"kill OK":"kill FAILED");
			port.reply(output);
			continue;
		}
		
	    if (msg.check("sigterm"))
		{
		    String alias(msg.find("sigterm").asString());
			output.addString(m_ProcessVector.Kill(alias)?"sigterm OK":"sigterm FAILED");
			port.reply(output);
			continue;
		}

		if (msg.check("sigtermall"))
		{
			m_ProcessVector.Killall();
			output.addString("sigtermall OK");
			port.reply(output);
			continue;
		}

		if (msg.check("ps"))
		{
			output.append(m_ProcessVector.PS());
			port.reply(output);
			continue;
		}

		if (msg.check("isrunning"))
		{
		    String alias(msg.find("isrunning").asString());
			output.addString(m_ProcessVector.IsRunning(alias)?"running":"not running");
			port.reply(output);
			continue;
		}

		if (msg.check("killstdio"))
		{
		    String alias(msg.find("killstdio").asString());
			m_StdioVector.Kill(alias);
			continue;
		}

		if (msg.check("exit"))
		{
			output.addString("exit OK");

			port.reply(output);
			port.close();
			pServerPort=0;
		}
	}

	Run::m_StdioVector.Killall();
	Run::m_ProcessVector.Killall();
	
	#if defined(WIN32) || defined(WIN64)
	//printf("Server mutex.wait()\n");
	Run::m_ProcessVector.mutex.wait();
	if (Run::hZombieHunter) TerminateThread(Run::hZombieHunter,0);
	//printf("Server mutex.post()\n");
	Run::m_ProcessVector.mutex.post();
	if (Run::aHandlesVector) delete [] Run::aHandlesVector;
	#endif

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
		if (config.find("stdio")=="") { Help("SYNTAX ERROR: missing remote stdio server\n"); return -1; }
		if (config.find("cmd")=="")   { Help("SYNTAX ERROR: missing command\n"); return -1; }
		if (!config.check("as") || config.find("as")=="") { Help("SYNTAX ERROR: missing tag\n"); return -1; }
		if (!config.check("on") || config.find("on")=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }

		msg.addList()=config.findGroup("stdio");
		msg.addList()=config.findGroup("cmd");
		msg.addList()=config.findGroup("as");
		msg.addList()=config.findGroup("on");
		
		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
		if (config.check("geometry")) msg.addList()=config.findGroup("geometry");

		Bottle response=SendMsg(msg,config.find("stdio").asString());
		if (!response.size()) return -1;
		return response.get(0).asInt()>0?0:2;
	}
	
	// DON'T USE A RUN SERVER TO MANAGE STDIO
	//
	// client -> cmd server
	//
	if (config.check("cmd"))
	{                
		if (config.find("cmd").asString()=="")   { Help("SYNTAX ERROR: missing command\n"); return -1; }
		if (!config.check("as") || config.find("as").asString()=="") { Help("SYNTAX ERROR: missing tag\n"); return -1; }
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }

		msg.addList()=config.findGroup("cmd");
		msg.addList()=config.findGroup("as");

		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");

		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return -1;
		return response.get(0).asInt()>0?0:2;
	}
	
	// client -> cmd server
	if (config.check("kill")) 
	{ 
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }
		if (config.findGroup("kill").get(1).asString()=="")  { Help("SYNTAX ERROR: missing tag\n"); return -1; }
		if (config.findGroup("kill").get(2).asInt()==0)	  { Help("SYNTAX ERROR: missing signum\n"); return -1; }

		msg.addList()=config.findGroup("kill");
		
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return -1;
		return response.get(0).asString()=="kill OK"?0:2;
	}

	// client -> cmd server
	if (config.check("sigterm")) 
	{ 
		if (config.find("sigterm").asString()=="") { Help("SYNTAX ERROR: missing tag"); return -1; }
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }
		
		msg.addList()=config.findGroup("sigterm");
		
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return -1;
		return response.get(0).asString()=="sigterm OK"?0:2;
	}

	// client -> cmd server
	if (config.check("sigtermall")) 
	{ 
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }
		
		msg.addList()=config.findGroup("sigtermall");
       
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return -1;
		return 0;
	}

	if (config.check("ps"))
	{
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }
		
		msg.addList()=config.findGroup("ps");
       
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return -1;
		return 0;
	}

	if (config.check("isrunning"))
	{
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }
		if (config.find("isrunning").asString()=="") { Help("SYNTAX ERROR: missing tag\n"); return -1; }

		msg.addList()=config.findGroup("isrunning");
		
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return -1;
		return response.get(0).asString()=="running"?0:2;
	}

	if (config.check("exit"))
	{
		if (!config.check("on") || config.find("on").asString()=="") { Help("SYNTAX ERROR: missing remote server\n"); return -1; }
		
		msg.addList()=config.findGroup("exit");
        
		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return -1;
		return 0;
	}

	return 0;
}

void Run::Help(const char *msg)
{
	fprintf(stderr,msg);
    fprintf(stderr,"\nUSAGE:\n\n");
    fprintf(stderr,"yarp run --server SERVERPORT\nrun a server on the local machine\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST] [--workdir WORKDIR]\nrun a command on SERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --stdio STDIOSERVERPORT [--geometry WxH+X+Y] --cmd COMMAND [ARGLIST] [--workdir WORKDIR]\n");
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

void Run::GetHandles(HANDLE* &lpHandles,DWORD &nCount)
{
	if (lpHandles)
	{
		delete [] lpHandles;
		lpHandles=0;
	}
	nCount=0;

	lpHandles=new HANDLE[m_ProcessVector.Size()+m_StdioVector.Size()];

	m_ProcessVector.GetHandles(lpHandles,nCount);
	m_StdioVector.GetHandles(lpHandles,nCount);
}

// CMD SERVER
int Run::ExecuteCmdAndStdio(Bottle& msg)
{
	String alias=msg.find("as").asString().c_str();

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
		return -1;
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
		return -1;
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
		
		return -1;
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

	bool bWorkdir=msg.check("workdir");
	String sWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

	bSuccess=CreateProcess(NULL,	// command name
								(char*)(sWorkdir+command_text).c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								0,             // creation flags 
								NULL, // use parent's environment 
								bWorkdir?sWorkdir.c_str():NULL, // working directory
								&cmd_startup_info,   // STARTUPINFO pointer 
								&cmd_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess && bWorkdir)
	{
			bSuccess=CreateProcess(NULL,	// command name
									(char*)command_text.c_str(), // command line 
									NULL,          // process security attributes 
									NULL,          // primary thread security attributes 
									TRUE,          // handles are inherited 
									0,             // creation flags 
									NULL,          // use parent's environment 
									sWorkdir.c_str(), // working directory 
									&cmd_startup_info,   // STARTUPINFO pointer 
									&cmd_process_info);  // receives PROCESS_INFORMATION 
	}

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
		
		return -1;
	}

	// EVERYTHING IS ALL RIGHT

	fprintf(stderr,"executed\n");
    String stdio = msg.find("stdio").asString().c_str();
	m_ProcessVector.Add(new YarpRunCmdWithStdioInfo(command_text,alias,m_PortName,
						cmd_process_info.hProcess,stdio,&m_StdioVector,
						stdin_process_info.hProcess,stdout_process_info.hProcess,
						read_from_pipe_stdin_to_cmd,write_to_pipe_stdin_to_cmd,
						read_from_pipe_cmd_to_stdout,write_to_pipe_cmd_to_stdout,
						cmd_process_info.dwProcessId));

	return cmd_process_info.dwProcessId;
}

int Run::ExecuteCmd(Bottle& msg)
{
	String alias=msg.find("as").asString().c_str();

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

	bool bWorkdir=msg.check("workdir");
	String sWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

	bool bSuccess=CreateProcess(NULL,	// command name
								(char*)(sWorkdir+command_text).c_str(), // command line 
								NULL,          // process security attributes 
								NULL,          // primary thread security attributes 
								TRUE,          // handles are inherited 
								0,             // creation flags 
								NULL,          // use parent's environment 
								bWorkdir?sWorkdir.c_str():NULL, // working directory 
								&cmd_startup_info,   // STARTUPINFO pointer 
								&cmd_process_info);  // receives PROCESS_INFORMATION 

	if (!bSuccess && bWorkdir)
	{
			bSuccess=CreateProcess(NULL,	// command name
									(char*)command_text.c_str(), // command line 
									NULL,          // process security attributes 
									NULL,          // primary thread security attributes 
									TRUE,          // handles are inherited 
									0,             // creation flags 
									NULL,          // use parent's environment 
									sWorkdir.c_str(), // working directory 
									&cmd_startup_info,   // STARTUPINFO pointer 
									&cmd_process_info);  // receives PROCESS_INFORMATION 
	}

	if (!bSuccess)
	{
		fprintf(stderr,"ERROR: can't fork cmd\n");
		
		return -1;
	}

	// EVERYTHING IS ALL RIGHT

	fprintf(stderr,"executed\n");
	m_ProcessVector.Add(new YarpRunProcInfo(command_text,alias,m_PortName,cmd_process_info.hProcess,cmd_process_info.dwProcessId));

	return cmd_process_info.dwProcessId;
}

// STDIO SERVER
int Run::UserStdio(Bottle& msg)
{
	String alias=msg.find("as").asString().c_str();

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
		
		return -1;
	}

	m_StdioVector.Add(new YarpRunProcInfo(command_line,alias,m_PortName,stdio_process_info.hProcess,stdio_process_info.dwProcessId));

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

void Run::CleanZombies(int* pZombies,int nZombies)
{
	if (m_ProcessVector.CleanZombies(pZombies,nZombies)<nZombies)
	{
	    m_StdioVector.CleanZombies(pZombies,nZombies);
	}
}

int Run::ExecuteCmdAndStdio(Bottle& msg)
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

		return -1;
	}

	if (IS_NEW_PROCESS(pid_stdout)) // STDOUT IMPLEMENTED HERE
	{
		REDIRECT_TO(STDIN_FILENO,pipe_cmd_to_stdout[READ_FROM_PIPE]);
		int ret=execlp("yarp","yarp","write",(String("/")+alias+"/"+"stdout").c_str(),NULL);
		exit(ret);
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
			
			return -1;
		}

		if (IS_NEW_PROCESS(pid_stdin)) // STDIN IMPLEMENTED HERE
		{
			REDIRECT_TO(STDOUT_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);
			REDIRECT_TO(STDERR_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);
			int ret=execlp("yarp","yarp","read",(String("/")+alias+"/"+"stdin").c_str(),NULL);
			exit(ret);
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
				
				return -1;
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
				
				return -1;
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

				int ret=execvp(arg_str[0],arg_str);

        		for (int s=0; s<command.size(); ++s)
        			delete [] arg_str[s];
        		delete [] arg_str;
        		delete [] Args;
        		delete [] cmd_str;		        

				exit(ret);
			}

			if (IS_PARENT_OF(pid_cmd))
			{
				fprintf(stderr,"executed\n");

                String cmd_str(msg.findGroup("cmd").toString());
                String stdio_str(msg.find("stdio").asString());

				m_ProcessVector.Add(new YarpRunCmdWithStdioInfo(cmd_str,alias,m_PortName,
						pid_cmd,stdio_str,&m_StdioVector,
						pid_stdin,pid_stdout,
						pipe_stdin_to_cmd[READ_FROM_PIPE],pipe_stdin_to_cmd[WRITE_TO_PIPE],
						pipe_cmd_to_stdout[READ_FROM_PIPE],pipe_cmd_to_stdout[WRITE_TO_PIPE],
						pid_cmd));

				return pid_cmd;
			}
		}
	}

	return -1;
}

int Run::ExecuteCmd(Bottle& msg)
{
	String alias(msg.find("as").asString());

	int pid_cmd=fork();

	if (IS_INVALID(pid_cmd))
	{
		fprintf(stderr,"ERROR: can't fork stdout\n");

		return -1;
	}

	if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
	{
		int null_file=open("/dev/null",O_WRONLY);

		REDIRECT_TO(STDOUT_FILENO,null_file);
		REDIRECT_TO(STDERR_FILENO,null_file);

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
        
		int ret=execvp(arg_str[0],arg_str);	
		
		for (int s=0; s<command.size(); ++s)
			delete [] arg_str[s];
		delete [] arg_str;
		delete [] Args;
		delete [] cmd_str;

		close(null_file);

		exit(ret);
	}

	if (IS_PARENT_OF(pid_cmd))
	{
		fprintf(stderr,"executed\n");

        String cmd_str(msg.findGroup("cmd").toString());
		m_ProcessVector.Add(new YarpRunProcInfo(cmd_str,alias,m_PortName,pid_cmd,pid_cmd));

		return pid_cmd;
	}

	return -1;
}

int Run::UserStdio(Bottle& msg)
{
	String alias(msg.find("as").asString());

	int pid_cmd=fork();

	if (IS_INVALID(pid_cmd))
	{
		fprintf(stderr,"ERROR: can't fork stdout\n");

		return -1;
	}

	if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
	{
		int ret;

		if (msg.check("geometry"))
		{
			String geometry(msg.find("geometry").asString());
			ret=execlp("xterm","xterm","-geometry",geometry.c_str(),"-title",alias.c_str(),"-e","yarp","readwrite",(String("/")+alias+"/user/stdout").c_str(),(String("/")+alias+"/user/stdin").c_str(),NULL);
		}
		else
		{
			ret=execlp("xterm","xterm","-title",alias.c_str(),"-e","yarp","readwrite",(String("/")+alias+"/user/stdout").c_str(),(String("/")+alias+"/user/stdin").c_str(),NULL);
		}
		
		exit(ret);
	}

	if (IS_PARENT_OF(pid_cmd))
	{
		fprintf(stderr,"executed\n");

        String rw_cmd(String("xterm -e yarp readwrite /")+alias+"/user/stdout /"+alias+"/user/stdin");
		m_StdioVector.Add(new YarpRunProcInfo(rw_cmd,alias,m_PortName,pid_cmd,pid_cmd));

		return pid_cmd;
	}

	return -1;
}

#endif


