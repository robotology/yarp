// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/Companion.h>
#include <yarp/NameClient.h>
#include <yarp/Logger.h>
#include <yarp/PortCommand.h>

#include <yarp/Carriers.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/PortCore.h>
#include <yarp/BottleImpl.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/NameServer.h>
#include <yarp/NameConfig.h>

// just for "write", which needs to read from standard input
#include <iostream>
using namespace std;


using namespace yarp;
using namespace yarp::os;

Companion Companion::instance;

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
    add("regression", &Companion::cmdRegression,
        "run regression tests, if linked");
    add("server",     &Companion::cmdServer,
        "run yarp name server");
    add("check",      &Companion::cmdCheck,
        "run a simple sanity check to see if yarp is working");
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
    Address address = nic.queryName("root");
    if (address.isValid()) {
        ACE_OS::printf("Name server is available at ip %s port %d\n",
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
    Route route("external",port,"text");
    try {
        out->open(route);
        //printf("Route %s TEXT mode %d\n", out->getRoute().toString().c_str(),
        // out->isTextMode());
        BufferedConnectionWriter bw(out->isTextMode());
        //bw.appendLine(msg);
        //writable.writeBlock(bw);
        bool ok = writable.write(bw);
        if (!ok) {
            throw IOException("writer failed");
        }

        out->write(bw);
        InputProtocol& ip = out->getInput();
        ConnectionReader& con = ip.beginRead();
        Bottle b;
        b.read(con);
        b.read(con);
        if (!quiet) {
            //ACE_OS::fprintf(stderr,"%s\n", b.toString().c_str());
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
    if (argc!=1) {
        ACE_OS::fprintf(stderr, "Please supply the port name\n");
        return 1;
    }

    const char *src = argv[0];
    return read(src);
}


int Companion::cmdWrite(int argc, char *argv[]) {
    if (argc<1) {
        ACE_OS::fprintf(stderr, "Please supply the port name, and optionally some targets\n");
        return 1;
    }

    const char *src = argv[0];
    return write(src,argc-1,argv+1);
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
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


int Companion::cmdCheck(int argc, char *argv[]) {
    Logger& log = Logger::get();
    NameClient& nic = NameClient::getNameClient();

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





int Companion::connect(const char *src, const char *dest, bool silent) {
    PortCommand pc('\0',slashify(dest));
    return sendMessage(src,pc,silent);
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
public:
    BottleReader(const char *name) : done(0) {
        NameClient& nic = NameClient::getNameClient();
        Address address = nic.registerName(name);
        core.setReadHandler(*this);
        if (address.isValid()) {
            ACE_OS::fprintf(stderr,"Port %s listening at %s\n", 
                            name,
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

    virtual bool read(ConnectionReader& reader) {
        BottleImpl bot;
        bot.read(reader);
        if (bot.size()==2 && bot.isInt(0) && bot.isString(1)) {
            int code = bot.getInt(0);
            if (code!=1) {
                ACE_OS::printf("%s\n", bot.getString(1).c_str());
                ACE_OS::fflush(stdout);
            }
            if (code==1) {
                done.post();
            }
        } else {
            ACE_OS::printf("%s\n", bot.toString().c_str());
            ACE_OS::fflush(stdout);
        }
        return true;
    }
  
    void close() {
        core.close();
        core.join();
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/




int Companion::read(const char *name) {
    try {
        BottleReader reader(name);
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
                            name,
                            address.toString().c_str());
            core.listen(address);
            core.start();
        } else {
            YARP_ERROR(Logger::get(),"could not create port");
            return 1;
        }
    
        for (int i=0; i<ntargets; i++) {
            connect(name,targets[i]);
        }


        while (!(cin.bad()||cin.eof())) {
            // make sure this works on windows
            char buf[25600] = "\0";
            cin.getline(buf,sizeof(buf),'\n');
            // TODO: add longer strings together
      
            if (!(cin.bad()||cin.eof())) {
                if (buf[0]<32 && buf[0]!='\n' && buf[0]!='\r' && buf[0]!='\0') {
                    break;  // for example, horrible windows ^D
                }
                BottleImpl bot;
                bot.addInt(0);
                bot.addString(buf);
                core.send(bot);
            }
        }

        BottleImpl bot;
        bot.addInt(1);
        bot.addString("<EOF>");
        core.send(bot);

        core.close();
        core.join();

        return 0;
    } catch (IOException e) {
        ACE_OS::fprintf(stderr,"write failed: %s\n",e.toString().c_str());    
    }
    return 1;
}



