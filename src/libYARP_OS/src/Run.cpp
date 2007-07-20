// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#define NEWVERSION
#ifdef NEWVERSION

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Process.h>
#include <ace/Vector_T.h>
#include <stdio.h>
#include <yarp/Companion.h>
#include <yarp/String.h>
#include <yarp/os/all.h>
#include <yarp/os/Run.h>

// does ACE require new c++ header files or not?
#if ACE_HAS_STANDARD_CPP_LIBRARY
#include <fstream>
using namespace std;
#else
#include <fstream.h>
#endif

#ifdef WIN32
BOOL WINAPI KillProcessEx(IN DWORD dwProcessId,IN BOOL bTree);
#endif

using namespace yarp;
using namespace yarp::os;

#define DBG if (true)

class TProcess
{
public:
	TProcess(String& workdir)
	{
		m_Workdir=workdir;
		m_Pid=-1;
		m_Serial=0;
		m_Name="default";
	}
	~TProcess()
	{
		Kill();
	}
	
	pid_t Spawn(ACE_Process_Options& options,String name="default",int serial=0,bool bWait=false)
	{
		m_Pid=m_Child.spawn(options);
		
		if (m_Pid!=-1)
		{
			m_Serial=serial;
			m_Name=name;
			if (bWait) m_Child.wait();
		}

        Time::delay(0.5);

		return m_Pid;
	}
	
	#ifndef ACE_LACKS_KILL
	void SendSigTree(pid_t parent_pid,int sig)
	{
	    pid_t* stack=new pid_t[4096];
	    
	    int head=1;
	    
	    stack[0]=parent_pid;
	    
	    char pidbuff[16],sigbuff[16];
	    sprintf(sigbuff,"%d ",sig);
		String Cmd("kill -");
		Cmd+=sigbuff;

	    while (head)
	    {
	        pid_t pid=stack[--head];
	        
	        sprintf(pidbuff,"%d",pid);
	        String filename(m_Workdir+"pgrep.txt");
	        
	        String pgrep_cmd=String("pgrep -P ")+pidbuff+" > "+filename;
	        DBG printf("%s\n",pgrep_cmd.c_str());
	        
	        FILE *pgrepsh=fopen((m_Workdir+"mypgrep.sh").c_str(),"wc");
	        fprintf(pgrepsh,"%s\n",pgrep_cmd.c_str());
	        fclose(pgrepsh);
	        
	        ACE_Process_Options pgrep_opt;
	        pgrep_opt.command_line("%s",("bash "+m_Workdir+"mypgrep.sh").c_str());
	        ACE_Process pgrep_proc;
	        pid_t pgrep_pid=pgrep_proc.spawn(pgrep_opt);
	        pgrep_proc.wait();
	        
	        if (pgrep_pid==-1) break;
	        
	        Time::delay(0.1);
	    
	        if (pid!=parent_pid)
	        {    
	            String kill_cmd=Cmd+pidbuff;
	            ACE_Process_Options kill_opt;
	            kill_opt.command_line("%s",kill_cmd.c_str());
	            ACE_Process kill_proc;
	            kill_proc.spawn(kill_opt);
	            kill_proc.wait();
	        }
	        
	        ifstream fpids(filename.c_str());
	        
            if (fpids.eof()||fpids.fail()) 
	        {        
	            printf("\n CANT OPEN FILE %s\n\n",filename.c_str());
		        continue;
            }

	        while (!(fpids.eof()||fpids.bad())) 
	        {
                fpids.getline(pidbuff,sizeof(pidbuff),'\n');
		        
		        int new_pid=atoi(pidbuff);
		        
		        if (new_pid>0) stack[head++]=new_pid;
            }
	    }
	    
	    delete [] stack;			
	}
	#endif

	void KillAllChilds(pid_t parent_pid)
	{
		#ifndef ACE_LACKS_KILL	
		SendSigTree(parent_pid,SIGTERM);
		m_Child.kill(SIGTERM);
		Time::delay(0.5);
		m_Child.wait();
		SendSigTree(parent_pid,SIGKILL);
		m_Child.kill(SIGKILL);
		Time::delay(0.5);
		m_Child.wait();
		#else
		#ifdef WIN32
		KillProcessEx(parent_pid,true);
		m_Child.terminate();
		m_Child.wait();
		Time::delay(0.5);
		#endif
		#endif
	}

	bool Kill()
	{
		if (m_Pid>0)
		{
		    KillAllChilds(m_Pid);
			m_Pid=-1;
			m_Serial=0;
			m_Name="default";
			
			return true;
		}

		return false;
	}

	int Serial(){ return m_Serial; }

	bool operator==(const String& name){ return name==m_Name; }
	bool operator==(pid_t pid){ return pid==m_Pid; }

protected:
    String m_Workdir;
	String m_Name;
	pid_t m_Pid;
	int m_Serial;
	ACE_Process m_Child;
};

class TProcessVector
{
public:
	TProcessVector(String& workdir)
	{
	    m_Workdir=workdir;
		m_nProcesses=0;
	}

	~TProcessVector()
	{
		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apChilds[i])
			{
				m_apChilds[i]->Kill();
				delete m_apChilds[i];
				m_apChilds[i]=0;
			}
		}
	}

	int Killall()
	{
		int killed=0;

		for (int i=m_nProcesses-1; i>=0; --i)
		{
			if (m_apChilds[i])
			{
				m_Mutex.wait();
				m_KillList[killed++]=m_apChilds[i]->Serial();
				m_apChilds[i]->Kill();
				m_Mutex.post();				
				delete m_apChilds[i];
				m_apChilds[i]=0;
			}
		}

		m_nProcesses=0;

		return killed;
	}

	int Kill(String name)
	{
		int killed=0;

		for (int i=m_nProcesses-1; i>=0; --i)
		{
			if (m_apChilds[i])
			{
				if (*m_apChilds[i]==name)
				{
					m_Mutex.wait();
					m_KillList[killed++]=m_apChilds[i]->Serial();
					m_apChilds[i]->Kill();
					m_Mutex.post();
					delete m_apChilds[i];
					m_apChilds[i]=0;
				}
			}
		}

		Pack();

		return killed;
	}

	pid_t Spawn(ACE_Process_Options& options,String name="default",int serial=0,bool bWait=false)
	{
		m_Mutex.wait();

		if (m_nProcesses>=MAX_PROCESSES)
		{
			m_Mutex.post();
			return -1;
		}

		TProcess* pChild=new TProcess(m_Workdir);
		
		m_apChilds[m_nProcesses]=pChild;
		pid_t pid=pChild->Spawn(options,name,serial,bWait);

		if (pid!=-1)
		{
			++m_nProcesses;
		}
		else
		{
			delete pChild;
			m_apChilds[m_nProcesses]=0;
		}

		m_Mutex.post();
		return pid;
	}

	int* GetKillList(){ return m_KillList; }

