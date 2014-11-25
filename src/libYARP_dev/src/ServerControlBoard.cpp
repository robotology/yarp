// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2006 RobotCub Consortium
* Authors: Giorgio Metta
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <string.h>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Log.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>

namespace yarp{
    namespace dev {
        class ServerControlBoard;
        class ImplementCallbackHelper;
        class CommandsHelper;
    }
}

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::dev;
using namespace yarp::sig;

/**
* Callback implementation after buffered input.
*/
class yarp::dev::ImplementCallbackHelper: public TypedReaderCallback<CommandMessage> {
protected:
    IPositionControl *pos;
    IVelocityControl *vel;

public:
    /**
    * Constructor.
    */
    ImplementCallbackHelper();

    /**
    * Initialization.
    * @param x is the instance of the container class using the callback.
    */
    void init(yarp::dev::ServerControlBoard *x);

    /**
    * Callback function.
    * @param v is the Vector being received.
    */
    virtual void onRead(CommandMessage& v);
};

/**
* Helper object for reading config commands for the ServerControlBoard
* class.
*/
class yarp::dev::CommandsHelper : public DeviceResponder {
protected:
    yarp::dev::ServerControlBoard   *caller;
    yarp::dev::IPidControl          *pid;
    yarp::dev::ITorqueControl       *trq;
    yarp::dev::IControlMode         *mod;
    yarp::dev::IPositionControl     *pos;
    yarp::dev::IVelocityControl     *vel;
    yarp::dev::IEncoders            *enc;
    yarp::dev::IAmplifierControl    *amp;
    yarp::dev::IControlLimits       *lim;
    yarp::dev::IAxisInfo            *info;
    yarp::dev::IControlCalibration2 *ical2;

    int nj;
    Vector vect;

public:
    /**
    * Constructor.
    */
    CommandsHelper();

    /**
    * Initialization.
    * @param x is the pointer to the instance of the object that uses the CommandsHelper.
    * This is required to recover the pointers to the interfaces that implement the responses
    * to the commands.
    */
    void init(yarp::dev::ServerControlBoard *x);

    virtual bool respond(const Bottle& cmd, Bottle& response);

    /**
    * Initialize the internal data.
    * @return true/false on success/failure
    */
    virtual bool initialize();
};

