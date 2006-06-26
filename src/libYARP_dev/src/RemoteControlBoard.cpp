// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/String.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/Vector.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace yarp{
    namespace dev {
      class RemoteControlBoard;
      class ServerControlBoard;
      class CommandsHelper;
      class ControlHelper;
    }
}

#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')

// interface IPidControl sets.
#define VOCAB_PID VOCAB3('p','i','d')
#define VOCAB_PIDS VOCAB4('p','i','d','s')
#define VOCAB_REF VOCAB3('r','e','f')
#define VOCAB_REFS VOCAB4('r','e','f','s')
#define VOCAB_LIM VOCAB3('l','i','m')
#define VOCAB_LIMS VOCAB4('l','i','m','s')
#define VOCAB_RESET VOCAB3('r','e','s')
#define VOCAB_DISABLE VOCAB3('d','i','s')
#define VOCAB_ENABLE VOCAB3('e','n','a')

// interface IPidControl gets.
#define VOCAB_ERR VOCAB3('e','r','r')
#define VOCAB_ERRS VOCAB4('e','r','r','s')
#define VOCAB_OUTPUT VOCAB3('o','u','t')
#define VOCAB_OUTPUTS VOCAB4('o','u','t','s')
#define VOCAB_REFERENCE VOCAB3('r','e','f')
#define VOCAB_REFERENCES VOCAB4('r','e','f','s')

// interface IPositionControl gets
#define VOCAB_AXES VOCAB4('a','x','e','s')
#define VOCAB_MOTION_DONE VOCAB3('d','o','n')
#define VOCAB_MOTION_DONES VOCAB4('d','o','n','s')

// interface IPositionControl sets
#define VOCAB_POSITION_MODE VOCAB4('p','o','s','d')
#define VOCAB_POSITION_MOVE VOCAB3('p','o','s')
#define VOCAB_POSITION_MOVES VOCAB4('p','o','s','s')
#define VOCAB_RELATIVE_MOVE VOCAB3('r','e','l')
#define VOCAB_RELATIVE_MOVES VOCAB4('r','e','l','s')
#define VOCAB_REF_SPEED VOCAB3('v','e','l')
#define VOCAB_REF_SPEEDS VOCAB4('v','e','l','s')
#define VOCAB_REF_ACCELERATION VOCAB3('a','c','c')
#define VOCAB_REF_ACCELERATIONS VOCAB4('a','c','c','s')
#define VOCAB_STOP VOCAB3('s','t','o')
#define VOCAB_STOPS VOCAB4('s','t','o','s')

// interface IEncoders sets
#define VOCAB_E_RESET VOCAB3('e','r','e')
#define VOCAB_E_RESETS VOCAB4('e','r','e','s')
#define VOCAB_ENCODER VOCAB3('e','n','c')
#define VOCAB_ENCODERS VOCAB4('e','n','c','s')

// interface IEncoders gets
#define VOCAB_ENCODER_SPEED VOCAB3('e','s','p')
#define VOCAB_ENCODER_SPEEDS VOCAB4('e','s','p','s')
#define VOCAB_ENCODER_ACCELERATION VOCAB3('e','a','c')
#define VOCAB_ENCODER_ACCELERATIONS VOCAB4('e','a','c','s')

// interface IAmplifierControl sets/gets
#define VOCAB_AMP_ENABLE VOCAB3('a','e','n')
#define VOCAB_AMP_DISABLE VOCAB3('a','d','i')
#define VOCAB_AMP_CURRENT VOCAB3('a','c','u')
#define VOCAB_AMP_CURRENTS VOCAB4('a','c','u','s')
#define VOCAB_AMP_MAXCURRENT VOCAB4('m','a','x','c')
#define VOCAB_AMP_STATUS VOCAB4('a','s','t','a')

// interface IControlLimits sets/gets
#define VOCAB_LIMITS VOCAB4('l','l','i','m')

/**
 * Helper object for reading config commands for the ServerControlBoard
 * class.
 */
class yarp::dev::CommandsHelper : public PortReader {
protected:
    yarp::dev::ServerControlBoard   *caller;
    yarp::dev::IPidControl          *pid;
    yarp::dev::IPositionControl     *pos;
    yarp::dev::IEncoders            *enc;
    yarp::dev::IAmplifierControl    *amp;
    yarp::dev::IControlLimits       *lim;
    int nj;

public:
    /**
     * Constructor.
     * @param x is the pointer to the instance of the object that uses the CommandsHelper.
     * This is required to recover the pointers to the interfaces that implement the responses
     * to the commands.
     */
    CommandsHelper(yarp::dev::ServerControlBoard *x);