protected:
    String m_Workdir;
	enum { MAX_PROCESSES=1024 };
	TProcess* m_apChilds[MAX_PROCESSES];
	int m_KillList[MAX_PROCESSES];
	int m_nProcesses;
	Semaphore m_Mutex;

	void Pack()
	{
		int tot=0;

		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apChilds[i]) m_apChilds[tot++]=m_apChilds[i]; 
		}

		for (int i=tot; i<m_nProcesses; ++i)
			m_apChilds[i]=0;

		m_nProcesses=tot;
	}
};

static bool writeBottleAsFile(Bottle& bottle,String& file_name) 
{
	FILE* fout=fopen(file_name.c_str(),"wc");
    
	if (!fout) return false;

	for (int i=0; i<bottle.size(); ++i)
	{
		fprintf(fout,"%s\n",bottle.get(i).asString().c_str());
	}
	
	fflush(fout);
	fclose(fout);

    return true;
}

static Bottle readScriptAsBottle(const char *file_name) 
{
    Bottle bot;

	bot.addString("script");
	
    ifstream fin(file_name);
    if (fin.eof()||fin.fail()) 
	{
		bot.addString("");
        return bot;
    }
   
    char buf[25600];

	while (!(fin.eof()||fin.bad())) 
	{
        fin.getline(buf,sizeof(buf),'\n');
		bot.addString(buf);
    }

    return bot;
}

class Connector : public Thread 
{
private:
    ConstString in, out;
public:
    Connector(const char *in, const char *out) : in(in), out(out){}

    virtual void run() 
	{
        char buf[256];
        ACE_OS::sprintf(buf,"%s",in.c_str());
        char *targets[] = { "verbatim", buf };
        Companion::write("...",2,targets);
        //printf("\"Write\" side of connection finished.\n");
    }

    void main() 
	{
        start();
        Companion::read("...",out.c_str());
        //printf("\"Read\" side of connection finished.\n");
        stop();
    }
};

int Run::runServerBash(ConstString& portname,String& workdir) 
{
	#ifdef WIN32
	String workdir_dos=workdir;

	for (unsigned int i=0; i<workdir.length(); ++i)
	{
		if (workdir[i]=='/') workdir_dos[i]='\\';
	}
	#endif

	Port port;
	port.open(portname);

	TProcessVector pv(workdir);

	bool bRun=true;

	int serial=0;
	char intbuff[16];

    while (bRun) 
	{
		sprintf(intbuff,"%06d",++serial);
		String serial_str(intbuff);

        Bottle msg,output;
        port.read(msg,true);

		DBG printf("\nBOTTLE = %s\n\n",msg.toString().c_str());

		int result=-1;

		String alias=msg.check("as")?msg.findGroup("as").get(1).asString().c_str():"default";

		if (msg.check("script"))
		{
			Bottle script_bottle; 
			script_bottle.append(msg.findGroup("script").tail());	
			String script_name=workdir+"script"+serial_str+".sh";
			
			writeBottleAsFile(script_bottle,script_name);			
			
			String command_text="yarp read /"+alias+"/stdin | bash "+script_name+" | yarp write /"+alias+"/stdout";
	        
	        
	        String command_name=workdir+"command"+serial_str+".sh";
	        	
			FILE *command_file=fopen(command_name.c_str(),"wc");
			fprintf(command_file,"%s\n",command_text.c_str());
			fflush(command_file);
			fclose(command_file);		
			
			String exec_text="bash "+command_name;
			ACE_Process_Options exec_options;
			exec_options.command_line("%s",exec_text.c_str());

			pid_t pid=pv.Spawn(exec_options,alias,serial);
			
			DBG printf("\nSPAWN %d %s\n\n",pid,alias.c_str());
			
			if (pid!=-1) result=0;

			output.addInt(pid);
		}
		else if (msg.check("cmd"))
		{
			Bottle command_bottle=msg.findGroup("cmd");
			command_bottle=command_bottle.tail();

            String command_text;

			for (int s=0; s<command_bottle.size(); ++s)
				command_text+=String(command_bottle.get(s).toString().c_str())+" ";

			command_text="yarp read /"+alias+"/stdin | "+command_text+" | yarp write /"+alias+"/stdout";
	        

	        String command_name=workdir+"command"+serial_str+".sh";

			FILE *command_file=fopen(command_name.c_str(),"wc");
			fprintf(command_file,"%s\n",command_text.c_str());
			fflush(command_file);
			fclose(command_file);
			
			String exec_text="bash "+command_name;
			ACE_Process_Options exec_options;
			exec_options.command_line("%s",exec_text.c_str());

			pid_t pid=pv.Spawn(exec_options,alias,serial);
			
			DBG printf("\nSPAWN %d %s\n\n",pid,alias.c_str());
			
			if (pid!=-1) result=0;

			output.addInt(pid);	
		}
		else if (msg.check("kill"))
		{
			Bottle killer=msg.findGroup("kill");
			String key=killer.get(1).asString().c_str();
			
			int killed=pv.Kill(key);

			if (killed)
			{			
				result=0;

				int* pKillList=pv.GetKillList();
				char rmstr[256];
				
				for (int i=0; i<killed; ++i)
				{
					ACE_Process_Options rmfiles_opt;
					sprintf(rmstr,"rm -f %scommand%06d.sh %sscript%06d.sh",workdir.c_str(),pKillList[i],workdir.c_str(),pKillList[i]);
					DBG printf("%s\n",rmstr);
					rmfiles_opt.command_line("%s",rmstr);
					ACE_Process rmfiles_proc;
					rmfiles_proc.spawn(rmfiles_opt);
					rmfiles_proc.wait();
				}
			}
			else
			{
			    printf("can't kill process %s\n",key.c_str());
			}

			DBG printf("\nKILL %s\n\n",key.c_str());
		}
		else if (msg.check("killall"))
		{
			int killed=pv.Killall();

			if (killed)
			{			
				result=0;

				int* pKillList=pv.GetKillList();
				char rmstr[256];

				for (int i=0; i<killed; ++i)
				{
					ACE_Process_Options rmfiles_opt;
					sprintf(rmstr,"rm -f %scommand%06d.sh %sscript%06d.sh",workdir.c_str(),pKillList[i],workdir.c_str(),pKillList[i]);
					DBG printf("%s\n",rmstr);
					rmfiles_opt.command_line("%s",rmstr);
					ACE_Process rmfiles_proc;
					rmfiles_proc.spawn(rmfiles_opt);
					rmfiles_proc.wait();
				}
			}

			DBG printf("\nKILLALL\n\n");
		}
		else if (msg.check("exit"))
		{
		    pv.Killall();
		
			bRun=false;
			result=0;
			
			DBG printf("\nEXIT\n\n");
			
			ACE_Process_Options rmdir_opt;
			rmdir_opt.command_line("%s",("rm -rf "+workdir).c_str());
			ACE_Process rmdir_proc;
			rmdir_proc.spawn(rmdir_opt);
			rmdir_proc.wait();
		}

		Bottle reply;
        reply.addVocab(result?VOCAB4('f','a','i','l'):VOCAB2('o','k'));
        
		reply.append(output);
        port.reply(reply);

        //Time::delay(0.5); // something strange with ACE_Process::spawn
    }
 
	port.close();

	return 0;
}

