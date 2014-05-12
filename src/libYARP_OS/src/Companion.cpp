// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <yarp/os/impl/Companion.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/Name.h>

#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/NameConfig.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Terminator.h>
#include <yarp/os/Run.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Ping.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/SystemClock.h>

#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformSignal.h>

#include <yarp/conf/system.h>
#ifdef YARP_CMAKE_CONFIG
#include <yarp/conf/version.h>
#else
// we do not have configuration information, disable some features.
#endif

#include <stdio.h>

#ifdef WITH_READLINE
    #include <readline/readline.h>
    #include <readline/history.h>
    #include <vector>
    #include <yarp/os/ConstString.h>
    static std::vector<yarp::os::impl::String> commands;
    static yarp::os::Port* rpcHelpPort=NULL;
    static bool commandListInitialized=false;

    static char* dupstr(char* s)
    {
        char *r;
        r = (char*) malloc ((strlen (s) + 1));
        strcpy (r, s);
        return (r);
    }
    /* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
    static char* command_generator (const char* text, int state)
    {
        static int list_index, len;
        char *name;

        /* if this is a new word to complete, initialize now.  this includes
            saving the length of text for efficiency, and initializing the index
            variable to 0. */
        if (!state)
            {
            list_index = 0;
            len = strlen (text);
            }

        if (!commandListInitialized)
        {
            commands.clear();
            yarp::os::Bottle helpCommand, helpBottle;
            helpCommand.addString("help");
            bool helpOk=false;
            if(rpcHelpPort)
                helpOk = rpcHelpPort->write(helpCommand,helpBottle);
            if(helpOk)
            {
                yarp::os::Bottle* cmdList=NULL;
                if (helpBottle.get(0).isVocab() && helpBottle.get(0).asVocab()==VOCAB4('m','a','n','y') )
                {
                    cmdList=helpBottle.get(1).asList();
                }
                else
                    cmdList=helpBottle.get(0).asList();
                if (cmdList && cmdList->get(0).asString() == "*** Available commands:")
                {
                    for (int i=1; i<cmdList->size(); ++i)
                        commands.push_back(cmdList->get(i).asString());
                }
            }
            commands.push_back(" ");
            commandListInitialized=true;
        }
        while ((list_index<commands.size()) && (name = (char*)commands[list_index].c_str()))
            {
            list_index++;
            if (strncmp (name, text, len) == 0)
                return (dupstr(name));
            }

        /* if no names matched, then return null. */
        return ((char *)NULL);
    }
    /* Attempt to complete on the contents of TEXT.  START and END show the
   region of TEXT that contains the word to complete.  We can use the
   entire line in case we want to do some simple parsing.  Return the
   array of matches, or NULL if there aren't any. */
    static char ** my_completion (const char* text, int start, int end)
    {
        char **matches;
        matches = (char **)NULL;

        /* If this word is at the start of the line, then it is a command
        to complete. If we are completing after "help ", it is a command again.
        Othwerwise, stop completing. */
        if (start == 0)
            matches = rl_completion_matches(text, &command_generator);
        else if (start == 5 && strncmp (text, "help ", 5))
            matches = rl_completion_matches(text, &command_generator);
        else
            rl_attempted_completion_over=1;

        return (matches);
    }

#endif

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp;

Companion Companion::instance;

static ConstString companion_unregister_name;
static Port *companion_active_port = NULL;

static void companion_sigint_handler(int sig) {
    double now = Time::now();
    static double firstCall = now;
    static bool showedMessage = false;
    static bool unregistered = false;
    if (now-firstCall<2) {
        Port *port = companion_active_port;
        if (!showedMessage) {
            showedMessage = true;
            YARP_LOG_INFO("Interrupting...");
        }
        if (companion_unregister_name!="") {
            if (!unregistered) {
                unregistered = true;
                NetworkBase::unregisterName(companion_unregister_name);
                if (port!=NULL) {
                    NetworkBase::unregisterName(port->getName());
                }
                exit(1);
            }
        }
        if (port!=NULL) {
            port->interrupt();
#ifndef YARP2_WINDOWS
            port->close();
#endif
        }
    } else {
        fprintf(stderr,"Aborting...\n");
        exit(1);
    }
}

static void companion_sigterm_handler(int sig) {
    companion_sigint_handler(sig);
}

#if defined(WIN32)
static void companion_sigbreak_handler()
{
    raise(SIGINT);
}
#else
static void companion_sighup_handler()
{
    raise(SIGINT);
}
#endif

static void companion_install_handler() {
	signal(SIGINT,companion_sigint_handler);
	signal(SIGTERM,companion_sigterm_handler);

    #if defined(WIN32)
    signal(SIGBREAK, (ACE_SignalHandler) companion_sigbreak_handler);
    #else
    signal(SIGHUP, (ACE_SignalHandler) companion_sighup_handler);
    #endif
}

static char* szLine = (char*)NULL;
static bool readlineEOF=false;
static bool EOFreached()
{
#ifdef WITH_READLINE
    return readlineEOF;
#else
    return feof(stdin);
#endif
}

static String getStdin() {
    String txt = "";

#ifdef WITH_READLINE
    if(szLine)
    {
        free(szLine);
        szLine = (char*)NULL;
    }

    szLine = readline(">>");
    if(szLine && *szLine)
    {
        txt = szLine;
        add_history(szLine);
    }
    else if (!szLine)
        readlineEOF=true;
#else

    bool done = false;
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

#endif

    return txt;
}

static void writeBottleAsFile(const char *fileName, const Bottle& bot) {
    FILE *fout = fopen(fileName,"w");
    if (fout==NULL) return;
    for (int i=0; i<bot.size(); i++) {
        fprintf(fout,"%s\n",bot.get(i).toString().c_str());
    }
    fclose(fout);
    fout = NULL;
}


Companion::Companion() {
    adminMode = false;
    waitConnect = false;
    add("check",      &Companion::cmdCheck,
        "run a simple sanity check to see if yarp is working");
    add("clean",  &Companion::cmdClean,
        "try to remove inactive entries from the name server");
    add("cmake",  &Companion::cmdMake,
        "create files to help compiling YARP projects");
    add("conf",       &Companion::cmdConf,
        "report location of configuration file, and optionally fix it");
    add("connect",    &Companion::cmdConnect,
        "create a connection between two ports");
    add("detect",     &Companion::cmdDetect,
        "search for the yarp name server");
    add("disconnect", &Companion::cmdDisconnect,
        "remove a connection between two ports");
    add("exists",  &Companion::cmdExists,
        "check if a port or connection is alive");
    add("forward",        &Companion::cmdForward,
        "forward commands to a port, in standard format (experimental)");
    add("help",       &Companion::cmdHelp,
        "get this list");
    add("merge",       &Companion::cmdMerge,
        "concatenate input from several ports into a single unit");
    add("name",       &Companion::cmdName,
        "send commands to the yarp name server");
    add("namespace",  &Companion::cmdNamespace,
        "set or query the name of the yarp name server (default is /root)");
    add("ping",  &Companion::cmdPing,
        "get live information about a port");
    add("plugin", &Companion::cmdPlugin,
        "check properties of a YARP plugin (device/carrier)");
    add("read",       &Companion::cmdRead,
        "read from the network and print to standard output");
    // needed by yarprun --stdio
	add("readwrite",  &Companion::cmdReadWrite,
        "read from the network and print to standard output, write to the network from standard input");
    add("regression", &Companion::cmdRegression,
        "run regression tests, if linked");
    add("resource",  &Companion::cmdResource,
        "locates resource files (see ResourceFinder class)");
    add("rpc",        &Companion::cmdRpc,
        "write commands to a port, and read replies");
    add("rpcserver",  &Companion::cmdRpcServer,
        "make a test RPC server to receive and reply to Bottle-format messages");
    add("run",  &Companion::cmdRun,
        "start and stop processes");
    add("sample", &Companion::cmdSample,
        "drop or duplicate messages to achieve a constant frame-rate");
    add("server",     &Companion::cmdServer,
        "run yarp name server");
    add("terminate",  &Companion::cmdTerminate,
        "terminate a yarp-terminate-aware process by name");
    add("time", &Companion::cmdTime,
        "show the time");
    add("topic",  &Companion::cmdTopic,
        "set a topic name");
    add("version",    &Companion::cmdVersion,
        "get version information");
    add("wait",  &Companion::cmdWait,
        "wait for a port to be alive");
    add("where",      &Companion::cmdWhere,
        "report where the yarp name server is running");
    add("write",      &Companion::cmdWrite,
        "write to the network from standard input");
}

int Companion::dispatch(const char *name, int argc, char *argv[]) {
    // new logic to handle some global arguments
    char **argv_copy = new char *[argc];
    int argc_copy = argc;
    if (!argv_copy) {
        YARP_SPRINTF0(Logger::get(),
                      error,
                      "Could not copy argument list");
        return 1;
    }
    int at = 0;
    int skip = 0;
    for (int i=0; i<argc; i++) {
        if (skip>0) {
            skip--;
            continue;
        }
        ConstString arg = argv[i];
        if (arg.find("--")==0) {
            if (i+1<argc) {
                if (arg=="--type") {
                    skip = 1;
                    argType = argv[i+1];
                    continue;
                }
            }
            if (arg=="--wait-connect") {
                skip = 0;
                waitConnect = true;
                continue;
            }
        }
        argv_copy[at] = argv[i];
        at++;
    }
    argc_copy = at;

    String sname(name);
    Entry e;
    int result = PLATFORM_MAP_FIND_RAW(action,sname,e);
    if (result!=-1) {
        return (this->*(e.fn))(argc_copy,argv_copy);
    } else {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "Could not find command \"%s\"",name);
    }
    return -1;
}


