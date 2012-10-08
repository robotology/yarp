// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __CONTROLBAORDWRAPPER__
#define __CONTROLBAORDWRAPPER__

/*
* Copyright (C) 2008 RobotCub Consortium
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// ControlBoardWrapper
// A modified version of the YARP remote control board class
// which remaps joints.
//


#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/PreciselyTimed.h>

#include <yarp/sig/Vector.h>

#include <yarp/os/Semaphore.h>
#include <yarp/dev/Wrapper.h>

#include <string>

#ifdef WIN32
    #pragma warning(disable:4355)
#endif

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* the control command message type
* head is a Bottle which contains the specification of the message type
* body is a Vector which move the robot accordingly
*/
typedef PortablePair<Bottle, Vector> CommandMessage;

class ControlBoardWrapper;

/**
* Helper object for reading config commands for the ControlBoardWrapper
* class.
*/
class CommandsHelper : public DeviceResponder {
protected:
    ControlBoardWrapper   *caller;
    yarp::dev::IPidControl          *pid;
    yarp::dev::IPositionControl     *pos;
    yarp::dev::IVelocityControl     *vel;
    yarp::dev::IEncoders            *enc;
    yarp::dev::IAmplifierControl    *amp;
    yarp::dev::IControlLimits       *lim;
    yarp::dev::ITorqueControl       *torque;
    yarp::dev::IAxisInfo            *info;
    yarp::dev::IControlCalibration2 *ical2;
    yarp::dev::IControlMode         *iControlMode;
    yarp::dev::IOpenLoopControl     *iOpenLoop;
    int controlledJoints;
    Vector vect;

    yarp::os::Stamp lastRpcStamp;
    Semaphore mutex;

public:
    /**
    * Constructor.
    * @param x is the pointer to the instance of the object that uses the CommandsHelper.
    * This is required to recover the pointers to the interfaces that implement the responses
    * to the commands.
    */
    CommandsHelper(ControlBoardWrapper *x);

    virtual bool respond(const Bottle& cmd, Bottle& response);
    void handleTorqueMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleControlModeMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    /**
    * Initialize the internal data.
    * @return true/false on success/failure
    */
    virtual bool initialize();
};


/**
* Callback implementation after buffered input.
*/
class ImplementCallbackHelper : public TypedReaderCallback<CommandMessage> {
protected:
    IPositionControl *pos;
    IVelocityControl *vel;

public:
    /**
    * Constructor.
    * @param x is the instance of the container class using the callback.
    */
    ImplementCallbackHelper(ControlBoardWrapper *x);

