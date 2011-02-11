// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010 Paul Fitzpatrick
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
#include <yarp/os/impl/Name.h>

#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/Name.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/NameConfig.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/Terminator.h>
#include <yarp/os/Run.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Ping.h>
#include <yarp/os/SharedLibraryClass.h>

//#include <ace/OS.h>

#include <yarp/conf/system.h>
#ifdef YARP_CMAKE_CONFIG
#include <yarp/conf/version.h>
#else
// we do not have configuration information, disable some features.
#endif

#include <stdio.h>

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
        }
    } else {
        fprintf(stderr,"Aborting...\n");
        exit(1);
    }
}

static void companion_sigterm_handler(int sig) {
    companion_sigint_handler(sig);
}

#if defined(WIN32) || defined(WIN64)
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

    #if defined(WIN32) || defined(WIN64)
    signal(SIGBREAK, (ACE_SignalHandler) companion_sigbreak_handler);
    #else
    signal(SIGHUP, (ACE_SignalHandler) companion_sighup_handler);
    #endif
}

static String getStdin() {
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
    add("server",     &Companion::cmdServer,
        "run yarp name server"); 
    add("terminate",  &Companion::cmdTerminate,
        "terminate a yarp-terminate-aware process by name");
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
    //ACE_OS::printf("Dispatching %s\n", name);
    String sname(name);
    Entry e;
    int result = action.find(sname,e);
    if (result!=-1) {
        return (this->*(e.fn))(argc,argv);
    } else {
        ACE_DEBUG((LM_ERROR,"Could not find command \"%s\"",name));
    }
    return -1;
}