int Companion::main(int argc, char *argv[]) {
    ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
    if (!rf.isConfigured()) {
        rf.configure(argc,argv);
    }

    // eliminate 0th arg, the program name
    argc--;
    argv++;

    if (argc<=0) {
        ACE_OS::printf("This is the YARP network companion.\n");
        ACE_OS::printf("Call with the argument \"help\" to see a list of ways to use this program.\n");
        return 0;
    }

    Companion& instance = getInstance();
    int verbose = 0;
    bool adminMode = false;
    bool more = true;
    while (more && argc>0) {
        more = false;
        String s = String(argv[0]);
        if (s == String("verbose")) {
            verbose++;
            argc--;
            argv++;
            more = true;
        }
        if (s == String("quiet")) {
            verbose--;
            argc--;
            argv++;
            more = true;
        }
        if (s == String("admin")) {
            adminMode = true;
            argc--;
            argv++;
            more = true;
        }
    }
    if (verbose!=0) {
        Logger::get().setVerbosity(verbose);
    }

    if (argc<=0) {
        ACE_OS::fprintf(stderr,"Please supply a command\n");
        return -1;
    }

    const char *cmd = argv[0];
    argc--;
    argv++;
    instance.adminMode = adminMode;
    return instance.dispatch(cmd,argc,argv);
}


int Companion::cmdTerminate(int argc, char *argv[]) {
    if (argc == 1) {
        ACE_OS::printf("Asking port %s to quit gracefully\n", argv[0]);
        Terminator::terminateByName(argv[0]);
        return 0;
    }

    ACE_OS::printf("Wrong parameter format, please specify a port name as a single parameter to terminate\n");
    return 1;
}



int Companion::cmdPing(int argc, char *argv[]) {
    bool time = false;
    bool rate = false;
    if (argc>=1) {
        while (argv[0][0]=='-') {
            if (ConstString(argv[0])=="--time") {
                time = true;
            } else if (ConstString(argv[0])=="--rate") {
                rate = true;
            } else {
                YARP_LOG_ERROR("Unrecognized option");
                argc = 1;
            }
            argc--;
            argv++;
        }
    }
    if (argc == 1) {
        char *targetName = argv[0];
        if (time) {
            printf("Timing communication with %s...\n", targetName);
            Ping ping;
            ping.setTarget(targetName);
            for (int i=0; i<10; i++) {
                ping.connect();
                ping.report();
                Time::delay(0.25);
            }
            return 0;
        }
        if (rate) {
            printf("Measuring rate of output from %s...\n", targetName);
            Ping ping;
            ping.setTarget(targetName);
            ping.sample();
            return 1;
        }
        return ping(targetName,false);
    }
    ACE_OS::fprintf(stderr,"Usage:\n");
    ACE_OS::fprintf(stderr,"  yarp ping /port\n");
    ACE_OS::fprintf(stderr,"  yarp ping --time /port\n");
    ACE_OS::fprintf(stderr,"  yarp ping --rate /port\n");
    return 1;
}

int Companion::ping(const char *port, bool quiet) {

    const char *connectionName = "<ping>";
    OutputProtocol *out = NULL;

    Contact address = NetworkBase::queryName(port);
    if (!address.isValid()) {
        if (!quiet) {
            YARP_ERROR(Logger::get(),"could not find port");
        }
        return 1;
    }

    if (address.getCarrier()=="tcp") {
        out = Carriers::connect(address);
        if (out==NULL) {
            YARP_ERROR(Logger::get(),"port found, but cannot connect");
            return 1;
        }
        Route r(connectionName,port,"text_ack");
        bool ok = out->open(r);
        if (!ok) {
            YARP_ERROR(Logger::get(),"could not connect to port");
            return 1;
        }
        OutputStream& os = out->getOutputStream();
        InputStream& is = out->getInputStream();
        StreamConnectionReader reader;

        PortCommand pc(0,"*");
        BufferedConnectionWriter bw(out->getConnection().isTextMode());
        pc.write(bw);
        bw.write(os);
        Bottle resp;
        reader.reset(is,NULL,r,0,true);
        bool done = false;
        while (!done) {
            resp.read(reader);
            String str = resp.toString().c_str();
            if (resp.get(0).asString()!="<ACK>") {
                printf("%s\n", str.c_str());
            } else {
                done = true;
            }
        }
        if (out!=NULL) {
            delete out;
        }
    } else {
        int e = exists(port,quiet);
        printf("%s %s.\n", port, (e==0)?"exists":"is not responding");
        return e;
    }

    return 0;
}


int Companion::cmdExists(int argc, char *argv[]) {
    if (argc == 1) {
        return exists(argv[0],true);
    }
    if (argc == 2) {
        bool ok = NetworkBase::isConnected(argv[0],argv[1],false);
        return ok?0:1;
    }

    ACE_OS::fprintf(stderr,"Please specify a port name\n");
    return 1;
}



int Companion::exists(const char *target, const ContactStyle& style) {
    bool silent = style.quiet;
    Contact address = NetworkBase::queryName(target);
    if (!address.isValid()) {
        if (!silent) {
            printf("Address of port %s is not valid\n", target);
        }
        return 2;
    }

    Contact address2(address);
    if (style.timeout>=0) {
        address2.setTimeout((float)style.timeout);
    }
    OutputProtocol *out = Carriers::connect(address2);

    if (out==NULL) {
        if (!silent) {
            printf("Cannot connect to port %s\n", target);
        }
        return 1;
    } else {
        out->close();
    }
    delete out;
    out = NULL;
    return 0;
}



int Companion::cmdWait(int argc, char *argv[]) {
    if (argc == 1) {
        return wait(argv[0],false);
    }

    ACE_OS::fprintf(stderr,"Please specify a port name\n");
    return 1;
}


int Companion::wait(const char *target, bool silent) {
    bool done = false;
    int ct = 1;
    while (!done) {
        if (ct%10==1) {
            if (!silent) {
                YARP_SPRINTF1(Logger::get(),info,
                              "Waiting for %s...\n", target);
            }
        }
        ct++;
        int result = exists(target,true);
        if (result!=0) {
            Time::delay(0.1);
        } else {
            done = true;
        }
    }
    return 0;
}



int Companion::cmdName(int argc, char *argv[]) {
    ContactStyle style;
    style.quiet = true;
    Bottle cmd, reply;
    for (int i=0; i<argc; i++) {
        Value v;
        v.fromString(argv[i]);
        cmd.add(v);
    }

    ConstString key = cmd.get(0).asString();
    if (key=="query") {
        Contact result = NetworkBase::queryName(cmd.get(1).asString());
        if (!result.isValid()) {
            fprintf(stderr, "%s not known.\n", cmd.get(1).asString().c_str());
            return 1;
        }
        ConstString txt = NameServer::textify(result);
        printf("%s", txt.c_str());
        return 0;
    }
    if (key=="register") {
        ConstString portName = cmd.get(1).asString();
        ConstString machine = "...";
        ConstString carrier = "...";
        int port = 0;
        bool spec = false;
        if (cmd.size()>2) {
            carrier = cmd.get(2).asString();
            spec = true;
        }
        if (cmd.size()>3) {
            machine = cmd.get(3).asString();
        }
        if (cmd.size()>4) {
            if (!cmd.get(4).isInt()) {
                port = 0;
            } else {
                port = cmd.get(4).asInt();
            }
        }
        Contact result;
        if (spec) {
            Contact c =
                Contact::bySocket(carrier,machine,port).addName(portName);
            result = NetworkBase::registerContact(c);
        } else {
            result = NetworkBase::registerName(portName);
        }
        ConstString txt = NameServer::textify(result);
        printf("%s", txt.c_str());
        return 0;
    }
    if (key=="unregister") {
        ConstString portName = cmd.get(1).asString();
        Contact result;
        result = NetworkBase::unregisterName(portName);
        printf("Unregistered name.\n");
        return 0;
    }


    bool ok = NetworkBase::writeToNameServer(cmd,
                                             reply,
                                             style);
    if (!ok) {
        ACE_OS::fprintf(stderr, "Failed to reach name server\n");
        return 1;
    }
    if (reply.size()==1&&reply.get(0).isString()) {
        printf("%s", reply.get(0).asString().c_str());
    } else if (reply.get(0).isVocab() && reply.get(0).asVocab()==VOCAB4('m','a','n','y')) {
        for (int i=1; i<reply.size(); i++) {
            Value& v = reply.get(i);
            if (v.isString()) {
                printf("  %s\n", v.asString().c_str());
            } else {
                printf("  %s\n", v.toString().c_str());
            }
        }
    } else {
        printf("%s\n", reply.toString().c_str());
    }
    return 0;
}

