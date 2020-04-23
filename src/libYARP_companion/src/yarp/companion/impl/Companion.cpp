/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/companion/impl/Companion.h>
#include <yarp/companion/yarpcompanion.h>

#include <yarp/conf/system.h>
#include <yarp/conf/version.h>
#include <yarp/conf/filesystem.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/Name.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Ping.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Terminator.h>
#include <yarp/os/Time.h>
#include <yarp/os/YarpPlugin.h>

#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/Terminal.h>

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>

#if defined(__unix__)
#include <unistd.h>
#include <termios.h>
#elif defined(_MSC_VER)
#include <conio.h>
#endif

using namespace yarp::companion::impl;
using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp;

#ifdef YARP_HAS_Libedit

#include <editline/readline.h>
#include <vector>
static std::vector<std::string> commands;
static yarp::os::Port* rpcHelpPort = nullptr;
static bool commandListInitialized = false;

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
    static size_t list_index, len;
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
        if (rpcHelpPort)
            helpOk = rpcHelpPort->write(helpCommand, helpBottle);
        if (helpOk)
        {
            yarp::os::Bottle* cmdList = nullptr;
            if (helpBottle.get(0).isVocab() && helpBottle.get(0).asVocab()==yarp::os::createVocab('m', 'a', 'n', 'y') )
            {
                cmdList=helpBottle.get(1).asList();
            }
            else
                cmdList=helpBottle.get(0).asList();
            if (cmdList && cmdList->get(0).asString() == "*** Available commands:")
            {
                for (size_t i=1; i<cmdList->size(); ++i)
                    commands.push_back(cmdList->get(i).asString());
            }
        }
        commands.emplace_back(" ");
        commandListInitialized=true;
    }
    while ((list_index<commands.size()) && (name = (char*)commands[list_index].c_str()))
        {
        list_index++;
        if (strncmp (name, text, len) == 0)
            return (dupstr(name));
        }

    /* if no names matched, then return null. */
    return ((char *)nullptr);
}

/* Attempt to complete on the contents of TEXT.  START and END show the
   region of TEXT that contains the word to complete.  We can use the
   entire line in case we want to do some simple parsing.  Return the
   array of matches, or nullptr if there aren't any. */
static char ** my_completion (const char* text, int start, int end)
{
    YARP_UNUSED(end);
    char **matches;
    matches = (char **)nullptr;

    /* If this word is at the start of the line, then it is a command
       to complete. If we are completing after "help ", it is a command again.
       Otherwise, stop completing. */
    if (start == 0)
        matches = rl_completion_matches(text, &command_generator);
    else if (start == 5 && strncmp (text, "help ", 5) != 0)
        matches = rl_completion_matches(text, &command_generator);
    else
        rl_attempted_completion_over=1;

    return (matches);
}

#endif // YARP_HAS_Libedit

Companion& Companion::getInstance()
{
    static Companion instance;
    return instance;
}

static std::string companion_unregister_name;
static Port *companion_active_port = nullptr;

static void companion_sigint_handler(int sig) {
    YARP_UNUSED(sig);
    Time::useSystemClock();
    double now = SystemClock::nowSystem();
    static double firstCall = now;
    static bool showedMessage = false;
    static bool unregistered = false;
    if (now-firstCall<2) {
        Port *port = companion_active_port;
        if (!showedMessage) {
            showedMessage = true;
            yInfo("Interrupting...");
        }
        if (companion_unregister_name!="") {
            if (!unregistered) {
                unregistered = true;
                NetworkBase::unregisterName(companion_unregister_name);
                if (port != nullptr) {
                    NetworkBase::unregisterName(port->getName());
                }
                std::exit(1);
            }
        }
        if (port != nullptr) {
            port->interrupt();
        }
    } else {
        fprintf(stderr, "Aborting...\n");
        std::exit(1);
    }
}

static void companion_sigterm_handler(int sig) {
    companion_sigint_handler(sig);
}

#if defined(_WIN32)
static void companion_sigbreak_handler(int signum)
{
    YARP_UNUSED(signum);
    yarp::os::impl::raise(SIGINT);
}
#else
static void companion_sighup_handler(int signum)
{
    YARP_UNUSED(signum);
    yarp::os::impl::raise(SIGINT);
}
#endif

static void companion_install_handler() {
    yarp::os::impl::signal(SIGINT, companion_sigint_handler);
    yarp::os::impl::signal(SIGTERM, companion_sigterm_handler);

    #if defined(_WIN32)
    yarp::os::impl::signal(SIGBREAK, companion_sigbreak_handler);
    #else
    yarp::os::impl::signal(SIGHUP, companion_sighup_handler);
    #endif
}

static void writeBottleAsFile(const char *fileName, const Bottle& bot) {
    FILE *fout = fopen(fileName, "w");
    if (!fout) {
        return;
    }
    for (size_t i=0; i<bot.size(); i++) {
        fprintf(fout, "%s\n", bot.get(i).toString().c_str());
    }
    fclose(fout);
    fout = nullptr;
}


Companion::Companion() :
    adminMode(false),
    waitConnect(false)
{
    add("check",           &Companion::cmdCheck,          "run a simple sanity check to see if yarp is working");
    add("clean",           &Companion::cmdClean,          "try to remove inactive entries from the name server");
    add("clock",           &Companion::cmdClock,          "creates a server publishing the system time");
    add("cmake",           &Companion::cmdMake,           "create files to help compiling YARP projects");
    add("conf",            &Companion::cmdConf,           "report location of configuration file, and optionally fix it");
    add("connect",         &Companion::cmdConnect,        "create a connection between two ports");
    add("detect",          &Companion::cmdDetect,         "search for the yarp name server");
    add("disconnect",      &Companion::cmdDisconnect,     "remove a connection between two ports");
    add("exists",          &Companion::cmdExists,         "check if a port or connection is alive");
    add("help",            &Companion::cmdHelp,           "get this list");
    add("merge",           &Companion::cmdMerge,          "concatenate input from several ports into a single unit");
    add("name",            &Companion::cmdName,           "send commands to the yarp name server");
    add("namespace",       &Companion::cmdNamespace,      "set or query the name of the yarp name server (default is /root)");
    add("ping",            &Companion::cmdPing,           "get live information about a port");
    add("plugin",          &Companion::cmdPlugin,         "check properties of a YARP plugin (device/carrier)");
    add("priority-qos",    &Companion::cmdPriorityQos,    "set/get the packet priority for a given connection");
    add("read",            &Companion::cmdRead,           "read from the network and print to standard output");
    add("readwrite",       &Companion::cmdReadWrite,      "read from the network and print to standard output, write to the network from standard input");
    add("regression",      &Companion::cmdRegression,     "run regression tests, if linked");
    add("resource",        &Companion::cmdResource,       "locates resource files (see ResourceFinder class)");
    add("rpc",             &Companion::cmdRpc,            "write commands to a port, and read replies");
    add("rpcserver",       &Companion::cmdRpcServer,      "make a test RPC server to receive and reply to Bottle-format messages");
    add("sample",          &Companion::cmdSample,         "drop or duplicate messages to achieve a constant frame-rate");
    add("priority-sched",  &Companion::cmdPrioritySched,  "set/get the thread policy and priority for a given connection");
    add("terminate",       &Companion::cmdTerminate,      "terminate a yarp-terminate-aware process by name");
    add("time",            &Companion::cmdTime,           "show the time");
    add("topic",           &Companion::cmdTopic,          "set a topic name");
    add("version",         &Companion::cmdVersion,        "get version information");
    add("wait",            &Companion::cmdWait,           "wait for a port to be alive");
    add("where",           &Companion::cmdWhere,          "report where the yarp name server is running");
    add("write",           &Companion::cmdWrite,          "write to the network from standard input");
}

