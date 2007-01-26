// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/Companion.h>
#include <yarp/NameClient.h>
#include <yarp/Logger.h>
#include <yarp/PortCommand.h>

#include <yarp/Carriers.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StreamConnectionReader.h>
#include <yarp/PortCore.h>
#include <yarp/BottleImpl.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/NameServer.h>
#include <yarp/NameConfig.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/Terminator.h>

#include <ace/OS.h>

// does ACE require new c++ header files or not?
#if ACE_HAS_STANDARD_CPP_LIBRARY
#include <fstream>
using namespace std;
#else
#include <fstream.h>
#endif

using namespace yarp;
using namespace yarp::os;

Companion Companion::instance;


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
    ofstream fout(fileName);
    for (int i=0; i<bot.size(); i++) {
        fout << bot.get(i).toString().c_str() << endl;
    }
    fout.close();
}


Companion::Companion() {
    add("help",       &Companion::cmdHelp,
        "get this list");
    add("version",    &Companion::cmdVersion,
        "get version information");
    add("where",      &Companion::cmdWhere,
        "report where the yarp name server is running");
    add("conf",       &Companion::cmdConf,
        "report what configuration file is being used");
    add("name",       &Companion::cmdName,
        "send commands to the yarp name server");
    add("connect",    &Companion::cmdConnect,
        "create a connection between two ports");
    add("disconnect", &Companion::cmdDisconnect,
        "remove a connection between two ports");
    add("read",       &Companion::cmdRead,
        "read from the network and print to standard output");
    add("write",      &Companion::cmdWrite,
        "write to the network from standard input");
    add("rpc",        &Companion::cmdRpc,
        "read/write commands to a port, in standard format");
    add("regression", &Companion::cmdRegression,
        "run regression tests, if linked");
    add("server",     &Companion::cmdServer,
        "run yarp name server");
    add("check",      &Companion::cmdCheck,
        "run a simple sanity check to see if yarp is working");
    add("terminate",  &Companion::cmdTerminate,
        "terminate a yarp-terminate-aware process by name");
    add("ping",  &Companion::cmdPing,
        "get live information about a port");
    add("exists",  &Companion::cmdExists,
        "check if a port is alive (useful for conditions in scripts)");
    add("cmake",  &Companion::cmdMake,
        "create files to help compiling YARP projects");
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

    ACE::init();

    try {

        // eliminate 0th arg, the program name
        argc--;
        argv++;

        if (argc<=0) {
            ACE_OS::printf("This is the YARP network companion.\n");
            ACE_OS::printf("Call with the argument \"help\" to see a list of ways to use this program.\n");
            return 0;
        }

        int verbose = 0;
        bool more = true;
        while (more && argc>0) {
            more = false;
            if (String(argv[0]) == String("verbose")) {
                verbose++;
                argc--;
                argv++;
                more = true;
            }
        }
        if (verbose>0) {
            Logger::get().setVerbosity(verbose);
        }

        if (argc<=0) {
            ACE_OS::fprintf(stderr,"Please supply a command\n");
            return -1;
        }

        const char *cmd = argv[0];
        argc--;
        argv++;

        return getInstance().dispatch(cmd,argc,argv);

    } catch (IOException e) {
        YARP_ERROR(Logger::get(),
                   String("exception: ") + e.toString());
        ACE::fini();
        return 1;
    }
    ACE::fini();
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
    if (argc == 1) {
        char *targetName = argv[0];
        char *connectionName = "<ping>";
        OutputProtocol *out = NULL;
        try {
            NameClient& nic = NameClient::getNameClient();
            Address address = nic.queryName(targetName);
            if (!address.isValid()) {
                YARP_ERROR(Logger::get(),"could not find port");
                return 1;
            }
            
            out = Carriers::connect(address);
            if (out==NULL) {
                throw IOException("cannot connect to port");
            }
            /*
            printf("RPC connection to %s at %s (connection name %s)\n", 
                   targetName, 
                   address.toString().c_str(),
                   connectionName);
            */
            Route r(connectionName,targetName,"text_ack");
            out->open(r);
            OutputStream& os = out->getOutputStream();
            InputStream& is = out->getInputStream();
            StreamConnectionReader reader;

            PortCommand pc(0,"*");
            BufferedConnectionWriter bw(out->isTextMode());
            bool ok = pc.write(bw);
            if (!ok) {
                throw IOException("writer failed");
            }
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
        } catch (IOException e) {
            ACE_OS::fprintf(stderr,
                            "write failed: %s\n",e.toString().c_str());    
        }
        if (out!=NULL) {
            delete out;
        }
        return 0;
    }
    ACE_OS::fprintf(stderr,"Please specify a port name\n");
    return 1;
}