int Companion::cmdConf(int argc, char *argv[]) {
    NameConfig nc;
    if (argc==0) {
        ACE_OS::printf("%s\n",nc.getConfigFileName().c_str());
        return 0;
    }
    if (argc>=2) {
        nc.fromFile();
        Contact prev = nc.getAddress();
        String prevMode = nc.getMode();
        Contact next(argv[0],atoi(argv[1]));
        nc.setAddress(next);
        if (argc>=3) {
            nc.setMode(argv[2]);
        } else {
            nc.setMode("yarp");
        }
        nc.toFile();
        nc.fromFile();
        Contact current = nc.getAddress();
        String currentMode = nc.getMode();
        printf("Configuration file:\n");
        printf("  %s\n",nc.getConfigFileName().c_str());
        if (prev.isValid()) {
            printf("Stored:\n");
            printf("  host %s port number %d (%s name server)\n",
                   prev.getHost().c_str(),
                   prev.getPort(),
                   prevMode.c_str());
        }
        if (current.isValid()) {
            printf("Now stores:\n");
            printf("  host %s port number %d (%s name server)\n",
                   current.getHost().c_str(),
                   current.getPort(),
                   currentMode.c_str());
        } else {
            printf("is not valid!\n");
            printf("Expected:\n");
            printf("  yarp conf [ip address] [port number]\n");
            printf("  yarp conf [ip address] [port number] [yarp|ros]\n");
            printf("For example:\n");
            printf("  yarp conf 192.168.0.1 10000\n");
            return 1;
        }
        return 0;
    }
    if (argc==1) {
        if (String(argv[0])=="--clean") {
            nc.toFile(true);
            printf("Cleared configuration file:\n");
            printf("  %s\n",nc.getConfigFileName().c_str());
            return 0;
        }
    }
    printf("Command not understood\n");
    return 1;
}


int Companion::cmdWhere(int argc, char *argv[]) {
    NameConfig nc;
    nc.fromFile();
    if (nc.getAddress().isValid()) {
        printf("Looking for name server on %s, port number %d\n",
               nc.getAddress().getHost().c_str(),
               nc.getAddress().getPort());
        printf("If there is a long delay, try:\n");
        printf("  yarp conf --clean\n");
    }
    Contact address = NetworkBase::getNameServerContact();

    bool reachable = false;
    if (address.isValid()) {
        OutputProtocol *out = Carriers::connect(address);
        if (out!=NULL) {
            reachable = true;
            out->close();
            delete out;
            out = NULL;
        }
    }

    if (address.isValid()&&reachable) {
        ACE_OS::printf("%sName server %s is available at ip %s port %d\n",
                       nc.getMode()=="ros"?"ROS ":"",
                       nc.getNamespace().c_str(),
                       address.getHost().c_str(), address.getPort());
        if (address.getCarrier()=="tcp") {
            ACE_OS::printf("Name server %s can be browsed at http://%s:%d/\n",
                           nc.getNamespace().c_str(),
                           address.getHost().c_str(), address.getPort());
        }
    } else {
        NameConfig conf;
        bool haveFile = conf.fromFile();
        Contact address = conf.getAddress();

        printf("\n");
        printf("=======================================================================\n");
        printf("==\n");
        printf("== PROBLEM\n");
        if (haveFile) {
            printf("== No valid YARP name server is available.\n");
            printf("== Here is the expected configuration:\n");
            printf("==   host: %s port number: %d\n", address.getHost().c_str(),
                   address.getPort());
            printf("==   namespace: %s\n", nc.getNamespace().c_str());
            if (conf.getMode()!="" && conf.getMode()!="//") {
                printf("==   type of name server: %s\n", conf.getMode().c_str());
            }
            printf("== But such a name server was not found.\n");
        } else {
            printf("== No address for a YARP name server is available.\n");
            printf("== A configuration file giving the location of the \n");
            printf("== YARP name server is required, but was not found.\n");
        }
        printf("==\n");
        printf("== SHORT SOLUTION\n");
        printf("== If you are fairly confident there is a name server running, try:\n");
        printf("== $ yarp detect --write\n");
        printf("== If you just want to make a quick test, start your own name server:\n");
        printf("== $ yarp namespace /your/name\n");
        printf("== $ yarp server\n");
        printf("==\n");
        printf("== DETAILED SOLUTION\n");
        printf("== To try to fix this problem automatically, do:\n");
        printf("== $ yarp detect --write\n");
        printf("== This will search your network for a nameserver\n");
        printf("== and then write the result to a configuration file.\n");
        printf("== If you know the address of the name server, you\n");
        printf("== can bypass this search by doing:\n");
        printf("== $ yarp conf [ip address] [port number]\n");
        printf("== If you would like to search the network for a\n");
        printf("== nameserver but *not* automatically update the\n");
        printf("== configuration file, do:\n");
        printf("== $ yarp detect\n");
        printf("== Or to determine the name of the required\n");
        printf("== configuration file for manual viewing/editing, do:\n");
        printf("== $ yarp conf\n");
        printf("==\n");
        printf("=======================================================================\n");

        return 1;
    }
    return 0;
}

int Companion::cmdHelp(int argc, char *argv[]) {
    ACE_OS::printf("Usage:\n");
    ACE_OS::printf("  <yarp> [verbose] [admin] command arg1 arg2 ...\n");
    ACE_OS::printf("Here are commands you can use:\n");
    for (unsigned i=0; i<names.size(); i++) {
        String name = names[i];
        const String& tip = tips[i];
        while (name.length()<12) {
            name += " ";
        }
        ACE_OS::printf("%s ", name.c_str());
        ACE_OS::printf("%s\n", tip.c_str());
    }
    return 0;
}


int Companion::cmdVersion(int argc, char *argv[]) {
    ACE_OS::printf("YARP version %s\n",
                   version().c_str());
    return 0;
}


int Companion::sendMessage(const String& port, PortWriter& writable,
                           String& output, bool quiet) {
    output = "";
    Contact srcAddress = NetworkBase::queryName(port.c_str());
    if (!srcAddress.isValid()) {
        if (!quiet) {
            ACE_OS::fprintf(stderr, "Cannot find port named %s\n", port.c_str());
        }
        return 1;
    }
    OutputProtocol *out = Carriers::connect(srcAddress);
    if (out==NULL) {
        if (!quiet) {
            ACE_OS::fprintf(stderr, "Cannot connect to port named %s at %s\n",
                            port.c_str(),
                            srcAddress.toURI().c_str());
        }
        return 1;
    }
    Route route("admin",port,"text");


    bool ok = out->open(route);
    if (!ok) {
        if (!quiet) ACE_OS::fprintf(stderr, "Cannot make connection\n");
        if (out!=NULL) delete out;
        return 1;
    }

    BufferedConnectionWriter bw(out->getConnection().isTextMode());
    PortCommand disconnect('\0',"q");
    bool wok = writable.write(bw);
    if (!wok) {
        if (!quiet) ACE_OS::fprintf(stderr, "Cannot write on connection\n");
        if (out!=NULL) delete out;
        return 1;
    }
    if (!disconnect.write(bw)) {
        if (!quiet) ACE_OS::fprintf(stderr, "Cannot write on connection\n");
        if (out!=NULL) delete out;
        return 1;
    }

    out->write(bw);
    InputProtocol& ip = out->getInput();
    ConnectionReader& con = ip.beginRead();
    Bottle b;
    b.read(con);
    b.read(con);
    output = b.toString().c_str();
    if (!quiet) {
        //ACE_OS::fprintf(stderr,"%s\n", b.toString().c_str());
        YARP_SPRINTF1(Logger::get(),info,"%s", b.toString().c_str());
    }
    ip.endRead();
    out->close();
    delete out;
    out = NULL;

    return 0;
}