    /**
    * Callback function.
    * @param v is the Vector being received.
    */
    virtual void onRead(CommandMessage& v);
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

/*
* A modified version of the network wrapper. Similar
* to the the network wrapper in YARP, but it
* maps only a subpart of the underlying device.
* Allows also deferred attach/detach of a subdevice.
*/
class ControlBoardWrapper :
    public DeviceDriver,
    public RateThread,
    public IPidControl,
    public IPositionControl,
    public IVelocityControl,
    public IEncoders,
    public IAmplifierControl,
    public IControlLimits,
    public IControlCalibration,
    public IControlCalibration2,
    public IMultipleWrapper,
    public ITorqueControl,
    public IControlMode,
    public IOpenLoopControl,
    public IAxisInfo
    // convenient to put these here just to make sure all
    // methods get implemented
{
private:
    bool spoke;
    bool verb;

    yarp::os::Stamp lastStateStamp;


    Port state_p;   // out port to read the state
    Port control_p; // in port to command the robot
    Port rpc_p;     // RPC to configure the robot

    PortWriterBuffer<yarp::sig::Vector> state_buffer;
    PortReaderBuffer<CommandMessage> control_buffer;

    ImplementCallbackHelper callback_impl;

    CommandsHelper command_reader;

    // for new interface
    PortReaderBuffer<Bottle> command_buffer;

    PolyDriver *subdevice;
    Vector      encoders;
    std::string      partName;

    int               deviceJoints;
    int               controlledJoints;
    int               base;
    int               top;
    int               thread_period;
    IPidControl       *pid;
    IPositionControl  *pos;
    IVelocityControl  *vel;
    IEncoders         *enc;
    IAmplifierControl *amp;
    IControlLimits    *lim;
    ITorqueControl    *torque;
    IControlCalibration *calib;
    IControlCalibration2 *calib2;
    IPreciselyTimed      *iTimed;
    IAxisInfo          *info;
    IControlMode        *iControlMode;
    IOpenLoopControl   *iOpenLoop;

    bool closeMain() {
        if (RateThread::isRunning()) {
            RateThread::stop();
        }

        // close the port connections here!
        rpc_p.close();
        control_p.close();
        state_p.close();

        return true;
    }


public:
    /**
    * Constructor.
    */
    ControlBoardWrapper() : RateThread(20), callback_impl(this), command_reader(this)
    {
        pid = NULL;
        pos = NULL;
        vel = NULL;
        enc = NULL;
        amp = NULL;
        lim = NULL;
        calib = NULL;
        calib2 = NULL;
        iTimed= NULL;
        info = NULL;
        torque=NULL;
        iControlMode=0;
        iOpenLoop=0;
        controlledJoints = 0;
        deviceJoints=0;
        thread_period = 20; // ms.
        subdevice=0;
        base=-1;
        top=-1;
        verb = false;
    }

    virtual ~ControlBoardWrapper() {
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
    virtual bool open(Searchable& prop)
    {
        printf("Using ControlBoardWrapper\n");
        //printf("%s\n", prop.toString().c_str());

        verb = (prop.check("verbose","if present, give detailed output"));
        if (verb)
            printf("running with verbose output\n");

        thread_period = prop.check("threadrate", 20, "thread rate in ms. for streaming encoder data").asInt();

        base=prop.check("start", -1, "starting joint for remapping").asInt();
        top=prop.check("end", -1, "last joint for remapping").asInt();

        partName=prop.check("name",Value("controlboard"),
            "prefix for port names").asString().c_str();
        std::string rootName="/";
        rootName+=(partName);

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

        return true;
    }

    virtual bool attachAll(const PolyDriverList &sub);
    virtual bool detachAll();

    /**
    * The thread main loop deals with writing on ports here.
    */
    virtual void run()
    {
        yarp::sig::Vector& v = state_buffer.get();
        v.size(controlledJoints);
        if (enc!=NULL) {
            enc->getEncoders(&encoders[0]);
        }

        for(int l=0;l<controlledJoints;l++)
        {
            v[l]=encoders[l+base];
        }


        lastStateStamp.update();
        state_p.setEnvelope(lastStateStamp);

        state_buffer.write();
    }

    /* IPidControl */
    /** Set new pid value for a joint axis.
    * @param j joint number
    * @param p new pid value
    * @return true/false on success/failure
    */
    virtual bool setPid(int j, const Pid &p) {
        if (pid)
            return pid->setPid(j+base, p);
        return false;
    }

    /** Set new pid value on multiple axes.
    * @param ps pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual bool setPids(const Pid *ps) {
        bool ret=false;
        if (pid) {
            ret=true;
            for(int l=0;l<controlledJoints;l++)
                ret=ret&&pid->setPid(l+base,ps[l]);
        }

        return ret;
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
            return pid->setReference(j+base, ref);
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
        bool ret=false;
        if (pid) {
            ret=true;
            for(int l=0;l<controlledJoints;l++)
                ret=ret&&pid->setReference(l+base,refs[l]);
        }

        return ret;
    }

    /** Set the error limit for the controller on a specifi joint
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual bool setErrorLimit(int j, double limit) {
        if (pid)
            return pid->setErrorLimit(j+base, limit);
        return false;
    }

    /** Get the error limit for the controller on all joints.
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual bool setErrorLimits(const double *limits) {
        bool ret=false;
        if (pid) {
            ret=true;
            for(int l=0;l<controlledJoints;l++)
                ret=ret&&pid->setErrorLimit(l+base,limits[l]);
        }

        return ret;
    }

    /** Get the current error for a joint.
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual bool getError(int j, double *err) {
        if (pid)
            return pid->getError(j+base, err);
        *err = 0.0;
        return false;
    }

    /** Get the error of all joints.
    * @param errs pointer to the vector that will store the errors.
    * @return true/false on success/failure.
    */
    virtual bool getErrors(double *errs) {
        bool ret=false;
        if (pid) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pid->getError(j+base, errs+j);
        }

        return ret;
    }

    /** Get the output of the controller (e.g. pwm value)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual bool getOutput(int j, double *out) {
        if (pid)
            return pid->getOutput(j+base, out);
        *out = 0.0;
        return false;
    }

    /** Get the output of the controllers (e.g. pwm value)
    * @param outs pinter to the vector that will store the output values
    */
    virtual bool getOutputs(double *outs) {
        bool ret=false;
        if (pid) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pid->getOutput(j+base, outs+j);
        }
        return ret;
    }