    /**
     * read from the connection.
     * @param connection is a reference to a ConnectionReader object which encapsulates
     * the current port connection.
     * @return true on a successful read.
     */
    virtual bool read(ConnectionReader& connection);
};

/**
 * Helper object for reading control commands for the ServerControlBoard
 * class.
 */
class yarp::dev::ControlHelper : public PortReader {
protected:
    yarp::dev::ServerControlBoard   *caller;
    yarp::dev::IPidControl          *pid;
    yarp::dev::IPositionControl     *pos;
    yarp::dev::IEncoders            *enc;
//            public IVelocityControl,
    yarp::dev::IAmplifierControl    *amp;
    yarp::dev::IControlLimits       *lim;
//            public IControlCalibration
//            public IControlDebug

public:
    /**
     * Constructor.
     * @param x is the pointer to the instance of the object that uses the ControlHelper.
     * This is required to recover the pointers to the interfaces that implement the responses
     * to the commands.
     */
    ControlHelper(yarp::dev::ServerControlBoard *x);

    /**
     * read from the connection.
     * @param connection is a reference to a ConnectionReader object which encapsulates
     * the current port connection.
     * @return true on a successful read.
     */
    virtual bool read(ConnectionReader& connection);
};

/**
 * ServerControlBoard class, implement the server side of a remote
 * control board device driver. The device contains three ports:
 * - rpc_p handling the configuration interfaces of the robot
 * - state_p streaming information about the current state of the robot
 * - control_p receiving a stream of control commands (e.g. position)
 * 
 */
class yarp::dev::ServerControlBoard : 
            public DeviceDriver, 
            public Thread,
            public IPidControl,
            public IPositionControl,
            public IEncoders,
//            public IVelocityControl,
            public IAmplifierControl,
            public IControlLimits