int Companion::cmdConnect(int argc, char *argv[]) {
    //int argc_org = argc;
    //char **argv_org = argv;
    bool persist = false;
    const char *mode = NULL;
    if (argc>0) {
        ConstString arg = argv[0];
        if (arg=="--persist") {
            persist = true;
        } else if (arg=="--persist-from") {
            persist = true;
            mode = "from";
        } else if (arg=="--persist-to") {
            persist = true;
            mode = "to";
        } else if (arg=="--list-carriers") {
            Bottle lst = Carriers::listCarriers();
            for (int i=0; i<lst.size(); i++) {
                printf("%s%s",(i>0)?" ":"",lst.get(i).asString().c_str());
            }
            printf("\n");
            return 0;
        } else if (arg=="--help") {
            printf("USAGE:\n\n");
            printf("yarp connect OUTPUT_PORT INPUT_PORT\n");
            printf("yarp connect OUTPUT_PORT INPUT_PORT CARRIER\n");
            printf("  Make a connection between two ports, which must both exist at the time the\n");
            printf("  connection is requested.  The connection will be terminated when either\n");
            printf("  port is closed.\n");
            printf("\n");
            printf("yarp connect --persist OUTPUT_PORT INPUT_PORT\n");
            printf("yarp connect --persist OUTPUT_PORT INPUT_PORT CARRIER\n");
            printf("  Ask the name server to make connections whenever the named ports are available.\n");
            printf("\n");
            printf("yarp connect --persist-from OUTPUT_PORT INPUT_PORT\n");
            printf("  Ask the name server to connect the OUTPUT_PORT, which must\n");
            printf("  exist at the time the connection is requested, and the INPUT_PORT\n");
            printf("  whenever it is available. The request expires when OUTPUT_PORT is closed.\n");
            printf("\n");
            printf("yarp connect --persist-to OUTPUT_PORT INPUT_PORT\n");
            printf("  Ask the name server to connect the OUTPUT_PORT whenever available to the\n");
            printf("  INPUT_PORT which exists at the time the connection is requested.  The \n");
            printf("  request expires when INPUT_PORT is closed.\n");
            printf("yarp connect --list-carriers\n");
            printf("  List carriers available for connections.\n");
            return 0;
        }
        if (persist) {
            argv++;
            argc--;
        }
    } else {
        fprintf(stderr,"[get help with 'yarp connect --help']\n");
    }
    if (argc<2||argc>3) {
        if (persist&&argc<2) {
            if (argc==0) {
                return subscribe(NULL,NULL);
            } else {
                return subscribe(argv[0],NULL);
            }
        }
        if (argc<2) {
            if (argc==0) {
                return subscribe(NULL,NULL);
            } else {
                int result = ping(argv[0],true);
                int result2 = subscribe(argv[0],NULL);
                return (result==0)?result2:result;
            }
        }
        ACE_OS::fprintf(stderr, "Currently must have two/three arguments, a sender port and receiver port (and an optional protocol)\n");
        return 1;
    }

    const char *src = argv[0];
    String dest = argv[1];
    if (argc>=3) {
        const char *proto = argv[2];
        dest = String(proto) + ":/" + slashify(dest);
    }

    if (persist) {
        return subscribe(src,dest.c_str(),mode);
    }

    return connect(src,dest.c_str(),false);
}


int Companion::cmdDisconnect(int argc, char *argv[]) {
    bool persist = false;
    if (argc>0) {
        if (ConstString(argv[0])=="--persist") {
            persist = true;
            argv++;
            argc--;
        }
    }
    if (argc!=2) {
        ACE_OS::fprintf(stderr, "Must have two arguments, a sender port and receiver port\n");
        return 1;
    }

    const char *src = argv[0];
    const char *dest = argv[1];
    if (persist) {
        return unsubscribe(src,dest);
    }
    return disconnect(src,dest);
}



int Companion::cmdRead(int argc, char *argv[]) {
    if (argc<1) {
        ACE_OS::fprintf(stderr, "Please supply the port name\n");
        return 1;
    }

    const char *name = argv[0];
    const char *src = NULL;
    bool showEnvelope = false;
    while (argc>1) {
        if (strcmp(argv[1],"envelope")==0) {
            showEnvelope = true;
        } else {
            src = argv[1];
        }
        argc--;
        argv++;
    }
    return read(name,src,showEnvelope);
}


int Companion::cmdWrite(int argc, char *argv[]) {
    if (argc<1) {
        ACE_OS::fprintf(stderr, "Please supply the port name, and optionally some targets\n");
        return 1;
    }

    const char *src = argv[0];
    return write(src,argc-1,argv+1);
}


int Companion::cmdRpcServer(int argc, char *argv[]) {
    if (argc<1) {
        ACE_OS::fprintf(stderr, "Please supply port name\n");
        return 1;
    }
    bool drop = false;
    if (String(argv[0])=="--single") {
        drop = true;
        argv++;
        argc--;
    }

    const char *name = argv[0];

    Port port;
    companion_install_handler();
    port.setRpcServer();
    applyArgs(port);
    port.open(name);
#ifndef YARP2_WINDOWS
    companion_unregister_name = name;
#endif

    while (true) {
        printf("Waiting for a message...\n");
        Bottle cmd;
        Bottle response;
        port.read(cmd,true);
        printf("Message: %s\n", cmd.toString().c_str());
        printf("Reply: ");
        String txt = getStdin();
        response.fromString(txt.c_str());
        if (drop) {
            port.replyAndDrop(response);
        } else {
            port.reply(response);
        }
    }
}


int Companion::cmdRpc(int argc, char *argv[]) {
    if (argc<1) {
        ACE_OS::fprintf(stderr, "Please supply remote port name\n");

        ACE_OS::fprintf(stderr, "(and, optionally, a name for this connection or port)\n");
        ACE_OS::fprintf(stderr, "You can also do \"yarp rpc --client /port\" to make a port for connecting later\n");
        return 1;
    }

    const char *dest = argv[0];
    const char *src;
    if (String(dest)=="--client") {
        return cmdRpc2(argc,argv);
    }
    Contact address = Name(dest).toAddress();
    if (address.getCarrier()=="") {
        address = NetworkBase::queryName(dest);
    }
    // no need for a port
    src = "anon_rpc";
    if (argc>1) { src = argv[1]; }
    return rpc(src,dest);
}


int Companion::cmdRpc2(int argc, char *argv[]) {
    if (argc<1) {
        ACE_OS::fprintf(stderr, "Please supply remote port name, and local port name\n");
        return 1;
    }

    const char *dest = argv[0];
    const char *src = "...";
    if (argc>1) {
        src = argv[1];
    }

    Port p;
    applyArgs(p);
    bool ok;
    if (argc>1) {
        ok = p.open(src);
    } else {
        ok = p.open();
    }
    if (ok) {
        if (String(dest)!="--client") {
            //NetworkBase::connect(p.getName().c_str(),dest);
            ok = p.addOutput(dest);
        }
    }
    while(ok) {
        String txt = getStdin();
        if (EOFreached()) {
            break;
        }
        Bottle cmd(txt.c_str()),reply;
        ok = p.write(cmd,reply);
        if (ok) {
            printf("%s\n", reply.toString().c_str());
        }
    }
    return 0;
}


int Companion::cmdForward(int argc, char *argv[]) {
    if (argc<2) {
        ACE_OS::fprintf(stderr, "Please supply local and remote port name\n");
        return 1;
    }

    const char *src = argv[0];
    const char *dest = argv[1];

    return forward(src,dest);
}


int Companion::cmdRegression(int argc, char *argv[]) {
    ACE_OS::fprintf(stderr,"no regression tests linked in this version\n");
    return 1;
}


int Companion::cmdServer(int argc, char *argv[]) {
    // Note: if YARP is compiled with a "persistent name server",
    // then the command "yarp server" will be intercepted here:
    //   [YARP root]/src/yarp/yarp.cpp
    return NameServer::main(argc,argv);
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class CompanionCheckHelper : public PortReader {
public:
    BottleImpl bot;
    bool got;
    virtual bool read(yarp::os::ConnectionReader& reader) {
        bot.read(reader);
        got = true;
        return true;
    }
    BottleImpl *get() {
        if (got) {
            return &bot;
        }
        return NULL;
    }
};

class TextReader : public PortReader {
public:
    ConstString str;
    virtual bool read(yarp::os::ConnectionReader& reader) {
        str = reader.expectText();
        return reader.isValid();
    }

};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


int Companion::cmdCheck(int argc, char *argv[]) {
    Logger& log = Logger::get();

    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== This is \"yarp check\"");
    YARP_INFO(log,"=== It is a very simple sanity check for your installation");
    YARP_INFO(log,"=== If it freezes, try deleting the file reported by \"yarp conf\"");
    YARP_INFO(log,"=== Also, if you are mixing terminal types, e.g. bash/cmd.exe");
    YARP_INFO(log,"=== on windows, make sure the \"yarp conf\" file is the same on each");
    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== Trying to register some ports");

    CompanionCheckHelper check;
    Port in;
    bool faking = false;
    if (!NetworkBase::exists(NetworkBase::getNameServerName())) {
        YARP_INFO(log,"=== NO NAME SERVER!  Switching to local, fake mode");
        NetworkBase::setLocalMode(true);
        faking = true;
    }
    in.setReader(check);
    in.open("...");
    Port out;
    out.open("...");

    Time::delay(1);

    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== Trying to connect some ports");

    connect(out.getName().c_str(),in.getName().c_str());

    Time::delay(1);

    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== Trying to write some data");

    Bottle bot;
    bot.addInt(42);
    out.write(bot);

    Time::delay(1);

    YARP_INFO(log,"==================================================================");
    bool ok = false;
    for (int i=0; i<3; i++) {
        YARP_INFO(log,"=== Trying to read some data");
        Time::delay(1);
        if (check.get()!=NULL) {
            int x = check.get()->getInt(0);
            char buf[256];
            ACE_OS::sprintf(buf, "*** Read number %d", x);
            YARP_INFO(log,buf);
            if (x==42) {
                ok = true;
                break;
            }
        }
    }
    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== Trying to close some ports");
    in.close();
    out.close();
    Time::delay(1);
    if (!ok) {
        YARP_INFO(log,"*** YARP seems broken.");
        //diagnose();
        return 1;
    } else {
        if (faking) {
            YARP_INFO(log,"*** YARP seems okay, but there is no name server available.");
        } else {
            YARP_INFO(log,"*** YARP seems okay!");
        }
    }
    return 0;
}