/*
* Implement the server side of a remote
* control board device driver. The device contains three ports:
* - rpc_p handling the configuration interfaces of the robot
* - state_p streaming information about the current state of the robot
* - control_p receiving a stream of control commands (e.g. position)
*
* Missing:
*          torque control, ONLY PARTIALLY IMPLEMENTED
*
*/
class yarp::dev::ServerControlBoard :
    public DeviceDriver,
    public Thread,
    public IPidControl,
    public IPositionControl,
    public IVelocityControl,
    public ITorqueControl,
    public IControlMode,
    public IEncoders,
    public IAmplifierControl,
    public IControlLimits,
    public IControlCalibration,
    public IControlCalibration2,
    public IAxisInfo,
    public IPreciselyTimed
    // convenient to put these here just to make sure all
    // methods get implemented
{
private:
    bool verb;

    Port rpc_p;     // RPC to configure the robot
    Port state_p;   // out port to read the state
    Port control_p; // in port to command the robot

    Stamp     stamp;
    Semaphore stampMutex;

    PortWriterBuffer<yarp::sig::Vector> state_buffer;
    PortReaderBuffer<CommandMessage> control_buffer;

    ImplementCallbackHelper callback_impl;
    CommandsHelper command_reader;

    // for new interface
    PortReaderBuffer<Bottle> command_buffer;

    PolyDriver poly;

    int                   nj;
    int                   thread_period;
    IPidControl          *pid;
    IPositionControl     *pos;
    IVelocityControl     *vel;
    ITorqueControl       *trq;
    IControlMode         *mod;
    IEncoders            *enc;
    IAmplifierControl    *amp;
    IControlLimits       *lim;
    IControlCalibration  *calib;
    IControlCalibration2 *calib2;
    IAxisInfo            *info;
    // LATER: other interfaces here.

    bool closeMain() {

        // interrupt the ports
        control_p.interrupt();
        state_p.interrupt();
        rpc_p.interrupt();

        if (Thread::isRunning()) {
            Thread::stop();
        }
        // close the port connections here!
        control_p.close();
        state_p.close();
        rpc_p.close();

        poly.close();
        return true;
    }

public:
    /**
    * Constructor.
    */
    ServerControlBoard()
    {
        callback_impl.init(this);
        command_reader.init(this);
        trq    = NULL;
        mod    = NULL;
        pid    = NULL;
        pos    = NULL;
        vel    = NULL;
        enc    = NULL;
        amp    = NULL;
        lim    = NULL;
        calib  = NULL;
        calib2 = NULL;
        info   = NULL;
        nj     = 0;
        thread_period = 20; // ms.
        verb = false;
    }

    virtual ~ServerControlBoard() {
        closeMain();
    }

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const { return verb; }

    /**
    * Default open() method.
    * @return always false since initialization requires certain parameters.
    */
    virtual bool open() {
        return false;
    }

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */

    virtual bool close() {

        return closeMain();

    }

    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are:
    * - verbose or v to print diagnostic information while running.
    * - subdevice to specify the name of the wrapped device.
    * - name to specify the predix of the port names.
    * - calibrator to specify the name of the calibrator object (created through a PolyDriver).
    * and all parameters required by the wrapped device driver.
    */
    virtual bool open(Searchable& prop) {
        rpc_p.setRpcServer();
        state_p.setWriteOnly();
        Vector v;
        state_p.promiseType(v.getType());
        control_p.setReadOnly();

        verb = (prop.check("verbose","if present, give detailed output"));
        if (verb)
            printf("running with verbose output\n");

        thread_period = prop.check("threadrate", 20, "thread rate in ms. for streaming encoder data").asInt();

        bool stream_state = (prop.check("stream_state", 1, "whether to stream state (1) or not (0)").asInt()!=0);

        Value *name;
        if (prop.check("subdevice",name,"name of specific control device to wrap")) {
            yInfo("ControlBoard subdevice is %s", name->toString().c_str());
            if (name->isString()) {
                // maybe user isn't doing nested configuration
                Property p;
                p.setMonitor(prop.getMonitor(),
                    "subdevice"); // pass on any monitoring
                p.fromString(prop.toString());
                p.put("device",name->toString());
                poly.open(p);
            } else {
                Bottle subdevice = prop.findGroup("subdevice").tail();
                poly.open(subdevice);
            }

            if (!poly.isValid()) {
                printf("cannot make <%s>\n", name->toString().c_str());
            }

        } else {
            printf("\"--subdevice <name>\" not set for server_controlboard\n");
            return false;
        }

        if (!poly.isValid()) {
            return false;
        }

        ConstString rootName =
            prop.check("name",Value("/controlboard"),

            "prefix for port names").asString();

        // attach readers.
        //rpc_p.setReader(command_reader);
        // changed so that streaming input accepted if offered
        command_buffer.attach(rpc_p);
        command_reader.attach(command_buffer);

        // attach buffers.
        state_buffer.attach(state_p);
        control_buffer.attach(control_p);
        // attach callback.
        control_buffer.useCallback(callback_impl);

        rpc_p.open((rootName+"/rpc:i").c_str());
        control_p.open((rootName+"/command:i").c_str());

        state_p.open((rootName+"/state:o").c_str());

        if (poly.isValid()) {
            poly.view(pid);
            poly.view(pos);
            poly.view(vel);
            poly.view(enc);
            poly.view(amp);
            poly.view(lim);
            poly.view(calib);
            poly.view(calib2);
            poly.view(info);
            poly.view(trq);
            poly.view(mod);
        }


        // experimental: let it be ok for not all interfaces to be
        // implemented.

        // we need at least the pos interface to get the number of
        // axes (is this true?)

        /*
        if (pid != NULL &&
        pos != NULL &&
        vel != NULL &&
        enc != NULL &&
        amp != NULL &&
        lim != NULL) {
        */

        if (pos!=NULL||vel!=NULL) {
            if (pos!=NULL) {
                if (!pos->getAxes(&nj)) {
                    printf ("problems: controlling 0 axes\n");
                    return false;
                }
            }

            if (vel!=NULL) {
                if (!vel->getAxes(&nj)) {
                    printf ("problems: controlling 0 axes\n");
                    return false;
                }
            }

            // initialization.
            command_reader.initialize();
            if (stream_state) {
                start();
            }
            return true;
        }

        printf("subdevice <%s> doesn't look like a control board (no appropriate interfaces were acquired)\n",
            name->toString().c_str());

        return false;

    }


    /**
    * The thread main loop deals with writing on ports here.
    */
    virtual void run() {
        yInfo("Server control board starting");
        double before, now;
        while (!isStopping()) {
            before = Time::now();
            yarp::sig::Vector& v = state_buffer.get();
            v.size(nj);
            if (enc!=NULL) {
                enc->getEncoders(&v[0]);
            }

            // bool ok = enc->getEncoders(&v[0]);
            // LATER: deal with the ok == false.
            stampMutex.wait();
            stamp.update();
            stampMutex.post();
            state_p.setEnvelope(stamp);
            state_buffer.write();

            now = Time::now();

            if ((now-before)*1000 < thread_period) {
                const double k = double(thread_period)/1000.0-(now-before);
                Time::delay(k);
            }
            else {
                yWarning("Can't comply with the %d ms period", thread_period);
            }
        }
        yInfo("Server control board stopping\n");
    }

    /* IPidControl */
    /** Set new pid value for a joint axis.
    * @param j joint number
    * @param p new pid value
    * @return true/false on success/failure
    */
    virtual bool setPid(int j, const Pid &p) {
        if (pid)
            return pid->setPid(j, p);
        return false;
    }

    /** Set new pid value on multiple axes.
    * @param ps pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual bool setPids(const Pid *ps) {
        if (pid)
            return pid->setPids(ps);
        return false;
    }

    /** Set the controller reference point for a given axis.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param j joint number
    * @param ref new reference point
    * @return true/false upon success/failure
    */
    virtual bool setReference(int j, double ref) {
        if (pid)
            return pid->setReference(j, ref);
        return false;
    }

    /** Set the controller reference points, multiple axes.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param refs pointer to the vector that contains the new reference points.
    * @return true/false upon success/failure
    */
    virtual bool setReferences(const double *refs) {
        if (pid)
            return pid->setReferences(refs);
        return false;
    }

    /** Set the error limit for the controller on a specifi joint
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual bool setErrorLimit(int j, double limit) {
        if (pid)
            return pid->setErrorLimit(j, limit);
        return false;
    }


    /** Get the error limit for the controller on all joints.
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual bool setErrorLimits(const double *limits) {
        if (pid)
            return pid->setErrorLimits(limits);
        return false;
    }

    /** Get the current error for a joint.
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual bool getError(int j, double *err) {
        if (pid)
            return pid->getError(j, err);
        *err = 0.0;
        return false;
    }

    /** Get the error of all joints.
    * @param errs pointer to the vector that will store the errors.
    * @return true/false on success/failure.
    */
    virtual bool getErrors(double *errs) {
        if (pid)
            return pid->getErrors(errs);
        memset (errs, 0, sizeof(double)*nj);
        return false;
    }

    /** Get the output of the controller (e.g. pwm value)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual bool getOutput(int j, double *out) {
        if (pid)
            return pid->getOutput(j, out);
        *out = 0.0;
        return false;
    }

    /** Get the output of the controllers (e.g. pwm value)
    * @param outs pinter to the vector that will store the output values
    */
    virtual bool getOutputs(double *outs) {
        if (pid)
            return pid->getOutputs(outs);
        memset(outs, 0, sizeof(double)*nj);
        return false;
    }

    virtual bool setOffset(int j, double v)
    {
        if (pid)
            return pid->setOffset(j,v);

        return false;
    }

    /** Get current pid value for a specific joint.
    * @param j joint number
    * @param p pointer to storage for the return value.
    * @return success/failure
    */
    virtual bool getPid(int j, Pid *p) {
        if (pid)
            return pid->getPid(j, p);
        memset(p, 0, sizeof(Pid));
        return false;
    }

    /** Get current pid value for a specific joint.
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual bool getPids(Pid *pids) {
        if (pid)
            return pid->getPids(pids);
        memset(pids, 0, sizeof(Pid)*nj);
        return false;
    }

    /** Get the current reference position of the controller for a specific joint.
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual bool getReference(int j, double *ref) {
        if (pid)
            return pid->getReference(j, ref);
        *ref=0.0;
        return false;
    }

    /** Get the current reference position of all controllers.
    * @param refs vector that will store the output.
    */
    virtual bool getReferences(double *refs) {
        if (pid)
            return pid->getReferences(refs);
        memset(refs, 0, sizeof(double)*nj);
        return false;
    }

    /** Get the error limit for the controller on a specific joint
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual bool getErrorLimit(int j, double *limit) {
        if (pid)
            return pid->getErrorLimit(j, limit);
        *limit = 0.0;
        return false;
    }

    /** Get the error limit for all controllers
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual bool getErrorLimits(double *limits) {
        if (pid)
            return pid->getErrorLimits(limits);
        memset(limits, 0, sizeof(double)*nj);
        return false;
    }

    /** Reset the controller of a given joint, usually sets the
    * current position of the joint as the reference value for the PID, and resets
    * the integrator.
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool resetPid(int j) {
        if (pid)
            return pid->resetPid(j);
        return false;
    }

    /**
    * Disable the pid computation for a joint
    * @param j is the axis number
    * @return true if successful, false on failure
    **/
    virtual bool disablePid(int j) {
        if (pid)
            return pid->disablePid(j);
        return false;
    }

    /**
    * Enable the pid computation for a joint
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool enablePid(int j) {
        if (pid)
            return pid->enablePid(j);
        return false;
    }

    /* IPositionControl */
    /**
    * Get the number of controlled axes. This command asks the number of controlled
    * axes for the current physical interface.
    * @param ax pointer to storage
    * @return true/false.
    */
    virtual bool getAxes(int *ax) {
        if (pos)
            return pos->getAxes(ax);
        if (vel)
            return vel->getAxes(ax);
        *ax = 0;
        return false;
    }

    /**
    * Set position mode. This command
    * is required by control boards implementing different
    * control methods (e.g. velocity/torque), in some cases
    * it can be left empty.
    * return true/false on success/failure
    */
    virtual bool setPositionMode() {
        if (pos)
            return pos->setPositionMode();
        return false;
    }

    /**
    * Set new reference point for a single axis.
    * @param j joint number
    * @param ref specifies the new ref point
    * @return true/false on success/failure
    */
    virtual bool positionMove(int j, double ref) {
        if (pos)
            return pos->positionMove(j, ref);
        return false;
    }

    /** Set new reference point for all axes.
    * @param refs array, new reference points.
    * @return true/false on success/failure
    */

    virtual bool positionMove(const double *refs) {
        if (pos)
            return pos->positionMove(refs);
        return false;
    }

    /** Set relative position. The command is relative to the
    * current position of the axis.
    * @param j joint axis number
    * @param delta relative command
    * @return true/false on success/failure
    */
    virtual bool relativeMove(int j, double delta) {
        if (pos)
            return pos->relativeMove(j, delta);
        return false;
    }

    /** Set relative position, all joints.
    * @param deltas pointer to the relative commands
    * @return true/false on success/failure
    */
    virtual bool relativeMove(const double *deltas) {
        if (pos)
            return pos->relativeMove(deltas);
        return false;
    }

    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param j the axis
    * @param flag true if the trajectory is terminated, false otherwise
    * @return false on failure
    */
    virtual bool checkMotionDone(int j, bool *flag) {
        if (pos)
            return pos->checkMotionDone(j, flag);
        *flag = true;
        return false;
    }

    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param flag true if the trajectory is terminated, false otherwise
    * @return false on failure
    */
    virtual bool checkMotionDone(bool *flag) {
        if (pos)
            return pos->checkMotionDone(flag);
        *flag = true;
        return false;
    }

    /** Set reference speed for a joint, this is the speed used during the
    * interpolation of the trajectory.
    * @param j joint number
    * @param sp speed value
    * @return true/false upon success/failure
    */
    virtual bool setRefSpeed(int j, double sp) {
        if (pos)
            return pos->setRefSpeed(j, sp);
        return false;
    }

    /** Set reference speed on all joints. These values are used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array of speed values.
    * @return true/false upon success/failure
    */
    virtual bool setRefSpeeds(const double *spds) {
        if (pos)
            return pos->setRefSpeeds(spds);
        return false;
    }

    /** Set reference acceleration for a joint. This value is used during the
    * trajectory generation.
    * @param j joint number
    * @param acc acceleration value
    * @return true/false upon success/failure
    */
    virtual bool setRefAcceleration(int j, double acc) {
        if (pos)
            return pos->setRefAcceleration(j, acc);
        return false;
    }

    /** Set reference acceleration on all joints. This is the valure that is
    * used during the generation of the trajectory.
    * @param accs pointer to the array of acceleration values
    * @return true/false upon success/failure
    */
    virtual bool setRefAccelerations(const double *accs) {
        if (pos)
            return pos->setRefAccelerations(accs);
        return false;
    }

    /** Get reference speed for a joint. Returns the speed used to
    * generate the trajectory profile.
    * @param j joint number
    * @param ref pointer to storage for the return value
    * @return true/false on success or failure
    */
    virtual bool getRefSpeed(int j, double *ref) {
        if (pos)
            return pos->getRefSpeed(j, ref);
        *ref = 0.0;
        return false;
    }

    /** Get reference speed of all joints. These are the  values used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array that will store the speed values.
    * @return true/false on success/failure.
    */
    virtual bool getRefSpeeds(double *spds) {
        if (pos)
            return pos->getRefSpeeds(spds);
        memset(spds, 0, sizeof(double)*nj);
        return false;
    }

    /** Get reference acceleration for a joint. Returns the acceleration used to
    * generate the trajectory profile.
    * @param j joint number
    * @param acc pointer to storage for the return value
    * @return true/false on success/failure
    */
    virtual bool getRefAcceleration(int j, double *acc) {
        if (pos)
            return pos->getRefAcceleration(j, acc);
        *acc = 0.0;
        return false;
    }

    /** Get reference acceleration of all joints. These are the values used during the
    * interpolation of the trajectory.
    * @param accs pointer to the array that will store the acceleration values.
    * @return true/false on success or failure
    */
    virtual bool getRefAccelerations(double *accs) {
        if (pos)
            return pos->getRefAccelerations(accs);
        memset(accs, 0, sizeof(double)*nj);
        return false;
    }

    /** Stop motion, single joint
    * @param j joint number
    * @return true/false on success/failure
    */
    virtual bool stop(int j) {
        if (pos)
            return pos->stop(j);
        return false;
    }

    /**
    * Stop motion, multiple joints
    * @return true/false on success/failure
    */
    virtual bool stop() {
        if (pos)
            return pos->stop();
        return false;
    }

    /* IVelocityControl */

    /**
    * Set new reference speed for a single axis.
    * @param j joint number
    * @param v specifies the new ref speed
    * @return true/false on success/failure
    */
    virtual bool velocityMove(int j, double v) {
        if (vel)
            return vel->velocityMove(j, v);
        return false;
    }

    /**
    * Set a new reference speed for all axes.
    * @param v is a vector of double representing the requested speed.
    * @return true/false on success/failure.
    */
    virtual bool velocityMove(const double *v) {
        if (vel)
            return vel->velocityMove(v);
        return false;
    }

    /**
    * Set the controller to velocity mode.
    * @return true/false on success/failure.
    */
    virtual bool setVelocityMode() {
        if (vel)
            return vel->setVelocityMode();
        return false;
    }

    /* IEncoders */

    /**
    * Reset encoder, single joint. Set the encoder value to zero
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool resetEncoder(int j) {
        if (enc)
            return enc->resetEncoder(j);
        return false;
    }

    /**
    * Reset encoders. Set the encoder values to zero for all axes
    * @return true/false
    */
    virtual bool resetEncoders() {
        if (enc)
            return enc->resetEncoders();
        return false;
    }

    /**
    * Set the value of the encoder for a given joint.
    * @param j encoder number
    * @param val new value
    * @return true/false
    */
    virtual bool setEncoder(int j, double val) {
        if (enc)
            return enc->setEncoder(j, val);
        return false;
    }

    /**
    * Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    virtual bool setEncoders(const double *vals) {
        if (enc)
            return enc->setEncoders(vals);
        return false;
    }

    /**
    * Read the value of an encoder.
    * @param j encoder number
    * @param v pointer to storage for the return value
    * @return true/false, upon success/failure (you knew it, uh?)
    */
    virtual bool getEncoder(int j, double *v) {
        if (enc)
            return enc->getEncoder(j, v);
        *v = 0.0;
        return false;
    }

    /**
    * Read the position of all axes.
    * @param encs pointer to the array that will contain the output
    * @return true/false on success/failure
    */
    virtual bool getEncoders(double *encs) {
        if (enc)
            return enc->getEncoders(encs);
        memset(encs, 0, sizeof(double)*nj);
        return false;
    }

    /**
    * Read the istantaneous speed of an axis.
    * @param j axis number
    * @param sp pointer to storage for the output
    * @return true if successful, false ... otherwise.
    */
    virtual bool getEncoderSpeed(int j, double *sp) {
        if (enc)
            return enc->getEncoderSpeed(j, sp);
        *sp = 0.0;
        return false;
    }

    /**
    * Read the instantaneous speed of all axes.
    * @param spds pointer to storage for the output values
    * @return guess what? (true/false on success or failure).
    */
    virtual bool getEncoderSpeeds(double *spds) {
        if (enc)
            return enc->getEncoderSpeeds(spds);
        memset(spds, 0, sizeof(double)*nj);
        return false;
    }

    /**
    * Read the instantaneous acceleration of an axis.
    * @param j axis number
    * @param acc pointer to the array that will contain the output
    */
    virtual bool getEncoderAcceleration(int j, double *acc) {
        if (enc)
            return enc->getEncoderAcceleration(j, acc);
        *acc = 0.0;
        return false;
    }

    /**
    * Read the istantaneous acceleration of all axes.
    * @param accs pointer to the array that will contain the output
    * @return true if all goes well, false if anything bad happens.
    */
    virtual bool getEncoderAccelerations(double *accs) {
        if (enc)
            return enc->getEncoderAccelerations(accs);
        memset(accs, 0, sizeof(double)*nj);
        return false;
    }

    /* IAmplifierControl */

    /**
    * Enable the amplifier on a specific joint. Be careful, check that the output
    * of the controller is appropriate (usually zero), to avoid
    * generating abrupt movements.
    * @return true/false on success/failure
    */
    virtual bool enableAmp(int j) {
        if (amp)
            return amp->enableAmp(j);
        return false;
    }

    /**
    * Disable the amplifier on a specific joint. All computations within the board
    * will be carried out normally, but the output will be disabled.
    * @return true/false on success/failure
    */
    virtual bool disableAmp(int j) {
        if (amp)
            return amp->disableAmp(j);
        return false;
    }

    /**
    * Read the electric current going to all motors.
    * @param vals pointer to storage for the output values
    * @return hopefully true, false in bad luck.
    */
    virtual bool getCurrents(double *vals) {
        if (amp)
            return amp->getCurrents(vals);
        memset(vals, 0, sizeof(double)*nj);

        return false;

    }

    /**
    * Read the electric current going to a given motor.
    * @param j motor number
    * @param val pointer to storage for the output value
    * @return probably true, might return false in bad times
    */
    virtual bool getCurrent(int j, double *val) {
        if (amp)
            return amp->getCurrent(j, val);
        *val = 0.0;
        return false;
    }


    /**
    * Set the maximum electric current going to a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
    * @param j motor number
    * @param v the new value
    * @return probably true, might return false in bad times
    */
    virtual bool setMaxCurrent(int j, double v) {
        if (amp)
            return amp->setMaxCurrent(j, v);
        return false;
    }

    /**
    * Get the status of the amplifiers, coded in a 32 bits integer for
    * each amplifier (at the moment contains only the fault, it will be
    * expanded in the future).
    * @param st pointer to storage
    * @return true in good luck, false otherwise.
    */
    virtual bool getAmpStatus(int *st) {
        if (amp)
            return amp->getAmpStatus(st);
        *st = 0;
        return false;
    }

    virtual bool getAmpStatus(int j, int *v)
    {
        if (amp)
            return amp->getAmpStatus(j, v);
        return false;
    }

    /* IControlLimits */

    /**
    * Set the software limits for a particular axis, the behavior of the
    * control card when these limits are exceeded, depends on the implementation.
    * @param axis joint number (why am I telling you this)
    * @param min the value of the lower limit
    * @param max the value of the upper limit
    * @return true or false on success or failure
    */
    virtual bool setLimits(int axis, double min, double max) {
        if (lim)
            return lim->setLimits(axis, min, max);
        return false;
    }

    /**
    * Get the software limits for a particular axis.
    * @param axis joint number
    * @param min pointer to store the value of the lower limit
    * @param max pointer to store the value of the upper limit
    * @return true if everything goes fine, false if something bad happens (yes, sometimes life is tough)
    */
    virtual bool getLimits(int axis, double *min, double *max) {
        if (lim)
            return lim->getLimits(axis, min, max);
        *min = 0.0;
        *max = 0.0;
        return false;
    }

    /* ITorqueControl */

    /**
     * Set torque control mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @return true/false on success failure
     */
    virtual bool setTorqueMode()
    {
        if (trq)
            return trq->setTorqueMode();
        return false;
    }

   /** Get the reference value of the torque for all joints.
     * This is NOT the feedback (see getTorques instead).
     * @param t pointer to the array of torque values
     * @return true/false
     */
    virtual bool getRefTorques(double *t)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set the reference value of the torque for a given joint.
     * This is NOT the feedback (see getTorque instead).
     * @param j joint number
     * @param t new value
     */
    virtual bool getRefTorque(int j, double *t)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false
     */
    virtual bool setRefTorques(const double *t)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     */
    virtual bool setRefTorque(int j, double t)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set the back-efm compensation gain for a given joint.
     * @param j joint number
     * @param bemf the returned bemf gain of joint j
     * @return true/false on success/failure
     */
    virtual bool getBemfParam(int j, double *bemf)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set the back-efm compensation gain for a given joint.
     * @param j joint number
     * @param bemf new value
     * @return true/false on success/failure
     */
    virtual bool setBemfParam(int j, double bemf)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

     /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setTorquePid(int j, const Pid &pid)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the value of the torque on a given joint (this is the
     * feedback if you have a torque sensor).
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool getTorque(int j, double *t)
    {
        if (trq)
            return trq->getTorque(j, t);
        *t = 0.0;
        return false;
    }

    /** Get the value of the torque for all joints (this is
     * the feedback if you have torque sensors).
     * @param t pointer to the array that will store the output
     */
    virtual bool getTorques(double *t)
    {
        if (trq)
            return trq->getTorques(t);
        memset(t, 0, sizeof(double)*nj);
        return false;
    }

    /** Get the full scale of the torque sensor of a given joint
     * @param j joint number
     * @param min minimum torque of the joint j
     * @param max maximum torque of the joint j
     * @return true/false on success/failure
     */
    virtual bool getTorqueRange(int j, double *min, double *max)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the full scale of the torque sensors of all joints
     * @param min pointer to the array that will store minimum torques of the joints
     * @param max pointer to the array that will store maximum torques of the joints
     * @return true/false on success/failure
     */
    virtual bool getTorqueRanges(double *min, double *max)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setTorquePids(const Pid *pids)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setTorqueErrorLimit(int j, double limit)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the torque error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setTorqueErrorLimits(const double *limits)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the current torque error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getTorqueError(int j, double *err)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the torque error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getTorqueErrors(double *errs)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getTorquePidOutput(int j, double *out)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getTorquePidOutputs(double *outs)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getTorquePid(int j, Pid *pid)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getTorquePids(Pid *pids)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return success/failure
     */
    virtual bool getTorqueErrorLimit(int j, double *limit)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Get the torque error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return success or failure
     */
    virtual bool getTorqueErrorLimits(double *limits)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Reset the controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     */
    virtual bool resetTorquePid(int j)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Disable the pid computation for a joint*/
    virtual bool disableTorquePid(int j)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Enable the pid computation for a joint*/
    virtual bool enableTorquePid(int j)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    /** Set offset value for a given pid*/
    virtual bool setTorqueOffset(int j, double v)
    {
        // @@@ NOT YET IMPLEMENTED
        return false;
    }

    virtual bool setPositionMode(int j)
    {
        if (mod)
            return mod->setPositionMode(j);
        return false;
    }
    virtual bool setVelocityMode(int j)
    {
        if (mod)
            return mod->setVelocityMode(j);
        return false;
    }
    virtual bool setTorqueMode(int j)
    {
        if (mod)
            return mod->setTorqueMode(j);
        return false;
    }
    virtual bool setImpedancePositionMode(int j)
    {
        if (mod)
            return mod->setImpedancePositionMode(j);
        return false;
    }
    virtual bool setImpedanceVelocityMode(int j)
    {
        if (mod)
            return mod->setImpedanceVelocityMode(j);
        return false;
    }
    virtual bool setOpenLoopMode(int j)
    {
        if (mod)
            return mod->setOpenLoopMode(j);
        return false;
    }
    virtual bool getControlMode(int j, int *mode)
    {
        if (mod)
            return mod->getControlMode(j, mode);
        *mode = 0;
        return false;
    }
    virtual bool getControlModes(int* modes)
    {
        if (mod)
            return mod->getControlModes(modes);
        memset(modes, 0, sizeof(double)*nj);
        return false;
    }

    /* IControlCalibration */

    virtual bool calibrate() {
        if (calib)
            return calib->calibrate();
        return false;
    }

    /**
    * Calibrate a single joint, the calibration method accepts a parameter
    * that is used to accomplish various things internally and is implementation
    * dependent.
    * @param j the axis number.
    * @param p is a double value that is passed to the calibration procedure.
    * @return true/false on success/failure.
    */
    virtual bool calibrate(int j, double p) {
        if (calib)
            return calib->calibrate(j, p);
        return false;
    }

    virtual bool calibrate2(int j, unsigned int ui, double v1, double v2, double v3) {
        if (calib2)
            return calib2->calibrate2(j, ui, v1, v2, v3);
        return false;
    }

    /**
    * Check whether the calibration has been completed.
    * @param j is the joint that has started a calibration procedure.
    * @return true/false on success/failure.
    */
    virtual bool done(int j) {
        if (calib2)
            return calib2->done(j);
        return false;
    }

    /* IAxisInfo */
    virtual bool getAxisName(int j, yarp::os::ConstString& name) {
        if (info) {
            return info->getAxisName(j, name);
        }
        return false;
    }

    /* IPreciselyTimed */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    virtual Stamp getLastInputStamp() {
        stampMutex.wait();
        Stamp ret = stamp;
        stampMutex.post();
        return ret;
    }
};