void Companion::setAdminMode(bool admin)
{
    adminMode = admin;
}

void Companion::add(const char* name,
                    int (Companion::*fn)(int argc, char* argv[]),
                    const char* tip = nullptr)
{
    Entry e(name, fn);
    action[std::string(name)] = e;
    // maintain a record of order of keys
    names.emplace_back(name);
    if (tip != nullptr) {
        tips.emplace_back(tip);
    } else {
        tips.emplace_back("");
    }
}

int Companion::dispatch(const char* name, int argc, char* argv[])
{
    // new logic to handle some global arguments
    char** argv_copy = new char*[argc];
    char** argv_copy_org = argv_copy;
    int argc_copy = argc;
    if (!argv_copy) {
        YARP_SPRINTF0(Logger::get(),
                      error,
                      "Could not copy argument list");
        return 1;
    }
    int at = 0;
    int skip = 0;
    for (int i = 0; i < argc; i++) {
        if (skip > 0) {
            skip--;
            continue;
        }
        std::string arg = argv[i];
        if (arg.find("--") == 0) {
            if (i + 1 < argc) {
                if (arg == "--type") {
                    skip = 1;
                    argType = argv[i + 1];
                    continue;
                }
            }
            if (arg == "--wait-connect") {
                skip = 0;
                waitConnect = true;
                continue;
            }
        }
        argv_copy[at] = argv[i];
        at++;
    }
    argc_copy = at;

    int v = -1;
    auto it = action.find(std::string(name));
    if (it != action.end()) {
        v = (this->*(it->second.fn))(argc_copy, argv_copy);
    } else {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "Could not find command \"%s\"",
                      name);
    }
    delete[] argv_copy_org;
    return v;
}


int yarp::companion::main(int argc, char *argv[]) {
    ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
    if (!rf.isConfigured()) {
        rf.configure(argc, argv);
    }

    // eliminate 0th arg, the program name
    argc--;
    argv++;

    if (argc<=0) {
        printf("This is the YARP network companion.\n");
        printf("Call with the argument \"help\" to see a list of ways to use this program.\n");
        return 0;
    }

    Companion& instance = Companion::getInstance();
    int verbose = 0;
    bool adminMode = false;
    bool more = true;
    while (more && argc>0) {
        more = false;
        std::string s = std::string(argv[0]);
        if (s == std::string("pray")) {
            argc++;
            argv--;
            // "pray" command requires the full command line
            Logger::get().setVerbosity(-1);
            return instance.cmdPray(argc, argv);
        }
        if (s == std::string("verbose")) {
            verbose++;
            argc--;
            argv++;
            more = true;
        }
        if (s == std::string("quiet")) {
            verbose--;
            argc--;
            argv++;
            more = true;
        }
        if (s == std::string("admin")) {
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
        fprintf(stderr, "Please supply a command\n");
        return -1;
    }

    const char *cmd = argv[0];
    argc--;
    argv++;
    instance.setAdminMode(adminMode);
    return instance.dispatch(cmd, argc, argv);
}


int Companion::cmdTerminate(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Asking port %s to quit gracefully\n", argv[0]);
        Terminator::terminateByName(argv[0]);
        return 0;
    }

    printf("Wrong parameter format, please specify a port name as a single parameter to terminate\n");
    return 1;
}