int Companion::cmdMake(int argc, char *argv[]) {
    Bottle f;
    f.add("# Generated by \"yarp cmake\"");
    f.add("");
    f.add("# A cmake file to get started with for new YARP projects.");
    f.add("# It assumes you want to build an executable from source code in ");
    f.add("# the current directory.");
    f.add("# Replace \"yarpy\" with whatever your executable should be called.");
    f.add("");
    f.add("cmake_minimum_required(VERSION 2.8.7)");
    f.add("");
    f.add("set(KEYWORD \"yarpy\")");
    f.add("");
    f.add("# Start a project.");
    f.add("project(${KEYWORD})");
    f.add("");
    f.add("# Find YARP.  Point the YARP_DIR environment variable at your build.");
    f.add("find_package(YARP REQUIRED)");
    f.add("list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})");
    f.add("");
    f.add("# Search for source code.");
    f.add("file(GLOB folder_source *.cpp *.cc *.c)");
    f.add("file(GLOB folder_header *.h)");
    f.add("source_group(\"Source Files\" FILES ${folder_source})");
    f.add("source_group(\"Header Files\" FILES ${folder_header})");
    f.add("");
    f.add("# Automatically add include directories if needed.");
    f.add("foreach(header_file ${folder_header})");
    f.add("  get_filename_component(p ${header_file} PATH)");
    f.add("  include_directories(${p})");
    f.add("endforeach(header_file ${folder_header})");
    f.add("");
    f.add("# Inclue any directories needed for YARP");
    f.add("include_directories(${YARP_INCLUDE_DIRS})");
    f.add("");
    f.add("# Set up our main executable.");
    f.add("if (folder_source)");
    f.add("  add_executable(${KEYWORD} ${folder_source} ${folder_header})");
    f.add("  target_link_libraries(${KEYWORD} ${YARP_LIBRARIES})");
    f.add("else (folder_source)");
    f.add("  message(FATAL_ERROR \"No source code files found. Please add something\")");
    f.add("endif (folder_source)");

    const char *target = "CMakeLists.txt";

    FILE *fin = fopen(target,"r");
    if (fin!=NULL) {
        printf("File %s already exists, please remove it first\n", target);
        fclose(fin);
        fin = NULL;
        return 1;
    }

    writeBottleAsFile(target,f);
    printf("Wrote to %s\n", target);
    printf("Run cmake to generate makefiles or project files for compiling.\n");
    return 0;
}


int Companion::cmdRun(int argc, char *argv[]) {
    return Run::main(argc,argv);
}



int Companion::cmdNamespace(int argc, char *argv[]) {
    NameConfig nc;
    if (argc!=0) {
        String fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
        ACE_OS::printf("Setting namespace in: %s\n",fname.c_str());
        ACE_OS::printf("Remove this file to revert to the default namespace (/root)\n");
        Bottle cmd;
        for (int i=0; i<argc; i++) {
            cmd.addString(argv[i]);
        }
        nc.writeConfig(fname,cmd.toString().c_str());
    }

    Bottle ns = nc.getNamespaces();

    //Bottle bot(nc.readConfig(fname).c_str());
    //String space = bot.get(0).asString().c_str();
    if (ns.size()==0) {
        ACE_OS::printf("No namespace specified\n");
    }
    if (ns.size()==1) {
        ACE_OS::printf("YARP namespace: %s\n", ns.get(0).asString().c_str());
    } else {
        ACE_OS::printf("YARP namespaces: %s\n", ns.toString().c_str());
    }
    return 0;
}


int Companion::cmdClean(int argc, char *argv[]) {
    Property options;
    if (argc==0) {
        printf("# If the cleaning process has long delays, you may wish to use a timeout,\n");
        printf("# specifying how long to wait (in seconds) for a test connection to a port:\n");
        printf("#   yarp clean --timeout 5.0\n");
    } else {
        options.fromCommand(argc,argv,false);
    }

    NameConfig nc;
    String name = nc.getNamespace();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");
    printf("Requesting list of ports from name server... ");
    NetworkBase::write(name.c_str(),
                       msg,
                       reply);
    int ct = reply.size()-1;
    printf("got %d port%s\n", ct, (ct!=1)?"s":"");
    double timeout = -1;
    if (options.check("timeout")) {
        timeout = options.find("timeout").asDouble();
        printf("Using a timeout of %g seconds\n", timeout);
    } else {
        printf("No timeout; to specify one, do \"yarp clean --timeout NN.N\"\n");
    }
    for (int i=1; i<reply.size(); i++) {
        Bottle *entry = reply.get(i).asList();
        if (entry!=NULL) {
            ConstString port = entry->check("name",Value("")).asString();
            if (port!="" && port!="fallback" && port!=name.c_str()) {
                Contact c = Contact::byConfig(*entry);
                if (c.getCarrier()=="mcast") {
                    printf("Skipping mcast port %s...\n", port.c_str());
                } else {
                    Contact addr = c;
                    printf("Testing %s at %s\n",
                           port.c_str(),
                           addr.toURI().c_str());
                    if (addr.isValid()) {
                        if (timeout>=0) {
                            addr.setTimeout((float)timeout);
                        }
                        OutputProtocol *out = Carriers::connect(addr);
                        if (out==NULL) {
                            printf("* No response, removing port %s\n", port.c_str());
                            NetworkBase::unregisterName(port.c_str());
                        } else {
                            delete out;
                        }
                    }
                }
            } else {
                if (port!="") {
                    printf("Ignoring %s\n", port.c_str());
                }
            }
        }
    }
    printf("Giving name server a chance to do garbage collection.\n");
    ConstString serverName = NetworkBase::getNameServerName();
    Bottle cmd2("gc"), reply2;
    NetworkBase::write(serverName,cmd2,reply2);
    printf("Name server says: %s\n",reply2.toString().c_str());

    return 0;
}


int Companion::cmdResource(int argc, char *argv[]) {
    if (argc==0) {
        printf("Looks for, and prints the complete path to, resource files.\n");
        printf("Example usage (from RobotCub project):\n");
        printf("   yarp resource --find icub.ini\n");
        printf("   yarp resource --find config --dir\n");
        printf("   yarp resource --icub icub.ini --find icub\n");
        printf("   yarp resource --from config.ini --find icub\n");
        printf("   yarp resource --find icub.ini --verbose 1\n");
        printf("   yarp resource --ICUB_ROOT /path/to/icub --find icub.ini\n");
        printf("   yarp resource --find config.ini --all\n");
        printf("To show what a config file loads as, specify --show\n");
        return 0;
    }
    ResourceFinder rf;
    rf.setVerbose();
    bool ok = rf.configure("",argc,argv,false);
    Bottle result;
    Property p;
    p.fromCommand(argc,argv,false);
    bool dir = p.check("dir");
    bool all = p.check("all");
    if (!p.check("find")) {
        fprintf(stderr,"Please specify a file to find, e.g. --find icub.ini\n");
        return 1;
    }
    if (ok) {
        ResourceFinderOptions opts;
        if (all) {
            opts.duplicateFilesPolicy = ResourceFinderOptions::All;
        }
        if (p.check("type")) {
            opts.resourceType = p.find("type").asString();
        }
        if (all) {
            result = rf.findPaths(p.check("find",Value("test.ini")).asString(),
                                  opts);
        } else {
            if (dir) {
                result.addString(rf.findPath(p.check("find",
                                                     Value("config")).asString(),
                                             opts));
            } else {
                result.addString(rf.findFile(p.check("find",Value("icub.ini")).asString(),
                                             opts));
            }
        }
    } else {
        if (p.check("strict")) {
            return 1;
        }
        fprintf(stderr,"No policy, continuing without search (specify --strict to avoid this)...\n");
        result.addString(p.check("find",Value("config.ini")).asString());;
    }
    printf("%s\n",result.toString().c_str());
    if (p.check("show")) {
        Property p2;
        p2.fromConfigFile(result.get(0).asString().c_str());
        printf(">>> %s\n", p2.toString().c_str());
    }
    return (result.size()>0)?0:1;
}


int Companion::cmdDetectRos(bool write) {
    bool have_xmlrpc = false;
    bool have_tcpros = false;
    Carrier *xmlrpc = Carriers::chooseCarrier("xmlrpc");
    if (xmlrpc!=NULL) {
        have_xmlrpc = true;
        delete xmlrpc;
    }
    Carrier *tcpros = Carriers::chooseCarrier("tcpros");
    if (tcpros!=NULL) {
        have_tcpros = true;
        delete tcpros;
    }
    if (!(have_xmlrpc&&have_tcpros)) {
        fprintf(stderr,"ROS support requires enabling some optional carriers\n");
        fprintf(stderr,"   xmlrpc %s\n", have_xmlrpc?"(already enabled)":"");
        fprintf(stderr,"   tcpros %s\n", have_tcpros?"(already enabled)":"");
        return 1;
    }

    ConstString uri = NetworkBase::getEnvironment("ROS_MASTER_URI");
    if (uri=="") {
        fprintf(stderr,"ROS_MASTER_URI environment variable not set.\n");
        uri = "http://127.0.0.1:11311/";
    }
    Contact root = Contact::fromString(uri).addCarrier("xmlrpc");
    fprintf(stderr,"Trying ROS_MASTER_URI=%s...\n", uri.c_str());
    OutputProtocol *out = Carriers::connect(root);
    bool ok = (out!=NULL);
    if (ok) delete out;
    if (!ok) {
        fprintf(stderr,"Could not reach server.\n");
        return 1;
    } else {
        fprintf(stderr,"Reachable.  Writing.\n");
    }
    NameConfig nc;
    nc.fromFile();
    nc.setAddress(root);
    nc.setMode("ros");
    nc.toFile();
    fprintf(stderr,"Configuration stored.  Testing.\n");
    return cmdWhere(0,NULL);
}


