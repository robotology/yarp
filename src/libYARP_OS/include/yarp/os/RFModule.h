// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP2_RFMODULE__
#define __YARP2_RFMODULE__

#include <yarp/os/Port.h>

namespace yarp {
    namespace os {
        class RFModule;
    }
}

/**
 * 
 * Disclaimer: this is currently under development. A new base-class 
 * for standard Yarp modules.  This class is similar to a Module, but 
 * it adds support for the ResourceFinder class.
 */
class yarp::os::RFModule {

public:
    /**
     * Constructor.
     */
    Module();

    /**
     * Destructor.
     */
    virtual ~Module();


    /**
     *
     * You can override this to control the approximate periodicity at which
     * updateModule() is called by runModule().  By default, it returns
     * 0 (no delay).
     * @return the desired period between successive calls to updateModule()
     * 
     */
    virtual double getPeriod() {
        return 0.0;
    }

    /**
     *
     * Override this to do whatever your module needs to do.  When
     * your module wants to stop, return false.  The module's actual
     * work could be done during this call, or it could just check the
     * state of a thread running in the background.  
     * @return true iff module should continue
     * 
     */
    virtual bool updateModule()=0;


    /**
     *
     * Calls updateModule() until that returns false.
     * updateModule() is called every getPeriod()
     * seconds.  Be aware that the the respond() command could be 
     * asycnhronously at any time, if there is input from the 
     * standard input or a port connected via attach().
     * @return true on success
     *
     */
    virtual bool runModule();

    /**
     * Simple helper method to call openFromCommand(), then runModule().
     * Most modules that run as a single executable will want to do this.
     * Their main() method would be something like:
     * \code
     *    int main(int argc, char *argv[]) {
     *        Network yarp;
     *        YourModule module;
     *        return module.runModule(argc,argv);
     *    }
     * \endcode
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @param policy name of the policy to use by the Resource Finder.
     * @return 0 upon success, non-zero upon failure
     */
    virtual int runModule(int argc, char *argv[], const char *policy=0);

    /**
     * Pass a list of command line arguments. These
     * will be passed verbatim to a RF with the
     * result of calling the open() function. The 
     * user should overwrite this function.
     * Also calls attachTerminal().
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @param policy name of the policy to use by the resource finder
     * @return true/false upon success/failure
     */
    virtual bool openFromCommand(int argc, char *argv[], const char *policy=0);


     /**
     *
     * Request that information be printed to the console on how
     * resources are being found.  This is especially useful to
     * understand why resources are *not* found or the wrong resource
     * is picked up.
     *
     * @param verbose set/suppress printing of information
     *
     * @return true iff information will be printed
     *
     */
    bool setVerbose(bool verbose = true);
    
    /*
    * Set default configuration files. Passed to the resource finder
    * class.
    */
    bool setDefaultConfigFile(const char *fname);

    /*
    * Set detaulf context. Passed to the resource finder class.
    */
    bool setDefaultContext(const char *contextName);

    /**
     * Respond to a message.  You can override this to respond
     * to messages in your own way.  It is useful, if your module
     * doesn't know what to do with a message, to call
     * Module::respond() for any default responses.
     * @param command the message received
     * @param reply the response you wish to make
     * @return true if there was no critical failure
     */
    virtual bool respond(const Bottle& command, Bottle& reply) {
        return basicRespond(command,reply);
    }

    /**
     * Make any input from a Port object go to the respond() method.
     * @param port the port to attach
     * @return true if port was attached correctly.
     */
    virtual bool attach(Port& port);

    /**
     * Make any input from a BufferedPort or PortReaderBuffer object go to 
     * the respond() method.
     * @param port the port or buffer to attach
     * @param handleStream control whether streaming messages (messages that
     * don't expect replies) are also sent to respond().  If they are,
     * replies will be discarded.
     * @return true if port was attached correctly.
     */
    virtual bool attach(TypedReader<Bottle>& port, bool handleStream = false);

    /**
     * Make any input from standard input (usually the keyboard) go to
     * the respond() method.  The reply will be send to standard output.
     * @return true on success.
     */
    virtual bool attachTerminal();

    /*
    * Open function. This is called automatically when you either call
    * openFromCommand() or runModule(). Override this to receive parameters
    * passed to your module.
    * @param rf a ResourceFinder object initialized from data passed 
    * in argc/argv from openFromCommand() or runModule(). You can customize
    * the behavior of the ResourceFinder object by first calling setDefaultContext
    * and setDefaultConfigFile().
    */
    virtual bool open(const yarp::os::ResourceFinder &rf)
    { return true; }

    /**
     * Return name of module, as set with --name flag or setName().
     * @param subName get nested name with this at the end
     * @return the name of the module
     */
    ConstString getName(const char *subName = 0/*NULL*/);

    /**
     * Set the name of the module.
     * @param name the desired name of the module
     */
    void setName(const char *name) {
        this->name = name;
    }

    /**
     * Wrapper around respond() that is guaranteed to process system messages.
     */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    bool safeRespond(const Bottle& command, Bottle& reply);
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

private:
    Property& getState() { return state; }

    virtual bool basicRespond(const Bottle& command, Bottle& reply);
};

#endif