#ifdef WIN32
int Run::runServerDos(ConstString& portname,String& workdir) 
{
	Port port;
	port.open(portname);

	TProcessVector pv(workdir);

	bool bRun=true;

	int serial=0;
	char intbuff[16];

	String workdir_dos=workdir;
	
	for (unsigned int i=0; i<workdir.length(); ++i)
	{
		if (workdir[i]=='/') workdir_dos[i]='\\';
	}
	
    while (bRun) 
	{
		sprintf(intbuff,"%06d",++serial);
		String serial_str(intbuff);

        Bottle msg,output;

        port.read(msg,true);

		DBG printf("\nBOTTLE = %s\n\n",msg.toString().c_str());

		int result=-1;

		String alias=msg.check("as")?msg.findGroup("as").get(1).asString().c_str():"default";

		if (msg.check("script"))
		{
			//output.addString("scripting not supported in DOS");

			Bottle script_bottle; 
			script_bottle.append(msg.findGroup("script").tail());	
			String script_name=workdir+"script"+serial_str+".bat";
			
			writeBottleAsFile(script_bottle,script_name);			
			String command_text="yarp read /"+alias+"/stdin | "+workdir_dos+"script"+serial_str+".bat | yarp write /"+alias+"/stdout";
	        
			Time::delay(0.5);
	        
	        String command_name=workdir+"cmd"+serial_str+".bat";
	        	
			FILE *command_file=fopen(command_name.c_str(),"wc");
			fprintf(command_file,"%s\n",command_text.c_str());
			fflush(command_file);
			fclose(command_file);		
			
			String exec_text=workdir_dos+"cmd"+serial_str+".bat";
			ACE_Process_Options exec_options;
			exec_options.command_line("%s",exec_text.c_str());

			pid_t pid=pv.Spawn(exec_options,alias,serial);
			
			DBG printf("\nSPAWN %d %s\n\n",pid,alias.c_str());
			
			if (pid!=-1) result=0;

			output.addInt(pid);
		}
		else if (msg.check("cmd"))
		{
			Bottle command_bottle=msg.findGroup("cmd");
			command_bottle=command_bottle.tail();

            String command_text;

			for (int s=0; s<command_bottle.size(); ++s)
				command_text+=String(command_bottle.get(s).toString().c_str())+" ";

			command_text="yarp read /"+alias+"/stdin | "+command_text+" | yarp write /"+alias+"/stdout";

	        String command_name=workdir+"cmd"+serial_str+".bat";

			FILE *command_file=fopen(command_name.c_str(),"wc");
			fprintf(command_file,"%s\n",command_text.c_str());
			fflush(command_file);
			fclose(command_file);
			
			String exec_text=workdir_dos+"cmd"+serial_str+".bat";
			ACE_Process_Options exec_options;
			exec_options.command_line("%s",exec_text.c_str());

			Time::delay(0.5);

			pid_t pid=pv.Spawn(exec_options,alias,serial);
			
			DBG printf("\nSPAWN %d %s\n\n",pid,alias.c_str());
			
			if (pid!=-1) result=0;

			output.addInt(pid);	
		}
		else if (msg.check("kill"))
		{
			Bottle killer=msg.findGroup("kill");
			String key=killer.get(1).asString().c_str();
			
			int killed=pv.Kill(key);
			
			if (killed)
			{ 
			    result=0;
				
				/*
				int* pKillList=pv.GetKillList();
				char delcommand[256];

				for (int i=0; i<killed; ++i)
				{
					ACE_Process_Options rmfiles_opt;
					sprintf(delcommand,"del /Q %scmd%06d.bat %sscript%06d.bat",workdir_dos.c_str(),pKillList[i],workdir_dos.c_str(),pKillList[i]);
					DBG printf("%s\n",delcommand);
					rmfiles_opt.command_line("%s",delcommand);
					ACE_Process rmfiles_proc;
					rmfiles_proc.spawn(rmfiles_opt);
					rmfiles_proc.wait();
					//Time::delay(3.0);
					//rmfiles_proc.terminate();
				}
				*/
			}
			else
			{
			    printf("can't kill process %s\n",key.c_str());
			}

			DBG printf("\nKILL %s\n\n",key.c_str());
		}
		else if (msg.check("killall"))
		{
			int killed=pv.Killall();

			if (killed)
			{			
				result=0;
				/* doesn't work
				int* pKillList=pv.GetKillList();
				
				char delcommand[256];

				for (int i=0; i<killed; ++i)
				{
					ACE_Process_Options rmfiles_opt;
					sprintf(delcommand,"del /Q %scmd%06d.bat %sscript%06d.bat",workdir_dos.c_str(),pKillList[i],workdir_dos.c_str(),pKillList[i]);
					DBG printf("%s\n",delcommand);
					rmfiles_opt.command_line("%s",delcommand);
					ACE_Process rmfiles_proc;
					rmfiles_proc.spawn(rmfiles_opt);
					rmfiles_proc.wait();
					//Time::delay(3.0);
					//rmfiles_proc.terminate();
				}
				*/
			}

			DBG printf("\nKILLALL\n\n");
		}
		else if (msg.check("exit"))
		{
			pv.Killall();
			bRun=false;
			result=0;

			DBG printf("\nEXIT\n\n");
			/* doesn't work
			ACE_Process_Options rmdir_opt;
			rmdir_opt.command_line("%s",("cmd rmdir /S /Q "+workdir_dos).c_str());
			ACE_Process rmdir_proc;
			rmdir_proc.spawn(rmdir_opt);
			//rmdir_proc.wait();
			Time::delay(3.0);
			rmdir_proc.terminate();
			*/
		}

		String s;

		Bottle reply;
        reply.addVocab(result?VOCAB4('f','a','i','l'):VOCAB2('o','k'));
        
		reply.append(output);
        port.reply(reply);

        //Time::delay(0.5); // something strange with ACE_Process::spawn
    }

	port.close();

	return 0;
}
#endif

