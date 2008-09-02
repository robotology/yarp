// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * Based on an earlier version by Paul Fitzpatrick
 *
 */

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
	
	bool Kill()
	{
		if (m_Pid>0)
		{
		    KillTree();
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

	#ifndef ACE_LACKS_KILL
	void SendTKSignals()
	{
	    pid_t* stack=new pid_t[4096];
	    pid_t* to_kill_list=new pid_t[4096];	    

	    int head=1,n_to_kill=0;
	    
	    stack[0]=m_Pid;

		char pidbuff[16];

	    while (head)
	    {
	        pid_t pid=stack[--head];
	        
	        sprintf(pidbuff,"%d",pid);
	        String filename(m_Workdir+"pgrep.txt");
	        
	        String pgrep_cmd=String("pgrep -P ")+pidbuff+" > "+filename;
	        DBG printf("%s\n",pgrep_cmd.c_str());
	        
	        FILE *pgrepsh=fopen((m_Workdir+"mypgrep.sh").c_str(),"wc");
	        fprintf(pgrepsh,"%s\n",pgrep_cmd.c_str());
			fflush(pgrepsh);
	        fclose(pgrepsh);

	        ACE_Process_Options pgrep_opt;
	        pgrep_opt.command_line("%s",("bash "+m_Workdir+"mypgrep.sh").c_str());
	        ACE_Process pgrep_proc;
	        pid_t pgrep_pid=pgrep_proc.spawn(pgrep_opt);
	        pgrep_proc.wait();
	        
	        if (pgrep_pid==-1) break;
	    
	        if (pid!=m_Pid)
			{    
				to_kill_list[n_to_kill++]=pid;
		
	            ACE_Process_Options kill_opt;
	            kill_opt.command_line("kill -15 %d",pid);
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

		Time::delay(2.0);

		char procfilename[32];
		FILE *procfile;

		for (int n=0; n<n_to_kill; ++n)
		{
			sprintf(procfilename,"/proc/%d/status",to_kill_list[n]);
			
			if ((procfile=fopen(procfilename,"r")))
			{
				fclose(procfile);

				printf("process %d won't die, sending SIGKILL\n",to_kill_list[n]);

	            ACE_Process_Options kill_opt;
	            kill_opt.command_line("kill -9 %d",to_kill_list[n]);
	            ACE_Process kill_proc;
	            kill_proc.spawn(kill_opt);
	            kill_proc.wait();
			}
		}

		delete [] to_kill_list;
	}
	#endif

	void KillTree()
	{
		#ifndef ACE_LACKS_KILL	
		SendTKSignals();
		m_Child.kill(SIGTERM);
		ACE_Time_Value tw(2);
		if (m_Child.wait(tw)!=m_Pid)
		{
			m_Child.kill(SIGKILL);
		}
		#else
		#ifdef WIN32
		KillProcessEx(m_Pid,true);
		m_Child.terminate();
		#endif
		#endif
	}
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

		DBG printf("BOTTLE = %s\n\n",msg.toString().c_str());

		int result=-1;

		String alias=msg.check("as")?msg.findGroup("as").get(1).asString().c_str():"default";

		if (msg.check("script"))
		{
			Bottle script_bottle; 
			script_bottle.append(msg.findGroup("script").tail());	
			String script_name=workdir+"script"+serial_str+".sh";
			
			writeBottleAsFile(script_bottle,script_name);			
			
			String command_text="yarp read /"+alias+"/stdin 2>&1 | bash "+script_name+" 2>&1 | yarp write /"+alias+"/stdout";
	        
	        
	        String command_name=workdir+"command"+serial_str+".sh";
	        	
			FILE *command_file=fopen(command_name.c_str(),"wc");
			fprintf(command_file,"%s\n",command_text.c_str());
			fflush(command_file);
			fclose(command_file);		
			
			String exec_text="bash "+command_name;
			ACE_Process_Options exec_options;
			exec_options.command_line("%s",exec_text.c_str());

			pid_t pid=pv.Spawn(exec_options,alias,serial);
			
			DBG printf("SPAWN %d %s\n",pid,alias.c_str());
			
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

			command_text="yarp read /"+alias+"/stdin 2>&1 | "+command_text+" 2>&1 | yarp write /"+alias+"/stdout";
	        

	        String command_name=workdir+"command"+serial_str+".sh";

			FILE *command_file=fopen(command_name.c_str(),"wc");
			fprintf(command_file,"%s\n",command_text.c_str());
			fflush(command_file);
			fclose(command_file);
			
			String exec_text="bash "+command_name;
			ACE_Process_Options exec_options;
			exec_options.command_line("%s",exec_text.c_str());

			pid_t pid=pv.Spawn(exec_options,alias,serial);
			
			DBG printf("SPAWN %d %s\n",pid,alias.c_str());
			
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

			DBG printf("KILL %s\n",key.c_str());
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

			DBG printf("KILLALL\n");
		}
		else if (msg.check("exit"))
		{
		    pv.Killall();
		
			bRun=false;
			result=0;
			
			DBG printf("EXIT\n\n");
			
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

		DBG printf("BOTTLE = %s\n\n",msg.toString().c_str());

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
			
			DBG printf("SPAWN %d %s\n\n",pid,alias.c_str());
			
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
			
			DBG printf("SPAWN %d %s\n",pid,alias.c_str());
			
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

			DBG printf("KILL %s\n",key.c_str());
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

			DBG printf("KILLALL\n");
		}
		else if (msg.check("exit"))
		{
			pv.Killall();
			bRun=false;
			result=0;

			DBG printf("EXIT\n");
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
	Bottle msg;
	ConstString target=config.check("on",Value("/run")).asString();

    bool verbose=false;
    if (config.check("verbose"))
        verbose=true;

	if (verbose)
        printf("CONFIG: %s\n",config.toString().c_str());

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

		if(verbose)
            printf("CMD: %s\n",msg.toString().c_str());
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
    Network::connect(port.getName().c_str(),target.c_str());
	Bottle response;
    port.write(msg,response);
    Network::disconnect(port.getName().c_str(),target.c_str());
    port.close();

	printf("yarp run on %s returned %s\n",
           target.c_str(), response.toString().c_str());

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

void Run::printHelp()
{
	printf("yarp run provides a server able to run/kill commands/scripts on a remote machine.\n\n");
	printf("To run a server on a machine:\n$ yarp run --server <serverport>\n\n");
	printf("To run a command by the remote server:\n$ yarp run --on <serverport> --as <tag> --cmd <command> [<arglist>]\n\n");
	printf("To run a script by the remote server:\n$ yarp run --on <serverport> --as <tag> --script <scriptname>\n");
	printf("in this case the script is copied and executed on the remote server\n\n");
	printf("To kill a command/script:\n$ yarp run --on <serverport> --kill <tag>\n\n");
	printf("To kill all commands/scripts on a server:\n$ yarp run --on <serverport> --killall\n\n");
	printf("To clean shutdown a server:\n$ yarp run --on <serverport> --exit\n\n");
    printf("Append --verbose to get debug messages\n");
}

int Run::main(int argc, char *argv[]) 
{
    Property config;

    config.fromCommand(argc,argv,false);

    bool verbose=true;
    if (config.check("verbose"))
        printf("Warning --verbose not implemented yet\n");
        
	if (config.check("help"))
	{
		printHelp();
		return 0;
	}

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

		String localdir=portname.c_str();

		for (unsigned int i=1; i<localdir.length(); ++i)
			if (localdir[i]=='/') localdir[i]='_';

		workdir+=localdir+"/";

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
	else printHelp();

    return 0;
}

#ifdef WIN32
//#define _WIN32_WINNT 0x500

#include <windows.h>
#include <tchar.h>
#ifdef MSVC
#include <crtdbg.h>
#else
#ifndef _ASSERTE
#define _ASSERTE(x)
#endif
#endif
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

//old version of yarprun moved to Run.bak
