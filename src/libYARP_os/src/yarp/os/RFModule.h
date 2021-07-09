/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RFMODULE_H
#define YARP_OS_RFMODULE_H

#include <yarp/os/Port.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RpcServer.h>

namespace yarp {
namespace os {

/**
 * A base-class for standard YARP modules that supports ResourceFinder.
 */
class YARP_os_API RFModule
{
public:
    /**
     * Constructor.
     */
    RFModule();

    /**
     * Destructor.
     */
    virtual ~RFModule();

    /**
     * You can override this to control the approximate periodicity at which
     * updateModule() is called by runModule().  By default, it returns
     * 1.0. Time here is in seconds.
     *
     * @return the desired period between successive calls to updateModule()
     */
    virtual double getPeriod();

    /**
     * Override this to do whatever your module needs to do.
     *
     * When your module wants to stop, return false.  The module's actual
     * work could be done during this call, or it could just check the
     * state of a thread running in the background.
     *
     * The thread calls the updateModule() function every <period> seconds.
     * At the end of each run, the thread will sleep the amounth of time
     * required, taking into account the time spent inside the loop function.
     * Example:  requested period is 10ms, the updateModule() function take
     * 3ms to be executed, the thread will sleep for 7ms.
     *
     * Note: after each run is completed, the thread will call a yield()
     * in order to facilitate other threads to run.
     *
     * @return true iff module should continue
     */
    virtual bool updateModule() = 0;

    /**
     * Calls updateModule() until that returns false.
     *
     * Make sure you first configure your module by calling the configure()
     * function. updateModule() is then called every getPeriod()
     * seconds.  During execution of updateModule() the following methods may be
     * executed asynchronously:
     *  - respond(): this is called if there is input from the standard input or
     *               a message from an input port connected to the module via
     *               attach().
     *  - interruptModule(): this method is called by the handlers of  the
     *               following signals: SIGINT, SIGTERM and SIGBREAK (WIN32).
     *               Interrupt is a good place to execute code that unblocks
     *               pending reads (i.e. blocking reads on port).
     *
     * After the last iteration of updateModule() the function close() is
     * executed.
     *
     * @return 0 on success
     *
     * \note attachTerminal() is no longer called automatically.
     */
    virtual int runModule();

    /**
     * Simple helper method to call configure() and then runModule().
     * See documentation of configure() and runModule() for more details.
     *
     * @param rf a previously initialized ResourceFinder
     * @return 0 upon success, non-zero upon failure
     */
    virtual int runModule(yarp::os::ResourceFinder& rf);

    /**
     * Calls updateModule() on a separate thread until that returns false.
     *
     * Make sure you first configure your module by calling the configure()
     * function. updateModule() is then called every getPeriod()
     * seconds.  During execution of updateModule() the following methods may be
     * executed asynchronously:
     *  - respond(): this is called if there is input from the standard input or
     *               a message from an input port connected to the module via
     *               attach().
     *  - interruptModule(): this method is called by the handlers of  the
     *               following signals: SIGINT, SIGTERM and SIGBREAK (WIN32).
     *               Interrupt is a good place to execute code that unblocks
     *               pending reads (i.e. blocking reads on port).
     *
     * After the last iteration of updateModule() the function close() is
     * executed.
     *
     * @return 0 on success
     *
     * \note attachTerminal() is no longer called automatically.
     */
    virtual int runModuleThreaded();

    /**
     * Simple helper method to call configure() and then runModule()
     * on a separate thread.
     * See documentation of configure() and runModule() for more details.
     *
     * @param rf a previously initialized ResourceFinder
     * @return 0 upon success, non-zero upon failure
     */
    virtual int runModuleThreaded(yarp::os::ResourceFinder& rf);

    /**
     * return the Thread unique identifier
     */
    virtual int getThreadKey();