int Run::runConnect(Searchable& config) 
{
    ConstString context=config.check("connect",Value("default")).toString();
    String r=String("/");
	r+=context;
	r+="/stdin";
    String w=String("/");
	w+=context;
	w+="/stdout";
    
	printf("*** Connecting to %s: writing to %s, reading from %s\n",
           context.c_str(), r.c_str(), w.c_str());
    
	Connector conn(r.c_str(),w.c_str());
    conn.main();
    
	return 0;
}

int Run::runClient(Searchable& config)
{
	DBG printf("\nCONFIG: %s\n\n",config.toString().c_str());

	Bottle msg;

	if (config.check("script"))
	{
		String fname=config.check("script",Value("")).toString().c_str();
		if (fname!="")
		{
			Bottle script=readScriptAsBottle(fname.c_str());
			msg.addList()=script;
		}
		
		if (config.check("as"))
			msg.addList()=config.findGroup("as");
    }
	else if (config.check("cmd"))
	{                
		msg.addList()=config.findGroup("cmd");

		if (config.check("as"))
			msg.addList()=config.findGroup("as");

		DBG printf("\nCMD: %s\n\n",msg.toString().c_str());
	}
	else if (config.check("kill")) 
	{ 
		msg.addList()=config.findGroup("kill");
	}
	else if (config.check("killall")) 
	{ 
		msg.addList()=config.findGroup("killall");
	}	
	else if (config.check("exit")) 
	{
        msg.addList()=config.findGroup("exit");
	}
	else return -1;

    Port port;
    port.open("...");
	ConstString target=config.check("on",Value("/run")).asString();
    Network::connect(port.getName().c_str(),target.c_str());
	Bottle response;
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),target.c_str());
    port.close();

	printf("\nRESPONSE = %s\n\n",response.toString().c_str());

	return 0;
}

bool Run::checkBash(String& workdir)
{
	FILE* fCheck=fopen((workdir+"checkbash.sh").c_str(),"wc");
	fprintf(fCheck,"printf \"hello I am bash\n\"");
	fclose(fCheck);

	ACE_Process_Options options;
	options.command_line("%s",("bash "+workdir+"checkbash.sh").c_str());
	ACE_Process proc;
	pid_t pid=proc.spawn(options);
	proc.wait();
	return pid!=-1;
}

int Run::main(int argc, char *argv[]) 
{
    Property config;

    config.fromCommand(argc,argv,false);

    if (config.check("server")) 
	{
		ConstString portname=config.check("server",Value("/run"),"port name for server").asString();

		char temp_dir_path[1024];
		if (ACE::get_temp_dir(temp_dir_path,1024)==-1)
		{
			temp_dir_path[0]=0;
			printf("WARNING: no temp directory found, using Local.\n");
		}

		String workdir=String(temp_dir_path)+"run";

		for (unsigned int i=0; i<workdir.length(); ++i)
			if (workdir[i]=='\\') workdir[i]='/';

		ACE_OS::mkdir(workdir.c_str());

		workdir+=String(portname.c_str())+"/";

		for (unsigned int i=0; i<workdir.length(); ++i)
			if (workdir[i]=='\\') workdir[i]='/';

		ACE_OS::mkdir(workdir.c_str());

		DBG printf("WORKDIR=%s\n",workdir.c_str());
		
		int ret=-1;

		if (checkBash(workdir))
		{
			printf("bash server found\n");
			ret=runServerBash(portname,workdir);
		}
		#ifdef WIN32
		else
		{
			printf("bash not available, running DOS server\n");
			ret=runServerDos(portname,workdir);
		}
		#else
		else
		{
			printf("ERROR: no bash, no dos, who am I???\n");
		}
		#endif
		
		return ret;
	} 
	if (config.check("connect")) 
	{
        return runConnect(config);
    } 
	else if (config.check("cmd") || config.check("kill") || config.check("killall") || config.check("exit") || config.check("script"))
	{ 
        return runClient(config);
    }
	else return -1;

    return 0;
}

#ifdef WIN32
#define _WIN32_WINNT 0x500

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include <stdio.h>
#include <stdarg.h>
#include <tlhelp32.h>