int Companion::cmdExists(int argc, char *argv[]) {
    if (argc == 1) {
        NameClient& nic = NameClient::getNameClient();
        Address address = nic.queryName(argv[0]);
        if (!address.isValid()) {
            return 2;
        }
        OutputProtocol *out = Carriers::connect(address);
        if (out==NULL) {
            return 1;
        }
        delete out;
        out = NULL;
        return 0;
    }

    ACE_OS::fprintf(stderr,"Please specify a port name\n");
    return 1;
}



int Companion::cmdName(int argc, char *argv[]) {
    String cmd = "NAME_SERVER";
    for (int i=0; i<argc; i++) {
        cmd += " ";
        cmd += argv[i];
    }
    NameClient& nic = NameClient::getNameClient();
    String result = nic.send(cmd);
    ACE_OS::printf("%s",result.c_str());
    return 0;
}

int Companion::cmdConf(int argc, char *argv[]) {
    NameConfig nc;
    ACE_OS::printf("%s\n",nc.getConfigFileName().c_str());
    return 0;
}


int Companion::cmdWhere(int argc, char *argv[]) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.queryName("/root");
    if (address.isValid()) {
        ACE_OS::printf("Name server is available at ip %s port %d\n",
                       address.getName().c_str(), address.getPort());
        ACE_OS::printf("Name server can be browsed at http://%s:%d/\n",
                       address.getName().c_str(), address.getPort());
    } else {
        ACE_OS::printf("Name server not found\n");
        return 1;
    }
    return 0;
}

int Companion::cmdHelp(int argc, char *argv[]) {
    ACE_OS::printf("Here are arguments you can give this program:\n");
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
    ACE_OS::printf("YARP Companion utility version %s implemented in C++\n", 
                   version().c_str());
    return 0;
}


int Companion::sendMessage(const String& port, Writable& writable, 
                           bool quiet) {
    NameClient& nic = NameClient::getNameClient();
    Address srcAddress = nic.queryName(port);
    //Address srcAddress("localhost",9999,"tcp");
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
    try {
        out->open(route);
        //printf("Route %s TEXT mode %d\n", out->getRoute().toString().c_str(),
        // out->isTextMode());
        BufferedConnectionWriter bw(out->isTextMode());
        //bw.appendLine(msg);
        //writable.writeBlock(bw);
        PortCommand disconnect('\0',"q");
        bool ok = writable.write(bw);
        if (!ok) {
            throw IOException("writer failed");
        }
        if (!disconnect.write(bw)) {
            throw IOException("writer failed");
        }

        out->write(bw);
        InputProtocol& ip = out->getInput();
        ConnectionReader& con = ip.beginRead();
        Bottle b;
        b.read(con);
        b.read(con);
        if (!quiet) {
            ACE_OS::fprintf(stderr,"%s\n", b.toString().c_str());
        }
        ip.endRead();
        out->close();
    } catch (IOException e) {
        YARP_ERROR(Logger::get(),e.toString() + " <<< exception during message");
    }
    delete out;
    out = NULL;
  
    return 0;
}


int Companion::cmdConnect(int argc, char *argv[]) {
    if (argc<2||argc>3) {
        ACE_OS::fprintf(stderr, "Currently must have two/three arguments, a sender port and receiver port (and an optional protocol)\n");
        return 1;
    }

    const char *src = argv[0];
    String dest = argv[1];
    if (argc>=3) {
        const char *proto = argv[2];
        dest = String(proto) + ":/" + slashify(dest);
    }

    // this should not be needed any more
    //disconnectInput(argv[1],src,true); // in connectionless cases

    return connect(src,dest.c_str());
}