// needed for the driver factory.
yarp::dev::DriverCreator *createServerControlBoard() {
    return new DriverCreatorOf<ServerControlBoard>("controlboard",
        "controlboard",
        "ServerControlBoard");
}

inline yarp::dev::ImplementCallbackHelper::ImplementCallbackHelper() {
}

void yarp::dev::ImplementCallbackHelper::init(yarp::dev::ServerControlBoard *x) {
    pos = dynamic_cast<yarp::dev::IPositionControl *> (x);
    yAssert(pos != 0);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (x);
    yAssert(vel != 0);
}


inline void yarp::dev::ImplementCallbackHelper::onRead(CommandMessage& v) {
    //printf("Data received on the control channel of size: %d\n", v.body.size());
    //int i;

    Bottle& b = v.head;
    //printf("bottle: %s\n", b.toString().c_str());
    switch (b.get(0).asVocab()) {
    case VOCAB_POSITION_MODE:
    case VOCAB_POSITION_MOVES: {
        //            printf("Received a position command\n");
        //            for (i = 0; i < v.body.size(); i++)
        //                printf("%.2f ", v.body[i]);
        //            printf("\n");

        if (pos) {
            bool ok = pos->positionMove(&(v.body[0]));
            if (!ok)
                printf("Issues while trying to start a position move\n");
        }
                               }
                               break;

    case VOCAB_VELOCITY_MODE:
    case VOCAB_VELOCITY_MOVES: {
        //          printf("Received a velocity command\n");
        //          for (i = 0; i < v.body.size(); i++)
        //              printf("%.2f ", v.body[i]);
        //          printf("\n");
        if (vel) {
            bool ok = vel->velocityMove(&(v.body[0]));
            if (!ok)
                printf("Issues while trying to start a velocity move\n");
        }
                               }
                               break;

    default: {
        printf("Unrecognized message while receiving on command port\n");
             }
             break;
    }

    //    printf("v: ");
    //    int i;
    //    for (i = 0; i < (int)v.size(); i++)
    //        printf("%.3f ", v[i]);
    //    printf("\n");
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
yarp::dev::CommandsHelper::CommandsHelper() {}

void yarp::dev::CommandsHelper::init(yarp::dev::ServerControlBoard *x) {
    yAssert(x != NULL);
    caller = x;
    trq   = dynamic_cast<yarp::dev::ITorqueControl *> (caller);
    mod   = dynamic_cast<yarp::dev::IControlMode *> (caller);
    pid   = dynamic_cast<yarp::dev::IPidControl *> (caller);
    pos   = dynamic_cast<yarp::dev::IPositionControl *> (caller);
    vel   = dynamic_cast<yarp::dev::IVelocityControl *> (caller);
    enc   = dynamic_cast<yarp::dev::IEncoders *> (caller);
    amp   = dynamic_cast<yarp::dev::IAmplifierControl *> (caller);
    lim   = dynamic_cast<yarp::dev::IControlLimits *> (caller);
    info  = dynamic_cast<yarp::dev::IAxisInfo *> (caller);
    ical2 = dynamic_cast<yarp::dev::IControlCalibration2 *> (caller);
    nj    = 0;
}

bool yarp::dev::CommandsHelper::initialize() {
    bool ok = false;
    if (pos) {
        ok = pos->getAxes(&nj);
    }
    DeviceResponder::makeUsage();
    addUsage("[get] [axes]", "get the number of axes");
    addUsage("[get] [name] $iAxisNumber", "get a human-readable name for an axis, if available");
    addUsage("[set] [pos] $iAxisNumber $fPosition", "command the position of an axis");
    addUsage("[set] [rel] $iAxisNumber $fPosition", "command the relative position of an axis");
    addUsage("[set] [vmo] $iAxisNumber $fVelocity", "command the velocity of an axis");
    addUsage("[get] [encs]", "get all encoder values");
    addUsage("[get] [enc] $iAxisNumber", "get the encoder value for an axis");


    ConstString args;
    for (int i=0; i<nj; i++) {
        if (i>0) {
            args += " ";
        }
        args = args + "$f" + Bottle::toString(i);
    }
    addUsage((ConstString("[set] [poss] (")+args+")").c_str(),
        "command the position of all axes");
    addUsage((ConstString("[set] [rels] (")+args+")").c_str(),
        "command the relative position of all axes");
    addUsage((ConstString("[set] [vmos] (")+args+")").c_str(),
        "command the velocity of all axes");

    addUsage("[set] [aen] $iAxisNumber", "enable (amplifier for) the given axis");
    addUsage("[set] [adi] $iAxisNumber", "disable (amplifier for) the given axis");
    addUsage("[get] [acu] $iAxisNumber", "get current for the given axis");
    addUsage("[get] [acus]", "get current for all axes");

    return ok;
}

bool yarp::dev::CommandsHelper::respond(const yarp::os::Bottle& cmd,
                                        yarp::os::Bottle& response)
{
    bool ok = false;
    bool rec = false; // is the command recognized?
    if (caller->verbose())
        printf("command received: %s\n", cmd.toString().c_str());
    int code = cmd.get(0).asVocab();
    switch (code) {
case VOCAB_CALIBRATE_JOINT:
    {
        rec=true;
        if (caller->verbose())
            printf("Calling calibrate joint\n");

        int j=cmd.get(1).asInt();
        int ui=cmd.get(2).asInt();
        double v1=cmd.get(3).asDouble();
        double v2=cmd.get(4).asDouble();
        double v3=cmd.get(5).asDouble();
        if (ical2==0)
            printf("Sorry I don't have a IControlCalibration2 interface\n");
        else
            ok=ical2->calibrate2(j,ui,v1,v2,v3);
    }
    break;
case VOCAB_CALIBRATE:
    {
        rec=true;
        if (caller->verbose())
            printf("Calling calibrate\n");
        ok=ical2->calibrate();
    }
    break;
case VOCAB_CALIBRATE_DONE:
    {
        rec=true;
        if (caller->verbose())
            printf("Calling calibrate done\n");
        int j=cmd.get(1).asInt();
        ok=ical2->done(j);
    }
    break;
case VOCAB_PARK:
    {
        rec=true;
        if (caller->verbose())
            printf("Calling park function\n");
        int flag=cmd.get(1).asInt();
        if (flag)
            ok=ical2->park(true);
        else
            ok=ical2->park(false);
        ok=true; //client would get stuck if returning false
    }
    break;
case VOCAB_SET:
    rec = true;
    if (caller->verbose())
        printf("set command received\n");
    {
        switch(cmd.get(1).asVocab())
        {
        case VOCAB_IMPEDANCE:
            //TO BE IMPLEMENTED
            break;

        case VOCAB_ICONTROLMODE:
            {
                int axis = cmd.get(3).asInt();
                switch(cmd.get(2).asVocab()) {
                    case VOCAB_CM_POSITION:
                        ok = mod->setPositionMode(axis);
                        break;
                    case VOCAB_CM_VELOCITY:
                        ok = mod->setVelocityMode(axis);
                        break;
                    case VOCAB_CM_TORQUE:
                        ok = mod->setTorqueMode(axis);
                        break;
                    case VOCAB_CM_IMPEDANCE_POS:
                        ok = mod->setImpedancePositionMode(axis);
                        break;
                    case VOCAB_CM_IMPEDANCE_VEL:
                        ok = mod->setImpedanceVelocityMode(axis);
                        break;
                    case VOCAB_CM_OPENLOOP:
                        ok = mod->setOpenLoopMode(axis);
                        break;
                }
            }
            break;
        case VOCAB_OFFSET:
            {
                double v;
                int j = cmd.get(2).asInt();
                Bottle& b = *(cmd.get(3).asList());
                v=b.get(0).asDouble();
                ok = pid->setOffset(j, v);
            }
            break;
        case VOCAB_PID:
            {
                Pid p;
                int j = cmd.get(2).asInt();
                Bottle& b = *(cmd.get(3).asList());
                p.kp = b.get(0).asDouble();
                p.kd = b.get(1).asDouble();
                p.ki = b.get(2).asDouble();
                p.max_int = b.get(3).asDouble();
                p.max_output = b.get(4).asDouble();
                p.offset = b.get(5).asDouble();
                p.scale = b.get(6).asDouble();
                p.stiction_up_val = b.get(7).asDouble();
                p.stiction_down_val = b.get(8).asDouble();
                ok = pid->setPid(j, p);
            }
            break;

        case VOCAB_PIDS:
            {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                yAssert(njs == nj);
                Pid *p = new Pid[njs];
                yAssert(p != NULL);
                for (i = 0; i < njs; i++)
                {
                    Bottle& c = *(b.get(i).asList());
                    p[i].kp = c.get(0).asDouble();
                    p[i].kd = c.get(1).asDouble();
                    p[i].ki = c.get(2).asDouble();
                    p[i].max_int = c.get(3).asDouble();
                    p[i].max_output = c.get(4).asDouble();
                    p[i].offset = c.get(5).asDouble();
                    p[i].scale = c.get(6).asDouble();
                    p[i].stiction_up_val = c.get(7).asDouble();
                    p[i].stiction_down_val = c.get(8).asDouble();
                }
                ok = pid->setPids(p);
                delete[] p;
            }
            break;

        case VOCAB_REF:
            {
                ok = pid->setReference (cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_REFS:
            {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                yAssert(njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                yAssert(p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pid->setReferences (p);
                delete[] p;
            }
            break;

        case VOCAB_LIM:
            {
                ok = pid->setErrorLimit (cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_LIMS:
            {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                yAssert(njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                yAssert(p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pid->setErrorLimits (p);
                delete[] p;
            }
            break;

        case VOCAB_RESET:
            {
                ok = pid->resetPid (cmd.get(2).asInt());
            }
            break;

        case VOCAB_DISABLE: {
            ok = pid->disablePid (cmd.get(2).asInt());
                            }
                            break;

        case VOCAB_ENABLE:
            {
                ok = pid->enablePid (cmd.get(2).asInt());
            }
            break;

        case VOCAB_VELOCITY_MODE:
            {
                ok = vel->setVelocityMode();
            }
            break;

        case VOCAB_VELOCITY_MOVE:
            {
                ok = vel->velocityMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_POSITION_MODE:
            {
                ok = pos->setPositionMode();
            }
            break;

        case VOCAB_POSITION_MOVE:
            {
                ok = pos->positionMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

            // this operation is also available on "command" port
        case VOCAB_POSITION_MOVES:
            {
                Bottle *b = cmd.get(2).asList();
                int i;
                if (b==NULL) break;
                const int njs = b->size();
                yAssert(njs == nj);
                vect.size(nj);
                for (i = 0; i < njs; i++) {
                    vect[i] = b->get(i).asDouble();
                }
                if (pos!=NULL) {
                    ok = pos->positionMove(&vect[0]);
                }

            }
            break;

            // this operation is also available on "command" port
        case VOCAB_VELOCITY_MOVES:
            {

                Bottle *b = cmd.get(2).asList();
                int i;
                if (b==NULL) break;
                const int njs = b->size();
                yAssert(njs == nj);
                vect.size(nj);
                for (i = 0; i < njs; i++)
                    vect[i] = b->get(i).asDouble();
                if (vel!=NULL) {
                    ok = vel->velocityMove(&vect[0]);
                }
            }
            break;

        case VOCAB_RELATIVE_MOVE:
            {
                ok = pos->relativeMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_RELATIVE_MOVES:
            {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                yAssert(njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                yAssert(p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pos->relativeMove(p);
                delete[] p;
            }
            break;

        case VOCAB_REF_SPEED:
            {
                ok = pos->setRefSpeed(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_REF_SPEEDS:
            {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                yAssert(njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                yAssert(p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pos->setRefSpeeds(p);
                delete[] p;
            }
            break;

        case VOCAB_REF_ACCELERATION:
            {
                ok = pos->setRefAcceleration(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_REF_ACCELERATIONS:
            {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                yAssert(njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                yAssert(p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pos->setRefAccelerations(p);
                delete[] p;
            }
            break;

        case VOCAB_STOP:
            {
                ok = pos->stop(cmd.get(2).asInt());
            }
            break;

        case VOCAB_STOPS:
            {
                ok = pos->stop();
            }
            break;

        case VOCAB_E_RESET:
            {
                ok = enc->resetEncoder(cmd.get(2).asInt());
            }
            break;

        case VOCAB_E_RESETS:
            {
                ok = enc->resetEncoders();
            }
            break;

        case VOCAB_ENCODER:
            {
                ok = enc->setEncoder(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_ENCODERS:
            {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                yAssert(njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                yAssert(p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = enc->setEncoders(p);
                delete[] p;
            }
            break;

        case VOCAB_AMP_ENABLE:
            {
                ok = amp->enableAmp(cmd.get(2).asInt());
            }
            break;

        case VOCAB_AMP_DISABLE:
            {
                ok = amp->disableAmp(cmd.get(2).asInt());
            }
            break;

        case VOCAB_AMP_MAXCURRENT:
            {
                ok = amp->setMaxCurrent(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

        case VOCAB_LIMITS:
            {
                ok = lim->setLimits(cmd.get(2).asInt(), cmd.get(3).asDouble(), cmd.get(4).asDouble());
            }
            break;

        default:
            printf("received an unknown command after a VOCAB_SET\n");
            break;
        }
    }
    break;

case VOCAB_GET:
    rec = true;
    if (caller->verbose())
        printf("get command received\n");

    {
        int tmp = 0;
        double dtmp = 0.0;
        response.addVocab(VOCAB_IS);
        response.add(cmd.get(1));
        switch(cmd.get(1).asVocab()) {

case VOCAB_TORQUE:
    {
        switch(cmd.get(2).asVocab()) {
        case VOCAB_TRQ:
            {
                ok= trq->getTorque(cmd.get(3).asInt(),&dtmp);
                response.addDouble(dtmp);
            }
            break;
        case VOCAB_TRQS:
            {
                double *p = new double[nj];
                yAssert(p!=NULL);
                ok= trq->getTorques(p);
                Bottle& b = response.addList();
                response.addDouble(dtmp);
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
            break;
        }
    }
    break;

case VOCAB_IMPEDANCE:
    //TO BE IMPLEMENTED
break;

case VOCAB_ICONTROLMODE:
    {
        switch(cmd.get(2).asVocab()) {
        case VOCAB_CM_CONTROL_MODES:
            {

                int *p = new int[nj];
                yAssert(p!=NULL);
                ok = mod->getControlModes(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addVocab(p[i]);
                delete[] p;
            }
            break;

        case VOCAB_CM_CONTROL_MODE:
            {
                ok= mod->getControlMode(cmd.get(3).asInt(),&tmp);
                response.addVocab(tmp);
            }
            break;
        }
    }
    break;

case VOCAB_ERR:
    {
        ok = pid->getError(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_ERRS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = pid->getErrors(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_OUTPUT:
    {
        ok = pid->getOutput(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_OUTPUTS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = pid->getOutputs(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_PID:
    {
        Pid p;
        ok = pid->getPid(cmd.get(2).asInt(), &p);
        Bottle& b = response.addList();
        b.addDouble(p.kp);
        b.addDouble(p.kd);
        b.addDouble(p.ki);
        b.addDouble(p.max_int);
        b.addDouble(p.max_output);
        b.addDouble(p.offset);
        b.addDouble(p.scale);
        b.addDouble(p.stiction_up_val);
        b.addDouble(p.stiction_down_val);
    }
    break;

case VOCAB_PIDS:
    {
        Pid *p = new Pid[nj];
        yAssert(p != NULL);
        ok = pid->getPids(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
        {
            Bottle& c = b.addList();
            c.addDouble(p[i].kp);
            c.addDouble(p[i].kd);
            c.addDouble(p[i].ki);
            c.addDouble(p[i].max_int);
            c.addDouble(p[i].max_output);
            c.addDouble(p[i].offset);
            c.addDouble(p[i].scale);
            c.addDouble(p[i].stiction_up_val);
            c.addDouble(p[i].stiction_down_val);
        }
        delete[] p;
    }
    break;

case VOCAB_REFERENCE:
    {
        ok = pid->getReference(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_REFERENCES:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = pid->getReferences(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_LIM:
    {
        ok = pid->getErrorLimit(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_LIMS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = pid->getErrorLimits(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_AXES:
    ok = pos->getAxes(&tmp);
    response.addInt(tmp);
    break;

case VOCAB_MOTION_DONE:
    {
        bool x = false;;
        ok = pos->checkMotionDone(cmd.get(2).asInt(), &x);
        response.addInt(x);
    }
    break;

case VOCAB_MOTION_DONES:
    {
        bool x = false;
        ok = pos->checkMotionDone(&x);
        response.addInt(x);
    }
    break;

case VOCAB_REF_SPEED:
    {
        ok = pos->getRefSpeed(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_REF_SPEEDS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = pos->getRefSpeeds(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_REF_ACCELERATION:
    {
        ok = pos->getRefAcceleration(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_REF_ACCELERATIONS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = pos->getRefAccelerations(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_ENCODER:
    {
        ok = enc->getEncoder(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_ENCODERS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = enc->getEncoders(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_ENCODER_SPEED:
    {
        ok = enc->getEncoderSpeed(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);

    }
    break;

case VOCAB_ENCODER_SPEEDS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = enc->getEncoderSpeeds(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_ENCODER_ACCELERATION:
    {
        ok = enc->getEncoderAcceleration(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_ENCODER_ACCELERATIONS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = enc->getEncoderAccelerations(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_AMP_CURRENT:
    {
        ok = amp->getCurrent(cmd.get(2).asInt(), &dtmp);
        response.addDouble(dtmp);
    }
    break;

case VOCAB_AMP_CURRENTS:
    {
        double *p = new double[nj];
        yAssert(p!=NULL);
        ok = amp->getCurrents(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addDouble(p[i]);
        delete[] p;
    }
    break;

case VOCAB_AMP_STATUS:
    {
        int *p = new int[nj];
        yAssert(p!=NULL);
        ok = amp->getAmpStatus(p);
        Bottle& b = response.addList();
        int i;
        for (i = 0; i < nj; i++)
            b.addInt(p[i]);
        delete[] p;
    }
    break;

case VOCAB_AMP_STATUS_SINGLE:
    {
        int itmp=0;
        ok = amp->getAmpStatus(cmd.get(2).asInt(),  &tmp);
        response.addInt(itmp);
    }
    break;

case VOCAB_LIMITS:
    {
        double min = 0.0, max = 0.0;
        ok = lim->getLimits(cmd.get(2).asInt(), &min, &max);
        response.addDouble(min);
        response.addDouble(max);
    }
    break;

case VOCAB_INFO_NAME:
    {
        ConstString name = "undocumented";
        ok = info->getAxisName(cmd.get(2).asInt(),name);
        response.addString(name.c_str());
    }
    break;
default:
    printf("received an unknown request after a VOCAB_GET: %s\n", yarp::os::Vocab::decode(cmd.get(1).asVocab()).c_str());
    break;
        }
    }
    break;
    }

    if (!rec) {
        ok = DeviceResponder::respond(cmd,response);
    }

    if (!ok) {
        // failed thus send only a VOCAB back.
        response.clear();
        response.addVocab(VOCAB_FAILED);
    }
    else
        response.addVocab(VOCAB_OK);
    return ok;
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

