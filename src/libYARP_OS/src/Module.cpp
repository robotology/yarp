// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/Logger.h>

#include <yarp/os/Module.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>

using namespace yarp::os;

class ModuleHelper : public yarp::os::PortReader,
                     public yarp::os::TypedReaderCallback<yarp::os::Bottle>,
                     public Thread {
    
private:
    Module& owner;
public:
    ModuleHelper(Module& owner) : owner(owner) {}

    /**
     * Handler for reading messages from the network, and passing 
     * them on to the respond() method.
     * @param connection a network connection to a port
     * @return true if the message was read successfully
     */
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
     * Alternative handler for reading messages from the network, and passing 
     * them on to the respond() method.  There can be no replies made
     * if this handler is used.
     * @param v the message
     * @return true if the message was read successfully
     */
    virtual void onRead(yarp::os::Bottle& v) {
        yarp::os::Bottle reply;
        owner.respond(v,reply);
    }

    /**
     * Attach this object to a source of messages.
     * @param source a BufferedPort or PortReaderBuffer that
     * receives data.
     */
    bool attach(yarp::os::TypedReader<yarp::os::Bottle>& source,
                bool handleStream) {
        if (handleStream) {
            source.useCallback(*this);
        }
        source.setReplier(*this);
        return true;
    }


    bool attach(yarp::os::Port& source) {
        source.setReader(*this);
        return true;
    }

    virtual void run() {
        bool isEof = false;
        while (!(isEof||isStopping()||owner.isStopping())) {
            ConstString str = Network::readString(&isEof);
            if (!isEof) {
                Bottle cmd(str.c_str());
                Bottle reply;
                bool ok = owner.respond(cmd,reply);
                if (ok) {
                    printf("%s\n", reply.toString().c_str());
                } else {
                    printf("Command not understood -- %s\n", str.c_str());
                }
            }
        }
        //printf("terminal shutting down\n");
        //owner.interruptModule();
    }
};


bool ModuleHelper::read(ConnectionReader& connection) {
    Bottle cmd, response;
    if (!cmd.read(connection)) { return false; }
    //printf("command received: %s\n", cmd.toString().c_str());
    bool result = owner.respond(cmd,response);
    if (response.size()>=1) {
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=NULL) {
            if (response.get(0).toString()=="many") {
                for (int i=1; i<response.size(); i++) {
                    Value& v = response.get(i);
                    if (v.isList()) {
                        v.asList()->write(*writer);
                    } else {
                        Bottle b;
                        b.add(v);
                        b.write(*writer);
                    }
                }
            } else {
                response.write(*writer);
            }
            
            //printf("response sent: %s\n", response.toString().c_str());
        }
    }
    return result;
}



#define HELPER(x) (*((ModuleHelper*)(x)))

Module::Module() {
    stopFlag = false;
    implementation = new ModuleHelper(*this);
    YARP_ASSERT(implementation!=NULL);
}

Module::~Module() {
    if (implementation!=NULL) {
        HELPER(implementation).stop();
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

bool Module::respond(const Bottle& command, Bottle& reply) {
    switch (command.get(0).asVocab()) {
    case VOCAB3('s','e','t'):
        state.put(command.get(1).toString(),command.get(2));
        reply.addVocab(Vocab::encode("ack"));
        return true;
        break;
    case VOCAB3('g','e','t'):
        reply.add(state.check(command.get(1).toString(),Value(0)));
        return true;
        break;
    case VOCAB4('q','u','i','t'):
    case VOCAB4('e','x','i','t'):
    case VOCAB3('b','y','e'):
        reply.addVocab(Vocab::encode("bye"));
        stopFlag = true;
        interruptModule();
        return true;
    default:
        reply.add("command not recognized");
        return false;
    }
    return false;
}


static Module *module = NULL;
static bool terminated = false;
static void handler (int) {
    static int ct = 0;
    ct++;
    if (ct>3) {
        printf("Aborting...\n");
        ACE_OS::exit(1);
    }
    printf("[try %d of 3] Trying to shut down\n", 
           ct);
    terminated = true;
    if (module!=NULL) {
        Bottle cmd, reply;
        cmd.fromString("quit");
        module->respond(cmd,reply);
        //printf("sent %s, got %s\n", cmd.toString().c_str(),
        //     reply.toString().c_str());
    }
}


bool Module::runModule() {
    if (module==NULL) {
        module = this;
        //module = &HELPER(implementation);
    } else {
        ACE_OS::printf("Module::runModule() signal handling currently only good for one module\n");
    }
    ACE_OS::signal(SIGINT, (ACE_SignalHandler) handler);
    while (updateModule()) {
        if (terminated) break;
        if (isStopping()) break;
        Time::delay(getPeriod());
        if (isStopping()) break;
        if (terminated) break;
    }
    printf("Module finished\n");
    if (terminated) {
        // only portable way to bring down a thread reading from
        // the keyboard -- no good way to interrupt.
        ACE_OS::exit(0);
    }
    return true;
}



bool Module::attach(Port& port) {
    return HELPER(implementation).attach(port);
}

bool Module::attach(TypedReader<Bottle>& port, bool handleStream) {
    return HELPER(implementation).attach(port,handleStream);
}



bool Module::attachTerminal() {
    HELPER(implementation).start();
    return true;
}