//---------------------------------------------------------------------------
// KillProcess
//
//  Terminates the specified process.
//
//  Parameters:
//	  dwProcessId - identifier of the process to terminate
//
//  Returns:
//	  TRUE, if successful, FALSE - otherwise.
//
BOOL
WINAPI
KillProcess(
	IN DWORD dwProcessId
	)
{
	HANDLE hProcess;
	DWORD dwError;

	// first try to obtain handle to the process without the use of any
	// additional privileges
	hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		if (GetLastError() != ERROR_ACCESS_DENIED)
			return FALSE;

		OSVERSIONINFO osvi;

		// determine operating system version
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		GetVersionEx(&osvi);

		// we cannot do anything else if this is not Windows NT
		if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
			return SetLastError(ERROR_ACCESS_DENIED), FALSE;

		// enable SE_DEBUG_NAME privilege and try again

		TOKEN_PRIVILEGES Priv, PrivOld;
		DWORD cbPriv = sizeof(PrivOld);
		HANDLE hToken;

		// obtain the token of the current thread 
		if (!OpenThreadToken(GetCurrentThread(), 
							 TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES,
							 FALSE, &hToken))
		{
			if (GetLastError() != ERROR_NO_TOKEN)
				return FALSE;

			// revert to the process token
			if (!OpenProcessToken(GetCurrentProcess(),
								  TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES,
								  &hToken))
				return FALSE;
		}

		_ASSERTE(ANYSIZE_ARRAY > 0);

		Priv.PrivilegeCount = 1;
		Priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Priv.Privileges[0].Luid);

		// try to enable the privilege
		if (!AdjustTokenPrivileges(hToken, FALSE, &Priv, sizeof(Priv),
								   &PrivOld, &cbPriv))
		{
			dwError = GetLastError();
			CloseHandle(hToken);
			return SetLastError(dwError), FALSE;
		}

		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		{
			// the SE_DEBUG_NAME privilege is not present in the caller's
			// token
			CloseHandle(hToken);
			return SetLastError(ERROR_ACCESS_DENIED), FALSE;
		}

		// try to open process handle again
		hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
		dwError = GetLastError();
		
		// restore the original state of the privilege
		AdjustTokenPrivileges(hToken, FALSE, &PrivOld, sizeof(PrivOld),
							  NULL, NULL);
		CloseHandle(hToken);

		if (hProcess == NULL)
			return SetLastError(FALSE), NULL;
	}

	// terminate the process
	if (!TerminateProcess(hProcess, (UINT)-1))
	{
		dwError = GetLastError();
		CloseHandle(hProcess);
		return SetLastError(dwError), FALSE;
	}

	CloseHandle(hProcess);

	// completed successfully
	return TRUE;
}

typedef LONG	NTSTATUS;
typedef LONG	KPRIORITY;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)

#define SystemProcessesAndThreadsInformation	5

typedef struct _CLIENT_ID {
    DWORD	    UniqueProcess;
    DWORD	    UniqueThread;
} CLIENT_ID;

typedef struct _UNICODE_STRING {
    USHORT	    Length;
    USHORT	    MaximumLength;
    PWSTR	    Buffer;
} UNICODE_STRING;

typedef struct _VM_COUNTERS {
    SIZE_T	    PeakVirtualSize;
    SIZE_T	    VirtualSize;
    ULONG	    PageFaultCount;
    SIZE_T	    PeakWorkingSetSize;
    SIZE_T	    WorkingSetSize;
    SIZE_T	    QuotaPeakPagedPoolUsage;
    SIZE_T	    QuotaPagedPoolUsage;
    SIZE_T	    QuotaPeakNonPagedPoolUsage;
    SIZE_T	    QuotaNonPagedPoolUsage;
    SIZE_T	    PagefileUsage;
    SIZE_T	    PeakPagefileUsage;
} VM_COUNTERS;

typedef struct _SYSTEM_THREADS {
    LARGE_INTEGER   KernelTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   CreateTime;
    ULONG			WaitTime;
    PVOID			StartAddress;
    CLIENT_ID	    ClientId;
    KPRIORITY	    Priority;
    KPRIORITY	    BasePriority;
    ULONG			ContextSwitchCount;
    LONG			State;
    LONG			WaitReason;
} SYSTEM_THREADS, * PSYSTEM_THREADS;

// Note that the size of the SYSTEM_PROCESSES structure is different on
// NT 4 and Win2K, but we don't care about it, since we don't access neither
// IoCounters member nor Threads array

typedef struct _SYSTEM_PROCESSES {
    ULONG			NextEntryDelta;
    ULONG			ThreadCount;
    ULONG			Reserved1[6];
    LARGE_INTEGER   CreateTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   KernelTime;
    UNICODE_STRING  ProcessName;
    KPRIORITY	    BasePriority;
    ULONG			ProcessId;
    ULONG			InheritedFromProcessId;
    ULONG			HandleCount;
    ULONG			Reserved2[2];
    VM_COUNTERS	    VmCounters;
#if _WIN32_WINNT >= 0x500
    IO_COUNTERS	    IoCounters;
#endif
    SYSTEM_THREADS  Threads[1];
} SYSTEM_PROCESSES, * PSYSTEM_PROCESSES;

//---------------------------------------------------------------------------
// KillProcessTreeNtHelper
//
//  This is a recursive helper function that terminates all the processes
//  started by the specified process and them terminates the process itself
//
//  Parameters:
//	  pInfo       - processes information
//	  dwProcessId - identifier of the process to terminate
//
//  Returns:
//	  Win32 error code.
//
static
BOOL
WINAPI
KillProcessTreeNtHelper(
	IN PSYSTEM_PROCESSES pInfo,
	IN DWORD dwProcessId
	)
{
	_ASSERTE(pInfo != NULL);

    PSYSTEM_PROCESSES p = pInfo;

    // kill all children first
    for (;;)
    {
		if (p->InheritedFromProcessId == dwProcessId)
			KillProcessTreeNtHelper(pInfo, p->ProcessId);

		if (p->NextEntryDelta == 0)
			break;

		// find the address of the next process structure
		p = (PSYSTEM_PROCESSES)(((LPBYTE)p) + p->NextEntryDelta);
    }

	// kill the process itself
    if (!KillProcess(dwProcessId)) return GetLastError();

	return ERROR_SUCCESS;
}