int Companion::cmdPing(int argc, char *argv[]) {
    bool time = false;
    bool rate = false;
    if (argc>=1) {
        while (argv[0][0]=='-') {
            if (std::string(argv[0])=="--time") {
                time = true;
            } else if (std::string(argv[0])=="--rate") {
                rate = true;
            } else {
                yError("Unrecognized option");
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
                SystemClock::delaySystem(0.25);
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
        return ping(targetName, false);
    }
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  yarp ping /port\n");
    fprintf(stderr, "  yarp ping --time /port\n");
    fprintf(stderr, "  yarp ping --rate /port\n");
    return 1;
}

int Companion::ping(const char *port, bool quiet) {

    const char *connectionName = "<ping>";
    OutputProtocol *out = nullptr;

    Contact address = NetworkBase::queryName(port);
    if (!address.isValid()) {
        if (!quiet) {
            YARP_ERROR(Logger::get(), "could not find port");
        }
        return 1;
    }

    if (address.getCarrier()=="tcp") {
        out = Carriers::connect(address);
        if (out == nullptr) {
            YARP_ERROR(Logger::get(), "port found, but cannot connect");
            return 1;
        }
        Route r(connectionName, port, "text_ack");
        bool ok = out->open(r);
        if (!ok) {
            YARP_ERROR(Logger::get(), "could not connect to port");
            return 1;
        }
        OutputStream& os = out->getOutputStream();
        InputStream& is = out->getInputStream();
        StreamConnectionReader reader;

        PortCommand pc(0, "*");
        BufferedConnectionWriter bw(out->getConnection().isTextMode());
        pc.write(bw);
        bw.write(os);
        Bottle resp;
        reader.reset(is, nullptr, r, 0, true);
        bool done = false;
        while (!done) {
            resp.read(reader);
            std::string str = resp.toString();
            if (resp.get(0).asString()!="<ACK>") {
                printf("%s\n", str.c_str());
            } else {
                done = true;
            }
        }
        if (out != nullptr) {
            delete out;
        }
    } else {
        int e = NetworkBase::exists(port, quiet);
        printf("%s %s.\n", port, (e==0)?"exists":"is not responding");
        return e;
    }

    return 0;
}


int Companion::cmdExists(int argc, char *argv[]) {
    if (argc == 1) {
        bool ok = NetworkBase::exists(argv[0], true);
        return ok?0:1;
    }
    if (argc == 2) {
        bool ok = NetworkBase::isConnected(argv[0], argv[1], "", false);
        return ok?0:1;
    }

    fprintf(stderr, "Please specify a port name\n");
    return 1;
}

int Companion::cmdWait(int argc, char *argv[]) {
    if (argc == 1) {
        return NetworkBase::waitPort(argv[0]);
    }
    if (argc == 2) {
        return NetworkBase::waitConnection(argv[0], argv[1]);
    }
    fprintf(stderr, "Please specify a single port name, or a source and destination port name\n");
    return 1;
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

    std::string key = cmd.get(0).asString();
    if (key=="query") {
        Contact result = NetworkBase::queryName(cmd.get(1).asString());
        if (!result.isValid()) {
            fprintf(stderr, "%s not known.\n", cmd.get(1).asString().c_str());
            return 1;
        }
        std::string txt = NameServer::textify(result);
        printf("%s", txt.c_str());
        return 0;
    }
    if (key=="register") {
        std::string portName = cmd.get(1).asString();
        std::string machine = "...";
        std::string carrier = "...";
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
            if (!cmd.get(4).isInt32()) {
                port = 0;
            } else {
                port = cmd.get(4).asInt32();
            }
        }
        Contact result;
        if (spec) {
            result = NetworkBase::registerContact(Contact(portName, carrier, machine, port));
        } else {
            result = NetworkBase::registerName(portName);
        }
        std::string txt = NameServer::textify(result);
        printf("%s", txt.c_str());
        return 0;
    }
    if (key=="unregister") {
        std::string portName = cmd.get(1).asString();
        Contact result;
        result = NetworkBase::unregisterName(portName);
        printf("Unregistered name.\n");
        return 0;
    }


    bool ok = NetworkBase::writeToNameServer(cmd,
                                             reply,
                                             style);
    if (!ok) {
        fprintf(stderr, "Failed to reach name server\n");
        return 1;
    }
    if (reply.size()==1&&reply.get(0).isString()) {
        printf("%s", reply.get(0).asString().c_str());
    } else if (reply.get(0).isVocab() && reply.get(0).asVocab()==yarp::os::createVocab('m', 'a', 'n', 'y')) {
        for (size_t i=1; i<reply.size(); i++) {
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
        printf("%s\n", nc.getConfigFileName().c_str());
        return 0;
    }
    if (argc>=2) {
        nc.fromFile();
        Contact prev = nc.getAddress();
        std::string prevMode = nc.getMode();
        Contact next(argv[0], atoi(argv[1]));
        nc.setAddress(next);
        if (argc>=3) {
            nc.setMode(argv[2]);
        } else {
            nc.setMode("yarp");
        }
        nc.toFile();
        nc.fromFile();
        Contact current = nc.getAddress();
        std::string currentMode = nc.getMode();
        printf("Configuration file:\n");
        printf("  %s\n", nc.getConfigFileName().c_str());
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
        if (std::string(argv[0])=="--clean") {
            nc.toFile(true);
            printf("Cleared configuration file:\n");
            printf("  %s\n", nc.getConfigFileName().c_str());
            return 0;
        }
    }
    printf("Command not understood\n");
    return 1;
}


int Companion::cmdWhere(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
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
        if (out != nullptr) {
            reachable = true;
            out->close();
            delete out;
            out = nullptr;
        }
    }

    if (address.isValid()&&reachable) {
        printf("%sName server %s is available at ip %s port %d\n",
                       nc.getMode()=="ros"?"ROS ":"",
                       nc.getNamespace().c_str(),
                       address.getHost().c_str(), address.getPort());
        if (address.getCarrier()=="tcp") {
            printf("Name server %s can be browsed at http://%s:%d/\n",
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
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    printf("Usage:\n");
    printf("  <yarp> [verbose] [admin] command arg1 arg2 ...\n");
    printf("Here are commands you can use:\n");
    for (unsigned i=0; i<names.size(); i++) {
        std::string name = names[i];
        const std::string& tip = tips[i];
        while (name.length()<12) {
            name += " ";
        }
        printf("%s ", name.c_str());
        printf("%s\n", tip.c_str());
    }
    return 0;
}


int Companion::cmdVersion(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    printf("YARP version %s\n",
                   version().c_str());
    return 0;
}


int Companion::cmdConnect(int argc, char *argv[]) {
    //int argc_org = argc;
    //char **argv_org = argv;
    bool persist = false;
    const char *mode = nullptr;
    if (argc>0) {
        std::string arg = argv[0];
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
            for (size_t i=0; i<lst.size(); i++) {
                printf("%s%s", (i>0)?" ":"", lst.get(i).asString().c_str());
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
        fprintf(stderr, "[get help with 'yarp connect --help']\n");
    }
    if (argc<2||argc>3) {
        if (persist&&argc<2) {
            if (argc==0) {
                return subscribe(nullptr, nullptr);
            } else {
                return subscribe(argv[0], nullptr);
            }
        }
        if (argc<2) {
            if (argc==0) {
                return subscribe(nullptr, nullptr);
            } else {
                int result = ping(argv[0], true);
                int result2 = subscribe(argv[0], nullptr);
                return (result==0)?result2:result;
            }
        }
        fprintf(stderr, "Currently must have two/three arguments, a sender port and receiver port (and an optional protocol)\n");
        return 1;
    }

    const char *src = argv[0];
    std::string dest = argv[1];
    if (argc>=3) {
        const char *proto = argv[2];
        dest = std::string(proto) + ":/" + slashify(dest);
    }

    if (persist) {
        return subscribe(src, dest.c_str(), mode);
    }

    return connect(src, dest.c_str(), false);
}


int Companion::cmdDisconnect(int argc, char *argv[]) {
    bool persist = false;
    if (argc>0) {
        if (std::string(argv[0])=="--persist") {
            persist = true;
            argv++;
            argc--;
        }
    }
    if (argc!=2) {
        fprintf(stderr, "Must have two arguments, a sender port and receiver port\n");
        return 1;
    }

    const char *src = argv[0];
    const char *dest = argv[1];
    if (persist) {
        return unsubscribe(src, dest);
    }
    return disconnect(src, dest);
}



int Companion::cmdRead(int argc, char *argv[]) {
    if (argc<1) {
        fprintf(stderr, "Please supply the port name\n");
        return 1;
    }

    const char *name = argv[0];
    const char *src = nullptr;
    bool showEnvelope = false;
    while (argc>1) {
        if (strcmp(argv[1], "envelope")==0) {
            showEnvelope = true;
        } else {
            src = argv[1];
        }
        argc--;
        argv++;
    }
    return read(name, src, showEnvelope);
}


int Companion::cmdWrite(int argc, char *argv[]) {
    if (argc<1) {
        fprintf(stderr, "Please supply the port name, and optionally some targets\n");
        return 1;
    }

    const char *src = argv[0];
    return write(src, argc-1, argv+1);
}


int Companion::cmdRpcServer(int argc, char *argv[]) {
    bool drop = false;
    bool stop = false;
    bool echo = false;
    while (argc>=1 && std::string(argv[0]).find("--")==0) {
        std::string cmd = argv[0];
        if (cmd=="--single") {
            drop = true;
        } else if (cmd=="--stop") {
            stop = true;
        } else if (cmd=="--echo") {
            echo = true;
        } else {
            fprintf(stderr, "Option not recognized: %s\n", cmd.c_str());
            return 1;
        }
        argv++;
        argc--;
    }
    if (argc<1) {
        fprintf(stderr, "Please call as:\n");
        fprintf(stderr, "  yarp rpcserver [--single] [--stop] [--echo] /port/name\n");
        fprintf(stderr, "By default, this shows commands and waits for user to enter replies. Flags:\n");
        fprintf(stderr, "  --single: respond to only a single command per connection, ROS-style\n");
        fprintf(stderr, "  --stop: stop the server entirely after a single command\n");
        fprintf(stderr, "  --echo: reply with the message received\n");
        return 1;
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
        port.read(cmd, true);
        printf("Message: %s\n", cmd.toString().c_str());
        printf("Reply: ");
        if (echo) {
            response = cmd;
        } else {
            std::string txt = yarp::os::impl::Terminal::getStdin();
            response.fromString(txt);
        }
        if (drop) {
            port.replyAndDrop(response);
        } else {
            port.reply(response);
        }
        if (stop) return 0;
    }
}


int Companion::cmdRpc(int argc, char *argv[]) {
    if (argc<1) {
        fprintf(stderr, "Please supply remote port name\n");

        fprintf(stderr, "(and, optionally, a name for this connection or port)\n");
        fprintf(stderr, "You can also do \"yarp rpc --client /port\" to make a port for connecting later\n");
        return 1;
    }

    const char *dest = argv[0];
    const char *src;
    if (std::string(dest)=="--client") {
        return cmdRpc2(argc, argv);
    }
    Contact address = Name(dest).toAddress();
    if (address.getCarrier()=="") {
        address = NetworkBase::queryName(dest);
    }
    // no need for a port
    src = "anon_rpc";
    if (argc>1) { src = argv[1]; }
    return rpc(src, dest);
}


int Companion::cmdRpc2(int argc, char *argv[]) {
    if (argc<1) {
        fprintf(stderr, "Please supply remote port name, and local port name\n");
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
        ok = p.open("...");
    }
    if (ok) {
        if (std::string(dest)!="--client") {
            //NetworkBase::connect(p.getName().c_str(), dest);
            ok = p.addOutput(dest);
        }
    }
    while(ok) {
        std::string txt = yarp::os::impl::Terminal::getStdin();
        if (yarp::os::impl::Terminal::EOFreached()) {
            break;
        }
        Bottle cmd(txt), reply;
        ok = p.write(cmd, reply);
        if (ok) {
            printf("%s\n", reply.toString().c_str());
        }
    }
    return 0;
}


int Companion::cmdRegression(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    fprintf(stderr, "no regression tests linked in this version\n");
    return 1;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class CompanionCheckHelper : public PortReader {
public:
    BottleImpl bot;
    bool got;
    bool read(yarp::os::ConnectionReader& reader) override {
        bot.read(reader);
        got = true;
        return true;
    }
    BottleImpl *get() {
        if (got) {
            return &bot;
        }
        return nullptr;
    }
};

class TextReader : public PortReader {
public:
    std::string str;
    bool read(yarp::os::ConnectionReader& reader) override {
        str = reader.expectText();
        return reader.isValid();
    }

};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


int Companion::cmdCheck(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    Logger& log = Logger::get();

    YARP_INFO(log, "==================================================================");
    YARP_INFO(log, "=== This is \"yarp check\"");
    YARP_INFO(log, "=== It is a very simple sanity check for your installation");
    YARP_INFO(log, "=== If it freezes, try deleting the file reported by \"yarp conf\"");
    YARP_INFO(log, "=== Also, if you are mixing terminal types, e.g. bash/cmd.exe");
    YARP_INFO(log, "=== on windows, make sure the \"yarp conf\" file is the same on each");
    YARP_INFO(log, "==================================================================");
    YARP_INFO(log, "=== Trying to register some ports");

    CompanionCheckHelper check;
    Port in;
    bool faking = false;
    if (!NetworkBase::exists(NetworkBase::getNameServerName())) {
        YARP_INFO(log, "=== NO NAME SERVER!  Switching to local, fake mode");
        NetworkBase::setLocalMode(true);
        faking = true;
    }
    in.setReader(check);
    in.open("...");
    Port out;
    out.open("...");

    SystemClock::delaySystem(1);

    YARP_INFO(log, "==================================================================");
    YARP_INFO(log, "=== Trying to connect some ports");

    connect(out.getName().c_str(), in.getName().c_str());

    SystemClock::delaySystem(1);

    YARP_INFO(log, "==================================================================");
    YARP_INFO(log, "=== Trying to write some data");

    Bottle bot;
    bot.addInt32(42);
    out.write(bot);

    SystemClock::delaySystem(1);

    YARP_INFO(log, "==================================================================");
    bool ok = false;
    for (int i=0; i<3; i++) {
        YARP_INFO(log, "=== Trying to read some data");
        SystemClock::delaySystem(1);
        if (check.get() != nullptr) {
            int x = check.get()->get(0).asInt32();
            char buf[256];
            sprintf(buf, "*** Read number %d", x);
            YARP_INFO(log, buf);
            if (x==42) {
                ok = true;
                break;
            }
        }
    }
    YARP_INFO(log, "==================================================================");
    YARP_INFO(log, "=== Trying to close some ports");
    in.close();
    out.close();
    SystemClock::delaySystem(1);
    if (!ok) {
        YARP_INFO(log, "*** YARP seems broken.");
        //diagnose();
        return 1;
    } else {
        if (faking) {
            YARP_INFO(log, "*** YARP seems okay, but there is no name server available.");
        } else {
            YARP_INFO(log, "*** YARP seems okay!");
        }
    }
    return 0;
}




int Companion::cmdMake(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    Bottle f;
    f.addString("# Generated by \"yarp cmake\"");
    f.addString("");
    f.addString("# A cmake file to get started with for new YARP projects.");
    f.addString("# It assumes you want to build an executable from source code in ");
    f.addString("# the current directory.");
    f.addString("# Replace \"yarpy\" with whatever your executable should be called.");
    f.addString("");
    f.addString("cmake_minimum_required(VERSION 3.12)");
    f.addString("project(yarpy)");
    f.addString("");
    f.addString("# Find YARP.  Point the YARP_DIR environment variable at your build.");
    f.addString("find_package(YARP REQUIRED)");
    f.addString("");
    f.addString("# Search for source code.");
    f.addString("file(GLOB folder_source *.cpp *.cc *.c)");
    f.addString("file(GLOB folder_header *.h)");
    f.addString("");
    f.addString("# Search for IDL files.");
    f.addString("file(GLOB idl_files *.thrift *.msg *.srv)");
    f.addString("foreach(idl ${idl_files})");
    f.addString("  yarp_idl_to_dir(${idl} ${CMAKE_BINARY_DIR}/idl IDL_SRC IDL_HDR IDL_INCLUDE)");
    f.addString("  set(folder_source ${folder_source} ${IDL_SRC})");
    f.addString("  set(folder_header ${folder_header} ${IDL_HDR})");
    f.addString("  include_directories(${IDL_INCLUDE})");
    f.addString("endforeach()");
    f.addString("");
    f.addString("# Automatically add include directories if needed.");
    f.addString("foreach(header_file ${folder_header})");
    f.addString("  get_filename_component(p ${header_file} DIRECTORY)");
    f.addString("  include_directories(${p})");
    f.addString("endforeach(header_file ${folder_header})");
    f.addString("");
    f.addString("# Set up our main executable.");
    f.addString("if(folder_source)");
    f.addString("  add_executable(${PROJECT_NAME})");
    f.addString("  target_sources(${PROJECT_NAME} PRIVATE ${folder_source} ${folder_header})");
    f.addString("  target_link_libraries(${PROJECT_NAME} PRIVATE ${YARP_LIBRARIES})");
    f.addString("else()");
    f.addString("  message(FATAL_ERROR \"No source code files found. Please add something\")");
    f.addString("endif()");

    const char *target = "CMakeLists.txt";

    FILE *fin = fopen(target, "r");
    if (fin) {
        printf("File %s already exists, please remove it first\n", target);
        fclose(fin);
        fin = nullptr;
        return 1;
    }

    writeBottleAsFile(target, f);
    printf("Wrote to %s\n", target);
    printf("Run cmake to generate makefiles or project files for compiling.\n");
    return 0;
}



int Companion::cmdNamespace(int argc, char *argv[]) {
    NameConfig nc;
    if (argc!=0) {
        std::string fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
        printf("Setting namespace in: %s\n", fname.c_str());
        printf("Remove this file to revert to the default namespace (/root)\n");
        Bottle cmd;
        for (int i=0; i<argc; i++) {
            cmd.addString(argv[i]);
        }
        nc.writeConfig(fname, cmd.toString());
    }

    Bottle ns = nc.getNamespaces();

    //Bottle bot(nc.readConfig(fname).c_str());
    //std::string space = bot.get(0).asString().c_str();
    if (ns.size()==0) {
        printf("No namespace specified\n");
    }
    if (ns.size()==1) {
        printf("YARP namespace: %s\n", ns.get(0).asString().c_str());
    } else {
        printf("YARP namespaces: %s\n", ns.toString().c_str());
    }
    return 0;
}


int Companion::cmdClean(int argc, char *argv[]) {
    Property options;
    if (argc==0) {
        printf("# If the cleaning process has long delays, you may wish to use a timeout, \n");
        printf("# specifying how long to wait (in seconds) for a test connection to a port:\n");
        printf("#   yarp clean --timeout 5.0\n");
    } else {
        options.fromCommand(argc, argv, false);
    }

    NameConfig nc;
    std::string name = nc.getNamespace();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");
    printf("Requesting list of ports from name server... ");
    NetworkBase::write(name,
                       msg,
                       reply);
    int ct = reply.size()-1;
    printf("got %d port%s\n", ct, (ct!=1)?"s":"");
    double timeout = -1;
    if (options.check("timeout")) {
        timeout = options.find("timeout").asFloat64();
    }
    if (timeout <= 0) {
        timeout = -1;
        printf("No timeout; to specify one, do \"yarp clean --timeout NN.N\"\n");
    } else {
        printf("Using a timeout of %g seconds\n", timeout);
    }
    for (size_t i=1; i<reply.size(); i++) {
        Bottle *entry = reply.get(i).asList();
        if (entry != nullptr) {
            std::string port = entry->check("name", Value("")).asString();
            if (port!="" && port!="fallback" && port!=name) {
                Contact c = Contact::fromConfig(*entry);
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
                        if (out == nullptr) {
                            printf("* No response, removing port %s\n", port.c_str());
                            NetworkBase::unregisterName(port);
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
    std::string serverName = NetworkBase::getNameServerName();
    Bottle cmd2("gc"), reply2;
    NetworkBase::write(serverName, cmd2, reply2);
    printf("Name server says: %s\n", reply2.toString().c_str());

    return 0;
}


int Companion::cmdResource(int argc, char *argv[]) {
    if (argc==0) {
        printf("Looks for, and prints the complete path to, resource files.\n");
        printf("Example usage:\n");
        printf("   yarp resource --context context-name --from file-name\n");
        printf("   yarp resource --context context-name --find file-name\n");
        printf("To show what a config file loads as, specify --show\n");
        printf("\n");
        printf("Note that the search through the available contexts complies\n");
        printf("with the current policies, therefore the content of the environment\n");
        printf("variable YARP_ROBOT_NAME might affect the final result\n");
        return 0;
    }
    ResourceFinder rf;
    rf.setVerbose();
    Property p;
    p.fromCommand(argc, argv, false);
    if (p.check("find")) {
        rf.setDefaultConfigFile(p.find("find").asString().c_str());
    }
    bool ok = rf.configure(argc, argv, false);
    if (ok) {
        if (rf.check("show")) {
            printf(">>> %s\n", rf.toString().c_str());
        }
        printf("\"%s\"\n", rf.findFile("from").c_str());
    }
    return (ok?0:1);
}


int Companion::cmdDetectRos(bool write) {
    YARP_UNUSED(write);
    bool have_xmlrpc = false;
    bool have_tcpros = false;
    Carrier *xmlrpc = Carriers::chooseCarrier("xmlrpc");
    if (xmlrpc != nullptr) {
        have_xmlrpc = true;
        delete xmlrpc;
    }
    Carrier *tcpros = Carriers::chooseCarrier("tcpros");
    if (tcpros != nullptr) {
        have_tcpros = true;
        delete tcpros;
    }
    if (!(have_xmlrpc&&have_tcpros)) {
        fprintf(stderr, "ROS support requires enabling some optional carriers\n");
        fprintf(stderr, "   xmlrpc %s\n", have_xmlrpc?"(already enabled)":"");
        fprintf(stderr, "   tcpros %s\n", have_tcpros?"(already enabled)":"");
        return 1;
    }

    std::string uri = NetworkBase::getEnvironment("ROS_MASTER_URI");
    if (uri=="") {
        fprintf(stderr, "ROS_MASTER_URI environment variable not set.\n");
        uri = "http://127.0.0.1:11311/";
    }
    Contact root = Contact::fromString(uri);
    root.setCarrier("xmlrpc");
    fprintf(stderr, "Trying ROS_MASTER_URI=%s...\n", uri.c_str());
    OutputProtocol *out = Carriers::connect(root);
    bool ok = (out != nullptr);
    if (ok) delete out;
    if (!ok) {
        fprintf(stderr, "Could not reach server.\n");
        return 1;
    } else {
        fprintf(stderr, "Reachable.  Writing.\n");
    }
    NameConfig nc;
    nc.fromFile();
    nc.setAddress(root);
    nc.setMode("ros");
    nc.toFile();
    fprintf(stderr, "Configuration stored.  Testing.\n");
    return cmdWhere(0, nullptr);
}


int Companion::cmdDetect(int argc, char *argv[]) {
    //NameConfig nc;
    //NameClient& nic = NameClient::getNameClient2();
    //nc.fromFile();
    //nic.setScan();
    bool shouldUseServer = false;
    bool ros = false;
    if (argc>0) {
        if (std::string(argv[0])=="--write") {
            //nic.setSave();
            shouldUseServer = true;
        } else if (std::string(argv[0])=="--ros") {
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
    bool ok = (out != nullptr);
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
    cmd.addString("subscribe");
    if (src != nullptr) { cmd.addString(src); }
    if (dest != nullptr) { cmd.addString(dest); }
    if (mode != nullptr) { cmd.addString(mode); }
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
        for (size_t i=0; i<subs.size(); i++) {
            Bottle *b = subs.get(i).asList();
            if (b != nullptr) {
                //Bottle& topic = b->findGroup("topic");
                const char *srcTopic = "";
                const char *destTopic = "";
                //if (topic.get(1).asInt32()) srcTopic=" (topic)";
                //if (topic.get(2).asInt32()) destTopic=" (topic)";
                printf("Persistent connection %s%s -> %s%s",
                       b->check("src", Value("?")).asString().c_str(),
                       srcTopic,
                       b->check("dest", Value("?")).asString().c_str(),
                       destTopic);
                std::string mode = b->check("mode", Value("")).asString();
                if (mode!="") {
                    printf(" [%s]", mode.c_str());
                }
                std::string carrier = b->check("carrier", Value("")).asString();
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
    cmd.addString("unsubscribe");
    cmd.addString(src);
    cmd.addString(dest);
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
    bool ok = NetworkBase::connect(src, dest, nullptr, silent);
    return ok?0:1;
}

int Companion::disconnect(const char *src, const char *dest, bool silent) {
    bool ok = NetworkBase::disconnect(src, dest, silent);
    return ok?0:1;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// just a temporary implementation until real ports are available
class BottleReader : public PortReader {
private:
    yarp::os::Semaphore done;
    bool raw;
    bool env;
    Contact address;
public:
    Port core;

    BottleReader() : done(0) {
        raw = false;
        env = false;
        core.setReader(*this);
        core.setReadOnly();
    }

    void open(const char *name, bool showEnvelope) {
        env = showEnvelope;
        if (core.open(name)) {
            companion_active_port = &core;
            address = core.where();
        } else {
            //YARP_ERROR(Logger::get(), "Could not create port");
            done.post();
        }
    }

    void wait() {
        done.wait();
        companion_active_port = nullptr;
    }

    void showEnvelope() {
        if (env) {
            Bottle envelope;
            core.getEnvelope(envelope);
            if (envelope.size()>0) {
                printf("%s ", envelope.toString().c_str());
            }
        }
    }

    bool read(ConnectionReader& reader) override {
        BottleImpl bot;
        if (!reader.isValid()) {
            done.post();
            return false;
        }
        if (bot.read(reader)) {
            if (bot.size()==2 && bot.isInt32(0) && bot.isString(1) && !raw) {
                int code = bot.get(0).asInt32();
                if (code!=1) {
                    showEnvelope();
                    printf("%s\n", bot.get(1).asString().c_str());
                    fflush(stdout);
                }
                if (code==1) {
                    done.post();
                }
            } else {
                // raw = true; // don't make raw mode "sticky"
                showEnvelope();
                printf("%s\n", bot.toString().c_str());
                fflush(stdout);
            }
            return true;
        }
        return false;
    }

    void close() {
        core.close();
    }

    std::string getName() {
        return address.getRegName();
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



int Companion::cmdReadWrite(int argc, char *argv[])
{
    if (argc<2)
    {
        fprintf(stderr, "Please supply the read and write port names\n");
        return 1;
    }

    const char *read_port_name=argv[0];
    const char *write_port_name=argv[1];
    const char *verbatim[] = { "verbatim", nullptr };

    companion_install_handler();
    BottleReader reader;
    reader.open(read_port_name, false);

    int ret = write(write_port_name, 1, (char**)&verbatim);

    reader.close();

    return ret;
}


int Companion::cmdTopic(int argc, char *argv[]) {
    int mode = 1;
    if (argc>=1) {
        if (std::string(argv[0]) == "--remove") {
            mode = -1;
            argc--;
            argv++;
        }
        if (std::string(argv[0]) == "--list") {
            Bottle cmd, reply;
            cmd.addString("topic");
            bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                         cmd,
                                         reply,
                                         false,
                                         true);
            if (!ok) {
                fprintf(stderr, "Failed to read topic list\n");
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
        fprintf(stderr, "Please supply the topic name\n");
        fprintf(stderr, "(Or: '--list' to list all topics)\n");
        fprintf(stderr, "(Or: '--remove <topic>' to remove a topic)\n");
        return 1;
    }

    Bottle cmd, reply;
    std::string act = (mode==1)?"create":"delete";
    cmd.addString((mode==1)?"topic":"untopic");
    if (std::string(argv[0])!="--list") {
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
        ok = reply.get(0).asVocab()==yarp::os::createVocab('o', 'k');
    }
    if (!ok) {
        fprintf(stderr, "Failed to %s topic %s:\n  %s\n",
                        act.c_str(), argv[0], reply.toString().c_str());
    } else {
        fprintf(stdout, "Topic %s %sd\n", argv[0], act.c_str());
    }

    return ok?0:1;
}



int Companion::read(const char *name, const char *src, bool showEnvelope) {
    companion_install_handler();
    BottleReader reader;
    applyArgs(reader.core);
    reader.open(name, showEnvelope);
    if (src != nullptr) {
        ContactStyle style;
        style.quiet = false;
        style.verboseOnSuccess = false;
        NetworkBase::connect(src, reader.getName(), style);
    }
    reader.wait();
    reader.close();
    return 0;
}




int Companion::write(const char *name, int ntargets, char *targets[]) {
    Port port;
    applyArgs(port);
    port.setWriteOnly();
#ifdef YARP_HAS_Libedit
    std::string hist_file;
    bool disable_file_history=false;
    if (yarp::os::impl::isatty(yarp::os::impl::fileno(stdin))) //if interactive mode
    {
        hist_file=yarp::os::ResourceFinder::getDataHome();
        std::string slash{yarp::conf::filesystem::preferred_separator};
        hist_file += slash;
        hist_file += "yarp_write";
        if (yarp::os::mkdir_p(hist_file.c_str(), 1) != 0)
        {
            fprintf(stderr, "Unable to create directory into \"%s\"\n",
                    yarp::os::ResourceFinder::getDataHome().c_str());
            return 1;
        }
        std::string temp;
        if (ntargets>0) {
            temp = targets[0];
        } else {
            temp = "any";
        }
        std::replace(temp.begin(), temp.end(), '/', '_');
        hist_file += slash;
        hist_file += temp;
        read_history(hist_file.c_str());
        disable_file_history=false;
    }
    else
        disable_file_history=true;
#endif
    if (companion_active_port == nullptr) {
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
        if (std::string(targets[i])=="verbatim") {
            raw = false;
        } else {
            if (connect(port.getName().c_str(), targets[i], true)!=0) {
                if (connect(port.getName().c_str(), targets[i], false)!=0) {
                    return 1;
                }
            }
        }
    }


    while (!yarp::os::impl::Terminal::EOFreached()) {
        std::string txt = yarp::os::impl::Terminal::getStdin();
        if (!yarp::os::impl::Terminal::EOFreached()) {
            if (txt.length()>0) {
                if (txt[0]<32 && txt[0]!='\n' &&
                    txt[0]!='\r' && txt[0]!='\t') {
                    break;  // for example, horrible windows ^D
                }
            }
            Bottle bot;
            if (!raw) {
                bot.addInt32(0);
                bot.addString(txt.c_str());
            } else {
                bot.fromString(txt);
            }
            //core.send(bot);
            if (waitConnect) {
                double delay = 0.1;
                while (port.getOutputCount()<1) {
                    SystemClock::delaySystem(delay);
                    delay *= 2;
                    if (delay>4) delay = 4;
                }
            }
            port.write(bot);
#ifdef YARP_HAS_Libedit
            if (!disable_file_history)
                write_history(hist_file.c_str());
#endif
        }
    }

    if (port.isWriting()) {
        double delay = 0.1;
        while (port.isWriting()) {
            SystemClock::delaySystem(delay);
            delay *= 2;
            if (delay>4) delay = 4;
        }
    }

    companion_active_port = nullptr;

    if (!raw) {
        Bottle bot;
        bot.addInt32(1);
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
    NetworkBase::connect(localName, targetName);
    while (true) {
        Bottle in, out;
        p.read(in, true);
        p.write(in, out);
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
#ifdef YARP_HAS_Libedit
    rl_attempted_completion_function = my_completion;
#endif

    while (!yarp::os::impl::Terminal::EOFreached()) {
        Port port;
        port.openFake(connectionName);
        if (!port.addOutput(targetName)) {
            fprintf(stderr, "Cannot make connection\n");
            YARP_ERROR(Logger::get(), "Alternative method: precede port name with --client");
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

#ifdef YARP_HAS_Libedit
    rpcHelpPort = &port;
#endif
        while (port.getOutputCount()==1&&!yarp::os::impl::Terminal::EOFreached()) {
            std::string txt;
            if (!resendFlag) {
                txt = yarp::os::impl::Terminal::getStdin();
            }

            if (!yarp::os::impl::Terminal::EOFreached()) {
                if (txt.length()>0) {
                    if (txt[0]<32 && txt[0]!='\n' &&
                        txt[0]!='\r') {
                        break;  // for example, horrible windows ^D
                    }
                }
                Bottle bot;
                if (!resendFlag) {
                    bot.fromString(txt);
                } else {
                    bot = resendContent;
                    resendFlag = false;
                }

                Bottle reply;
                bool ok = port.write(bot, reply);
                if (!ok) {
                    resendContent = bot;
                    resendFlag = true;
                    resendCount++;
                    break;
                }
                if (reply.get(0).isVocab() && reply.get(0).asVocab()==yarp::os::createVocab('m', 'a', 'n', 'y')) {
                    printf("Responses:\n");
                    Bottle *lst = &reply;
                    int start = 1;
                    if (reply.size()==2 && reply.get(1).isList()) {
                        lst = reply.get(1).asList();
                        start = 0;
                    }
                    for (size_t i=start; i<lst->size(); i++) {
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

std::string Companion::version() {
    return YARP_VERSION;
}

static void plugin_signal_handler(int) {
   // prevent infinite recursion if say_hi() causes another segfault
    yarp::os::impl::signal(SIGSEGV, SIG_DFL);
    yarp::os::impl::signal(SIGABRT, SIG_DFL);
    throw std::exception();
}

static bool plugin_test(YarpPluginSettings& settings) {
    SharedLibraryFactory lib;
    settings.open(lib);
    if (!lib.isValid()) {
        fprintf(stderr, "    Cannot find or load shared library\n");
        return false;
    } else {
        const SharedLibraryClassApi& api = lib.getApi();
        char className[256] = "unknown";
        api.getClassName(className, sizeof(className));
        char baseClassName[256] = "unknown";
        api.getBaseClassName(baseClassName, sizeof(baseClassName));
        printf("  * library:        %s\n", lib.getName().c_str());
        printf("  * system version: %d\n", (int)api.systemVersion);
        printf("  * class name:     %s\n", className);
        printf("  * base class:     %s\n", baseClassName);

        bool ok = true;
        yarp::os::impl::signal(SIGSEGV, plugin_signal_handler);
        yarp::os::impl::signal(SIGABRT, plugin_signal_handler);
        try {
            void* tmp = api.create();
            api.destroy(tmp);
        } catch (...) {
            printf("\n");
            printf("  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
            printf("  X                                                     X\n");
            printf("  X                       WARNING                       X\n");
            printf("  X                                                     X\n");
            printf("  X            === This plugin is BROKEN ===            X\n");
            printf("  X                                                     X\n");
            printf("  X                                                     X\n");
            printf("  X  Author information: The most plausible reason is   X\n");
            printf("  X  that the destructor is deleting some pointer that  X\n");
            printf("  X  is not allocated in the constructor, and that is   X\n");
            printf("  X  not initialized to a null pointer.                 X\n");
            printf("  X                                                     X\n");
            printf("  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
            ok = false;
        }
        yarp::os::impl::signal(SIGSEGV, SIG_DFL);
        yarp::os::impl::signal(SIGABRT, SIG_DFL);
        return ok;
    }
}

static void plugin_usage()
{
    printf("Print information about installed plugins\n");
    printf("\n");
    printf("Usage:\n");
    printf(" * Test a specific plugin:\n");
    printf("     yarp plugin [--verbose] <pluginname>\n");
    printf("     yarp plugin [--verbose] /path/to/plugin/<libraryname>.(so|dll|dylib) <pluginpart>\n");
    printf(" * Test all the plugins:\n");
    printf("     yarp plugin [--verbose] --all\n");
    printf(" * Print a list of plugins:\n");
    printf("     yarp plugin [--verbose] --list\n");
    printf(" * Print plugin search path:\n");
    printf("     yarp plugin [--verbose] --search-path\n");
    printf(" * Print this help and exit:\n");
    printf("     yarp plugin --help\n");
    printf("\n");
}

int Companion::cmdPlugin(int argc, char *argv[]) {
    if (argc<1) {
        plugin_usage();
        return 1;
    }

    std::string arg = argv[0];
    if (arg=="--help") {
        plugin_usage();
        return 0;
    }

    bool verbose = false;
    if (arg=="--verbose") {
        verbose = true;
        argc--;
        argv++;
        arg = argv[0];
    }

    YarpPluginSelector selector;
    selector.scan();

    if (arg=="--search-path") {
        Bottle lst = selector.getSearchPath();
        if (lst.size()==0) {
            printf("No search path.\n");
            return 1;
        }
        printf("Search path:\n");
        for (size_t i=0; i<lst.size(); i++) {
            Value& options = lst.get(i);
            std::string name = options.asList()->get(0).toString();
            std::string path = options.check("path", Value("unknown path")).asString();
            std::string type = options.check("type", Value("unknown type")).asString();
            if (type == "shared") {
                printf("  %15s:\t%s\n", name.c_str(), path.c_str());
            }
        }
        return 0;
    }

    if (arg=="--list") {
        Bottle lst = selector.getSelectedPlugins();
        for (size_t i=0; i<lst.size(); i++) {
            Value& options = lst.get(i);
            std::string name = options.check("name", Value("untitled")).asString();
            printf("%s\n", name.c_str());
        }
        return 0;
    }
    if (arg=="--all") {
        Bottle lst = selector.getSelectedPlugins();
        if (lst.size()==0) {
            printf("No plugins found.\n");
            return 1;
        }
        printf("Runtime plugins found:\n");
        bool ok = true;
        for (size_t i=0; i<lst.size(); i++) {
            Value& options = lst.get(i);
            std::string name = options.check("name", Value("untitled")).asString();
            std::string type = options.check("type", Value("unknown type")).asString();
            printf("\n");
            printf("%s %s\n", type.c_str(), name.c_str());
            printf("  * ini file:       %s\n", options.find("inifile").toString().c_str());
            options.asList()->pop();
            printf("  * config:         %s\n", options.toString().c_str());
            YarpPluginSettings settings;
            settings.setVerboseMode(verbose);
            settings.setSelector(selector);
            settings.readFromSearchable(options, name);
            ok &= plugin_test(settings);
        }
        return ok ? 0 : 1;
    } else {
        Property p;
        YarpPluginSettings settings;
        settings.setVerboseMode(verbose);
        if (argc>=2) {
            settings.setLibraryMethodName(argv[0], argv[1]);
        } else {
            settings.setPluginName(argv[0]);
        }
        if (!settings.setSelector(selector)) {
            fprintf(stderr, "cannot find a plugin with the specified name\n");
        } else {
            printf("Yes, this is a YARP plugin\n");
        }
        return plugin_test(settings) ? 0 : 1;
    }
}


class CompanionMergeInput : public TypedReaderCallback<Bottle> {
public:
    Contactable *port{nullptr};
    Semaphore *sema{nullptr};
    std::mutex mutex;

    Bottle value;
    Stamp stamp;

    CompanionMergeInput() = default;

    void init(Contactable& port, Semaphore& sema) {
        this->port = &port;
        this->sema = &sema;
    }

    using yarp::os::TypedReaderCallback<Bottle>::onRead;
    void onRead(Bottle& datum) override {
        mutex.lock();
        value = datum;
        port->getEnvelope(stamp);
        mutex.unlock();
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
- the data are appended in the output vector using the same order in which the input ports are specified by the user, i.e. o0 = [i0 | i1 | ... | iN].
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
- /portsMerge/i* the input ports (*=1, 2...n)

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
    BufferedPort<Bottle >*  inPort = nullptr;
    CompanionMergeInput *   inData = nullptr;
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
    options.fromCommand(argc, argv, false);
    Bottle& inputs = options.findGroup("input");
    if (!inputs.isNull()) {
        nPorts = inputs.size()-1;
    }

    inPort  = new BufferedPort<Bottle > [nPorts];
    inData  = new CompanionMergeInput   [nPorts];

    Semaphore product(0);

    //set a callback
    for (int i = 0; i< nPorts; i++) {
        inData[i].init(inPort[i], product);
        inPort[i].useCallback(inData[i]);
    }

    //open the ports
    char buff[255];
    std::string s = options.check("worker", Value("/portsMerge/i")).asString();
    for (int i = 0; i< nPorts; i++) {
        sprintf(buff, "%s%d", s.c_str(), i);
        inPort[i].open(buff);
    }
    s = options.check("output", Value("/portsMerge/o0")).asString();
    outPort.open(s);

    //makes the connection
    for (int i=0; i<nPorts; i++) {
        std::string tmp;
        if (!inputs.isNull()) {
            tmp = inputs.get(i+1).asString();
        } else {
            tmp = argv[i];
        }
        bool b = yarp::os::NetworkBase::connect(tmp.c_str(), inPort[i].getName().c_str(), options.check("carrier", Value("udp")).asString().c_str(), false);
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
                inData[i].mutex.lock();
                out.append(inData[i].value);
                inData[i].mutex.unlock();
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
    options.fromCommand(argc, argv, false);
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

    if (!port.open(options.find("output").asString())) {
        fprintf(stderr, "Failed to open output port\n");
        return 1;
    }
    if (options.check("period")) {
        port.setTargetPeriod(options.find("period").asFloat64());
    }
    if (options.check("rate")) {
        port.setTargetPeriod(1.0/options.find("rate").asFloat64());
    }
    if (options.check("input")) {
        std::string input = options.find("input").asString();
        std::string carrier = options.find("carrier").asString();
        if (carrier!="") {
            NetworkBase::connect(input.c_str(), port.getName().c_str(),
                                 carrier.c_str());
        } else {
            NetworkBase::connect(input, port.getName());
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
    bool ros = false;
    if (argc>0) {
        if (std::string(argv[0])=="--ros") {
            ros = true;
        }
    }
    while (true) {
        double t = Time::now();
        if (ros) {
            int sec = (int) t;
            int nsec = (int)((t-sec)*1e9);
            printf("%d %d\n", sec, nsec);
        } else {
            printf("%f\n", t);
        }
        fflush(stdout);
        clk.delay(0.1);
    }
    return 0;
}

int Companion::cmdClock(int argc, char *argv[])
{
    double init, offset;
    Property config;
    SystemClock clock;
    std::string portName;
    yarp::os::BufferedPort<yarp::os::Bottle> streamPort;

    config.fromCommand(argc, argv, false, true);
    double period = config.check("period", Value(30), "update period, default 30ms").asFloat64() /1000.0;
    double timeFactor = config.check("rtf", Value(1), "real time factor. Upscale or downscale the clock frequency by a multiplier factor. Default 1").asFloat64();
    bool system = config.check("systemTime", "Publish system clock. If false time starts from zero. Default false");
    bool help = config.check("help");

    if(help)
    {
        printf("This command publishes a clock time through a YARP port\n\n");
        printf("Accepted parameters are:\n");
        printf("period:     update period [ms]. Default 30\n");
        printf("name:       name of yarp port to be opened. Default: check YARP_CLOCK environment variable; if missing use '/clock'\n");
        printf("rtf:        realt time factor. Elapsed time will be multiplied by this factor to simulate faster or slower then real time clock frequency. Default 1 (real time)\n");
        printf("systemTime: If present the published time will start at the same value as system clock. If if not present (default) the published time will start from 0. \n");
        printf("help:       print this help\n");
        printf("\n");
        return 1;
    }

    /* Determine clock port name.
     *
     * If the user specify a name, use it.
     * If not, we check the environment variable.
     * If no env variable is present, use the '/clock' as fallback.
     */
    portName = Network::getEnvironment("YARP_CLOCK");
    if(portName == "")
        portName = "/clock";
    portName = config.check("name", Value(portName), "name of port broadcasting the time").asString();

    printf("Clock configuration is the following:\n");
    printf("period %.3f msec\n", period*1000);                  std::fflush(stdout);
    printf("name   %s\n", portName.c_str());                    std::fflush(stdout);
    printf("rtf    %.3f\n", timeFactor);                        std::fflush(stdout);
    printf("system %s\n", system?"true":"false");               std::fflush(stdout);

    if(!streamPort.open(portName) )
    {
        printf("yarp clock error: Cannot open '/clock' port");
        return 1;
    }

    printf("\n\n");                                             std::fflush(stdout);
    double sec, nsec, elapsed;
    double time = clock.now();

    if(system)
    {
        init = time;
        offset = time;
    }
    else
    {
        init = time;
        offset = 0;
    }

    bool done = false;
    while (true)
    {
        elapsed = clock.now() - init;
        time = elapsed * timeFactor + offset;
        Bottle &tick = streamPort.prepare();
        // convert time to sec, nsec
        nsec = std::modf(time, &sec) *1e9;

        tick.clear();
        tick.addInt32((int32_t)sec);
        tick.addInt32((int32_t)nsec);
        streamPort.write();

        if( (((int) elapsed %5) == 0))
        {
            if(!done) {
                printf("yarp clock running happily...\n");
                std::fflush(stdout);
            }
            done = true;
        }
        else
            done = false;

        clock.delay(period);
    }
    return 0;
}


int Companion::cmdPray(int argc, char *argv[])
{
    auto cmdPray_usage = [](){
        printf("Usage:\n");
        printf("yarp pray [port]\n");
    };

    auto cmdPray_getch = []() {
#if defined(__unix__)
        char buf = 0;
        struct termios old;
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (::read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
#elif defined(_MSC_VER)
        return static_cast<char>(_getch());
#else
        return static_cast<char>(getchar());
#endif
    };

    bool palindrome = false;

    auto cmdPray_makePalindrome = [](std::string s) {
        bool newline = false;
        if (s.find('\n') != std::string::npos) {
            newline = true;
            s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        }
        std::string sr = s;
        std::reverse(sr.begin(), sr.end());

        sr = sr.substr(1);
        return s + sr + (newline ? "\n" : "");
    };

    auto cmdPray_printf = [&](const char* format, ...) YARP_ATTRIBUTE_FORMAT(printf, 2, 3)
    {
        va_list args;
        va_start(args, format);
        if (!palindrome) {
            printf(format, args);
        } else {
            constexpr size_t buf_size = 1024;
            char buffer[buf_size];
            vsnprintf(buffer, buf_size, format, args);
            va_end(args);
            auto s = cmdPray_makePalindrome(buffer);
            printf("%s", s.c_str());
        }
    };


    if (argc > 3) {
        cmdPray_usage();
        return 1;
    }

    std::string name;
    bool local = true;
    std::string state;
    if (argc == 3) {
        name = argv[2];
        if (!NetworkBase::exists(name, true)) {
            // Remove initial "/"
            while (name[0] == '/') {
                name = name.substr(1);
            }
            // Keep only the first part of the port name
            auto i = name.find('/');
            if (i != std::string::npos) {
                name = name.substr(0, i);
            }
            state = "not available";
        } else {
            local = false;
        }
    } else {
        if (std::string(argv[0]) == "yarp" && std::string(argv[1]) == "pray") {
            palindrome = true;
            cmdPray_printf("Entering palindrome mode.\n");
            state = "palindrome";
        } else {
            state = "displeased";
        }
        bool found = false;
        name = NetworkBase::getEnvironment("YARP_ROBOT_NAME", &found);
        if (!found) {
            name = "YARPino";
        }
    }

    char c;
    printf("Are you sure you want to pray? [yn] (n) ");
    fflush(stdout);

    do {
        c = cmdPray_getch();
    } while (c != 'y' && c != 'n');
    printf("\n");

    if (c == 'n') {
        return 0;
    }

    if (local) {
        printf("You begin praying to %s.  You finish your prayer.  You feel that %s is %s.\n", name.c_str(), name.c_str(), state.c_str());
        return 0;
    }

    yarp::os::Bottle cmd;
    yarp::os::Bottle reply;
    cmd.addVocab(yarp::os::createVocab('p', 'r', 'a', 'y'));
    yarp::os::NetworkBase::write(name, cmd, reply, true, true);

    bool first = true;
    for (size_t i = 0; i < reply.size(); ++i) {
        if (!first) {
            printf("  ");
        }
        first = false;
        printf("%s", reply.get(i).asString().c_str());
    }
    printf("\n");
    return 0;

}