    virtual bool setOffset(int j, double v)
    {
        if (pid)
            return pid->setOffset(j+base,v);

        return false;
    }

    /** Get current pid value for a specific joint.
    * @param j joint number
    * @param p pointer to storage for the return value.
    * @return success/failure
    */
    virtual bool getPid(int j, Pid *p) {
        if (pid)
            return pid->getPid(j+base, p);
        return false;
    }

    /** Get current pid value for a specific joint.
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual bool getPids(Pid *pids) {
        bool ret=false;
        if (pid) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pid->getPid(j+base, pids+j);
        }
        return ret;
    }

    /** Get the current reference position of the controller for a specific joint.
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual bool getReference(int j, double *ref) {
        if (pid)
            return pid->getReference(j+base, ref);
        *ref=0.0;
        return false;
    }

    /** Get the current reference position of all controllers.
    * @param refs vector that will store the output.
    */
    virtual bool getReferences(double *refs) {
        bool ret=false;
        if (pid) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pid->getReference(j+base, refs+j);
        }
        return false;
    }

    /** Get the error limit for the controller on a specific joint
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual bool getErrorLimit(int j, double *limit) {
        if (pid)
            return pid->getErrorLimit(j+base, limit);
        *limit = 0.0;
        return false;
    }

    /** Get the error limit for all controllers
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual bool getErrorLimits(double *limits) {
        bool ret=false;
        if (pid) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pid->getErrorLimit(j+base, limits+j);
        }
        return ret;
    }

    /** Reset the controller of a given joint, usually sets the
    * current position of the joint as the reference value for the PID, and resets
    * the integrator.
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool resetPid(int j) {
        if (pid)
            return pid->resetPid(j+base);
        return false;
    }

    /**
    * Disable the pid computation for a joint
    * @param j is the axis number
    * @return true if successful, false on failure
    **/
    virtual bool disablePid(int j) {
        if (pid)
            return pid->disablePid(j+base);
        return false;
    }

    /**
    * Enable the pid computation for a joint
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool enablePid(int j) {
        if (pid)
            return pid->enablePid(j+base);
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
        {
            *ax=controlledJoints;
            return true;
        }
        if (vel)
        {
            *ax=controlledJoints;
            return true;
        }

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

    virtual bool setOpenLoopMode() {
        if (iOpenLoop)
            return iOpenLoop->setOpenLoopMode();
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
            return pos->positionMove(j+base, ref);
        return false;
    }

    /** Set new reference point for all axes.
    * @param refs array, new reference points.
    * @return true/false on success/failure
    */
    virtual bool positionMove(const double *refs) {
        bool ret=false;
        if (pos) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pos->positionMove(j+base, refs[j]);
        }

        return ret;
    }

    /** Set relative position. The command is relative to the
    * current position of the axis.
    * @param j joint axis number
    * @param delta relative command
    * @return true/false on success/failure
    */
    virtual bool relativeMove(int j, double delta) {
        if (pos)
            return pos->relativeMove(j+base, delta);
        return false;
    }

    /** Set relative position, all joints.
    * @param deltas pointer to the relative commands
    * @return true/false on success/failure
    */
    virtual bool relativeMove(const double *deltas) {
        bool ret=false;
        if (pos) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pos->relativeMove(j+base, deltas[j]);
        }
        return ret;
    }

    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param j the axis
    * @param flag true if the trajectory is terminated, false otherwise
    * @return false on failure
    */
    virtual bool checkMotionDone(int j, bool *flag) {
        if (pos)
            return pos->checkMotionDone(j+base, flag);
        *flag = true;
        return false;
    }

    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param flag true if the trajectory is terminated, false otherwise
    * @return false on failure
    */
    virtual bool checkMotionDone(bool *flag) {
        bool ret=false;
        *flag=true;
        if (pos)
        {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
            {
                bool tmpF=false;
                ret=ret&&pos->checkMotionDone(j+base,&tmpF);
                *flag=*flag&&tmpF;
            }
            return ret;
        }

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
            return pos->setRefSpeed(j+base, sp);
        return false;
    }

    /** Set reference speed on all joints. These values are used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array of speed values.
    * @return true/false upon success/failure
    */
    virtual bool setRefSpeeds(const double *spds) {
        bool ret=false;
        if (pos) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pos->setRefSpeed(j+base, spds[j]);
        }
        return ret;
    }

    /** Set reference acceleration for a joint. This value is used during the
    * trajectory generation.
    * @param j joint number
    * @param acc acceleration value
    * @return true/false upon success/failure
    */
    virtual bool setRefAcceleration(int j, double acc) {
        if (pos)
            return pos->setRefAcceleration(j+base, acc);
        return false;
    }

    /** Set reference acceleration on all joints. This is the valure that is
    * used during the generation of the trajectory.
    * @param accs pointer to the array of acceleration values
    * @return true/false upon success/failure
    */
    virtual bool setRefAccelerations(const double *accs) {
        bool ret=false;
        if (pos) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pos->setRefAcceleration(j+base, accs[j]);
        }
        return ret;
    }

    /** Get reference speed for a joint. Returns the speed used to
    * generate the trajectory profile.
    * @param j joint number
    * @param ref pointer to storage for the return value
    * @return true/false on success or failure
    */
    virtual bool getRefSpeed(int j, double *ref) {
        if (pos)
            return pos->getRefSpeed(j+base, ref);
        *ref = 0.0;
        return false;
    }

    /** Get reference speed of all joints. These are the  values used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array that will store the speed values.
    * @return true/false on success/failure.
    */
    virtual bool getRefSpeeds(double *spds) {
        bool ret=false;
        if (pos) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pos->getRefSpeed(j+base, spds+j);
        }
        return ret;
    }

    /** Get reference acceleration for a joint. Returns the acceleration used to
    * generate the trajectory profile.
    * @param j joint number
    * @param acc pointer to storage for the return value
    * @return true/false on success/failure
    */
    virtual bool getRefAcceleration(int j, double *acc) {
        if (pos)
            return pos->getRefAcceleration(j+base, acc);
        *acc = 0.0;
        return false;
    }

    /** Get reference acceleration of all joints. These are the values used during the
    * interpolation of the trajectory.
    * @param accs pointer to the array that will store the acceleration values.
    * @return true/false on success or failure
    */
    virtual bool getRefAccelerations(double *accs) {
        bool ret=false;
        if (pos) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&pos->getRefAcceleration(j+base, accs+j);
        }
        return ret;
    }

    /** Stop motion, single joint
    * @param j joint number
    * @return true/false on success/failure
    */
    virtual bool stop(int j) {
        if (pos)
            return pos->stop(j+base);
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
            return vel->velocityMove(j+base, v);
        return false;
    }

    /**
    * Set a new reference speed for all axes.
    * @param v is a vector of double representing the requested speed.
    * @return true/false on success/failure.
    */
    virtual bool velocityMove(const double *v) {
        bool ret=false;
        if (vel) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&vel->velocityMove(j+base, v[j]);
        }
        return ret;
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
            return enc->resetEncoder(j+base);
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
            return enc->setEncoder(j+base, val);
        return false;
    }

    /**
    * Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    virtual bool setEncoders(const double *vals) {
        bool ret=false;
        if (enc) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&enc->setEncoder(j+base, vals[j]);
        }
        return ret;
    }

    /**
    * Read the value of an encoder.
    * @param j encoder number
    * @param v pointer to storage for the return value
    * @return true/false, upon success/failure (you knew it, uh?)
    */
    virtual bool getEncoder(int j, double *v) {
        if (enc)
            return enc->getEncoder(j+base, v);
        *v = 0.0;
        return false;
    }

    /**
    * Read the position of all axes.
    * @param encs pointer to the array that will contain the output
    * @return true/false on success/failure
    */
    virtual bool getEncoders(double *encs) {
        bool ret=false;
        if (enc) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&enc->getEncoder(j+base, encs+j);
        }
        return ret;
    }

    /**
    * Read the istantaneous speed of an axis.
    * @param j axis number
    * @param sp pointer to storage for the output
    * @return true if successful, false ... otherwise.
    */
    virtual bool getEncoderSpeed(int j, double *sp) {
        if (enc)
            return enc->getEncoderSpeed(j+base, sp);
        *sp = 0.0;
        return false;
    }

    /**
    * Read the instantaneous speed of all axes.
    * @param spds pointer to storage for the output values
    * @return guess what? (true/false on success or failure).
    */
    virtual bool getEncoderSpeeds(double *spds) {
        bool ret=false;
        if (enc) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&enc->getEncoderSpeed(j+base, spds+j);
        }
        return ret;
    }

    /**
    * Read the instantaneous acceleration of an axis.
    * @param j axis number
    * @param acc pointer to the array that will contain the output
    */
    virtual bool getEncoderAcceleration(int j, double *acc) {
        if (enc)
            return enc->getEncoderAcceleration(j+base, acc);
        *acc = 0.0;
        return false;
    }

    /**
    * Read the istantaneous acceleration of all axes.
    * @param accs pointer to the array that will contain the output
    * @return true if all goes well, false if anything bad happens.
    */
    virtual bool getEncoderAccelerations(double *accs) {
        bool ret=false;
        if (enc) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&enc->getEncoderAcceleration(j+base, accs+j);
        }
        return ret;
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
            return amp->enableAmp(j+base);
        return false;
    }

    /**
    * Disable the amplifier on a specific joint. All computations within the board
    * will be carried out normally, but the output will be disabled.
    * @return true/false on success/failure
    */
    virtual bool disableAmp(int j) {
        if (amp)
            return amp->disableAmp(j+base);
        return false;
    }

    /**
    * Read the electric current going to all motors.
    * @param vals pointer to storage for the output values
    * @return hopefully true, false in bad luck.
    */
    virtual bool getCurrents(double *vals) {
        bool ret=false;
        if (amp) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&amp->getCurrent(j+base, vals+j);
        }
        return ret;
    }

    /**
    * Read the electric current going to a given motor.
    * @param j motor number
    * @param val pointer to storage for the output value
    * @return probably true, might return false in bad times
    */
    virtual bool getCurrent(int j, double *val) {
        if (amp)
            return amp->getCurrent(j+base, val);
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
            return amp->setMaxCurrent(j+base, v);
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
        // AMP_STATUS TODO
        return true; //bypass
        if (amp)
            return amp->getAmpStatus(st);
        *st = 0;
        return false;
    }

    virtual bool getAmpStatus(int k, int *st)
    {
        // AMP_STATUS TODO
        if (amp)
            return amp->getAmpStatus(k, st);
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
            return lim->setLimits(axis+base, min, max);
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
            return lim->getLimits(axis+base, min, max);
        *min = 0.0;
        *max = 0.0;
        return false;
    }

    /* IControlCalibration */

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
            return calib->calibrate(j+base, p);
        return false;
    }

    virtual bool calibrate2(int j, unsigned int ui, double v1, double v2, double v3) {
        if (calib2)
            return calib2->calibrate2(j+base, ui, v1, v2, v3);
        return false;
    }

    /**
    * Check whether the calibration has been completed.
    * @param j is the joint that has started a calibration procedure.
    * @return true/false on success/failure.
    */
    virtual bool done(int j) {
        if (calib2)
            return calib2->done(j+base);
        return false;
    }

    virtual bool abortPark()
    {
        fprintf(stderr, "ControlBoardWrapper::Calling abortPark\n");
        if (calib2)
            return calib2->abortPark();
        return false;
    }

    virtual bool abortCalibration()
    {
        fprintf(stderr, "ControlBoardWrapper::Calling abortCalibration\n");
        if (calib2)
            return calib2->abortCalibration();
        else
            fprintf(stderr, "calib2 was null\n");
        return false;
    }

    /* IAxisInfo */
    virtual bool getAxisName(int j, yarp::os::ConstString& name) {
        if (info) {
            return info->getAxisName(j+base, name);
        }
        return false;
    }

    virtual bool setTorqueMode()
    {
        if (torque)
            return torque->setTorqueMode();
        return false;
    }

    virtual bool getRefTorques(double *t)
    {
        if (torque)
            return torque->getRefTorques(t);
        return false;
    }

    virtual bool getRefTorque(int j, double *t)
    {
        if (torque)
            return torque->getRefTorque(j+base, t);
        return false;
    }

    virtual bool setRefTorques(const double *t)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->setRefTorque(j+base, t[j]);
        }
        return ret;
    }

    virtual bool setRefTorque(int j, double t)
    {
        if (torque)
            return torque->setRefTorque(j+base, t);
        return false;
    }

    virtual bool setTorquePid(int j, const Pid &tpid)
    {
        if (torque)
            return torque->setTorquePid(j+base, tpid);
        return false;
    }

    virtual bool getTorque(int j, double *t)
    {
        if (torque)
            return torque->getTorque(j+base, t);
        return false;
    }

    virtual bool getTorques(double *t)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->getTorque(j+base, t+j);
        }
        return ret;
    }

    virtual bool getTorqueRange(int j, double *min, double *max)
    {
        if (torque)
            return torque->getTorqueRange(j+base, min, max);
        return false;
    }

    virtual bool getTorqueRanges(double *min, double *max)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->getTorqueRange(j+base, min+j, max+j);
        }
        return ret;
    }

    virtual bool setTorquePids(const Pid *pids)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->setTorquePid(j+base, pids[j]);
        }
        return ret;
    }

    virtual bool setTorqueErrorLimit(int j, double limit)
    {
        if (torque)
            return torque->setTorqueErrorLimit(j+base, limit);
        return false;
    }


    virtual bool setTorqueErrorLimits(const double *limits)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->setTorqueErrorLimit(j+base, limits[j]);
        }
        return ret;
    }


    virtual bool getTorqueError(int j, double *err)
    {
        if (torque)
            return torque->getTorqueError(j+base, err);
        return false;
    }

    virtual bool getTorqueErrors(double *errs)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->getTorqueError(j+base, errs+j);
        }
        return ret;
    }

    virtual bool getTorquePidOutput(int j, double *out)
    {
        if (torque)
            return torque->getTorquePidOutput(j+base, out);
        return false;
    }

    virtual bool getTorquePidOutputs(double *outs)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->getTorquePidOutput(j+base, outs+j);
        }
        return ret;
    }

    virtual bool getTorquePid(int j, Pid *tpid)
    {
        if (torque)
            return torque->getTorquePid(j+base, tpid);
        return false;
    }

    virtual bool getTorquePids(Pid *pids)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->getTorquePid(j+base, pids+j);
        }
        return ret;
    }

    virtual bool getTorqueErrorLimit(int j, double *limit)
    {
        if (torque)
            return torque->getTorqueErrorLimit(j+base, limit);
        return false;
    }

    virtual bool getTorqueErrorLimits(double *limits)
    {
        bool ret=false;
        if (torque) {
            ret=true;
            for(int j=0;j<controlledJoints;j++)
                ret=ret&&torque->getTorqueErrorLimit(j+base, limits+j);
        }
        return ret;
    }

    virtual bool resetTorquePid(int j)
    {
        if (torque)
            return torque->resetTorquePid(j+base);
        return false;
    }

    virtual bool disableTorquePid(int j)
    {
        if (torque)
            return torque->disableTorquePid(base+j);
        return false;
    }

    virtual bool enableTorquePid(int j)
    {
        if (torque)
            return torque->enableTorquePid(j+base);
        return false;
    }

    virtual bool setTorqueOffset(int j, double v)
    {
        if (torque)
            return torque->setTorqueOffset(j+base, v);
        return false;
    }

    virtual bool setPositionMode(int j)
    {
        if (iControlMode)
            return iControlMode->setPositionMode(j+base);
        return false;
    }

    virtual bool setTorqueMode(int j)
    {
        if (iControlMode)
            return iControlMode->setTorqueMode(j+base);
        return false;
    }

    virtual bool setImpedancePositionMode(int j)
    {
        if (iControlMode)
            return iControlMode->setImpedancePositionMode(j+base);
        return false;
    }

    virtual bool setImpedanceVelocityMode(int j)
    {
        if (iControlMode)
            return iControlMode->setImpedanceVelocityMode(j+base);
        return false;
    }
    virtual bool setOpenLoopMode(int j)
    {
        if (iControlMode)
            return iControlMode->setOpenLoopMode(j+base);
        return false;
    }

    virtual bool setVelocityMode(int j)
    {
        if (iControlMode)
            return iControlMode->setVelocityMode(j+base);
        return false;
    }

    virtual bool getControlMode(int j, int *mode)
    {
        if (iControlMode)
            return iControlMode->getControlMode(j+base, mode);
        return false;
    }

    virtual bool getControlModes(int *modes)
    {
        if (iControlMode)
            return iControlMode->getControlModes(modes);
        return false;
    }

    virtual bool setOutput(int j, double v)
    {
        if (iOpenLoop)
           return iOpenLoop->setOutput(j+base, v);
        return false;
    }

    virtual bool setOutputs(const double *v) {
        bool ret=false;
        if (iOpenLoop) {
            ret=true;
            for(int l=0;l<controlledJoints;l++)
                ret=ret&&iOpenLoop->setOutput(l+base,v[l]);
        }

        return ret;
    }
};

/* check whether the last command failed */
inline bool CHECK_FAIL(bool ok, Bottle& response) {
    if (ok) {
        if (response.get(0).isVocab() && response.get(0).asVocab() == VOCAB_FAILED) {
            return false;
        }
    }
    else
        return false;

    return true;
}

#endif