    /**
     * Configure the module, pass a ResourceFinder object to the module.
     * This function can perform initialization including object creation and
     * memory allocation; returns false to notify that initialization was not
     * successful and that the module should not start. Cleanup should be
     * performed by the function close(). In case of failure during the
     * initialization and before returning false, the function configure()
     * should cleanup memory and resources allocated.
     *
     * @param rf a previously initialized ResourceFinder
     * @return true/false upon success/failure
     *
     * \note attachTerminal() is no longer called automatically. You
     * can call it in the configure function.
     */
    virtual bool configure(yarp::os::ResourceFinder& rf);

    /**
     * Respond to a message.
     *
     * You can override this to respond to messages in your own way.
     * It is useful, if your module doesn't know what to do with a message,
     * to call RFModule::respond() for any default responses.
     *
     * @param command the message received
     * @param reply the response you wish to make
     * @return true if there was no critical failure
     */
    virtual bool respond(const Bottle& command, Bottle& reply);

    /**
     * Make any input from a Port object go to the respond() method.
     *
     * @param source the port to attach
     * @return true if source was attached correctly.
     */
    virtual bool attach(yarp::os::Port& source);

    /**
     * Make any input from an RpcServer object go to the respond() method.
     *
     * @param source the RpcServer port to attach
     * @return true if source was attached correctly.
     */
    virtual bool attach(yarp::os::RpcServer& source);

    /**
     * Make any input from standard input (usually the keyboard) go to
     * the respond() method.
     *
     * The reply will be send to standard output.
     *
     * @return true on success.
     */
    bool attachTerminal();

    /**
     * Detach terminal.
     */
    bool detachTerminal();

    /**
     * Try to halt any ongoing operations by threads managed by the module.
     *
     * This is called asynchronously just after a quit command is received.
     * By default it does nothing - you may want to override this.
     * If you have created any ports, and have any threads that are
     * might be blocked on reading data from those ports, this is a
     * good place to add calls to BufferedPort::interrupt() or
     * Port::interrupt().
     *
     * @return true if there was no catastrophic failure
     */
    virtual bool interruptModule();

    /**
     * Close function.
     *
     * This is called automatically when the module closes, after the last call
     * to updateModule. Override this to cleanup memory allocated in the
     * configure() function or perform other activities that ensure graceful
     * shutdown.
     *
     * @return true/false on success failure.
     */
    virtual bool close();

    /**
     * Ask the module to stop. Called automatically by signal handlers or when a
     * quit message is received by the respond() (by the console or by a port if
     * attached). It raises an internal flag that notifies the module to stop
     * executing updateModule() and then calls interruptModule().
     *
     * @param wait if RFModule is run threaded, specifies if stop should
     * call join and wait for thread termination.
     */
    void stopModule(bool wait = false);

    /**
     * Check if the module should stop.
     *
     * @return true/false if the module should stop or not.
     */
    bool isStopping();

    /**
     * The function returns when the thread execution has completed.
     *
     * Stops the execution of the thread that calls this function until either
     * the thread to join has finished execution (when it returns from run())
     * or after \a seconds seconds.
     *
     * If RFModule has not been thredified, the function returns true immediately.
     *
     * @param seconds the maximum number of seconds to block the thread.
     * @return true if the thread execution is finished or when
     * RFModule has not been thredified, false on time out.
     */
    bool joinModule(double seconds = -1);

    /**
     * Return name of module, as set with setName().
     *
     * If a string is passed to the function, it gets concatenated to the module
     * name.
     * This function can be useful to form port names used by the module.
     * Important: strings are concatenated "as they are", no slashes are
     * appended at the beginning of the strings. To support legacy
     * code the function will make sure subName contains a trailing
     * slash (this behavior is deprecated and will disappear).
     *
     * @param subName get nested name with this at the end
     * @return the name of the module
     */
    std::string getName(const std::string& subName = "");

    /**
     * Set the name of the module.
     *
     * @param name the desired name of the module
     */
    void setName(const char* name);

    /**
     * Wrapper around respond() that is guaranteed to process system messages.
     */
    bool safeRespond(const Bottle& command, Bottle& reply);

private:
    ResourceFinder resourceFinder;
    bool stopFlag;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name;

    virtual bool basicRespond(const Bottle& command, Bottle& reply);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_RFMODULE_H