//            public IControlCalibration
//            public IControlDebug
            // convenient to put these here just to make sure all
            // methods get implemented
{
private:
	bool spoke;
    Port rpc_p; // RPC to configure the robot
    Port state_p;   // out port to read the state
    Port control_p; // in port to command the robot

    PortWriterBuffer<yarp::sig::Vector> state_buffer;
    PortReaderBuffer<yarp::sig::Vector> control_buffer;

    yarp::dev::CommandsHelper command_reader;
    yarp::dev::ControlHelper control_reader;

    PolyDriver poly;

    int               nj;
    IPidControl       *pid;
    IPositionControl  *pos;
    IEncoders         *enc;
    IAmplifierControl *amp;
    IControlLimits    *lim;
    //IVelocityControl  *vel;

    // LATER: other interfaces here.

public:
    ServerControlBoard() : command_reader(this), control_reader(this) {
        pid = NULL;
        pos = NULL;
        enc = NULL;
        amp = NULL;
        lim = NULL;
		spoke = false;
    }
    
    virtual bool open() {
        return false;
    }
    
    virtual bool close() {
        // close the port connections here!
        // Network::
        return true;
    }
    
    virtual bool open(Searchable& prop) {
        // attach readers.
        rpc_p.setReader(command_reader);
        control_p.setReader(control_reader);
        // attach writers.
        state_buffer.attach(state_p);
        control_buffer.attach(control_p);
        
        BottleBit *name;
        if (prop.check("subdevice",name)) {
            ACE_OS::printf("Subdevice %s\n", name->toString().c_str());
            if (name->isString()) {
                // maybe user isn't doing nested configuration
                Property p;
                p.fromString(prop.toString());
                p.put("device",name->toString());
                poly.open(p);
            } else {
                poly.open(*name);
            }
            if (!poly.isValid()) {
                ACE_OS::printf("cannot make <%s>\n", name->toString().c_str());
            }
        } else {
            ACE_OS::printf("\"--subdevice <name>\" not set for server_framegrabber\n");
            return false;
        }

        if (prop.check("name",name)) {
            String s((size_t)1024);
            ACE_OS::sprintf(&s[0], "%s/rpc:i", name->asString().c_str());
            rpc_p.open(s.c_str());
            ACE_OS::sprintf(&s[0], "%s/control:i", name->asString().c_str());
            control_p.open(s.c_str());
            ACE_OS::sprintf(&s[0], "%s/state:o", name->asString().c_str());
            state_p.open(s.c_str());
        } else {
            rpc_p.open("/controlboard/rpc:i");
            control_p.open("/controlboard/control:i");
            state_p.open("/controlboard/state:o");
        }
        
        if (poly.isValid()) {
            poly.view(pid);
            poly.view(pos);
            poly.view(enc);
            poly.view(amp);
            poly.view(lim);
        }
        
        if (pid != NULL &&
            pos != NULL &&
            enc != NULL &&
            amp != NULL &&
            lim != NULL) {
            if (!pos->getAxes(&nj)) {
                ACE_OS::printf ("problems: controlling 0 axes\n");
                return false;
            }
            start();
            return true;
        }
        
        ACE_OS::printf("subdevice <%s> doesn't look like a control board (not all interfaces were acquired)\n",
               name->toString().c_str());
        
        return false;
    }

    /**
     * The thread main loop deals with writing on ports here.
     */
    virtual void run() {
        ACE_OS::printf("Server control board starting\n");
        while (!isStopping()) {
            yarp::sig::Vector& v = state_buffer.get();
            v.size(10);

            // getCurrentPosition()
			if (!spoke) {
                ACE_OS::printf("Network control board writing a %d vector of double...\n", v.size());
				spoke = true;
			}
            state_buffer.write();
        }
        ACE_OS::printf("Server control board stopping\n");
    }

    /* IPidControl */

    /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setPid(int j, const Pid &p) {
        if (pid)
            return pid->setPid(j, p);
        return false;
    }

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
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
     * @param errs pointer to the vector that will store the errors
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

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
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
     * @param true if the trajectory is terminated, false otherwise
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
     * @param true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(bool *flag) { 
        if (pos)
            return pos->checkMotionDone(flag);
        ACE_OS::memset(flag, 1, sizeof(bool)*nj);
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

    /** Stop motion, multiple joints 
     * @return true/false on success/failure
     */
    virtual bool stop() {
        if (pos)
            return pos->stop();
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
     * @param spds pointer to the array that will contain the output
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
     * @param axis joint number (again... why am I telling you this)
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false if something bad happens (yes, sometimes life is tough)
     */
    virtual bool getLimits(int axis, double *min, double *max) {
        if (lim)
            return lim->getLimits(axis, min, max);
        *min = 0.0;
        *max = 0.0;
        return false;
    }
};

/**
 * The remoted side of the control board (client side).
 */
class yarp::dev::RemoteControlBoard : 
            public IPidControl,
            public IPositionControl, 
            public IEncoders,
            public IAmplifierControl,
            public IControlLimits,
            public DeviceDriver {
protected:
    Port rpc_p;
    Port command_p;
    Port state_p;

    PortReaderBuffer<yarp::sig::Vector> state_buffer;
    PortWriterBuffer<yarp::sig::Vector> command_buffer;

    String remote;
    String local;

    int nj;

    bool setCommand(int code) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        rpc_p.write(cmd);
        return true;
    }

    bool setCommand(int code, double v) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(v);
        rpc_p.write(cmd);
        return true;
    }

    bool setCommand(int code, int v) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt(v);
        rpc_p.write(cmd);
        return true;
    }

    bool getCommand(int code, double& v) const {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        rpc_p.write(cmd, response);
        // response should be [cmd] [name] value
        v = response.get(2).asDouble();
        return true;
    }

    bool getCommand(int code, int& v) const {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        rpc_p.write(cmd, response);
        // response should be [cmd] [name] value
        v = response.get(2).asInt();
        return true;
    }

    /**
     * Helper method to set a double value to a single axis.
     * @param code is the name of the command to be transmitted
     * @param j is the axis
     * @param val is the double value
     * @return true/false on success/failure
     */
    bool setDouble (int code, int j, double val) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt(j);
        cmd.addDouble(val);
        rpc_p.write(cmd);
        return true;
    }

    /** 
     * Helper method used to set an array of double to all axes.
     * @param v is the command to set
     * @param val is the double array (of length <nj>)
     * @return true/false on success/failure
     */
    bool setDoubleArray(int v, const double *val) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++)
            l.addDouble(val[i]);
        rpc_p.write(cmd);
        return true;
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
        rpc_p.write(cmd, response);
        *val = response.get(2).asDouble();
        return true;
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
        rpc_p.write(cmd, response);
        int i;
        Bottle& l = *(response.get(2).asList());
        int njs = l.size();
        ACE_ASSERT (nj == njs);
        for (i = 0; i < nj; i++)
            val[i] = l.get(i).asDouble();
        return true;
    }