int Companion::cmdDetect(int argc, char *argv[]) {
    //NameConfig nc;
    //NameClient& nic = NameClient::getNameClient2();
    //nc.fromFile();
    //nic.setScan();
    bool shouldUseServer = false;
    bool ros = false;
    if (argc>0) {
        if (String(argv[0])=="--write") {
            //nic.setSave();
            shouldUseServer = true;
        } else if (String(argv[0])=="--ros") {
            ros = true;
        } else {
            YARP_ERROR(Logger::get(), "Argument not understood");
            return 1;
        }
    }
    if (ros) {
        return cmdDetectRos(shouldUseServer);
    }
    bool didScan = false;
    bool didUse = false;
    Contact addr = NetworkBase::detectNameServer(shouldUseServer,
                                                 didScan,
                                                 didUse);
    if (addr.isValid()) {
        printf("Checking for name server at ip %s port %d\n",
               addr.getHost().c_str(),
               addr.getPort());
        printf("If there is a long delay, try:\n");
        printf("  yarp conf --clean\n");
    }
    OutputProtocol *out = Carriers::connect(addr);
    bool ok = (out!=NULL);
    if (ok) delete out;
    if (ok) {
        printf("\n");
        printf("=========================================================\n");
        printf("==\n");
        printf("== FOUND\n");
        printf("== %s is available at ip %s port %d\n",
               addr.getName().c_str(),
               addr.getHost().c_str(), addr.getPort());
        printf("== %s can be browsed at http://%s:%d/\n",
               addr.getName().c_str(),
               addr.getHost().c_str(), addr.getPort());
        if (didScan&&!didUse) {
            printf("== \n");
            printf("== WARNING\n");
            printf("== This address was found by scanning the network, but\n");
            printf("== has not been saved to a configuration file.\n");
            printf("== Regular YARP programs will not be able to use the \n");
            printf("== name server until this address is saved.  To do so:\n");
            printf("==   yarp detect --write\n");
        }
        if (didUse) {
            printf("== \n");
            printf("== Address saved.\n");
            printf("== YARP programs will now be able to use the name server.\n");
        }
        printf("== \n");
        printf("=========================================================\n");
    } else {
        printf("\n");
        printf("=========================================================\n");
        printf("==\n");
        printf("== PROBLEM\n");
        printf("== No valid YARP name server was found.\n");
        printf("==\n");
        printf("== TIPS\n");
        printf("== #1 Make sure a YARP name server is running.\n");
        printf("== A command for starting the server is:\n");
        printf("== $ yarp server\n");
        printf("==\n");
        printf("== #2 Make sure the YARP name server is running in the\n");
        printf("== same namespace as you.  Your namespace is set as:\n");
        printf("==   %s\n", NetworkBase::getNameServerName().c_str());
        printf("== You can change your namespace to /EXAMPLE by doing:\n");
        printf("==   yarp namespace /EXAMPLE\n");
        printf("== You can check your namespace by doing:\n");
        printf("==   yarp namespace\n");
        printf("==\n");
        printf("== #3 Find out the ip address and port number the YARP\n");
        printf("== name server is running on, and do:\n");
        printf("== $ yarp conf [ip address] [port number]\n");
        printf("== This information is printed out when the server is\n");
        printf("== started.\n");
        printf("==\n");
        printf("== #4 To determine the name of the required configuration\n");
        printf("== file for manual viewing/editing, do:\n");
        printf("== $ yarp conf\n");
        printf("== The simplest possible configuration file would look\n");
        printf("== like something this:\n");
        printf("==   192.168.0.1 10000\n");
        printf("==\n");
        printf("=========================================================\n");
        return 1;
    }
    return 0;
}

int Companion::subscribe(const char *src, const char *dest, const char *mode) {
    Bottle cmd, reply;
    cmd.add("subscribe");
    if (src!=NULL) { cmd.add(src); }
    if (dest!=NULL) { cmd.add(dest); }
    if (mode!=NULL) { cmd.add(mode); }
    bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                 cmd,
                                 reply);
    bool fail = reply.get(0).toString()=="fail";
    if (fail) {
        printf("Persistent connection operation failed.\n");
        return 1;
    }
    if (reply.get(0).toString()=="subscriptions") {
        Bottle subs = reply.tail();
        for (int i=0; i<subs.size(); i++) {
            Bottle *b = subs.get(i).asList();
            if (b!=NULL) {
                //Bottle& topic = b->findGroup("topic");
                const char *srcTopic = "";
                const char *destTopic = "";
                //if (topic.get(1).asInt()) srcTopic=" (topic)";
                //if (topic.get(2).asInt()) destTopic=" (topic)";
                printf("Persistent connection %s%s -> %s%s",
                       b->check("src",Value("?")).asString().c_str(),
                       srcTopic,
                       b->check("dest",Value("?")).asString().c_str(),
                       destTopic);
                ConstString mode = b->check("mode",Value("")).asString();
                if (mode!="") {
                    printf(" [%s]", mode.c_str());
                }
                ConstString carrier = b->check("carrier",Value("")).asString();
                if (carrier!="") {
                    printf(" (%s)", carrier.c_str());
                }
                printf("\n");
            }
        }
        if (subs.size()==0) {
            printf("No persistent connections.\n");
        }
    } else if (ok&&reply.get(0).toString()!="ok") {
        printf("This name server does not support persistent connections.\n");
    }
    return 0;
}


int Companion::unsubscribe(const char *src, const char *dest) {
    Bottle cmd, reply;
    cmd.add("unsubscribe");
    cmd.add(src);
    cmd.add(dest);
    NetworkBase::write(NetworkBase::getNameServerContact(),
                       cmd,
                       reply);
    bool ok = reply.get(0).toString()=="ok";
    if (!ok) {
        printf("Unsubscription failed.\n");
    }
    return ok?0:1;
}


int Companion::connect(const char *src, const char *dest, bool silent) {
    bool ok = NetworkBase::connect(src,dest,NULL,silent);
    return ok?0:1;
    /*
    int err = 0;
    String result = "";
    Address srcAddr = Name(src).toAddress();
    Address destAddr = Name(dest).toAddress();
    if (destAddr.getCarrierName()=="" &&
        srcAddr.getCarrierName()!="") {
        // old behavior unhelpful; move modifier from src to dest in
        // this case
        String newDest = srcAddr.getCarrierName() + ":/" + slashify(dest);
        String newSrc = slashify(srcAddr.getRegName());
        PortCommand pc('\0',slashify(newDest));
        err = sendMessage(newSrc.c_str(),pc,result,silent);
    } else {
        // do not risk changing old behavior
        PortCommand pc('\0',slashify(dest));
        err = sendMessage(src,pc,result,silent);
    }
    if (err==0) {
        err = 1;
        //printf("RESULT is %s err is %d\n", result.c_str(), err);
        // specifically test for "Added output..." message
        if (result.length()>0) {
            if (result[0]=='A') {
                err = 0;
            }
        }
    }
    return err;
    */
}


int Companion::poll(const char *target, bool silent) {
    PortCommand pc('\0',"*");
    return sendMessage(target,pc,silent);
}

int Companion::disconnect(const char *src, const char *dest, bool silent) {
    //PortCommand pc('\0',String("!")+dest);
    //return sendMessage(src,pc,silent);
    bool ok = NetworkBase::disconnect(src,dest,silent);
    return ok?0:1;
}

int Companion::disconnectInput(const char *src, const char *dest,
                               bool silent) {
    PortCommand pc('\0',String("~")+dest);
    return sendMessage(src,pc,silent);
}



#ifndef DOXYGEN_SHOULD_SKIP_THIS

// just a temporary implementation until real ports are available
class BottleReader : public PortReader {
private:
    SemaphoreImpl done;
    bool raw;
    bool env;
    Contact address;
public:
    Port core;

    BottleReader(const char *name, bool showEnvelope) : done(0) {
        raw = false;
        env = showEnvelope;
        core.setReader(*this);
        core.setReadOnly();
        if (core.open(name)) {
            companion_active_port = &core;
            address = core.where();
        } else {
            //YARP_ERROR(Logger::get(),"Could not create port");
            done.post();
        }
    }

    void wait() {
        done.wait();
        companion_active_port = NULL;
    }

    void showEnvelope() {
        if (env) {
            Bottle envelope;
            core.getEnvelope(envelope);
            if (envelope.size()>0) {
                ACE_OS::printf("%s ", envelope.toString().c_str());
            }
        }
    }