int Companion::main(int argc, char *argv[]) {
    //Network yarp;
    //ACE::init();

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
        if (String(argv[0]) == String("verbose")) {
            verbose++;
            argc--;
            argv++;
            more = true;
        }
        if (String(argv[0]) == String("quiet")) {
            verbose--;
            argc--;
            argv++;
            more = true;
        }
        if (String(argv[0]) == String("admin")) {
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
    
    /*
    } catch (IOException e) {
        YARP_ERROR(Logger::get(),
                   String("exception: ") + e.toString());
        ACE::fini();
        return 1;
    }
    */

    //ACE::fini();
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
    
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.queryName(port);
    if (!address.isValid()) {
        if (!quiet) {
            YARP_ERROR(Logger::get(),"could not find port");
        }
        return 1;
    }
            
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
    BufferedConnectionWriter bw(out->isTextMode());
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



int Companion::exists(const char *target, bool silent) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.queryName(target);
    if (!address.isValid()) {
        if (!silent) {
            printf("Address of port %s is not valid\n", target);
        }
        return 2;
    }
    OutputProtocol *out = Carriers::connect(address);
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
    String cmd = "NAME_SERVER";
    for (int i=0; i<argc; i++) {
        cmd += " ";
        cmd += argv[i];
    }
    NameClient& nic = NameClient::getNameClient();
    String result = nic.send(cmd);
    printf("%s",result.c_str());
    return 0;
}

int Companion::cmdConf(int argc, char *argv[]) {
    NameConfig nc;
    if (argc==0) {
        ACE_OS::printf("%s\n",nc.getConfigFileName().c_str());
        return 0;
    }
    if (argc==2) {
        nc.fromFile();
        Address prev = nc.getAddress();
        Address next(String(argv[0]),atoi(argv[1]));
        nc.setAddress(next);
        nc.toFile();
        nc.fromFile();
        Address current = nc.getAddress();
        printf("Configuration file:\n");
        printf("  %s\n",nc.getConfigFileName().c_str());
        if (prev.isValid()) {
            printf("Stored:\n");
            printf("  host %s port number %d\n",prev.getName().c_str(),
                   prev.getPort());
        }
        if (current.isValid()) {
            printf("Now stores:\n");
            printf("  host %s port number %d\n",current.getName().c_str(),
                   current.getPort());
        } else {
            printf("is not valid!\n");
            printf("Expected:\n");
            printf("  yarp conf [ip address] [port number]\n");
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
    NameClient& nic = NameClient::getNameClient();
    nc.fromFile();
    if (nc.getAddress().isValid()) {
        printf("Looking for name server on %s, port number %d\n",
               nc.getAddress().getName().c_str(),
               nc.getAddress().getPort());
        printf("If there is a long delay, try:\n");
        printf("  yarp conf --clean\n");
    }
    Address address = nic.queryName(nc.getNamespace());
    if (address.isValid()) {
        ACE_OS::printf("Name server %s is available at ip %s port %d\n",
                       nc.getNamespace().c_str(),
                       address.getName().c_str(), address.getPort());
        ACE_OS::printf("Name server %s can be browsed at http://%s:%d/\n",
                       nc.getNamespace().c_str(),
                       address.getName().c_str(), address.getPort());
    } else {
        NameConfig conf;
        bool haveFile = conf.fromFile();
        Address address = conf.getAddress();

        printf("\n");
        printf("=========================================================\n");
        printf("==\n");
        printf("== PROBLEM\n");
        if (haveFile) {
            printf("== No valid address for a YARP name server is available.\n");
            printf("== The following is the configured address:\n");
            printf("==   host %s port number %d\n", address.getName().c_str(),
                   address.getPort());
            printf("== But a name server was not found at this address.\n");
        } else {
            printf("== No address for a YARP name server is available.\n");
            printf("== A configuration file giving the location of the \n");
            printf("== YARP name server is required, but was not found.\n");
        }
        printf("==\n");
        printf("== SHORT SOLUTION\n");
        printf("== Do:\n");
        printf("== $ yarp detect --write\n");
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
        printf("=========================================================\n");

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
    NameClient& nic = NameClient::getNameClient();
    Address srcAddress = nic.queryName(port);
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
                            srcAddress.toString().c_str());
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
    
    //printf("Route %s TEXT mode %d\n", out->getRoute().toString().c_str(),
    // out->isTextMode());
    BufferedConnectionWriter bw(out->isTextMode());
    //bw.appendLine(msg);
    //writable.writeBlock(bw);
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


/*
static int metaConnect(int argc, char *argv[], bool disconnect) {
    // parse arguments
    Property p;
    p.fromCommand(argc,argv,false);
    if (!(p.check("src")&&p.check("dest"))) {
        if (argc>=2) {
            ConstString carrier = "";
            if (argc>=3) {
                if (argv[argc-1][0]!='/') {
                    carrier = argv[argc-1];
                    Contact c = Contact::fromString(carrier);
                    if (c.getCarrier()=="") {
                        argc--;
                    } else {
                        carrier = "";
                    }
                }
            }
            p.fromCommand(argc-2,argv,false);
            p.put("src",argv[argc-2]);
            p.put("dest",argv[argc-1]);
            if (carrier!="") {
                p.put("carrier",carrier.c_str());
            }
        }
    }
    if (disconnect) {
        p.put("disconnect",1);
    }

    // okay, we have our arguments.
    // now, to work
    return metaConnect(p);
}
*/

int Companion::cmdConnect(int argc, char *argv[]) {
    //int argc_org = argc;
    //char **argv_org = argv;
    bool persist = false;
    if (argc>0) {
        if (ConstString(argv[0])=="--persist") {
            persist = true;
            argv++;
            argc--;
        } 
        /*else if (ConstString(argv[0])=="--meta") {
            argv++;
            argc--;
            return metaConnect(argc,argv,false);
        }
        */
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
        return subscribe(src,dest.c_str());
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

    const char *name = argv[0];

    Port port;
    port.open(name);

    while (true) {
        printf("Waiting for a message...\n");
        Bottle cmd;
        Bottle response;
        port.read(cmd,true);
        printf("Message: %s\n", cmd.toString().c_str());
        printf("Reply: ");
        String txt = getStdin();
        response.fromString(txt.c_str());
        port.reply(response);
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
    Address address = Name(dest).toAddress();
    if (address.getCarrierName()=="") {
        NameClient& nic = NameClient::getNameClient();
        address = nic.queryName(dest);
    }
    if (address.getCarrierName()==""||
        address.getCarrierName()=="tcp") {
        // no need for a port
        src = "anon_rpc";
        if (argc>1) { src = argv[1]; }
        return rpc(src,dest);
    }
    return cmdRpc2(argc,argv);

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
    bool ok = p.open(src);
    if (ok) {
        if (String(dest)!="--client") {
            NetworkBase::connect(p.getName().c_str(),dest);
        }
    }
    while(ok) {
        String txt = getStdin();
        if (feof(stdin)) {
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
    NameClient& nic = NameClient::getNameClient();

    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== This is \"yarp check\"");
    YARP_INFO(log,"=== It is a very simple sanity check for your installation");
    YARP_INFO(log,"=== If it freezes, try deleting the file reported by \"yarp conf\"");
    YARP_INFO(log,"=== Also, if you are mixing terminal types, e.g. bash/cmd.exe");
    YARP_INFO(log,"=== on windows, make sure the \"yarp conf\" file is the same on each");
    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== Trying to register some ports");

    CompanionCheckHelper check;
    PortCore in;
    Address address = nic.registerName("...");
    bool faking = false;
    if (!address.isValid()) {
        YARP_INFO(log,"=== NO NAME SERVER!  Switching to local, fake mode");
        NetworkBase::setLocalMode(true);
        address = nic.registerName("...");
        faking = true;
    }
    in.listen(address);
    in.setReadHandler(check);
    in.start();
    PortCore out;
    Address address2 = nic.registerName("...");
    out.listen(address2);
    out.start();

    Time::delay(1);

    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== Trying to connect some ports");

    connect(out.getName().c_str(),in.getName().c_str());

    Time::delay(1);

    YARP_INFO(log,"==================================================================");
    YARP_INFO(log,"=== Trying to write some data");

    BottleImpl bot;
    bot.addInt(42);
    out.send(bot);

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
    f.add("cmake_minimum_required(VERSION 2.6)");
    f.add("");
    f.add("set(KEYWORD \"yarpy\")");
    f.add("");
    f.add("# Start a project.");
    f.add("project(${KEYWORD})");    
    f.add("");
    f.add("# Find YARP.  Point the YARP_DIR environment variable at your build.");
    f.add("find_package(YARP REQUIRED)");
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
    String fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
    if (argc!=0) {
        ACE_OS::printf("Setting namespace in: %s\n",fname.c_str());
        ACE_OS::printf("Remove this file to revert to the default namespace (/root)\n");
        String space = argv[0];
        argc--;
        argv++;
        nc.writeConfig(fname,space);
    }
    Bottle bot(nc.readConfig(fname).c_str());
    String space = bot.get(0).asString().c_str();
    if (space=="") {
        ACE_OS::printf("No namespace specified, using default\n");
        space = "/root";
    }
    ACE_OS::printf("YARP namespace: %s\n",space.c_str());
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

    NameClient& nic = NameClient::getNameClient();
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
                    Address addr = Address::fromContact(c);
                    printf("Testing %s at %s\n", 
                           port.c_str(),
                           addr.toString().c_str());
                    if (addr.isValid()) {
                        if (timeout>=0) {
                            addr.setTimeout((float)timeout);
                        }
                        OutputProtocol *out = Carriers::connect(addr);
                        if (out==NULL) {
                            printf("* No response, removing port %s\n", port.c_str());
                            nic.unregisterName(port.c_str());
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
    String cmd = "NAME_SERVER gc";
    String result = nic.send(cmd);
    printf("Name server says: %s",result.c_str());

    return 0;
}


int Companion::cmdResource(int argc, char *argv[]) {
    if (argc==0) {
        printf("Looks for, and prints the complete path to, resource files.\n");
        printf("Example usage (from RobotCub project):\n");
        printf("   yarp resource --policy ICUB_ROOT --find icub.ini\n");
        printf("   yarp resource --policy ICUB_ROOT --find config --dir\n");
        printf("   yarp resource --policy ICUB_ROOT --icub icub.ini --find icub\n");
        printf("   yarp resource --policy ICUB_ROOT --from config.ini --find icub\n");
        printf("   yarp resource --policy ICUB_ROOT --find icub.ini --verbose 1\n");
        printf("   yarp resource --policy ICUB_ROOT --ICUB_ROOT /path/to/icub --find icub.ini\n");
        printf("If a policy file is required, be sure to specify --strict\n");
        printf("To show what a config file loads as, specify --show\n");
        return 0;
    }
    ResourceFinder rf;
    rf.setVerbose();
    bool ok = rf.configure("",argc,argv,false);
    String result = "";
    Property p;
    p.fromCommand(argc,argv,false);
    bool dir = p.check("dir");
    if (!p.check("find")) {
        fprintf(stderr,"Please specify a file to find, e.g. --find icub.ini\n");
        return 1;
    }
    if (ok) {
        if (dir) {
            result = String(rf.findPath(p.check("find",Value("config")).asString().c_str()));
        } else {
            result = String(rf.findFile(p.check("find",Value("icub.ini")).asString().c_str()));
        }
    } else {
        if (p.check("strict")) {
            return 1;
        }
        fprintf(stderr,"No policy, continuing without search (specify --strict to avoid this)...\n");
        result = p.check("find",Value("config.ini")).asString().c_str();
    }
    printf("%s\n",result.c_str());
    if (p.check("show")) {
        Property p2;
        p2.fromConfigFile(result.c_str());
        printf(">>> %s\n", p2.toString().c_str());
    }
    return (result!="")?0:1;
}



int Companion::cmdDetect(int argc, char *argv[]) {
    NameConfig nc;
    NameClient& nic = NameClient::getNameClient();
    nc.fromFile();
    nic.setScan();
    if (argc>0) {
        if (String(argv[0])=="--write") {
            nic.setSave();
        } else {
            YARP_ERROR(Logger::get(), "Argument not understood");
            return 1;
        }
    }
    if (nc.getAddress().isValid()) {
        printf("Checking for name server at ip %s port %d\n",
               nc.getAddress().getName().c_str(),
               nc.getAddress().getPort());
        printf("If there is a long delay, try:\n");
        printf("  yarp conf --clean\n");
    }
    Address address = nic.queryName(nc.getNamespace());
    if (address.isValid()) {
        printf("\n");
        printf("=========================================================\n");
        printf("==\n");
        printf("== FOUND\n");
        printf("== %s is available at ip %s port %d\n",
               nc.getNamespace().c_str(),
               address.getName().c_str(), address.getPort());
        printf("== %s can be browsed at http://%s:%d/\n",
               nc.getNamespace().c_str(),
               address.getName().c_str(), address.getPort());
        if (nic.didScan()&&!nic.didSave()) {
            printf("== \n");
            printf("== WARNING\n");
            printf("== This address was found by scanning the network, but\n");
            printf("== has not been saved to a configuration file.\n");
            printf("== Regular YARP programs will not be able to use the \n");
            printf("== name server until this address is saved.  To do so:\n");
            printf("==   yarp detect --write\n");
        }
        if (nic.didSave()) {
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
        printf("==   %s\n", nc.getNamespace().c_str());
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

int Companion::subscribe(const char *src, const char *dest) {
    Bottle cmd, reply;
    cmd.add("subscribe");
    if (src!=NULL) { cmd.add(src); }
    if (dest!=NULL) { cmd.add(dest); }
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
    Port core;
    SemaphoreImpl done;
    bool raw;
    bool env;
    Address address;
public:
    BottleReader(const char *name, bool showEnvelope) : done(0) {
        raw = false;
        env = showEnvelope;
        core.setReader(*this);
        if (core.open(name)) {
            companion_active_port = &core;
            Contact contact = core.where();
            address = Address::fromContact(contact);
        } else {
            YARP_ERROR(Logger::get(),"could not create port");
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

    companion_install_handler();
    BottleReader reader(read_port_name,false);
    
    const char *verbatim[] = { "verbatim", NULL };
	int ret = write(write_port_name,1,(char**)&verbatim);

    reader.wait();
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
        cmd.addString(argv[0]);
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
    }
    reader.wait();
    reader.close();
    return 0;
}




int Companion::write(const char *name, int ntargets, char *targets[]) {
    Port port;
    if (companion_active_port==NULL) {
        companion_install_handler();
    }
    companion_unregister_name = name;
    if (!port.open(name)) {
        return 1;
    }
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
    
    
    while (!feof(stdin)) {
        String txt = getStdin();
        if (!feof(stdin)) {
            if (txt[0]<32 && txt[0]!='\n' && 
                txt[0]!='\r' && txt[0]!='\0' && txt[0]!='\t') {
                break;  // for example, horrible windows ^D
            }
            BottleImpl bot;
            if (!raw) {
                bot.addInt(0);
                bot.addString(txt.c_str());
            } else {
                bot.fromString(txt.c_str());
            }
            //core.send(bot);
            port.write(bot);
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

    bool firstTimeRound = true;

    while (!feof(stdin)) {
        NameClient& nic = NameClient::getNameClient();
        Address address = nic.queryName(targetName);
        if (!address.isValid()) {
            YARP_ERROR(Logger::get(),"Could not connect to port.");
            YARP_ERROR(Logger::get(),"If you want to *make* a port, precede port name with --client");
            return 1;
        }
        
        OutputProtocol *out = Carriers::connect(address);
        if (out==NULL) {
            ACE_OS::fprintf(stderr, "Cannot make connection\n");
            YARP_ERROR(Logger::get(),"If you want to *make* a port, precede port name with --client");
            return 1;
        }
        if (!firstTimeRound) {
            printf("Target disappeared, reconnecting...\n");
        }
        firstTimeRound = false;
        printf("RPC connection to %s at %s (connection name %s)\n", targetName, 
               address.toString().c_str(),
               connectionName);
        String carrier = address.getCarrierName();
        Route r(connectionName,targetName,"text_ack");
        out->open(r);
        OutputStream& os = out->getOutputStream();
        InputStream& is = out->getInputStream();
        StreamConnectionReader reader;

        bool err = false;
        while (!err&&!feof(stdin)) {
            String txt = getStdin();
            
            if (!feof(stdin)) {
                if (txt[0]<32 && txt[0]!='\n' && 
                    txt[0]!='\r' && txt[0]!='\0') {
                    break;  // for example, horrible windows ^D
                }
                Bottle bot;
                bot.fromString(txt.c_str());
                
                PortCommand pc(0,adminMode?"a":"d");
                BufferedConnectionWriter bw(out->isTextMode());
                bool ok = pc.write(bw);
                if (!ok) {
                    ACE_OS::fprintf(stderr, "Cannot write on connection\n");
                    if (out!=NULL) delete out;
                    return 1;
                }
                ok = bot.write(bw);
                if (!ok) {
                    ACE_OS::fprintf(stderr, "Cannot write on connection\n");
                    if (out!=NULL) delete out;
                    return 1;
                }
                bw.write(os);
                Bottle resp;
                TextReader formattedResp;
                reader.reset(is,NULL,r,0,true);
                bool done = false;
                bool first = true;
                while (!done) {
                    if (reader.isError()) {
                        err = true;
                        done = true;
                        break;
                    }
                    if (reader.isTextMode()) {
                        formattedResp.read(reader);
                        resp.fromString(formattedResp.str.c_str());
                    } else {
                        resp.read(reader);
                    }
                    if (String(resp.get(0).asString())=="<ACK>") {
                        if (first) {
                            printf("Acknowledged\n");
                        }
                        done = true;
                    } else {
                        ConstString txt;
                        if (reader.isTextMode()) {
                            txt = formattedResp.str;
                        } else {
                            txt = resp.toString().c_str();
                        }
                        printf("Response: %s\n", txt.c_str());
                    }
                    first = false;
                }
            }
        }
        
        if (out!=NULL) {
            delete out;
            out = NULL;
        }
    }
    
    return 0;
}



String Companion::readString(bool *eof) {
    bool end = false;

    String txt;

    if (!feof(stdin)) {
        txt = getStdin();
    }

    if (feof(stdin)) {
        end = true;
    } else if (txt[0]<32 && txt[0]!='\n' && 
               txt[0]!='\r' && txt[0]!='\0') {
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
    if (argc!=1) {
        fprintf(stderr,"please provide filename for shared library\n");
        return 1;
    }
    SharedLibraryFactory lib(argv[0]);
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
    printf("Yes, this is a YARP plugin/carrier\n");
    return 0;
}