public:
    RemoteControlBoard() { 
        nj = 0;
    }

    virtual ~RemoteControlBoard() {
    }

    virtual bool open() {
        return true;
    }

    virtual bool open(Searchable& config) {
        remote = config.find("remote").asString().c_str();
        local = config.find("local").asString().c_str();
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
            // connect twice for RPC ports.
            Network::connect(s2.c_str(), s1.c_str());
            Network::connect(s1.c_str(), s2.c_str());
            s1 = remote;
            s1 += "/command:i";
            s2 = local;
            s2 += "/command:o";
            Network::connect(s2.c_str(), s1.c_str());
            s1 = remote;
            s1 += "/state:o";
            s2 = local;
            s2 += "/state:i";
            Network::connect(s2.c_str(), s1.c_str());
        }
        
        state_buffer.attach(state_p);
        command_buffer.attach(command_p);

        bool ok = getCommand(VOCAB_AXES, nj);
        if (!ok) {
            ACE_OS::printf("Problems with obtaining the number of controlled axes\n");
            return false;
        }

        return true;
    }

    virtual bool close() {
        return true;
    }

    /** 
     * Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setPid(int j, const Pid &pid) {
        Bottle cmd;
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
        rpc_p.write(cmd);
        return true;
    }

    /** 
     * Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setPids(const Pid *pids) {
        Bottle cmd;
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
        rpc_p.write(cmd);
        return true;
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
        rpc_p.write(cmd, response);
        Bottle& l = *(response.get(2).asList());
        pid->kp = l.get(0).asDouble();
        pid->kd = l.get(1).asDouble();
        pid->ki = l.get(2).asDouble();
        pid->max_int = l.get(3).asDouble();
        pid->max_output = l.get(4).asDouble();
        pid->offset = l.get(5).asDouble();
        pid->scale = l.get(6).asDouble();
        return true;
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
        rpc_p.write(cmd, response);
        int i;
        Bottle& l = *(response.get(2).asList());
        const int njs = l.size();
        ACE_ASSERT (njs == nj);
        for (i = 0; i < nj; i++)
        {
            Bottle& m = *(l.get(i).asList());
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
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_RESET);
        cmd.addInt(j);
        rpc_p.write(cmd);
        return true;
    }

    /** 
     * Disable the pid computation for a joint
     * @param j is the joint number
     * @return true/false on success/failure
     */
    virtual bool disablePid(int j) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_DISABLE);
        cmd.addInt(j);
        rpc_p.write(cmd);
        return true;
    }

    /** 
     * Enable the pid computation for a joint
     * @param j is the joint number
     * @return true/false on success/failure
     */
    virtual bool enablePid(int j) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_ENABLE);
        cmd.addInt(j);
        rpc_p.write(cmd);
        return true;
    }

    /* IEncoder */

    /**
     * Reset encoder, single joint. Set the encoder value to zero 
     * @param j is the axis number
     * @return true/false on success/failure
     */
    virtual bool resetEncoder(int j) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_E_RESET);
        cmd.addInt(j);
        rpc_p.write(cmd);   // missing return value, success/failure.
        return true;
    }

    /**
     * Reset encoders. Set the encoders value to zero 
     * @return true/false
     */
    virtual bool resetEncoders() {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_E_RESETS);
        rpc_p.write(cmd);
        return true;
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
        return getDoubleArray(VOCAB_ENCODERS, encs);
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
     * @param spds pointer to the array that will contain the output
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
        return setDoubleArray(VOCAB_POSITION_MOVES, refs);
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
        rpc_p.write(cmd, response);
        *flag = (bool)(response.get(2).asInt());
        return true;
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(bool *flag) { 
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_MOTION_DONES);
        rpc_p.write(cmd, response);
        int i;
        Bottle& l = *(response.get(2).asList());
        int njs = l.size();
        ACE_ASSERT (nj == njs);
        for (i = 0; i < nj; i++)
            flag[i] = (bool)(l.get(i).asInt());
        return true;
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
        return setDoubleArray(VOCAB_REF_SPEED, spds);
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
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_STOP);
        cmd.addInt(j);
        rpc_p.write(cmd);   // missing return value, success/failure.
        return true;
    }

    /** 
     * Stop motion, multiple joints 
     * @return true/false on success/failure
     */
    virtual bool stop() { 
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_STOPS);
        rpc_p.write(cmd);   // missing return value, success/failure.
        return true;
    }

    /* IAmplifierControl */

    /** 
     * Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid 
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmp(int j) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_AMP_ENABLE);
        cmd.addInt(j);
        rpc_p.write(cmd);   // missing return value, success/failure.
        return true;
    }

    /** 
     * Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmp(int j) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_AMP_DISABLE);
        cmd.addInt(j);
        rpc_p.write(cmd);   // missing return value, success/failure.
        return true;
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
     * @return probably true, might return false in bad times
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
     * @return probably true, might return false in bad times
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
        rpc_p.write(cmd, response);
        *st = response.get(2).asInt();
        return true;        
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
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_LIMITS);
        cmd.addInt(axis);
        cmd.addDouble(min);
        cmd.addDouble(max);
        rpc_p.write(cmd);   // missing return value, success/failure.
        return true;
    }
    
    /**
     * Get the software limits for a particular axis.
     * @param axis joint number (again... why am I telling you this)
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false if something bad happens (yes, sometimes life is tough)
     */
    virtual bool getLimits(int axis, double *min, double *max) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_LIMITS);
        cmd.addInt(axis);
        rpc_p.write(cmd, response);
        *min = response.get(2).asDouble();
        *max = response.get(3).asDouble();
        return true;        
    }

};



