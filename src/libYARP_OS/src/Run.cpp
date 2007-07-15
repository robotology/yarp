// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * Old version by Paul Fitzpatrick
 * New version by Alessandro Scalzo
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

#ifndef ACE_LACKS_KILL
#define KILL() kill(SIGKILL)
#else
#define KILL() terminate()
#endif

using namespace yarp;
using namespace yarp::os;

#define DBG if (true)

class TProcess
{
public:
	TProcess()
	{
		m_Pid=-1;
		m_Name="default";
	}
	~TProcess()
	{
		Kill();
	}
	
	pid_t Spawn(ACE_Process_Options& options,String name="default")
	{
		m_Pid=m_Child.spawn(options);
		
		if (m_Pid!=-1)
		{
			m_Name=name;
		}

		return m_Pid;
	}

	bool Kill()
	{
		if (m_Pid!=-1)
		{
			m_Child.KILL();
			m_Child.wait();
			//Time::delay(0.5);
			m_Pid=-1;
			m_Name="default";
			return true;
		}
		return false;
	}

	bool operator==(const String& name){ return name==m_Name; }
	bool operator==(pid_t pid){ return pid==m_Pid; }

protected:
	String m_Name;
	pid_t m_Pid;
	ACE_Process m_Child;
};

class TProcessVector
{
public:
	TProcessVector()
	{
		m_nProcesses=0;
		//m_apChilds.clear();
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

		//m_apChilds.clear();
	}

