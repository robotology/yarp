// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/String.h>
#include <yarp/NetType.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>

#include <yarp/sig/Vector.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace yarp{
    namespace dev {
        class RemoteControlBoard;
        class ServerControlBoard;
        class CommandsHelper;
        class ImplementCallbackHelper;
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* the control command message type 
 * head is a Bottle which contains the specification of the message type
 * body is a Vector which move the robot accordingly
 */
typedef PortablePair<Bottle, Vector> CommandMessage;

/**
 * Helper object for reading config commands for the ServerControlBoard
 * class.
 */
class yarp::dev::CommandsHelper : public DeviceResponder {
protected:
    yarp::dev::ServerControlBoard   *caller;
    yarp::dev::IPidControl          *pid;
    yarp::dev::IPositionControl     *pos;
    yarp::dev::IVelocityControl     *vel;
    yarp::dev::IEncoders            *enc;
    yarp::dev::IAmplifierControl    *amp;
    yarp::dev::IControlLimits       *lim;
    yarp::dev::IAxisInfo            *info;
    yarp::dev::IControlCalibration2   *ical2;
    int nj;
    Vector vect;

public:
    /**
     * Constructor.
     * @param x is the pointer to the instance of the object that uses the CommandsHelper.
     * This is required to recover the pointers to the interfaces that implement the responses
     * to the commands.
     */
    CommandsHelper(yarp::dev::ServerControlBoard *x);

    virtual bool respond(const Bottle& cmd, Bottle& response);

    /**
     * Initialize the internal data.
     * @return true/false on success/failure
     */
    virtual bool initialize();
};


/**
 * Callback implementation after buffered input.
 */
class yarp::dev::ImplementCallbackHelper : public TypedReaderCallback<CommandMessage> {
protected:
    IPositionControl *pos;
    IVelocityControl *vel;

public:
    /**
     * Constructor.
     * @param x is the instance of the container class using the callback.
     */
    ImplementCallbackHelper(yarp::dev::ServerControlBoard *x);

    /**
     * Callback function.
     * @param v is the Vector being received.
     */
    virtual void onRead(CommandMessage& v);
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * @ingroup dev_impl_wrapper
 *
 * Implement the server side of a remote
 * control board device driver. The device contains three ports:
 * - rpc_p handling the configuration interfaces of the robot
 * - state_p streaming information about the current state of the robot
 * - control_p receiving a stream of control commands (e.g. position)
 * 
 * Missing: 
 *          torque control, NOT IMPLEMENTED
 *
 */
class yarp::dev::ServerControlBoard : 
    public DeviceDriver, 
            public Thread,
            public IPidControl,
            public IPositionControl,
            public IVelocityControl,
            public IEncoders,
            public IAmplifierControl,
            public IControlLimits,
            public IControlCalibration,
            public IControlCalibration2,
            public IAxisInfo
 // convenient to put these here just to make sure all
 // methods get implemented
{
private:
	bool spoke;
	bool verb;

    Port rpc_p;     // RPC to configure the robot
    Port state_p;   // out port to read the state
    Port control_p; // in port to command the robot

    PortWriterBuffer<yarp::sig::Vector> state_buffer;
    PortReaderBuffer<CommandMessage> control_buffer;

    yarp::dev::ImplementCallbackHelper callback_impl;

    yarp::dev::CommandsHelper command_reader;

    // for new interface
    PortReaderBuffer<Bottle> command_buffer;

    PolyDriver poly;
    PolyDriver polyCalib;

    int               nj;
    int               thread_period;
    IPidControl       *pid;
    IPositionControl  *pos;
    IVelocityControl  *vel;
    IEncoders         *enc;
    IAmplifierControl *amp;
    IControlLimits    *lim;
    IControlCalibration *calib;
    IControlCalibration2 *calib2;
    IAxisInfo          *info;
    // LATER: other interfaces here.

    bool closeMain() {
        if (Thread::isRunning()) {
            Thread::stop();
        }

        // close the port connections here!
        rpc_p.close();
        control_p.close();
        state_p.close();


        poly.close();
        polyCalib.close();

        return true;
    }
    

public:
    /**
     * Constructor.
     */
    ServerControlBoard() : callback_impl(this), command_reader(this)
    {
        pid = NULL;
        pos = NULL;
        vel = NULL;
        enc = NULL;
        amp = NULL;
        lim = NULL;
		calib = NULL;
        calib2 = NULL;
        info = NULL;
        nj = 0;
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

        bool newInterface = prop.check("single_port","if present, use new simplified port interface");

		verb = (prop.check("verbose","if present, give detailed output"));
		if (verb)
			ACE_OS::printf("running with verbose output\n");

        thread_period = prop.check("rate", 20, "thread rate in ms. for streaming encoder data").asInt();

        Value *name;
        if (prop.check("subdevice",name,"name of specific control device to wrap")) {
            ACE_OS::printf("Subdevice %s\n", name->toString().c_str());
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
                ACE_OS::printf("cannot make <%s>\n", name->toString().c_str());
            }
        } else {
            ACE_OS::printf("\"--subdevice <name>\" not set for server_controlboard\n");
            return false;
        }

        if (!poly.isValid()) {
            return false;
        }

        String rootName = 
            prop.check("name",Value("/controlboard"),
                       "prefix for port names").asString().c_str();

       if (newInterface) {
            command_buffer.attach(rpc_p);
            command_reader.attach(command_buffer);
            state_buffer.attach(rpc_p);
            rpc_p.open(rootName.c_str());
        } else {
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
        }
            
        if (prop.check("calibrator", name,"calibration device to use, if any"))
            {
                Property p;
                p.fromString(prop.toString());
                p.put("device",name->toString());
                polyCalib.open(p);
            }

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
        }

        // set calibrator //
        if (polyCalib.isValid())
            {
                ICalibrator *icalibrator;
                polyCalib.view(icalibrator);
                calib->setCalibrator(icalibrator);
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
                    ACE_OS::printf ("problems: controlling 0 axes\n");
                    return false;
                }
            }
            if (vel!=NULL) {
                if (!vel->getAxes(&nj)) {
                    ACE_OS::printf ("problems: controlling 0 axes\n");
                    return false;
                }
            }

            // initialization.
            command_reader.initialize();

            start();

            return true;
        }
        
        ACE_OS::printf("subdevice <%s> doesn't look like a control board (no appropriate interfaces were acquired)\n",
                       name->toString().c_str());
        
        return false;
    }

    /**
     * The thread main loop deals with writing on ports here.
     */
    virtual void run() {
        ACE_OS::printf("Server control board starting\n");
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
            state_buffer.write();
            now = Time::now();
            if ((now-before)*1000 < thread_period) {
                const double k = double(thread_period)/1000.0-(now-before);
			    Time::delay(k);
            }
            else {
                ACE_OS::printf("Can't comply with the %d ms period\n", thread_period);
            }
        }
        ACE_OS::printf("Server control board stopping\n");
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
        ACE_OS::memset (errs, 0, sizeof(double)*nj);
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
        ACE_OS::memset(outs, 0, sizeof(double)*nj);
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
        ACE_OS::memset(p, 0, sizeof(Pid));
        return false;
    }

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getPids(Pid *pids) {
        if (pid)
            return pid->getPids(pids);
        ACE_OS::memset(pids, 0, sizeof(Pid)*nj);
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
        ACE_OS::memset(refs, 0, sizeof(double)*nj);
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
        ACE_OS::memset(limits, 0, sizeof(double)*nj);
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
        ACE_OS::memset(spds, 0, sizeof(double)*nj);
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
        ACE_OS::memset(accs, 0, sizeof(double)*nj);
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
        ACE_OS::memset(encs, 0, sizeof(double)*nj);
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
        ACE_OS::memset(spds, 0, sizeof(double)*nj);
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
        ACE_OS::memset(accs, 0, sizeof(double)*nj);
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
        ACE_OS::memset(vals, 0, sizeof(double)*nj);
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
			return calib->calibrate(j, p);
		return false;
	}

    virtual bool calibrate()
    {
        if (calib)
            return calib->calibrate();
        return false;
    }

    virtual bool park(bool wait=true)
    {
        if (calib)
            return calib->park(wait);
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


/**
 * @ingroup dev_impl_wrapper
 *
 * The client side of the control board, connects to a ServerControlBoard.
 */
class yarp::dev::RemoteControlBoard : 
    public IPidControl,
            public IPositionControl, 
            public IVelocityControl,
            public IEncoders,
            public IAmplifierControl,
            public IControlLimits,
            public IAxisInfo,
            public IControlCalibration2,
			public DeviceDriver {
protected:
    Port rpc_p;
    Port command_p;
    Port state_p;

    PortReaderBuffer<yarp::sig::Vector> state_buffer;
    PortWriterBuffer<CommandMessage> command_buffer;

    String remote;
    String local;

    int nj;

    /** 
     * Send a SET command without parameters and wait for a reply.
     * @param code is the command Vocab identifier.
     * @return true/false on success/failure.
     */
    bool setCommand(int code) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /** 
     * Send a SET command and an additional double valued variable 
     * and then wait for a reply.
     * @param code is the command to send.
     * @param v is a double valued parameter.
     * @return true/false on success/failure.
     */
    bool setCommand(int code, double v) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(v);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Send a SET command with an additional integer valued variable
     * and then wait for a reply.
     * @param code is the command to send.
     * @param v is an integer valued parameter.
     * @return true/false on success/failure.
     */
    bool setCommand(int code, int v) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt(v);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Send a GET command expecting a double value in return.
     * @param code is the Vocab code of the GET command.
     * @param v is a reference to the return variable.
     * @return true/false on success/failure.
     */
    bool getCommand(int code, double& v) const {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // response should be [cmd] [name] value
            v = response.get(2).asDouble();
            return true;
        }
        return false;
    }

    /**
     * Send a GET command expecting an integer value in return.
     * @param code is the Vocab code of the GET command.
     * @param v is a reference to the return variable.
     * @return true/false on success/failure.
     */
    bool getCommand(int code, int& v) const {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // response should be [cmd] [name] value
            v = response.get(2).asInt();
            return true;
        }
        return false;
    }

    /**
     * Helper method to set a double value to a single axis.
     * @param code is the name of the command to be transmitted
     * @param j is the axis
     * @param val is the double value
     * @return true/false on success/failure
     */
    bool setDouble (int code, int j, double val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt(j);
        cmd.addDouble(val);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /** 
     * Helper method used to set an array of double to all axes.
     * @param v is the command to set
     * @param val is the double array (of length nj)
     * @return true/false on success/failure
     */
    bool setDoubleArray(int v, const double *val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++)
            l.addDouble(val[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Helper method used to get a double value from the remote peer.
     * @param v is the command to query for
     * @param j is the axis number
     * @param val is the return value
     * @return true/false on success/failure
     */
    bool getDouble(int v, int j, double *val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // ok
            *val = response.get(2).asDouble();
            return true;
        }
        return false;
    }

    /**
     * Helper method to get an array of double from the remote peer.
     * @param v is the name of the command
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool getDoubleArray(int v, double *val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;

            int njs = l.size();
            ACE_ASSERT (nj == njs);
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asDouble();
            return true;
        }

        return false;
    }

public:
    /**
     * Constructor.
     */
    RemoteControlBoard() { 
        nj = 0;
    }

    /**
     * Destructor.
     */
    virtual ~RemoteControlBoard() {
    }

    /**
     * Default open.
     * @return always true.
     */
    virtual bool open() {
        return true;
    }

    virtual bool open(Searchable& config) {
        remote = config.find("remote").asString().c_str();
        local = config.find("local").asString().c_str();
        ConstString carrier = 
            config.check("carrier",
                         Value("tcp"),
                         "default carrier for streaming robot state").asString().c_str();
        if (local != "") {
            String s1 = local;
            s1 += "/rpc:o";
            rpc_p.open(s1.c_str());
            s1 = local;
            s1 += "/command:o";
            command_p.open(s1.c_str());
            s1 = local;
            s1 += "/state:i";
            state_p.open(s1.c_str());
        }

        if (remote != "") {
            String s1 = remote;
            s1 += "/rpc:i";
            String s2 = local;
            s2 += "/rpc:o";
            Network::connect(s2.c_str(), s1.c_str());
            s1 = remote;
            s1 += "/command:i";
            s2 = local;
            s2 += "/command:o";
            Network::connect(s2.c_str(), s1.c_str());
            s1 = remote;
            s1 += "/state:o";
            s2 = local;
            s2 += "/state:i";
            Network::connect(s1.c_str(), s2.c_str(), carrier);
        }
        
        state_buffer.attach(state_p);
		state_buffer.setStrict(false);
        command_buffer.attach(command_p);

        bool ok = getCommand(VOCAB_AXES, nj);
		fprintf(stderr, "AXES:%d\n", nj);
        if (nj==0) {
            ok = false;
        }
        if (!ok) {
            ACE_OS::printf("Problems with obtaining the number of controlled axes\n");
            return false;
        }

        return true;
    }
	
    /**
     * Close the device driver and stop the port connections.
     * @return true/false on success/failure.
     */
    virtual bool close() {
        rpc_p.close();
        command_p.close();
        state_p.close();
        return true;
    }

    /** 
     * Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setPid(int j, const Pid &pid) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addInt(j);
        Bottle& l = cmd.addList();
        l.addDouble(pid.kp);
        l.addDouble(pid.kd);
        l.addDouble(pid.ki);
        l.addDouble(pid.max_int);
        l.addDouble(pid.max_output);
        l.addDouble(pid.offset);
        l.addDouble(pid.scale);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /** 
     * Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setPids(const Pid *pids) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PIDS);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++) {
            Bottle& m = l.addList();
            m.addDouble(pids[i].kp);
            m.addDouble(pids[i].kd);
            m.addDouble(pids[i].ki);
            m.addDouble(pids[i].max_int);
            m.addDouble(pids[i].max_output);
            m.addDouble(pids[i].offset);
            m.addDouble(pids[i].scale);
        }
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /** 
     * Set the controller reference point for a given axis.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param j joint number
     * @param ref new reference point
     * @return true/false upon success/failure
     */
    virtual bool setReference(int j, double ref) {
        return setDouble(VOCAB_REF, j, ref);
    }

    /** 
     * Set the controller reference points, multiple axes.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param refs pointer to the vector that contains the new reference points.
     * @return true/false upon success/failure
     */
    virtual bool setReferences(const double *refs) {
        return setDoubleArray(VOCAB_REFS, refs);
    }

    /** 
     * Set the error limit for the controller on a specifi joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setErrorLimit(int j, double limit) {
        return setDouble(VOCAB_LIM, j, limit);
    }

    /** 
     * Get the error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setErrorLimits(const double *limits) {
        return setDoubleArray(VOCAB_LIMS, limits);
    }

    /** 
     * Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getError(int j, double *err) {
        return getDouble(VOCAB_ERR, j, err);
    }

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getErrors(double *errs) {
        return getDoubleArray(VOCAB_ERRS, errs);
    }

    /** 
     * Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutput(int j, double *out) {
        return getDouble(VOCAB_OUTPUT, j, out);
    }

    /** 
     * Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getOutputs(double *outs) {
        return getDoubleArray(VOCAB_OUTPUTS, outs);
    }

    /** 
     * Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getPid(int j, Pid *pid) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PID);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            pid->kp = l.get(0).asDouble();
            pid->kd = l.get(1).asDouble();
            pid->ki = l.get(2).asDouble();
            pid->max_int = l.get(3).asDouble();
            pid->max_output = l.get(4).asDouble();
            pid->offset = l.get(5).asDouble();
            pid->scale = l.get(6).asDouble();
            return true;
        }
        return false;
    }

    /** 
     * Get current pid value for all controlled axes.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getPids(Pid *pids) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PIDS);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            const int njs = l.size();
            ACE_ASSERT (njs == nj);
            for (i = 0; i < nj; i++)
                {
                    Bottle& m = *(l.get(i).asList());
                    if (&m == 0)
                        return false;
                    pids->kp = m.get(0).asDouble();
                    pids->kd = m.get(1).asDouble();
                    pids->ki = m.get(2).asDouble();
                    pids->max_int = m.get(3).asDouble();
                    pids->max_output = m.get(4).asDouble();
                    pids->offset = m.get(5).asDouble();
                    pids->scale = m.get(6).asDouble();
                }
            return true;
        }
        return false;
    }

    /** 
     * Get the current reference position of the controller for a specific joint.
     * @param j is joint number
     * @param ref pointer to storage for return value
     * @return true/false on success/failure
     */
    virtual bool getReference(int j, double *ref) {
        return getDouble(VOCAB_REF, j, ref);
    }

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     */
    virtual bool getReferences(double *refs) {
        return getDoubleArray(VOCAB_REFS, refs);
    }

    /** 
     * Get the error limit for the controller on a specific joint
     * @param j is the joint number
     * @param limit pointer to storage
     * @return true/false on success/failure
     */
    virtual bool getErrorLimit(int j, double *limit) {
        return getDouble(VOCAB_LIM, j, limit);
    }

    /** 
     * Get the error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getErrorLimits(double *limits) {
        return getDoubleArray(VOCAB_LIMS, limits);
    }

    /** 
     * Reset the controller of a given joint, usually sets the 
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     */
    virtual bool resetPid(int j) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_RESET);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /** 
     * Disable the pid computation for a joint
     * @param j is the joint number
     * @return true/false on success/failure
     */
    virtual bool disablePid(int j) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_DISABLE);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /** 
     * Enable the pid computation for a joint
     * @param j is the joint number
     * @return true/false on success/failure
     */
    virtual bool enablePid(int j) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_ENABLE);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /* IEncoder */

    /**
     * Reset encoder, single joint. Set the encoder value to zero 
     * @param j is the axis number
     * @return true/false on success/failure
     */
    virtual bool resetEncoder(int j) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_E_RESET);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Reset encoders. Set the encoders value to zero 
     * @return true/false
     */
    virtual bool resetEncoders() {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_E_RESETS);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Set the value of the encoder for a given joint. 
     * @param j encoder number
     * @param val new value
     * @return true/false on success/failure
     */
    virtual bool setEncoder(int j, double val) {
        return setDouble(VOCAB_ENCODER, j, val);
    }

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setEncoders(const double *vals) {
        return setDoubleArray(VOCAB_ENCODERS, vals);
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getEncoder(int j, double *v) {
        return getDouble(VOCAB_ENCODER, j, v);
    }

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    virtual bool getEncoders(double *encs) {
        Vector *v = state_buffer.read(true);
        if (v != NULL) {
            ACE_ASSERT (v->size() == nj);
            ACE_OS::memcpy (encs, &(v->operator [](0)), sizeof(double)*nj);
            return true;
        }

        return false;
        //return getDoubleArray(VOCAB_ENCODERS, encs);
    }

    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getEncoderSpeed(int j, double *sp) {
        return getDouble(VOCAB_ENCODER_SPEED, j, sp);
    }

    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getEncoderSpeeds(double *spds) {
        return getDoubleArray(VOCAB_ENCODER_SPEEDS, spds);
    }
    
    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param acc pointer to the array that will contain the output
     */
    virtual bool getEncoderAcceleration(int j, double *acc) {
        return getDouble(VOCAB_ENCODER_ACCELERATION, j, acc);
    }

    /**
     * Read the istantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens. 
     */
    virtual bool getEncoderAccelerations(double *accs) {
        return getDoubleArray(VOCAB_ENCODER_ACCELERATIONS, accs);
    }

    /* IPositionControl */

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param ax pointer to storage
     * @return true/false.
     */
    virtual bool getAxes(int *ax) {
        return getCommand(VOCAB_AXES, *ax);
    }

    /** 
     * Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * return true/false on success/failure
     */
    virtual bool setPositionMode() {
        return setCommand(VOCAB_POSITION_MODE);
    }

    /** 
     * Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool positionMove(int j, double ref) { 
        return setDouble(VOCAB_POSITION_MOVE, j, ref);
    }

    /** 
     * Set new reference point for all axes.
     * @param refs array, new reference points
     * @return true/false on success/failure
     */
    virtual bool positionMove(const double *refs) { 
        CommandMessage& c = command_buffer.get();
		c.head.clear();
        c.head.addVocab(VOCAB_POSITION_MOVES);
        c.body.size(nj);
        ACE_OS::memcpy(&(c.body[0]), refs, sizeof(double)*nj);
        command_buffer.write();
        return true;
    }

    /** 
     * Set relative position. The command is relative to the 
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    virtual bool relativeMove(int j, double delta) { 
        return setDouble(VOCAB_RELATIVE_MOVE, j, delta); 
    }

    /** 
     * Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const double *deltas) { 
        return setDoubleArray(VOCAB_RELATIVE_MOVES, deltas); 
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(int j, bool *flag) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_MOTION_DONE);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            *flag = (bool)(response.get(2).asInt());
            return true;
        }
        return false;
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(bool *flag) { 
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_MOTION_DONES);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            *flag = (bool)(response.get(2).asInt());
			return true;
        }
        return false;
    }

    /** 
     * Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeed(int j, double sp) { 
        return setDouble(VOCAB_REF_SPEED, j, sp);
    }

    /** 
     * Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const double *spds) { 
        return setDoubleArray(VOCAB_REF_SPEEDS, spds);
    }

    /** 
     * Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual bool setRefAcceleration(int j, double acc) {
        return setDouble(VOCAB_REF_ACCELERATION, j, acc);
    }

    /** 
     * Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const double *accs) { 
        return setDoubleArray(VOCAB_REF_ACCELERATIONS, accs);
    }

    /** 
     * Get reference speed for a joint. Returns the speed used to 
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    virtual bool getRefSpeed(int j, double *ref) {
        return getDouble(VOCAB_REF_SPEED, j, ref);
    }

    /** 
     * Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    virtual bool getRefSpeeds(double *spds) {
        return getDoubleArray(VOCAB_REF_SPEEDS, spds);
    }

    /** 
     * Get reference acceleration for a joint. Returns the acceleration used to 
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    virtual bool getRefAcceleration(int j, double *acc) {
        return getDouble(VOCAB_REF_ACCELERATION, j, acc);
    }

    /** 
     * Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure 
     */
    virtual bool getRefAccelerations(double *accs) { 
        return getDoubleArray(VOCAB_REF_ACCELERATIONS, accs);
    }

    /** 
     * Stop motion, single joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool stop(int j) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_STOP);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /** 
     * Stop motion, multiple joints 
     * @return true/false on success/failure
     */
    virtual bool stop() { 
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_STOPS);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /* IVelocityControl */

    /** 
     * Set new reference speed for a single axis.
     * @param j joint number
     * @param v specifies the new ref speed
     * @return true/false on success/failure
     */
    virtual bool velocityMove(int j, double v) {
        return setDouble(VOCAB_VELOCITY_MOVE, j, v);
    }

    /**
     * Set a new reference speed for all axes.
     * @param v is a vector of double representing the requested speed.
     * @return true/false on success/failure.
     */
    virtual bool velocityMove(const double *v) {
        CommandMessage& c = command_buffer.get();
		c.head.clear();
        c.head.addVocab(VOCAB_VELOCITY_MOVES);
        c.body.size(nj);
        ACE_OS::memcpy(&(c.body[0]), v, sizeof(double)*nj);
        command_buffer.write();
        return true;
    }

    /**
     * Set the controller to velocity mode.
     * @return true/false on success/failure.
     */
    virtual bool setVelocityMode() {
        return setCommand(VOCAB_VELOCITY_MODE);
    }

    /* IAmplifierControl */

    /** 
     * Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid 
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmp(int j) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_AMP_ENABLE);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

	virtual bool setOffset(int j, double v)
	{
		Bottle cmd, response;
		cmd.addVocab(VOCAB_SET);
		cmd.addVocab(VOCAB_OFFSET);
		cmd.addInt(j);
		cmd.addDouble(v);

		bool ok = rpc_p.write(cmd, response);
		return CHECK_FAIL(ok, response);
	}

    /** 
     * Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmp(int j) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_AMP_DISABLE);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrents(double *vals) {
        return getDoubleArray(VOCAB_AMP_CURRENTS, vals);
    }

    /** 
     * Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad time
     */
    virtual bool getCurrent(int j, double *val) {
        return getDouble(VOCAB_AMP_CURRENT, j, val);
    }

    /**
     * Set the maximum electric current going to a given motor. The behavior 
     * of the board/amplifier when this limit is reached depends on the
     * implementation.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad time
     */
    virtual bool setMaxCurrent(int j, double v) {
        return setDouble(VOCAB_AMP_MAXCURRENT, j, v);
    }

    /**
     * Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be 
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    virtual bool getAmpStatus(int *st) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_AMP_STATUS);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            *st = response.get(2).asInt();
            return true;
        }
        return false;        
    }

    /* IControlLimits */

    /**
     * Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimits(int axis, double min, double max) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_LIMITS);
        cmd.addInt(axis);
        cmd.addDouble(min);
        cmd.addDouble(max);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }
    
    /**
     * Get the software limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false if something bad happens
     */
    virtual bool getLimits(int axis, double *min, double *max) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_LIMITS);
        cmd.addInt(axis);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            *min = response.get(2).asDouble();
            *max = response.get(3).asDouble();
            return true;
        }
        return false;
    }



    /* IAxisInfo */
    
    virtual bool getAxisName(int j, yarp::os::ConstString& name) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_INFO_NAME);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            name = response.get(2).asString();
            return true;
        }
        return false;
    }

    /* IControlCalibration */
    bool virtual calibrate()
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_CALIBRATE);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool virtual park(bool wait=true)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_PARK);
        cmd.addInt(wait);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool virtual calibrate2(int j, unsigned int ui, double v1, double v2, double v3)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_CALIBRATE_JOINT);
        cmd.addInt(j);
        cmd.addInt(ui);
        cmd.addDouble(v1);
        cmd.addDouble(v2);
        cmd.addDouble(v3);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool virtual done(int j)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_CALIBRATE_DONE);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

};



