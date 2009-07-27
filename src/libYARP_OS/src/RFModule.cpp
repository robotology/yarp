#include <yarp/os/RFModule.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/String.h>
#include <yarp/os/Network.h>

#include <ace/OS.h>

using namespace yarp::os;
using namespace yarp;

class RFModuleHelper : public yarp::os::PortReader, public Thread
{    
private:
    bool attachedToPort;
    bool attachedTerminal;
    RFModule& owner;

public:
     /**
     * Handler for reading messages from the network, and passing 
     * them on to the respond() method.
     * @param connection a network connection to a port
     * @return true if the message was read successfully
     */
    virtual bool read(yarp::os::ConnectionReader& connection);

    RFModuleHelper(RFModule& owner) : owner(owner), attachedToPort(false), attachedTerminal(false) {}
    
    virtual void run() {
        printf("Listening to terminal (type \"quit\" to stop module)\n");
        bool isEof = false;
        while (!(isEof||isStopping()||owner.isStopping())) {
            ConstString str = Network::readString(&isEof);
            if (!isEof) {
                Bottle cmd(str.c_str());
                Bottle reply;
                bool ok = owner.safeRespond(cmd,reply);
                if (ok) {
                    //printf("ALL: %s\n", reply.toString().c_str());
                    //printf("ITEM 1: %s\n", reply.get(0).toString().c_str());
                    if (reply.get(0).toString()=="help") {
                        for (int i=0; i<reply.size(); i++) {
                            ACE_OS::printf("%s\n", 
                                           reply.get(i).toString().c_str());
                        }
                    } else {
                        ACE_OS::printf("%s\n", reply.toString().c_str());
                    }
                } else {
                    ACE_OS::printf("Command not understood -- %s\n", str.c_str());
                }
            }
        }
        //printf("terminal shutting down\n");
        //owner.interruptModule();
    }

    bool attach(yarp::os::Port& source) 
    {
        if (attachedTerminal)
        {
            fprintf(stderr, "Warning, reading from terminal, cannot read from port\n");
            return false;
        }

        attachedToPort=true;
        source.setReader(*this);
        return true;
    }

    bool attachTerminal()
    {
        if (attachedToPort)
        {
            fprintf(stderr, "Warning, reading from port, cannot read from terminal\n");
            return false;
        }

        attachedTerminal=true;

        Thread::start();
        return true;
    }

    bool detachTerminal()
    {
        fprintf(stderr, "Critial: stopping thread, this might hang\n");
        Thread::stop();
        fprintf(stderr, "done!\n");
        return true;
    }

private:
public:

};

bool RFModuleHelper::read(ConnectionReader& connection) {
    Bottle cmd, response;
    if (!cmd.read(connection)) { return false; }
    printf("command received: %s\n", cmd.toString().c_str());
    
    bool result = owner.safeRespond(cmd,response);
    if (response.size()>=1) {
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=0) {
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


#define HELPER(x) (*((RFModuleHelper*)(x)))

RFModule::RFModule() {
    implementation = new RFModuleHelper(*this);
    stopFlag=false;
}

RFModule::~RFModule() {
    if (implementation!=0) {
        //HELPER(implementation).stop();
        delete &HELPER(implementation);
        implementation = 0;
    }
}

 /**
* Attach this object to a source of messages.
* @param source a BufferedPort or PortReaderBuffer that
* receives data.
*/
bool RFModule::attach(yarp::os::Port &source) {
    HELPER(implementation).attach(source);
    return true;
}

bool RFModule::basicRespond(const Bottle& command, Bottle& reply) {
    switch (command.get(0).asVocab()) {
    case VOCAB4('q','u','i','t'):
    case VOCAB4('e','x','i','t'):
    case VOCAB3('b','y','e'):
        reply.addVocab(Vocab::encode("bye"));
        stop(false);
   //     interruptModule();
        return true;
    default:
        reply.add("command not recognized");
        return false;
    }
    return false;
}

bool RFModule::safeRespond(const Bottle& command, Bottle& reply) {
    bool ok = respond(command,reply);
    if (!ok) {
        // just in case derived classes don't correctly pass on messages
        ok = basicRespond(command,reply);
    }
    return ok;
}


static RFModule *module = 0;
static bool terminated = false;
static void handler (int) {
    static int ct = 0;
    ct++;
    if (ct>3) {
        ACE_OS::printf("Aborting...\n");
        ACE_OS::exit(1);
    }
    ACE_OS::printf("[try %d of 3] Trying to shut down\n", 
                   ct);
    if (module!=0)
    {
        module->stop(false);
    }
  //  if (module!=NULL) {
  //      Bottle cmd, reply;
  //      cmd.fromString("quit");
   //     module->safeRespond(cmd,reply);
        //printf("sent %s, got %s\n", cmd.toString().c_str(),
        //     reply.toString().c_str());
   // }
}


int RFModule::runModule() {
    stopFlag=false;
    attachTerminal();

    if (module==NULL) {
        module = this;
        //module = &HELPER(implementation);
    } else {
        ACE_OS::printf("Module::runModule() signal handling currently only good for one module\n");
    }
    ACE_OS::signal(SIGINT, (ACE_SignalHandler) handler);
    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) handler);
    while (updateModule()) {
        if (isStopping()) break;
        Time::delay(getPeriod());
    }

    ACE_OS::printf("Module closing\n");
    detachTerminal();
    close();
    ACE_OS::printf("Module finished\n");
    return 0;
}


int RFModule::runModule(yarp::os::ResourceFinder &rf) {
    if (!configure(rf)) {
        ACE_OS::printf("Module failed to open\n");
        return false;
    }
    bool ok = runModule();
    return ok?0:1;
}

bool RFModule::configure(yarp::os::ResourceFinder &rf)
{
    //pass it on to user open function
    return open(resourceFinder);
}

ConstString RFModule::getName(const char *subName) {
    if (subName==0) {
        return name;
    }
    String base = name.c_str();
    base += "/";
    base += subName;
    return base.c_str();
}

bool RFModule::attachTerminal() {
    HELPER(implementation).attachTerminal();
    return true;
}

bool RFModule::detachTerminal()
{
    HELPER(implementation).detachTerminal();
    return true;
}