// implementation of CommandsHelper
yarp::dev::CommandsHelper::CommandsHelper(yarp::dev::ServerControlBoard *x) { 
    ACE_ASSERT (x != NULL);
    caller = x; 
    pid = dynamic_cast<yarp::dev::IPidControl *> (caller);
    pos = dynamic_cast<yarp::dev::IPositionControl *> (caller);
    enc = dynamic_cast<yarp::dev::IEncoders *> (caller);
    amp = dynamic_cast<yarp::dev::IAmplifierControl *> (caller);
    lim = dynamic_cast<yarp::dev::IControlLimits *> (caller);
    pos->getAxes(&nj);
}

bool yarp::dev::CommandsHelper::read(ConnectionReader& connection) {
    Bottle cmd, response;
    bool ok = false;
    cmd.read(connection);
    ACE_OS::printf("command received: %s\n", cmd.toString().c_str());
    int code = cmd.get(0).asVocab();
    switch (code) {
    case VOCAB_SET:
        ACE_OS::printf("set command received\n");
        {
            switch(cmd.get(1).asVocab()) {
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

            case VOCAB_POSITION_MODE: {
                ok = pos->setPositionMode();
            }
            break;

            case VOCAB_POSITION_MOVE: {
                ok = pos->positionMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
            }
            break;

            case VOCAB_POSITION_MOVES: {
                Bottle& b = *(cmd.get(2).asList());
                int i;
                const int njs = b.size();
                ACE_ASSERT (njs == nj);
                double *p = new double[njs];    // LATER: optimize to avoid allocation. 
                ACE_ASSERT (p != NULL);
                for (i = 0; i < njs; i++)
                    p[i] = b.get(i).asDouble();
                ok = pos->positionMove(p);
                delete[] p;                
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
        ACE_OS::printf("get command received\n");
        {
            int tmp = 0;
            double dtmp = 0.0;
            response.addVocab(VOCAB_IS);
            response.addBit(cmd.get(1));
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
                bool *p = new bool[nj];
                ACE_ASSERT(p!=NULL);
                ok = pos->checkMotionDone(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < nj; i++)
                    b.addInt(p[i]);
                delete[] p;
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

            default:
                ACE_OS::printf("received an unknown request after a VOCAB_GET\n");
                break;
            }

            if (!ok) {
                // leave answer blank
            }
        }
        break;
    }

    if (response.size() >= 1) {
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=NULL) {
            response.write(*writer);
            ACE_OS::printf("response sent: %s\n", response.toString().c_str());
        }
    }

    return ok;
}

// implementation of ControlHelper
yarp::dev::ControlHelper::ControlHelper(yarp::dev::ServerControlBoard *x) { 
    ACE_ASSERT (x != NULL);
    caller = x; 
    pos = dynamic_cast<yarp::dev::IPositionControl *> (caller);
}

bool yarp::dev::ControlHelper::read(ConnectionReader& connection) {
    Vector v;
    v.read(connection);
    ACE_OS::printf("data received of size: %d\n", v.size());

    // do I need to check for the type of data (e.g. Bottle)?
    // Vector should be binary compatible with Bottle...

    // LATER: switch depending on the control mode.
    // ctrl->positionMove (&v[0]);

    // no response is needed here.
    return true;
}

// needed for the driver factory.
yarp::dev::DriverCreator *createServerControlBoard() {
    return new DriverCreatorOf<ServerControlBoard>("controlboard", "controlboard");
}

yarp::dev::DriverCreator *createRemoteControlBoard() {
    return new DriverCreatorOf<RemoteControlBoard>("remote_controlboard", "controlboard");
}