//---------------------------------------------------------------------------
// KillProcessTreeWinHelper
//
//  This is a recursive helper function that terminates all the processes
//  started by the specified process and them terminates the process itself
//
//  Parameters:
//	  dwProcessId - identifier of the process to terminate
//
//  Returns:
//	  Win32 error code.
//
static
BOOL
WINAPI
KillProcessTreeWinHelper(
	IN DWORD dwProcessId
	)
{
	HINSTANCE hKernel;
	HANDLE (WINAPI * _CreateToolhelp32Snapshot)(DWORD, DWORD);
	BOOL (WINAPI * _Process32First)(HANDLE, PROCESSENTRY32 *);
	BOOL (WINAPI * _Process32Next)(HANDLE, PROCESSENTRY32 *);

	// get handle to KERNEL32.DLL
	hKernel = GetModuleHandle(_T("kernel32.dll"));
	_ASSERTE(hKernel != NULL);

	// locate necessary functions in KERNEL32.DLL
	*(FARPROC *)&_CreateToolhelp32Snapshot =
		GetProcAddress(hKernel, "CreateToolhelp32Snapshot");
	*(FARPROC *)&_Process32First =
		GetProcAddress(hKernel, "Process32First");
	*(FARPROC *)&_Process32Next =
		GetProcAddress(hKernel, "Process32Next");

	if (_CreateToolhelp32Snapshot == NULL ||
		_Process32First == NULL ||
		_Process32Next == NULL)
		return ERROR_PROC_NOT_FOUND;

	HANDLE hSnapshot;
	PROCESSENTRY32 Entry;

	// create a snapshot
	hSnapshot = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return GetLastError();

	Entry.dwSize = sizeof(Entry);
	if (!_Process32First(hSnapshot, &Entry))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hSnapshot);
		return dwError;
	}

	// kill all children first
	do
	{
		if (Entry.th32ParentProcessID == dwProcessId)
			KillProcessTreeWinHelper(Entry.th32ProcessID);

		Entry.dwSize = sizeof(Entry);
	}
	while (_Process32Next(hSnapshot, &Entry));

	CloseHandle(hSnapshot);

	// kill the process itself
    if (!KillProcess(dwProcessId)) return GetLastError();

	return ERROR_SUCCESS;
}

//---------------------------------------------------------------------------
// KillProcessEx
//
//  Terminates the specified process and, optionally, all processes started
//	from the specified process (the so-called process tree).
//
//  Parameters:
//	  dwProcessId - identifier of the process to terminate
//	  bTree		  - specifies whether the entire process tree should be
//					terminated
//
//  Returns:
//	  TRUE, if successful, FALSE - otherwise.
//
BOOL
WINAPI
KillProcessEx(
	IN DWORD dwProcessId,
	IN BOOL bTree
	)
{
	if (!bTree)
		return KillProcess(dwProcessId);

	OSVERSIONINFO osvi;
	DWORD dwError;

	// determine operating system version
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	GetVersionEx(&osvi);

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion < 5)
	{
		HINSTANCE hNtDll;
		NTSTATUS (WINAPI * _ZwQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);

		// get handle to NTDLL.DLL
		hNtDll = GetModuleHandle(_T("ntdll.dll"));
		_ASSERTE(hNtDll != NULL);

		// find the address of ZwQuerySystemInformation
		*(FARPROC *)&_ZwQuerySystemInformation =
			GetProcAddress(hNtDll, "ZwQuerySystemInformation");
		if (_ZwQuerySystemInformation == NULL)
			return SetLastError(ERROR_PROC_NOT_FOUND), NULL;

		// obtain a handle to the default process heap
		HANDLE hHeap = GetProcessHeap();
    
		NTSTATUS Status;
		ULONG cbBuffer = 0x8000;
		PVOID pBuffer = NULL;

		// it is difficult to say a priory which size of the buffer 
		// will be enough to retrieve all information, so we start
		// with 32K buffer and increase its size until we get the
		// information successfully
		do
		{
			pBuffer = HeapAlloc(hHeap, 0, cbBuffer);
			if (pBuffer == NULL)
				return SetLastError(ERROR_NOT_ENOUGH_MEMORY), FALSE;

			Status = _ZwQuerySystemInformation(
							SystemProcessesAndThreadsInformation,
							pBuffer, cbBuffer, NULL);

			if (Status == STATUS_INFO_LENGTH_MISMATCH)
			{
				HeapFree(hHeap, 0, pBuffer);
				cbBuffer *= 2;
			}
			else if (!NT_SUCCESS(Status))
			{
				HeapFree(hHeap, 0, pBuffer);
				return SetLastError(Status), NULL;
			}
		}
		while (Status == STATUS_INFO_LENGTH_MISMATCH);

		// call the helper function
		dwError = KillProcessTreeNtHelper((PSYSTEM_PROCESSES)pBuffer, 
										  dwProcessId);
		
		HeapFree(hHeap, 0, pBuffer);
	}
	else
	{
		// call the helper function
		dwError = KillProcessTreeWinHelper(dwProcessId);
	}

	SetLastError(dwError);
	return dwError == ERROR_SUCCESS;
}
#endif

// ***********************************************************************
// OLD VERSION
// ***********************************************************************

#else

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Process.h>
#include <ace/Vector_T.h>
#include <stdio.h>
#include <yarp/Companion.h>
#include <yarp/String.h>
#include <yarp/os/all.h>
#include <yarp/os/Run.h>

// does ACE require new c++ header files or not?
#if ACE_HAS_STANDARD_CPP_LIBRARY
#include <fstream>
using namespace std;
#else
#include <fstream.h>
#endif

using namespace yarp;
using namespace yarp::os;

static Semaphore procMutex(0);
static bool startingProcess = false;
static ConstString myName = "/unknown";

#define DBG if (0) 

class Processor : public Thread {
public:
    ConstString name;
    Bottle script;
    int result;
    ConstString pidPlace;
    bool finished;

    Processor(const char *name) : name(name) {
        pidPlace = "PID.TXT"; 
        finished = false;
    }

    int runLine(const char *str) {
        script.clear();
        script.addString(str);
        runNormal();
        return result;
    }

    void runNormal() {
        if (startingProcess) {
            procMutex.wait();
            startingProcess = false;
        }
        ConstString cmd = script.get(0).asString();
        printf("Running: %s ...\n", cmd.c_str());
        ACE_Process_Options options;
        options.command_line("%s",cmd.c_str());
        ACE_Process child;
        pid_t id = child.spawn(options);
        options.release_handles();
        ofstream fout(pidPlace.c_str());
        fout << id << endl;
        fout.close();
        //printf("PID is %d\n",id);
        child.wait();
        result = child.exit_code();
        //printf("PID result is %d %d\n",id,result);
    }