// implementation of CommandsHelper

#ifndef DOXYGEN_SHOULD_SKIP_THIS

yarp::dev::CommandsHelper::CommandsHelper(yarp::dev::ServerControlBoard *x) { 
    ACE_ASSERT (x != NULL);
    caller = x; 
    pid = dynamic_cast<yarp::dev::IPidControl *> (caller);
    pos = dynamic_cast<yarp::dev::IPositionControl *> (caller);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (caller);
    enc = dynamic_cast<yarp::dev::IEncoders *> (caller);
    amp = dynamic_cast<yarp::dev::IAmplifierControl *> (caller);
    lim = dynamic_cast<yarp::dev::IControlLimits *> (caller);
    info = dynamic_cast<yarp::dev::IAxisInfo *> (caller);
    ical2= dynamic_cast<yarp::dev::IControlCalibration2 *> (caller);
    nj = 0;
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
    addUsage("[get] [enc] $iAxisNumber", "get the encoder value for an axis");


    String args;
    for (int i=0; i<nj; i++) {
        if (i>0) {
            args += " ";
        }
        args = args + "$f" + NetType::toString(i);
    }
    addUsage((String("[set] [poss] (")+args+")").c_str(), 
             "command the position of all axes");
    addUsage((String("[set] [rels] (")+args+")").c_str(), 
             "command the relative position of all axes");
    addUsage((String("[set] [vmos] (")+args+")").c_str(), 
             "command the velocity of all axes");

    addUsage("[set] [aen] $iAxisNumber", "enable (amplifier for) the given axis");
    addUsage("[set] [adi] $iAxisNumber", "disable (amplifier for) the given axis");
    addUsage("[get] [acu] $iAxisNumber", "get current for the given axis");
    addUsage("[get] [acus]", "get current for all axes");

    return ok;
}