	int Killall()
	{
		int killed=0;

		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apChilds[i])
			{
				++killed;
				m_Mutex.wait();
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

		for (int i=0; i<m_nProcesses; ++i)
		{
			if (m_apChilds[i])
			{
				if (*m_apChilds[i]==name)
				{
					++killed;
					m_Mutex.wait();
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

	pid_t Spawn(ACE_Process_Options& options,String name="unnamed")
	{
		m_Mutex.wait();

		if (m_nProcesses>=MAX_PROCESSES)
		{
			m_Mutex.post();
			return -1;
		}

		TProcess* pChild=new TProcess;
		//m_apChilds.push_back(pChild);
		m_apChilds[m_nProcesses]=pChild;
		pid_t pid=pChild->Spawn(options,name);

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

protected:
	TProcess* m_apChilds[1024];
	int m_nProcesses;
	Semaphore m_Mutex;

	enum { MAX_PROCESSES=1024 };

	void Pack()
	{
		int tot=0;

		int i;
		for (i=0; i<m_nProcesses; ++i)
		{
			if (m_apChilds[i]) m_apChilds[tot++]=m_apChilds[i]; 
		}

		for (i=tot; i<m_nProcesses; ++i)
			m_apChilds[i]=0;

		m_nProcesses=tot;
	}
};

// compiler warns that this is not used anymore
/*
static ConstString getStdin(bool& term) 
{
    bool done = false;
    String txt = "";
    char buf[2048];

	while (!done) 
	{
        char *result = ACE_OS::fgets(buf,sizeof(buf),stdin);
    
		if (result!=NULL) 
		{
            for (unsigned int i=0; i<ACE_OS::strlen(buf); i++) 
			{
                if (buf[i]=='\n') 
				{
                    buf[i] = '\0';
                    done = true;
                    break;
                }
            }
          
			txt += buf;
        } 
		else 
		{
            done = true;
        }
    }

    term = feof(stdin);
    return txt.c_str();
}

static Bottle readStdinAsBottle() 
{
    Bottle bot;
    bool done = false;

	while (!done) 
	{
        bool term = false;
        ConstString str = getStdin(term);
    
		if (term) 
		{
            done = true;
        } 
		else 
		{
            printf("adding string %s\n", str.c_str());
            bot.addString(str.c_str());
        }
    }

    for (int i=0; i<bot.size(); i++) 
	{
        printf("have line %s\n", bot.get(i).asString().c_str());
    }
    
	return bot;
}
*/

static bool writeBottleAsFile(Bottle& bottle,String& filename) 
{
	FILE* fout=fopen(filename.c_str(),"wc");
    
	if (!fout) return false;

	for (int i=0; i<bottle.size(); ++i)
	{
		fprintf(fout,"%s\n",bottle.get(i).asString().c_str());
	}

	fclose(fout);

    return true;
}

static Bottle readScriptAsBottle(const char *fileName) 
{
    Bottle bot;

	bot.addString("script");
	//bot.addString(fileName);

    ifstream fin(fileName);
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

Run::~Run()
{
	ACE_OS::rmdir("runner");
}

int Run::runServer(Searchable& config) 
{
	Port port;
    port.open(config.check("server",Value("/run"),"port name for server").asString());
    String myPort = port.getName().c_str();
	String directory("runner");
	ACE_OS::mkdir(directory.c_str());

	TProcessVector pv;

	bool bRun=true;

	int serial=0;

	char intbuf[16];

    while (bRun) 
	{
		++serial;
		sprintf(intbuf,"%d",serial);
		String serial_str(intbuf);

        Bottle msg,output;
        port.read(msg,true);

		DBG printf("\nBOTTLE = %s\n\n",msg.toString().c_str());
		//fflush(stdout);

		int result=-1;

		String key=msg.check("as")?msg.findGroup("as").get(1).asString().c_str():"default";
		String yarp_ctrl=String("YARP_CTRL=\"yarp run --on ")+myPort+" --as "+key+" --cmd \"";

		if (msg.check("cmd"))
		{
			Bottle cmd=msg.findGroup("cmd");
			cmd=cmd.tail();

			bool isDos=cmd.get(0).asString()=="batch";

			if (isDos) yarp_ctrl=yarp_ctrl.substring(11,yarp_ctrl.length()-12);

			if (msg.check("script"))
			{
				Bottle script; 
				if (!isDos) script.addString(yarp_ctrl.c_str());
				script.append(msg.findGroup("script").tail());
				//String filename=script.get(0).asString().c_str();
				//script=script.tail();
				String filename;
				if (isDos)
				{
					filename=directory+"\\script_"+serial_str+".bat";
					cmd.clear();
				}
				else
				{
					filename=directory+"/script_"+serial_str+".sh";
				}

				cmd.addString(filename.c_str());

				if (isDos)
				{
					Bottle newscript;

					for (int i=0; i<script.size(); ++i)
					{
						String line=script.get(i).asString().c_str();
						String linenew;

						printf("line=%s\n",line.c_str());
						
						for (unsigned int s=0; s<line.length(); ++s)
						{
							if (line[s]!='$')
								linenew+=line[s];
							else
							{
								if (line.substr(s,12)=="${YARP_CTRL}")
								{
									linenew+=yarp_ctrl;
									s+=11;
								}
							}
						}
						printf("linenew=%s\n",linenew.c_str());
						newscript.addString(linenew.c_str());
					}

					script=newscript;
				}

				printf("filename=%s\n",filename.c_str());
				writeBottleAsFile(script,filename);
			}
			
			bool bNeedBatch=false;

			if (msg.check("stdin"))
			{
				bNeedBatch=true;
				Bottle Stdin=msg.findGroup("stdin");
				String StdinCmd=String("yarp read ")+Stdin.get(1).toString().c_str()+String(" | ");
				Bottle tmp;
				tmp.addString(StdinCmd.c_str());
				String cmd_without_quotes=cmd.toString().c_str();
				cmd_without_quotes=cmd_without_quotes.substring(1,cmd_without_quotes.length()-2);
				if (isDos) tmp.addString(" CALL ");
				tmp.addString(cmd_without_quotes.c_str());
				cmd=tmp;
			}

			if (msg.check("stdout"))
			{
				bNeedBatch=true;
				Bottle Stdout=msg.findGroup("stdout");
				String StdoutCmd=String(" | yarp write ")+Stdout.get(1).toString().c_str();
				cmd.addString(StdoutCmd.c_str());
			}

			String command;
			
			for (int s=0; s<cmd.size(); ++s)
				command+=String(cmd.get(s).toString().c_str())+" ";
			
			if (bNeedBatch)
			{
				String filename;
				if (isDos)
					filename=directory+"\\batch_"+serial_str+".bat";
				else
					filename=directory+"/batch_"+serial_str+".sh";
				

				printf("\nFILENAME: %s\n\n",filename.c_str());

				FILE* file=fopen(filename.c_str(),"wc");
				if (!isDos) fprintf(file,"%s\n",yarp_ctrl.c_str());
				fprintf(file,"%s\n",command.c_str());
				fclose(file);
				
				if (isDos)
					command=filename;
				else
					command="bash "+filename;
			}

			ACE_Process_Options options;
			options.command_line("%s",command.c_str());
			printf("command=%s\n",command.c_str());

			pid_t pid=pv.Spawn(options,key);
			
			DBG printf("\nSPAWN %d %s\n\n",pid,key.c_str());
			
			if (pid!=-1) result=0;

			output.addInt(pid);
		}
		else if (msg.check("kill"))
		{
			Bottle killer=msg.findGroup("kill");
			String key=killer.get(1).asString().c_str();
			if (pv.Kill(key))
			{ 
			    result=0;
			}
			else
			{
			    printf("can't kill process %s\n",key.c_str());
			}

			DBG printf("\nKILL %s\n\n",key.c_str());
			//fflush(stdout);
		}
		else if (msg.check("killall"))
		{
			if (pv.Killall()) result=0;

			DBG printf("\nKILLALL\n\n");
			//fflush(stdout);
		}
		else if (msg.check("exit"))
		{
			bRun=false;
			result=0;
			DBG printf("\nEXIT\n\n");
		}


		Bottle reply;
        reply.addVocab(result?VOCAB4('f','a','i','l'):VOCAB2('o','k'));
        
		reply.append(output);
        port.reply(reply);

        //Time::delay(0.5); // something strange with ACE_Process::spawn
    }
 
	return 0;
}

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
	printf("\nCONFIG: %s\n\n",config.toString().c_str());

	Bottle msg;

	if (config.check("cmd"))
	{                
		msg.addList()=config.findGroup("cmd");

		if (config.check("as"))
			msg.addList()=config.findGroup("as");

		if (config.check("blocking"))
			msg.addList()=config.findGroup("blocking");

		if (config.check("script"))
		{
			String fname=config.check("script",Value("")).toString().c_str();
			if (fname!="")
			{
				Bottle script=readScriptAsBottle(fname.c_str());
				msg.addList()=script;
			}
        }

		if (config.check("stdin"))
		{
			msg.addList()=config.findGroup("stdin");
		}

		if (config.check("stdout"))
		{
			msg.addList()=config.findGroup("stdout");
		}

		printf("\nSCRIPT: %s\n\n",msg.toString().c_str());
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

int Run::main(int argc, char *argv[]) 
{
    Property config;

    config.fromCommand(argc,argv,false);

    if (config.check("server")) 
	{
        DBG printf("server state %s:%d\n",__FILE__,__LINE__);
        return runServer(config);
    } 
	if (config.check("connect")) 
	{
        DBG printf("connect state %s:%d\n",__FILE__,__LINE__);
        return runConnect(config);
    } 
	else if (config.check("cmd") || config.check("kill") || config.check("killall") || config.check("exit"))
	{ 
        DBG printf("client state %s:%d\n",__FILE__,__LINE__);
        return runClient(config);
    }
	else return -1;

    return 0;
}



// OLD VERSION

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


		/*
		else if (bot.get(0).asString()=="script")
		{
			result=0;

			bot.copy(bot,1,bot.size());

			for (int i=0; i<bot.size(); ++i)
			{
				String command=bot.get(i).asString().c_str();

				int comment_pos=command.find('#');

				if (comment_pos>-1) command=command.substring(0,comment_pos);

				while (command.find(' ')==0)
				{
					command=command.substring(1);
				}

				if (command.length()<=0) continue;

				String name;
				int name_len=command.find(' ');
				
				if (name_len>-1)
				{
					name=command.substring(0,name_len);
				}
				else
				{
					name=command;
				}
				
				printf("%d) %s\n",i,command.c_str());	
				printf("NAME= %s\n",name.c_str());
				
				ACE_Process_Options options;
				options.command_line("%s",command.c_str());
				pid_t pid=pv.Spawn(options,name.c_str());
				DBG printf("\nSPAWN %d\n\n",pid);
				//fflush(stdout);
				if (pid==-1) result=-1;

				output.addInt(pid);
			}
		}
		*/