    virtual bool read(ConnectionReader& reader) {
        BottleImpl bot;
        if (!reader.isValid()) {
            done.post();
            return false;
        }
        if (bot.read(reader)) {
            if (bot.size()==2 && bot.isInt(0) && bot.isString(1) && !raw) {
                int code = bot.getInt(0);
                if (code!=1) {
                    showEnvelope();
                    ACE_OS::printf("%s\n", bot.getString(1).c_str());
                    ACE_OS::fflush(stdout);
                }
                if (code==1) {
                    done.post();
                }
            } else {
                // raw = true; // don't make raw mode "sticky"
                showEnvelope();
                ACE_OS::printf("%s\n", bot.toString().c_str());
                ACE_OS::fflush(stdout);
            }
            return true;
        }
        return false;
    }

    void close() {
        core.close();
    }

    String getName() {
        return address.getRegName();
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



int Companion::cmdReadWrite(int argc, char *argv[])
{
	if (argc<2)
	{
        ACE_OS::fprintf(stderr, "Please supply the read and write port names\n");
        return 1;
    }

	const char *read_port_name=argv[0];
	const char *write_port_name=argv[1];
    const char *verbatim[] = { "verbatim", NULL };

    companion_install_handler();
    BottleReader reader(read_port_name,false);

	int ret = write(write_port_name,1,(char**)&verbatim);

    reader.close();

	return ret;
}


int Companion::cmdTopic(int argc, char *argv[]) {
    int mode = 1;
    if (argc>=1) {
        if (ConstString(argv[0]) == "--remove") {
            mode = -1;
            argc--;
            argv++;
        }
        if (ConstString(argv[0]) == "--list") {
            Bottle cmd, reply;
            cmd.addString("topic");
            bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                         cmd,
                                         reply,
                                         false,
                                         true);
            if (!ok) {
                ACE_OS::fprintf(stderr, "Failed to read topic list\n");
                return 1;
            }
            if (reply.size()==0) {
                printf("No topics\n");
            } else {
                printf("Topics: %s\n", reply.toString().c_str());
            }
            return 0;
        }
    }
	if (argc<1)
	{
        ACE_OS::fprintf(stderr, "Please supply the topic name\n");
        ACE_OS::fprintf(stderr, "(Or: '--list' to list all topics)\n");
        ACE_OS::fprintf(stderr, "(Or: '--remove <topic>' to remove a topic)\n");
        return 1;
    }

    Bottle cmd, reply;
    ConstString act = (mode==1)?"create":"delete";
    cmd.addString((mode==1)?"topic":"untopic");
    if (ConstString(argv[0])!="--list") {
        for (int i=0; i<argc; i++) {
            cmd.addString(argv[i]);
        }
    }
    bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                 cmd,
                                 reply,
                                 false,
                                 true);
    if (ok) {
        ok = reply.get(0).asVocab()==VOCAB2('o','k');
    }
    if (!ok) {
        ACE_OS::fprintf(stderr, "Failed to %s topic %s:\n  %s\n",
                        act.c_str(), argv[0], reply.toString().c_str());
        ACE_OS::fprintf(stderr, "  Topics are only supported by yarpserver3.\n");
        return 1;
    } else {
        ACE_OS::fprintf(stdout, "Topic %s %sd\n", argv[0], act.c_str());
    }

    return ok?0:1;
}



int Companion::read(const char *name, const char *src, bool showEnvelope) {
    companion_install_handler();
    BottleReader reader(name,showEnvelope);
    if (src!=NULL) {
        NetworkBase::connect(src,reader.getName().c_str());
        //reader.core.addOutput(reader.getName().c_str());
    }
    reader.wait();
    reader.close();
    return 0;
}




int Companion::write(const char *name, int ntargets, char *targets[]) {
    Port port;
    applyArgs(port);
    port.setWriteOnly();
    if (companion_active_port==NULL) {
        companion_install_handler();
    }
    if (!port.open(name)) {
        return 1;
    }
#ifndef YARP2_WINDOWS
    companion_unregister_name = port.getName();
#endif
    if (adminMode) {
        port.setAdminMode();
    }

    bool raw = true;
    for (int i=0; i<ntargets; i++) {
        if (String(targets[i])=="verbatim") {
            raw = false;
        } else {
            if (connect(port.getName().c_str(),targets[i],true)!=0) {
                if (connect(port.getName().c_str(),targets[i],false)!=0) {
                    return 1;
                }
            }
        }
    }


    while (!EOFreached()) {
        String txt = getStdin();
        if (!EOFreached()) {
            if (txt.length()>0) {
                if (txt[0]<32 && txt[0]!='\n' &&
                    txt[0]!='\r' && txt[0]!='\t') {
                    break;  // for example, horrible windows ^D
                }
            }
            BottleImpl bot;
            if (!raw) {
                bot.addInt(0);
                bot.addString(txt.c_str());
            } else {
                bot.fromString(txt.c_str());
            }
            //core.send(bot);
            if (waitConnect) {
                double delay = 0.1;
                while (port.getOutputCount()<1) {
                    Time::delay(delay);
                    delay *= 2;
                    if (delay>4) delay = 4;
                }
            }
            port.write(bot);
        }
    }

    if (port.isWriting()) {
        double delay = 0.1;
        while (port.isWriting()) {
            Time::delay(delay);
            delay *= 2;
            if (delay>4) delay = 4;
        }
    }

    companion_active_port = NULL;

    if (!raw) {
        BottleImpl bot;
        bot.addInt(1);
        bot.addString("<EOF>");
        //core.send(bot);
        port.write(bot);
    }

    //core.close();
    //core.join();
    port.close();

    return 0;
}


int Companion::forward(const char *localName, const char *targetName) {
    Port p;
    p.open(localName);
    NetworkBase::connect(localName,targetName);
    while (true) {
        Bottle in, out;
        p.read(in,true);
        p.write(in,out);
        printf("in [%s] out [%s]\n", in.toString().c_str(),
               out.toString().c_str());
        p.reply(out);
    }
    return 0;
}


int Companion::rpc(const char *connectionName, const char *targetName) {
    Bottle resendContent;
    bool resendFlag = false;
    int resendCount = 0;

    bool firstTimeRound = true;
#ifdef WITH_READLINE
    rl_attempted_completion_function = my_completion;
#endif

    while (!EOFreached()) {
        Port port;
        port.openFake(connectionName);
        if (!port.addOutput(targetName)) {
            ACE_OS::fprintf(stderr, "Cannot make connection\n");
            YARP_ERROR(Logger::get(),"Alternative method: precede port name with --client");
            return 1;
        }
        if (adminMode) {
            port.setAdminMode();
        }

        if (!firstTimeRound) {
            printf("Target disappeared, reconnecting...\n");
        }
        firstTimeRound = false;

        if (resendFlag) {
            if (resendCount==3) {
                resendFlag = false;
                resendCount = 0;
            }
        }

#ifdef WITH_READLINE
    rpcHelpPort = &port;
#endif
        while (port.getOutputCount()==1&&!EOFreached()) {
            String txt;
            if (!resendFlag) {
                txt = getStdin();
            }

            if (!EOFreached()) {
                if (txt.length()>0) {
                    if (txt[0]<32 && txt[0]!='\n' &&
                        txt[0]!='\r') {
                        break;  // for example, horrible windows ^D
                    }
                }
                Bottle bot;
                if (!resendFlag) {
                    bot.fromString(txt.c_str());
                } else {
                    bot = resendContent;
                    resendFlag = false;
                }

                Bottle reply;
                bool ok = port.write(bot,reply);
                if (!ok) {
                    resendContent = bot;
                    resendFlag = true;
                    resendCount++;
                    break;
                }
                if (reply.get(0).isVocab() && reply.get(0).asVocab()==VOCAB4('m','a','n','y')) {
                    printf("Responses:\n");
                    Bottle *lst = &reply;
                    int start = 1;
                    if (reply.size()==2 && reply.get(1).isList()) {
                        lst = reply.get(1).asList();
                        start = 0;
                    }
                    for (int i=start; i<lst->size(); i++) {
                        Value& v = lst->get(i);
                        if (v.isString()) {
                            printf("  %s\n", v.asString().c_str());
                        } else {
                            printf("  %s\n", v.toString().c_str());
                        }
                    }
                } else {
                    printf("Response: %s\n", reply.toString().c_str());
                }
                resendCount = 0;
            }
        }
    }

    return 0;
}



String Companion::readString(bool *eof) {
    bool end = false;

    String txt;

    if (!EOFreached()) {
        txt = getStdin();
    }

    if (EOFreached()) {
        end = true;
    } else if (txt.length()>0 && txt[0]<32 && txt[0]!='\n' &&
               txt[0]!='\r') {
        end = true;
    }
    if (end) {
        txt = "";
    }
    if (eof!=NULL) {
        *eof = end;
    }
    return txt;
}

String Companion::version() {
#ifdef YARP_VERSION
    return YARP_VERSION;
#else
    return "2";
#endif
}