    void setup(const char *str, const char *pidPlace) {
        finished = false;
        this->pidPlace = pidPlace;
        script.clear();
        script.addString(str);
    }

    virtual void run() {
        ConstString cmd = script.get(0).asString();
        printf("Starting [%s]\n", cmd.c_str());
        runLine(cmd.c_str());
        printf("Finished [%s]\n", cmd.c_str());
        finished = true;
    }

    ConstString getName() {
        return name;
    }

    bool isFinished() {
        return finished;
    }
};


class Processors {
public:
    ACE_Vector<Processor*> procs;

    virtual ~Processors() {
        for (unsigned int i=0; i<procs.size(); i++) {
            if (procs[i]!=NULL) {
                delete procs[i];
            }
            procs[i] = NULL;
        }
    }

    Processor& add(const char *name) {
        for (unsigned int i=0; i<procs.size(); i++) {
            if (procs[i]!=NULL) {
                if (procs[i]->isFinished()) {
                    delete procs[i];
                    procs[i] = new Processor(name);
                    return *(procs[i]);
                }
            }
        }
        Processor *bck = new Processor(name); 
        procs.push_back(bck);
        return *(bck);
    }
};


class Connector : public Thread {
private:
    ConstString in, out;
public:
    Connector(const char *in, const char *out) : in(in), out(out) {
    }

    virtual void run() {
        char buf[256];
        ACE_OS::sprintf(buf,"%s",in.c_str());
        char *targets[] = { "verbatim", buf };
        Companion::write("...",2,targets);
        printf("\"Write\" side of connection finished.\n");
    }

    void main() {
        start();
        Companion::read("...",out.c_str());
        printf("\"Read\" side of connection finished.\n");
        stop();
    }
};


static ConstString getStdin(bool& term) {
    bool done = false;
    String txt = "";
    char buf[2048];
    while (!done) {
        char *result = ACE_OS::fgets(buf,sizeof(buf),stdin);
        if (result!=NULL) {
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) {
                if (buf[i]=='\n') {
                    buf[i] = '\0';
                    done = true;
                    break;
                }
            }
            txt += buf;
        } else {
            done = true;
        }
    }
    term = feof(stdin);
    return txt.c_str();
}

static Bottle readStdinAsBottle() {
    Bottle bot;
    bool done = false;
    while (!done) {
        bool term = false;
        ConstString str = getStdin(term);
        if (term) {
            done = true;
        } else {
            printf("adding string %s\n", str.c_str());
            bot.addString(str.c_str());
        }
    }
    for (int i=0; i<bot.size(); i++) {
        printf("have line %s\n", bot.get(i).asString().c_str());
    }
    return bot;
}


static Bottle readFileAsBottle(const char *fileName) {
    Bottle bot;
    ifstream fin(fileName);
    if (fin.eof()||fin.fail()) {
        return "";
    }
    while (!(fin.eof()||fin.bad())) {
        char buf[25600];
        fin.getline(buf,sizeof(buf),'\n');
        if (!(fin.eof()||fin.bad())) {
            bot.addString(buf);
        }
    }
    return bot;
}

static void writeBottleAsFile(const char *fileName, const Bottle& bot) {
    ofstream fout(fileName);
    for (int i=0; i<bot.size(); i++) {
        fout << bot.get(i).toString().c_str() << endl;
    }
    fout.close();
}



static String reader(const char *context) {
    return (String("/") + context + "/stdin").c_str();
}

static String writer(const char *context) {
    return (String("/") + context + "/stdout").c_str();
}

static int runConnect(Searchable& config) {
    ConstString context = config.check("connect", 
                                       Value("default")).toString();
    String r = reader(context);
    String w = writer(context);
    printf("*** Connecting to %s: writing to %s, reading from %s\n",
           context.c_str(), r.c_str(), w.c_str());
    Connector con(r.c_str(),w.c_str());
    con.main();
    return 0;
}


static int run(Processors& procs, const Bottle& input0, Bottle& output, 
               Searchable& config) {

    Bottle input = input0;

    if (input.get(0).asString()=="help") {
        output.clear();
        output.addString("I should help");
        return 0;
    }

    // grab client properties if they exist
    Property client;
    for (int i=0; i<input.size(); i++) {
        ConstString str = input.get(i).asString();
        if (str.length()>5) {
            if (str[0]=='#'&&str[1]=='p') {
                Bottle line(str);
                if (line.get(0).asString() == "#pragma") {
                    client.fromString(str,false);
                }
            }
        }
    }
    if (input.get(0).asString()=="as") {
        client.put("as",input.get(1).asString().c_str());
        input = input.tail().tail();
    }

    String context = client.check("as", Value("default")).toString().c_str();
    printf("Settings are %s\n", client.toString().c_str());
    printf("Command is %s\n", input.toString().c_str());

    Bottle tmp;
    String fout, ferr, fexec, fwrap, fpid, fstop;
    String prefix = "runner";
    ACE_OS::mkdir(prefix.c_str());
    String prefixed = prefix + "/" + context;
    ACE_OS::mkdir(prefixed.c_str());

    fwrap = prefixed + "/" + "wrapper.sh";
    fstop = prefixed + "/" + "stop.sh";
    fexec = prefixed + "/" + "core.sh";
    fout = prefixed + "/" + "out.txt";
    ferr = prefixed + "/" + "err.txt";
    fpid = prefixed + "/" + "pid.txt";

    writeBottleAsFile(fexec.c_str(),input);

    String wrapCmd = "yarp read ";
    wrapCmd = wrapCmd + reader(context.c_str()) + " | sh " + fexec + 
        " | yarp write " + writer(context.c_str()) + " verbatim &";
    tmp.clear();
    tmp.addString(wrapCmd.c_str());
    tmp.addString("disown %sh");
    tmp.addString("disown %yarp");
    writeBottleAsFile(fwrap.c_str(),tmp);


    String stopCmd = "";
    stopCmd += "yarp terminate " + reader(context.c_str()) + "\n";
    stopCmd += "yarp terminate " + writer(context.c_str()) + "\n";
    tmp.clear();
    tmp.addString(stopCmd.c_str());
    writeBottleAsFile(fstop.c_str(),tmp);
    
    String cmd = String("sh -c \"sh ") + fwrap + " > " + fout + " 2> " + 
        ferr + "\"";
    String cmdCore = String("sh -c \"sh ") + fexec + " > " + fout + " 2> " + 
        ferr + "\"";

    int result = 1;
    if (client.check("no_stop")) {
        printf("handled elsewhere\n");
    } else if (client.check("as")) {
        printf("wrap command is %s\n", cmd.c_str());
        Processor proc("default");
        result = proc.runLine(cmd.c_str());
        printf("... Reading result from %s\n", fout.c_str());
        output = readFileAsBottle(fout.c_str());
        output.clear();
        String msg = context + " reads from " + 
            reader(context.c_str()).c_str() + 
            ", writes to " + writer(context.c_str()).c_str();
        output.addString(msg.c_str());
    } else {
        printf("core command is %s\n", cmdCore.c_str());
        Processor proc("default");
        result = proc.runLine(cmdCore.c_str());
        printf("... Reading result from %s\n", fout.c_str());
        output = readFileAsBottle(fout.c_str());
    }

    printf("Done\n");

    return result;
}