bool yarp::dev::CommandsHelper::respond(const yarp::os::Bottle& cmd, 
                                        yarp::os::Bottle& response) {
    bool ok = false;
    bool rec = false; // is the command recognized?
	if (caller->verbose())
	    ACE_OS::printf("command received: %s\n", cmd.toString().c_str());
    int code = cmd.get(0).asVocab();
    switch (code) {
    case VOCAB_CALIBRATE_JOINT:
        {
            rec=true;
            if (caller->verbose())
                ACE_OS::printf("Calling calibrate joint\n");

            int j=cmd.get(1).asInt();
            int ui=cmd.get(2).asInt();
            double v1=cmd.get(3).asDouble();
            double v2=cmd.get(4).asDouble();
            double v3=cmd.get(5).asDouble();
            if (ical2==0)
                ACE_OS::printf("Sorry I don't have a IControlCalibration2 interface\n");
            else
                ok=ical2->calibrate2(j,ui,v1,v2,v3);
        }
        break;
    case VOCAB_CALIBRATE:
        {
            rec=true;
            if (caller->verbose())
                ACE_OS::printf("Calling calibrate\n");
            ok=ical2->calibrate();
        }
        break;
    case VOCAB_CALIBRATE_DONE:
        {
            rec=true;
            if (caller->verbose())
                ACE_OS::printf("Calling calibrate done\n");
            int j=cmd.get(1).asInt();
            ok=ical2->done(j);
        }
        break;
    case VOCAB_PARK:
        {
            rec=true;
            if (caller->verbose())
                ACE_OS::printf("Calling park function\n");
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
			ACE_OS::printf("set command received\n");

        {
            switch(cmd.get(1).asVocab()) {
			case VOCAB_OFFSET:{
			    double v;
                int j = cmd.get(2).asInt();
                Bottle& b = *(cmd.get(3).asList());
                v=b.get(0).asDouble();
                ok = pid->setOffset(j, v);
			}
                break;
            case VOCAB_PID: {
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
                ok = pid->setPid(j, p);
            }
                break;

            case VOCAB_PIDS: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                Pid *p = new Pid[njs];
                ACE_ASSERT (p != NULL);
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
                    }
                ok = pid->setPids(p);
                delete[] p;
            }
                break;

            case VOCAB_REF: {
                ok = pid->setReference (cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_REFS: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation. 
                ACE_ASSERT (p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pid->setReferences (p);
                delete[] p;
            }
                break;

            case VOCAB_LIM: {
                ok = pid->setErrorLimit (cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_LIMS: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation. 
                ACE_ASSERT (p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pid->setErrorLimits (p);
                delete[] p;                
            }
                break;

            case VOCAB_RESET: {
                ok = pid->resetPid (cmd.get(2).asInt());
            }
                break;

            case VOCAB_DISABLE: {
                ok = pid->disablePid (cmd.get(2).asInt());
            }
                break;

            case VOCAB_ENABLE: {
                ok = pid->enablePid (cmd.get(2).asInt());
            }
                break;

            case VOCAB_VELOCITY_MODE: {
                ok = vel->setVelocityMode();
            }
                break;

            case VOCAB_VELOCITY_MOVE: {
                ok = vel->velocityMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_POSITION_MODE: {
                ok = pos->setPositionMode();
            }
                break;

            case VOCAB_POSITION_MOVE: {
                ok = pos->positionMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

                // this operation is also available on "command" port
            case VOCAB_POSITION_MOVES: {
                Bottle *b = cmd.get(2).asList();
                int i;
                if (b==NULL) break;
                const int njs = b->size();
                ACE_ASSERT (njs == nj);
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
            case VOCAB_VELOCITY_MOVES: {

                Bottle *b = cmd.get(2).asList();
                int i;
                if (b==NULL) break;
                const int njs = b->size();
                ACE_ASSERT (njs == nj);
                vect.size(nj);
                for (i = 0; i < njs; i++)
                    vect[i] = b->get(i).asDouble();
                if (vel!=NULL) {
                    ok = vel->velocityMove(&vect[0]);
                }
            }
                break;

            case VOCAB_RELATIVE_MOVE: {
                ok = pos->relativeMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_RELATIVE_MOVES: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation. 
                ACE_ASSERT (p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pos->relativeMove(p);
                delete[] p;                
            }
                break;

            case VOCAB_REF_SPEED: {
                ok = pos->setRefSpeed(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_REF_SPEEDS: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation. 
                ACE_ASSERT (p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pos->setRefSpeeds(p);
                delete[] p;                
            }
                break;

            case VOCAB_REF_ACCELERATION: {
                ok = pos->setRefAcceleration(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_REF_ACCELERATIONS: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation. 
                ACE_ASSERT (p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pos->setRefAccelerations(p);
                delete[] p;                
            }
                break;

            case VOCAB_STOP: {
                ok = pos->stop(cmd.get(2).asInt());
            }
                break;

            case VOCAB_STOPS: {
                ok = pos->stop();
            }
                break;

            case VOCAB_E_RESET: {
                ok = enc->resetEncoder(cmd.get(2).asInt());
            }
                break;

            case VOCAB_E_RESETS: {
                ok = enc->resetEncoders();
            }
                break;

            case VOCAB_ENCODER: {
                ok = enc->setEncoder(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_ENCODERS: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation. 
                ACE_ASSERT (p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = enc->setEncoders(p);
                delete[] p;                
            }
                break;

            case VOCAB_AMP_ENABLE: {
                ok = amp->enableAmp(cmd.get(2).asInt());
            }
                break;

            case VOCAB_AMP_DISABLE: {
                ok = amp->disableAmp(cmd.get(2).asInt());
            }
                break;

            case VOCAB_AMP_MAXCURRENT: {
                ok = amp->setMaxCurrent(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
                break;

            case VOCAB_LIMITS: {
                ok = lim->setLimits(cmd.get(2).asInt(), cmd.get(3).asDouble(), cmd.get(4).asDouble());
            }
                break; 

            default:
                ACE_OS::printf("received an unknown command after a VOCAB_SET\n");
                break;
            }
        }
        break;

    case VOCAB_GET:
        rec = true;
		if (caller->verbose())
			ACE_OS::printf("get command received\n");

        {
            int tmp = 0;
            double dtmp = 0.0;
            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));
            switch(cmd.get(1).asVocab()) {
            case VOCAB_ERR: {
                ok = pid->getError(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_ERRS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = pid->getErrors(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_OUTPUT: {
                ok = pid->getOutput(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_OUTPUTS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = pid->getOutputs(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_PID: {
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
            }
                break;

            case VOCAB_PIDS: {
                Pid *p = new Pid[nj];
                ACE_ASSERT (p != NULL);
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
                    }
                delete[] p;
            }
                break;

            case VOCAB_REFERENCE: {
                ok = pid->getReference(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_REFERENCES: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = pid->getReferences(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_LIM: {
                ok = pid->getErrorLimit(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_LIMS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
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

            case VOCAB_MOTION_DONE: {
                bool x = false;;
                ok = pos->checkMotionDone(cmd.get(2).asInt(), &x);
                response.addInt(x);
            }
                break;

            case VOCAB_MOTION_DONES: {
				bool x = false;
                ok = pos->checkMotionDone(&x);
				response.addInt(x);
            }
                break;

            case VOCAB_REF_SPEED: {
                ok = pos->getRefSpeed(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_REF_SPEEDS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = pos->getRefSpeeds(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_REF_ACCELERATION: {
                ok = pos->getRefAcceleration(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_REF_ACCELERATIONS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = pos->getRefAccelerations(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_ENCODER: {
                ok = enc->getEncoder(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_ENCODERS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = enc->getEncoders(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_ENCODER_SPEED: {
                ok = enc->getEncoderSpeed(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_ENCODER_SPEEDS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = enc->getEncoderSpeeds(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_ENCODER_ACCELERATION: {
                ok = enc->getEncoderAcceleration(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_ENCODER_ACCELERATIONS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = enc->getEncoderAccelerations(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_AMP_CURRENT: {
                ok = amp->getCurrent(cmd.get(2).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
                break;

            case VOCAB_AMP_CURRENTS: {
                double *p = new double[nj];
                ACE_ASSERT(p!=NULL);
                ok = amp->getCurrents(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
                break;

            case VOCAB_AMP_STATUS: {
                ok = amp->getAmpStatus(&tmp);
                response.addInt(tmp);
            }
                break;

            case VOCAB_LIMITS: {
                double min = 0.0, max = 0.0;
                ok = lim->getLimits(cmd.get(2).asInt(), &min, &max);
                response.addDouble(min);
                response.addDouble(max);
            }
                break;

            case VOCAB_INFO_NAME: {
                ConstString name = "undocumented";
                ok = info->getAxisName(cmd.get(2).asInt(),name);
                response.addString(name.c_str());
            }
                break;


            default:
                ACE_OS::printf("received an unknown request after a VOCAB_GET\n");
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

// ImplementCallbackHelper class.

yarp::dev::ImplementCallbackHelper::ImplementCallbackHelper(yarp::dev::ServerControlBoard *x) {
    pos = dynamic_cast<yarp::dev::IPositionControl *> (x);
    ACE_ASSERT (pos != 0);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (x);
    ACE_ASSERT (vel != 0);
}

void yarp::dev::ImplementCallbackHelper::onRead(CommandMessage& v) {
    //ACE_OS::printf("Data received on the control channel of size: %d\n", v.body.size());
    //	int i;

    Bottle& b = v.head;
    //ACE_OS::printf("bottle: %s\n", b.toString().c_str());

    switch (b.get(0).asVocab()) {
    case VOCAB_POSITION_MODE: 
    case VOCAB_POSITION_MOVES: {
        //            ACE_OS::printf("Received a position command\n");
        //			for (i = 0; i < v.body.size(); i++)
        //				ACE_OS::printf("%.2f ", v.body[i]);
        //			ACE_OS::printf("\n");

        if (pos) {
            bool ok = pos->positionMove(&(v.body[0]));
            if (!ok)
                ACE_OS::printf("Issues while trying to start a position move\n");
        }
    }
        break;

    case VOCAB_VELOCITY_MODE:
    case VOCAB_VELOCITY_MOVES: {
        //          ACE_OS::printf("Received a velocity command\n");
        //			for (i = 0; i < v.body.size(); i++)
        //				ACE_OS::printf("%.2f ", v.body[i]);
        //			ACE_OS::printf("\n");
        if (vel) {
            bool ok = vel->velocityMove(&(v.body[0]));
            if (!ok)
                ACE_OS::printf("Issues while trying to start a velocity move\n");
        }
    }
        break;

    default: {
        ACE_OS::printf("Unrecognized message while receiving on command port\n");
    }
        break;
    }

    //    ACE_OS::printf("v: ");
    //    int i;
    //    for (i = 0; i < (int)v.size(); i++)
    //        ACE_OS::printf("%.3f ", v[i]);
    //    ACE_OS::printf("\n");
}


// needed for the driver factory.
yarp::dev::DriverCreator *createServerControlBoard() {
    return new DriverCreatorOf<ServerControlBoard>("controlboard", 
                                                   "controlboard",
                                                   "ServerControlBoard");
}

yarp::dev::DriverCreator *createRemoteControlBoard() {
    return new DriverCreatorOf<RemoteControlBoard>("remote_controlboard", 
                                                   "controlboard",
                                                   "RemoteControlBoard");
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