int Companion::cmdPlugin(int argc, char *argv[]) {
#ifdef YARP_HAS_ACE
    if (argc<1) {
        printf("To test a specific plugin, do:\n");
        printf("  yarp plugin <pluginname> /path/to/plugin/lib<libraryname>.so\n");
        printf("List of runtime plugins:\n");
        YarpPluginSelector selector;
        selector.scan();
        Bottle lst = selector.getSelectedPlugins();
        if (lst.size()==0) {
            printf("None found.\n");
        }
        for (int i=0; i<lst.size(); i++) {
            Value& options = lst.get(i);
            ConstString name = options.check("name",Value("untitled")).asString();
            ConstString kind = options.check("type",Value("unknown type")).asString();
            SharedLibraryFactory lib;
            YarpPluginSettings settings;
            settings.setSelector(selector);
            settings.readFromSearchable(options,name);
            settings.open(lib);
            ConstString location = lib.getName().c_str();
            if (lib.isValid()) {
                printf("\n");
                printf("%s %s\n", kind.c_str(), name.c_str());
                printf("  %s\n", options.toString().c_str());
                printf("  found by linking to %s\n", lib.getName().c_str());
            }
        }

        return 1;
    }
    SharedLibraryFactory lib;
    Property p;
    YarpPluginSettings settings;
    settings.setVerboseMode(true);
    if (argc>=2) {
        settings.setLibraryMethodName(argv[0],argv[1]);
    } else {
        settings.setPluginName(argv[0]);
    }
    settings.open(lib);
    if (!lib.isValid()) {
        int problem = lib.getStatus();
        switch (problem) {
        case SharedLibraryFactory::STATUS_LIBRARY_NOT_LOADED:
            fprintf(stderr,"cannot load shared library\n");
            break;
        case SharedLibraryFactory::STATUS_FACTORY_NOT_FOUND:
            fprintf(stderr,"cannot find YARP hook in shared library\n");
            break;
        case SharedLibraryFactory::STATUS_FACTORY_NOT_FUNCTIONAL:
            fprintf(stderr,"YARP hook in shared library misbehaved\n");
            break;
        default:
            fprintf(stderr,"Unknown error\n");
            break;
        }
        return 1;
    }
    printf("Yes, this is a YARP plugin\n");
    const SharedLibraryClassApi& api = lib.getApi();
    char className[256] = "unknown";
    api.getClassName(className,sizeof(className));
    char baseClassName[256] = "unknown";
    api.getBaseClassName(baseClassName,sizeof(baseClassName));
    printf("  * system version %d\n",(int)api.systemVersion);
    printf("  * class name '%s'\n",className);
    printf("  * base class '%s'\n",baseClassName);
    return 0;
#endif
    fprintf(stderr,"Command not available without ACE\n");
    return 1;
}


class CompanionMergeInput : public TypedReaderCallback<Bottle> {
public:
    Contactable *port;
    Semaphore *sema;
    Semaphore mutex;

    Bottle value;
    Stamp stamp;

    CompanionMergeInput() : port(0),
                            sema(0),
                            mutex(1) {
    }

    void init(Contactable& port, Semaphore& sema) {
        this->port = &port;
        this->sema = &sema;
    }

    virtual void onRead(Bottle& datum) {
        mutex.wait();
        value = datum;
        port->getEnvelope(stamp);
        mutex.post();
        sema->post();
    }
};

/**
*

Originally from portsMerge tool in ICUB repository.

Merge the data (bottles) of multiple ports into one single port.

\section intro_sec Description
The user has to specifies a list of streaming ports (i0, i1, ..., iN) that he wants to merge in a single output port (o0).
The module creates a corresponding number of input ports and automatically makes the connections.
The values read from the input ports are merged and synchronized into the output port according to the following rules:
- the data are appended in the ouput vector using the same order in which the input ports are specified by the user, i.e. o0 = [i0 | i1 | ... | iN].
- the timestamp of the output port o0 is assigned by the module.
- if in a time instant no data is received from ALL the input ports (i0, i1, ..., iN), no data is sent on the output port (o0).
- if in a time instant no data is received from SOME ports (e.g. iJ), the output vector will be o0 = [i0 | i1 | ... |iJ-1| ... |in], i.e. the previous sample will be used.
The output of the module can be logged in order to obtain a quick log of multiple data streams on a single file.

\section lib_sec Libraries
YARP libs.

\section parameters_sec Parameters
No parameters.

\section portsa_sec Ports Accessed
The module automatically connects to the ports passed as argument to command line.

\section portsc_sec Ports Created
The module creates multiple input ports to receive data, and one outport port to produce the merged result.

Output ports:
- /portsMerge/o0 the output port

Input ports:
- /portsMerge/i* the input ports (*=1,2...n)

\section tested_os_sec Tested OS
Linux and Windows.

\section example_sec Example Instantiation of the Module

Just run:

\code
yarp merge /icub/left_arm/state:o /icub/left_arm/analog:o
\endcode

the output of the module can be logged on a file:

\code
yarp read ... /portsMerge/o0 envelope &> logfile.txt
\endcode

\author Marco Randazzo

**/
int Companion::cmdMerge(int argc, char *argv[]) {
    BufferedPort<Bottle >   outPort;
    BufferedPort<Bottle >*  inPort = 0;
    CompanionMergeInput *   inData = 0;
    yarp::os::Stamp         outStamp;

    int nPorts = argc;
    if (nPorts == 0) {
        printf("This is yarp merge. Please provide a list of ports to read from, e.g:\n");
        printf("  yarp merge /port1 /port2\n");
        printf("Alternative syntax:\n");
        printf("  yarp merge --input /p1 /p2 --output /p3 --worker /prefix --carrier udp\n");
        return -1;
    }

    Property options;
    options.fromCommand(argc,argv,false);
    Bottle& inputs = options.findGroup("input");
    if (!inputs.isNull()) {
        nPorts = inputs.size()-1;
    }

    inPort  = new BufferedPort<Bottle > [nPorts];
    inData  = new CompanionMergeInput   [nPorts];

    Semaphore product(0);

    //set a callback
    for (int i = 0; i< nPorts; i++) {
        inData[i].init(inPort[i],product);
        inPort[i].useCallback(inData[i]);
    }

    //open the ports
    char buff[255];
    ConstString s = options.check("worker",Value("/portsMerge/i")).asString();
    for (int i = 0; i< nPorts; i++) {
        sprintf(buff,"%s%d", s.c_str(), i);
        inPort[i].open(buff);
    }
    s = options.check("output",Value("/portsMerge/o0")).asString();
    outPort.open(s.c_str());

    //makes the connection
    for (int i=0; i<nPorts; i++) {
        ConstString tmp;
        if (!inputs.isNull()) {
            tmp = inputs.get(i+1).asString();
        } else {
            tmp = argv[i];
        }
        bool b = yarp::os::NetworkBase::connect(tmp.c_str(),inPort[i].getName().c_str(),options.check("carrier",Value("udp")).asString().c_str(),false);
        if (!b) {
            delete [] inPort;
            delete [] inData;
		    return -1;
        }
    }

    printf ("Ready. Output goes to %s\n", outPort.getName().c_str());
    while(true) {
        product.wait();
        while (product.check()) product.wait();

        //write
        outStamp.update();
        if (outPort.getOutputCount()>0) {
            Bottle &out=outPort.prepare();
            out.clear();
            for (int i = 0; i< nPorts; i++) {
                inData[i].mutex.wait();
                out.append(inData[i].value);
                inData[i].mutex.post();
            }
            outPort.setEnvelope(outStamp);
            outPort.write();
        }
    }

    delete [] inPort  ;
    delete [] inData  ;
    return 0;
}



int Companion::cmdSample(int argc, char *argv[]) {
    BufferedPort<Bottle> port;

    Property options;
    options.fromCommand(argc,argv,false);
    if (argc==0 || !((options.check("period")||options.check("rate"))&&options.check("output"))) {
        printf("This is yarp sample. Syntax:\n");
        printf("  yarp sample --output /port --period 0.01\n");
        printf("  yarp sample --output /port --rate 100\n");
        printf("  yarp sample --input /p1 --output /p2 --rate 50 --carrier udp\n");
        printf("  yarp sample --output /port --rate 100 --show\n");
        printf("Data is read from the input port and repeated on the output port at the\n");
        printf("specified rate/period.  If the 'show' flag is given, the data is also printed\n");
        printf("on standard output.\n");
        return 1;
    }

    if (!port.open(options.find("output").asString().c_str())) {
        fprintf(stderr,"Failed to open output port\n");
        return 1;
    }
    if (options.check("period")) {
        port.setTargetPeriod(options.find("period").asDouble());
    }
    if (options.check("rate")) {
        port.setTargetPeriod(1.0/options.find("rate").asDouble());
    }
    if (options.check("input")) {
        ConstString input = options.find("input").asString();
        ConstString carrier = options.find("carrier").asString();
        if (carrier!="") {
            NetworkBase::connect(input.c_str(),port.getName().c_str(),
                                 carrier.c_str());
        } else {
            NetworkBase::connect(input.c_str(),port.getName().c_str());
        }
    }

    bool show = options.check("show");
    while (true) {
        Bottle *bot = port.read();
        if (!bot) continue;
        if (show) {
            printf("%s\n", bot->toString().c_str());
        }
        if (port.getOutputCount()>0) {
            port.prepare() = *bot;
            port.write();
        }
    }

    return 0;
}


void Companion::applyArgs(yarp::os::Contactable& port) {
    if (argType!="") {
        port.promiseType(Type::byNameOnWire(argType.c_str()));
    }
}


int Companion::cmdTime(int argc, char *argv[]) {
    SystemClock clk;
    while (true) {
        printf("%f\n", Time::now());
        clk.delay(0.1);
    }
    return 0;
}
