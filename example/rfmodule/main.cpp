#include <iostream>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>

using namespace std;
using namespace yarp::os;

class MyModule:public RFModule
{
    RpcServer handlerPort; //a port to handle messages
    int count;
public:

    double getPeriod()
    {
        return 1; //module periodicity (seconds)
    }

    /*
    * This is our main function. Will be called periodically every getPeriod() seconds.
    */
    bool updateModule()
    {
        count++;
        //printf("[%d] updateModule\n", count);
        cout<<"["<<count<<"]"<< " updateModule... "<<endl;

        return true;
    }

    /*
    * Message handler. Just echo all received messages.
    */
    bool respond(const Bottle& command, Bottle& reply) 
    {
        cout<<"Got something, echo is on"<<endl;
        if (command.get(0).asString()=="quit")
            return false;     
        else
            reply=command;
        return true;
    }

    /* 
    * Configure function. Receive a previously initialized
    * resource finder object. Use it to configure your module.
    * Open port and attach it to message handler.
    */
    bool configure(yarp::os::ResourceFinder &rf)
    {
        count=0;
        if (!handlerPort.open("/myModule"))
            return false;

        attach(handlerPort);
        return true;
    }

    /*
    * Interrupt function.
    */
    bool interruptModule()
    {
        cout<<"Interrupting your module, for port cleanup"<<endl;
        return true;
    }

    /*
    * Close function, to perform cleanup.
    */
    bool close()
    {
        cout<<"Calling close function\n";
        handlerPort.close();
        return true;
    }
};

int main(int argc, char * argv[])
{
    Network yarp;

    MyModule module;
    ResourceFinder rf;
    rf.configure(argc, argv);
    // rf.setVerbose(true);

    cout<<"Configure module & start module..."<<endl;
    if (!module.runModule(rf))
    {
        cerr<<"Error module did not start"<<endl;
        return 1;
    }

    cout<<"Main returning..."<<endl;
    return 0;
}