static int runServer(int argc, char *argv[], Searchable& config) {
    Processors procs;
    Port port;
    port.open(config.check("server",
                           Value("/run"),
                           "port name for server").asString());
    myName = port.getName();

    if (!config.check("nowarn")) {
        printf("******************************************************************************\n");
        printf("*** You have just started a server that lets people run things on your machine\n");
        printf("*** If your machine is reachable from the internet, say goodbye to it\n");
        printf("*** (start yarprun with the --nowarn option to suppress this warning)\n");
        printf("******************************************************************************\n");
    }

    while (true) {
        Bottle bot, reply;
        port.read(bot,true);
        Bottle output;
        int result = run(procs,bot,output,config);
        reply.addVocab(result?VOCAB4('f','a','i','l'):VOCAB2('o','k'));
        //DBG printf("Reply  is %s\n", reply.toString().c_str());
        // figure it out later
        reply.append(output);
        port.reply(reply);
        DBG printf("Reply finished\n");

        Time::delay(0.5); // something strange with ACE_Process::spawn
        if (startingProcess) {
            procMutex.post();
        }
    }
    return 0;
}

static int runClient(int argc, char *argv[], Searchable& config) {
    //printf("config stuff is %s\n", config.toString().c_str());

    Bottle tmp;


    DBG printf("client state %s:%d\n",__FILE__,__LINE__);
    String prefix = "#pragma yarp ";
    prefix += config.toString().c_str();

    DBG printf("client state %s:%d\n",__FILE__,__LINE__);
    if (config.check("cmd")) {        
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        tmp = config.findGroup("cmd");
        tmp.copy(tmp,1,tmp.size());
        if (tmp.size()>1) {
            ConstString desc = tmp.toString();
            tmp.clear();
            tmp.addString(desc);
        }
    } else if (config.check("script")) {
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        String fname = config.check("script",
                                    Value("")).toString().c_str();
        if (fname!="") {
            tmp = readFileAsBottle(fname.c_str());
        } else {
            DBG printf("client state %s:%d\n",__FILE__,__LINE__);
            tmp = readStdinAsBottle();
        }
    } else if (config.check("stop")) {
        tmp.clear();
        String fname = config.check("stop",
                                    Value("")).toString().c_str();
        if (fname!="") {
            String stop = "sh runner/";
            stop += fname;
            stop += "/stop.sh";
            tmp.addString(stop.c_str());
        }
    }

    if (tmp.size()>=1) {
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        Bottle cmd,response;
        cmd = tmp;
        cmd.addString(prefix.c_str());
        DBG printf("Have a command to send: %s\n", cmd.toString().c_str());
        Port port;
        port.open("...");
        ConstString target = config.check("on",Value("/run")).asString();
        Network::connect(port.getName().c_str(),target.c_str());
        port.write(cmd,response);
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        Network::disconnect(port.getName().c_str(),target.c_str());
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        port.close();
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        printf("\n");
        for (int i=1; i<response.size(); i++) {
            printf("%s\n", response.get(i).toString().c_str());
        }
        int result = (response.get(0).asVocab() == VOCAB2('o','k'))?0:1;
        if (result!=0) {
            fprintf(stderr, "*** error reported\n");
        }
        return result;
    }
    fprintf(stderr, "No command chosen.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "To start a server:\n");
    fprintf(stderr, "   yarp run --server /SERVER\n");
    fprintf(stderr, "A server can be asked to run commands. ");
    fprintf(stderr, "Make sure you are well firewalled!\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "To ask a server to run a command, and await result:\n");
    fprintf(stderr, "   yarp run --on /SERVER --cmd \"ls *\"\n");
    fprintf(stderr, "(the command should not be interactive)\n\n");
    fprintf(stderr, "To ask a server to run a command in the background:\n");
    fprintf(stderr, "   yarp run --on /SERVER --cmd \"cat\" --as KEY\n\n");
    fprintf(stderr, "To connect to a command running in the background:\n");
    fprintf(stderr, "   yarp run --connect KEY\n");    
    fprintf(stderr, "\n");
    fprintf(stderr, "Have you considered using ssh MACHINE COMMAND instead?\n");    
    return 1;
}



int Run::main(int argc, char *argv[]) {

    Property config;

    config.fromCommand(argc,argv,false);

    bool isServer = false;

    if (config.check("server")) {
        isServer = true;
    }

    DBG printf("server state %s:%d\n",__FILE__,__LINE__);
    if (isServer) {
        DBG printf("server state %s:%d\n",__FILE__,__LINE__);
        return runServer(argc,argv,config);
    } else {
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        if (config.check("connect")) {
            DBG printf("client state %s:%d\n",__FILE__,__LINE__);
            return runConnect(config);
        } else { 
            DBG printf("client state %s:%d\n",__FILE__,__LINE__);
            return runClient(argc,argv,config);
        }
    }

    return 0;
}

#endif