int Companion::cmdDisconnect(int argc, char *argv[]) {
    if (argc!=2) {
        ACE_OS::fprintf(stderr, "Must have two arguments, a sender port and receiver port\n");
        return 1;
    }

    const char *src = argv[0];
    const char *dest = argv[1];
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


int Companion::cmdRpc(int argc, char *argv[]) {
    if (argc<1) {
        ACE_OS::fprintf(stderr, "Please supply remote port name\n");
        ACE_OS::fprintf(stderr, "(and, optionally, a name for this connection)\n");
        return 1;
    }

    const char *dest = argv[0];
    const char *src = "anon_rpc";
    if (argc>1) {
        src = argv[1];
    }

    return rpc(src,dest);
}


int Companion::cmdRegression(int argc, char *argv[]) {
    ACE_OS::fprintf(stderr,"no regression tests linked in this version\n");
    return 1;
}


int Companion::cmdServer(int argc, char *argv[]) {
    return NameServer::main(argc,argv);
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class CompanionCheckHelper : public Readable {
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

class TextReader : public Readable {
public:
    ConstString str;
    virtual bool read(yarp::os::ConnectionReader& reader) {
        try {
            str = reader.expectText();
        } catch (IOException e) {
            return false;
        }
        return true;
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
        Network::setLocalMode(true);
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
    f.add("# A fairly generic cmake file to get started with for new YARP projects.");
    f.add("# It assumes you want to build an executable from source code in ");
    f.add("# the current directory.");
    f.add("# Replace \"yarpy\" with whatever your executable should be called.");
    f.add("SET(KEYWORD \"yarpy\")");
    f.add("");
    f.add("# Find YARP.  Point the YARP_BUILD environment variable at your build.");
    f.add("SET(YARP_DIR \"$ENV{YARP_BUILD}\" CACHE LOCATION \"where is yarp?\")");
    f.add("FIND_PACKAGE(YARP)");
    f.add("");
    f.add("# Start a project.");
    f.add("PROJECT(${KEYWORD})");    
    f.add("");
    f.add("# Check if there are any plugin device subdirectories.");
    f.add("# Device subdirectories contain a config.cmake file.");
    f.add("FILE(GLOB_RECURSE device_config config.cmake)");
    f.add("IF (device_config)");
    f.add("  YarpDevice(${device_config})");
    f.add("ENDIF (device_config)");
    f.add("");
    f.add("# Search for source code.");
    f.add("FILE(GLOB_RECURSE folder_source *.cpp *.cc *.c)");
    f.add("FILE(GLOB_RECURSE folder_header *.h)");
    f.add("SOURCE_GROUP(\"Source Files\" FILES ${folder_source})");
    f.add("SOURCE_GROUP(\"Header Files\" FILES ${folder_header})");
    f.add("");
    f.add("# Automatically add include directories if needed.");
    f.add("FOREACH(header_file ${folder_header})");
    f.add("  GET_FILENAME_COMPONENT(p ${header_file} PATH)");
    f.add("  INCLUDE_DIRECTORIES(${p})");
    f.add("ENDFOREACH(header_file ${folder_header})");
    f.add("");
    f.add("# Set up our main executable.");
    f.add("IF (folder_source)");
    f.add("  ADD_EXECUTABLE(${KEYWORD} ${folder_source} ${folder_header})");
    f.add("ELSE (folder_source)");
    f.add("  MESSAGE(FATAL_ERROR \"No source code files found. Please add something\")");
    f.add("ENDIF (folder_source)");

    const char *target = "CMakeLists.txt";

    ifstream fin(target);
    if (!fin.fail()) {
        printf("File %s already exists, please remove it first\n", target);
        fin.close();
        return 1;
    }

    writeBottleAsFile(target,f);
    printf("Wrote to %s\n", target);
    printf("Run cmake to generate makefiles or project files for compiling.\n");
    return 0;
}





int Companion::connect(const char *src, const char *dest, bool silent) {
    PortCommand pc('\0',slashify(dest));
    return sendMessage(src,pc,silent);
}


int Companion::poll(const char *target, bool silent) {
    PortCommand pc('\0',"*");
    return sendMessage(target,pc,silent);
}

int Companion::disconnect(const char *src, const char *dest, bool silent) {
    PortCommand pc('\0',String("!")+dest);
    return sendMessage(src,pc,silent);
}

int Companion::disconnectInput(const char *src, const char *dest,
                               bool silent) {
    PortCommand pc('\0',String("~")+dest);
    return sendMessage(src,pc,silent);
}



#ifndef DOXYGEN_SHOULD_SKIP_THIS

// just a temporary implementation until real ports are available
class BottleReader : public Readable {
private:
    PortCore core;
    SemaphoreImpl done;
    bool raw;
    bool env;
    Address address;
public:
    BottleReader(const char *name, bool showEnvelope) : done(0) {
        NameClient& nic = NameClient::getNameClient();
        address = nic.registerName(name);
        raw = false;
        env = showEnvelope;
        core.setReadHandler(*this);
        if (address.isValid()) {
            ACE_OS::fprintf(stderr,"Port %s listening at %s\n", 
                            address.getRegName().c_str(),
                            address.toString().c_str());
            core.listen(address);
            core.start();
        } else {
            YARP_ERROR(Logger::get(),"could not create port");
            done.post();
        }
    }

    void wait() {
        done.wait();
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
        core.join();
    }

    String getName() {
        return address.getRegName();
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/




int Companion::read(const char *name, const char *src, bool showEnvelope) {
    try {
        BottleReader reader(name,showEnvelope);
        if (src!=NULL) {
            Network::connect(src,reader.getName().c_str());
        }
        reader.wait();
        reader.close();
        return 0;
    } catch (IOException e) {
        ACE_OS::fprintf(stderr,"read failed: %s\n",e.toString().c_str());    
    }
    return 1;
}




int Companion::write(const char *name, int ntargets, char *targets[]) {
    try {
        PortCore core;
        NameClient& nic = NameClient::getNameClient();
        Address address = nic.registerName(name);
        if (address.isValid()) {
            ACE_OS::fprintf(stderr,"Port %s listening at %s\n", 
                            address.getRegName().c_str(),
                            address.toString().c_str());
            core.listen(address);
            core.start();
        } else {
            YARP_ERROR(Logger::get(),"could not create port");
            return 1;
        }

        bool raw = true;
        for (int i=0; i<ntargets; i++) {
            if (String(targets[i])=="verbatim") {
                raw = false;
            } else {
                connect(address.getRegName().c_str(),targets[i]);
            }
        }


        while (!feof(stdin)) {
            String txt = getStdin();
            if (!feof(stdin)) {
                if (txt[0]<32 && txt[0]!='\n' && 
                    txt[0]!='\r' && txt[0]!='\0') {
                    break;  // for example, horrible windows ^D
                }
                BottleImpl bot;
                if (!raw) {
                    bot.addInt(0);
                    bot.addString(txt.c_str());
                } else {
                    bot.fromString(txt.c_str());
                }
                core.send(bot);
            }
        }

        if (!raw) {
            BottleImpl bot;
            bot.addInt(1);
            bot.addString("<EOF>");
            core.send(bot);
        }

        core.close();
        core.join();

        return 0;
    } catch (IOException e) {
        ACE_OS::fprintf(stderr,"write failed: %s\n",e.toString().c_str());    
    }
    return 1;
}



int Companion::rpc(const char *connectionName, const char *targetName) {
    try {
        NameClient& nic = NameClient::getNameClient();
        Address address = nic.queryName(targetName);
        if (!address.isValid()) {
            YARP_ERROR(Logger::get(),"could not find port");
            return 1;
        }

        OutputProtocol *out = Carriers::connect(address);
        if (out==NULL) {
            throw IOException("cannot connect to port");
        }
        printf("RPC connection to %s at %s (connection name %s)\n", targetName, 
               address.toString().c_str(),
               connectionName);
        Route r(connectionName,targetName,"text_ack");
        out->open(r);
        OutputStream& os = out->getOutputStream();
        InputStream& is = out->getInputStream();
        StreamConnectionReader reader;

        while (!feof(stdin)) {
            String txt = getStdin();

            if (!feof(stdin)) {
                if (txt[0]<32 && txt[0]!='\n' && 
                    txt[0]!='\r' && txt[0]!='\0') {
                    break;  // for example, horrible windows ^D
                }
                Bottle bot;
                bot.fromString(txt.c_str());

                PortCommand pc(0,"d");
                BufferedConnectionWriter bw(out->isTextMode());
                bool ok = pc.write(bw);
                if (!ok) {
                    throw IOException("writer failed");
                }
                ok = bot.write(bw);
                if (!ok) {
                    throw IOException("writer failed");
                }
                bw.write(os);
                Bottle resp;
                TextReader formattedResp;
                reader.reset(is,NULL,r,0,true);
                bool done = false;
                bool first = true;
                while (!done) {
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

        return 0;
    } catch (IOException e) {
        ACE_OS::fprintf(stderr,"write failed: %s\n",e.toString().c_str());    
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


