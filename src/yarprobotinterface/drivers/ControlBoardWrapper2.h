// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __CONTROLBAORDWRAPPER2__
#define __CONTROLBAORDWRAPPER2__

/*
* Copyright (C) 2008 RobotCub Consortium
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// ControlBoardWrapper2
// A modified version of the YARP remote control board class
// which remaps joints. Similar to ControlBoardWrapper, but it
// also merge can networks into a single parts.
//

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <iCub/DebugInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/Wrapper.h>

#include <string>
#include <vector>

#include "debugging.h"

//
//#define CONTROLBOARDWRAPPER2_DEBUG
const int DEBUG_PRINTF_BUFFER_LENGTH=255;
#include <stdarg.h>
inline void DEBUG_CW2(const char *fmt, ...)
{
#ifdef CONTROLBOARDWRAPPER2_DEBUG
    va_list ap;
    va_start(ap, fmt);
    char buffer[DEBUG_PRINTF_BUFFER_LENGTH];
#ifdef WIN32
    _vsnprintf(buffer, DEBUG_PRINTF_BUFFER_LENGTH, fmt, ap);
#else
    vsnprintf(buffer, DEBUG_PRINTF_BUFFER_LENGTH, fmt, ap);
#endif
    fprintf(stderr, "%s", buffer);
    va_end(ap);
#endif
}

#include <yarp/os/impl/Logger.h>

#include "ControlBoardWrapper.h"

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os::impl;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* the control command message type
* head is a Bottle which contains the specification of the message type
* body is a Vector which move the robot accordingly
*/
//typedef PortablePair<Bottle, Vector> CommandMessage; //defined in ControlBoardWrapper.h

class ControlBoardWrapper2;

/**
* Helper object for reading config commands for the ControlBoardWrapper
* class.
*/
class CommandsHelper2 : public DeviceResponder {
protected:
    ControlBoardWrapper2   *caller;
    yarp::dev::IPidControl          *pid;
    yarp::dev::IPositionControl     *pos;
    yarp::dev::IVelocityControl     *vel;
    yarp::dev::IEncodersTimed       *enc;
    yarp::dev::IAmplifierControl    *amp;
    yarp::dev::IControlLimits       *lim;
    yarp::dev::ITorqueControl       *torque;
    yarp::dev::IControlMode         *iMode;
    yarp::dev::IDebugInterface      *iDbg;
    yarp::dev::IAxisInfo            *info;
    yarp::dev::IControlCalibration2   *ical2;
    yarp::dev::IOpenLoopControl     *iOpenLoop;
    yarp::dev::IImpedanceControl    *iImpedance;
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
    CommandsHelper2(ControlBoardWrapper2 *x);

    virtual bool respond(const Bottle& cmd, Bottle& response);

    void handleTorqueMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleControlModeMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleImpedanceMsg(const yarp::os::Bottle& cmd,
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
class ImplementCallbackHelper2 : public TypedReaderCallback<CommandMessage> {
protected:
    IPositionControl *pos;
    IVelocityControl *vel;
    IOpenLoopControl *iOpenLoop;

public:
    /**
    * Constructor.
    * @param x is the instance of the container class using the callback.
    */
    ImplementCallbackHelper2(ControlBoardWrapper2 *x);

    /**
    * Callback function.
    * @param v is the Vector being received.
    */
    virtual void onRead(CommandMessage& v);
};

class SubDevice
{
public:
    std::string id;
    int base;
    int top;
    int axes;

    bool configuredF;
    bool attachedF;

    PolyDriver *subdevice;
    IPidControl       *pid;
    IPositionControl  *pos;
    IVelocityControl  *vel;
    IEncodersTimed *enc;
    IAmplifierControl *amp;
    IControlLimits    *lim;
    IControlCalibration *calib;
    IControlCalibration2 *calib2;
    IPreciselyTimed      *iTimed;
    ITorqueControl       *iTorque;
    IImpedanceControl    *iImpedance;
    IOpenLoopControl     *iOpenLoop;
    IDebugInterface      *iDbg;
    IControlMode         *iMode;
    IAxisInfo          *info;

    yarp::sig::Vector encoders;
    yarp::sig::Vector encodersTimes;

    SubDevice();

    bool attach(PolyDriver *d, const std::string &id);
    void detach();

    bool configure(int base, int top, int axes, const std::string &id);

    inline void refreshEncoders()
    {
        enc->getEncodersTimed(encoders.data(), encodersTimes.data());
    }

    bool isAttached()
    { return attachedF; }
};

typedef std::vector<SubDevice> SubDeviceVector;

struct DevicesLutEntry
{
    int offset; //an offset, the device is mapped starting from this joint
    int deviceEntry; //index to the joint corresponding subdevice in the list
};


class WrappedDevice
{
public:
    SubDeviceVector subdevices;
    std::vector<DevicesLutEntry> lut;

    inline SubDevice *getSubdevice(unsigned int i)
    {
        if (i>=subdevices.size())
            return 0;

        return &subdevices[i];
    }
};


#endif // DOXYGEN_SHOULD_SKIP_THIS

/*
* A modified version of the network wrapper. Similar
* to the the network wrapper in YARP, but it
* maps only a subpart of the underlying device.
* Allows also deferred attach/detach of a subdevice.
*/
class ControlBoardWrapper2 : public DeviceDriver,
                             public RateThread,
                             public IPidControl,
                             public IPositionControl,
                             public IVelocityControl,
                             public IEncodersTimed,
                             public IAmplifierControl,
                             public IControlLimits,
                             public IDebugInterface,
                             public IControlCalibration,
                             public IControlCalibration2,
                             public IOpenLoopControl,
                             public ITorqueControl,
                             public IImpedanceControl,
                             public IControlMode,
                             public IMultipleWrapper,
                             public IAxisInfo,
                             public IPreciselyTimed
{
private:
    bool spoke;
    bool verb;

    WrappedDevice device;

    Port state_p;   // out port to read the state
    Port control_p; // in port to command the robot
    Port rpc_p;     // RPC to configure the robot
    Stamp time;     // envelope to attach to the state port
    Semaphore timeMutex;

    PortWriterBuffer<yarp::sig::Vector> state_buffer;
    PortReaderBuffer<CommandMessage> control_buffer;

    ImplementCallbackHelper2 callback_impl;

    CommandsHelper2 command_reader;

    // for new interface
    PortReaderBuffer<Bottle> command_buffer;

    Vector            encoders;
    std::string       partName;

    int               controlledJoints;
    int               base;
    int               top;
    int               thread_period;


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
    ControlBoardWrapper2() : RateThread(20), callback_impl(this), command_reader(this)
    {
        ////YARP_TRACE(Logger::get(),"ControlBoardWrapper2::ControlBoardWrapper2()", Logger::get().log_files.f3);
        controlledJoints = 0;
        thread_period = 20; // ms.

        verb = false;
    }

    virtual ~ControlBoardWrapper2() {
        //YARP_TRACE(Logger::get(),"ControlBoardWrapper2::~ControlBoardWrapper2()", Logger::get().log_files.f3);
        closeMain();
    }

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const { return verb; }

    /**
    * Default open() method.
    * @return always false since initialization requires parameters.
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
    virtual bool open(Searchable& prop);

    virtual bool detachAll()
    {
        RateThread::stop();

        int devices=device.subdevices.size();
        for(int k=0;k<devices;k++)
            device.getSubdevice(k)->detach();

        return true;
    }

    virtual bool attachAll(const yarp::dev::PolyDriverList &l);

    /**
    * The thread main loop deals with writing on ports here.
    */
    virtual void run();

    /* IPidControl */
    /** Set new pid value for a joint axis.
    * @param j joint number
    * @param p new pid value
    * @return true/false on success/failure
    */
    virtual bool setPid(int j, const Pid &p)
    {
        // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::setPid", Logger::get().log_files.f3);
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *s=device.getSubdevice(subIndex);
        if (!s)
            return false;

        if (s->pid)
        {
            return s->pid->setPid(off+base, p);
        }
        return false;
    }

    /** Set new pid value on multiple axes.
    * @param ps pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual bool setPids(const Pid *ps)
    {
        // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::setPids", Logger::get().log_files.f3);
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->setPid(off+base, ps[l]);
            }
            else
                ret=false;
        }
        // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::setPids - niente!", Logger::get().log_files.f3);
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
    virtual bool setReference(int j, double ref)
    {
        // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::setReference", Logger::get().log_files.f3);
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->setReference(off+base, ref);
        }
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
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->setReference(off+base, refs[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    /** Set the error limit for the controller on a specifi joint
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual bool setErrorLimit(int j, double limit) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->setErrorLimit(off+base, limit);
        }
        return false;
    }

    /** Get the error limit for the controller on all joints.
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual bool setErrorLimits(const double *limits) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->setErrorLimit(off+base, limits[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    /** Get the current error for a joint.
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual bool getError(int j, double *err) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->getError(off+base, err);
        }
        *err = 0.0;
        return false;
    }

    /** Get the error of all joints.
    * @param errs pointer to the vector that will store the errors.
    * @return true/false on success/failure.
    */
    virtual bool getErrors(double *errs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->getError(off+base, errs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    /** Get the output of the controller (e.g. pwm value)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual bool getOutput(int j, double *out) {

        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->getOutput(off+base, out);
        }
        *out=0.0;
        return false;
    }

    /** Get the output of the controllers (e.g. pwm value)
    * @param outs pinter to the vector that will store the output values
    */
    virtual bool getOutputs(double *outs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->getOutput(off+base, outs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool setOffset(int j, double v)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->setOffset(off+base, v);
        }
        return false;
    }

    /** Get current pid value for a specific joint.
    * @param j joint number
    * @param p pointer to storage for the return value.
    * @return success/failure
    */
    virtual bool getPid(int j, Pid *p)
    {
        // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::getPid", Logger::get().log_files.f3);
//#warning "check for max number of joints!?!?!"
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *s=device.getSubdevice(subIndex);
        if (!s)
            return false;

        if (s->pid)
        {
            return s->pid->getPid(off+base, p);
        }
        return false;
    }

    /** Get current pid value for a specific joint.
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual bool getPids(Pid *pids)
    {
        // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::getPids", Logger::get().log_files.f3);
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->getPid(off+base, pids+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    /** Get the current reference position of the controller for a specific joint.
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual bool getReference(int j, double *ref) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->getReference(off+base, ref);
        }
        return false;
    }

    /** Get the current reference position of all controllers.
    * @param refs vector that will store the output.
    */
    virtual bool getReferences(double *refs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->getReference(off+base, refs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    /** Get the error limit for the controller on a specific joint
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual bool getErrorLimit(int j, double *limit) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->getErrorLimit(off+base, limit);
        }
        return false;
    }

    /** Get the error limit for all controllers
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual bool getErrorLimits(double *limits) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->getErrorLimit(off+base, limits+l);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->resetPid(off+base);
        }
        return false;
    }

    /**
    * Disable the pid computation for a joint
    * @param j is the axis number
    * @return true if successful, false on failure
    **/
    virtual bool disablePid(int j) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->disablePid(off+base);
        }
        return false;
    }

    /**
    * Enable the pid computation for a joint
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool enablePid(int j) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            return p->pid->enablePid(off+base);
        }
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
        *ax=controlledJoints;
        return true;
    }

    /**
    * Set position mode. This command
    * is required by control boards implementing different
    * control methods (e.g. velocity/torque), in some cases
    * it can be left empty.
    * return true/false on success/failure
    */
    virtual bool setPositionMode() {
        bool ret=true;
        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                //calling iControlMode interface
                ret=ret&&p->iMode->setPositionMode(off+base);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool setOpenLoopMode() {
        bool ret=true;
        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                //calling iControlMode interface
                ret=ret&&p->iMode->setOpenLoopMode(off+base);
            }
            else
                ret=false;
        }
        return ret;
        return false;
    }

    /**
    * Set new reference point for a single axis.
    * @param j joint number
    * @param ref specifies the new ref point
    * @return true/false on success/failure
    */
    virtual bool positionMove(int j, double ref) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->positionMove(off+base, ref);
        }

        return false;
    }

    /** Set new reference point for all axes.
    * @param refs array, new reference points.
    * @return true/false on success/failure
    */
    virtual bool positionMove(const double *refs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pos)
            {
                ret=ret&&p->pos->positionMove(off+base, refs[l]);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->relativeMove(off+base, delta);
        }

        return false;
    }

    /** Set relative position, all joints.
    * @param deltas pointer to the relative commands
    * @return true/false on success/failure
    */
    virtual bool relativeMove(const double *deltas) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pos)
            {
                ret=ret&&p->pos->relativeMove(off+base, deltas[l]);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->checkMotionDone(off+base, flag);
        }

        return false;
    }

    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param flag true if the trajectory is terminated, false otherwise
    * @return false on failure
    */
    virtual bool checkMotionDone(bool *flag) {
        bool ret=true;
        *flag=true;
        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                bool tmpF=false;
                ret=ret&&p->pos->checkMotionDone(off+base, &tmpF);
                *flag=*flag&&tmpF;
            }
            else
                ret=false;
        }
        return ret;
    }

    /** Set reference speed for a joint, this is the speed used during the
    * interpolation of the trajectory.
    * @param j joint number
    * @param sp speed value
    * @return true/false upon success/failure
    */
    virtual bool setRefSpeed(int j, double sp) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->setRefSpeed(off+base, sp);
        }
        return false;
    }

    /** Set reference speed on all joints. These values are used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array of speed values.
    * @return true/false upon success/failure
    */
    virtual bool setRefSpeeds(const double *spds) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pos)
            {
                ret=ret&&p->pos->setRefSpeed(off+base, spds[l]);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->setRefAcceleration(off+base, acc);
        }
        return false;
    }

    /** Set reference acceleration on all joints. This is the valure that is
    * used during the generation of the trajectory.
    * @param accs pointer to the array of acceleration values
    * @return true/false upon success/failure
    */
    virtual bool setRefAccelerations(const double *accs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                ret=ret&&p->pos->setRefAcceleration(off+base, accs[l]);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->getRefSpeed(off+base, ref);
        }
        *ref=0;
        return false;
    }

    /** Get reference speed of all joints. These are the  values used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array that will store the speed values.
    * @return true/false on success/failure.
    */
    virtual bool getRefSpeeds(double *spds) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                ret=ret&&p->pos->getRefSpeed(off+base, spds+l);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->getRefAcceleration(off+base, acc);
        }
        *acc=0;
        return false;
    }

    /** Get reference acceleration of all joints. These are the values used during the
    * interpolation of the trajectory.
    * @param accs pointer to the array that will store the acceleration values.
    * @return true/false on success or failure
    */
    virtual bool getRefAccelerations(double *accs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                ret=ret&&p->pos->getRefAcceleration(off+base, accs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    /** Stop motion, single joint
    * @param j joint number
    * @return true/false on success/failure
    */
    virtual bool stop(int j) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            return p->pos->stop(off+base);
        }
        return false;
    }

    /**
    * Stop motion, multiple joints
    * @return true/false on success/failure
    */
    virtual bool stop() {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                ret=ret&&p->pos->stop(off+base);
            }
            else
                ret=false;
        }
        return ret;
    }

    /* IVelocityControl */

    /**
    * Set new reference speed for a single axis.
    * @param j joint number
    * @param v specifies the new ref speed
    * @return true/false on success/failure
    */
    virtual bool velocityMove(int j, double v) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            return p->vel->velocityMove(off+base, v);
        }
        return false;
    }

    /**
    * Set a new reference speed for all axes.
    * @param v is a vector of double representing the requested speed.
    * @return true/false on success/failure.
    */
    virtual bool velocityMove(const double *v) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->vel)
            {
                ret=ret&&p->vel->velocityMove(off+base, v[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    /**
    * Set the controller to velocity mode.
    * @return true/false on success/failure.
    */
    virtual bool setVelocityMode() {
        bool ret=true;
        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                //calling iControlMode interface
                ret=ret&&p->iMode->setVelocityMode(off+base);
            }
            else
                ret=false;
        }
        return ret;
    }

    /* IEncoders */

    /**
    * Reset encoder, single joint. Set the encoder value to zero
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool resetEncoder(int j) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->enc)
        {
            return p->enc->resetEncoder(off+base);
        }
        return false;
    }

    /**
    * Reset encoders. Set the encoder values to zero for all axes
    * @return true/false
    */
    virtual bool resetEncoders() {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->enc)
            {
                ret=ret&&p->enc->resetEncoder(off+base);
            }
            else
                ret=false;
        }
        return ret;
    }

    /**
    * Set the value of the encoder for a given joint.
    * @param j encoder number
    * @param val new value
    * @return true/false
    */
    virtual bool setEncoder(int j, double val) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->enc)
        {
            return p->enc->setEncoder(off+base,val);
        }
        return false;
    }

    /**
    * Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    virtual bool setEncoders(const double *vals) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->enc)
            {
                ret=ret&&p->enc->setEncoder(off+base, vals[l]);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->enc->getEncoder(off+base, v);
        }
        *v=0.0;
        return false;
    }

    /**
    * Read the position of all axes.
    * @param encs pointer to the array that will contain the output
    * @return true/false on success/failure
    */
    virtual bool getEncoders(double *encs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->enc)
            {
                ret=ret&&p->enc->getEncoder(off+base, encs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getEncodersTimed(double *encs, double *t) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->enc)
            {
                ret=ret&&p->enc->getEncoderTimed(off+base, encs+l, t+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getEncoderTimed(int j, double *v, double *t) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->enc->getEncoderTimed(off+base, v, t);
        }
        *v=0.0;
        return false;
    }

    /**
    * Read the istantaneous speed of an axis.
    * @param j axis number
    * @param sp pointer to storage for the output
    * @return true if successful, false ... otherwise.
    */
    virtual bool getEncoderSpeed(int j, double *sp) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->enc->getEncoderSpeed(off+base, sp);
        }
        *sp=0.0;
        return false;
    }

    /**
    * Read the instantaneous speed of all axes.
    * @param spds pointer to storage for the output values
    * @return guess what? (true/false on success or failure).
    */
    virtual bool getEncoderSpeeds(double *spds) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->enc)
            {
                ret=ret&&p->enc->getEncoderSpeed(off+base, spds+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    /**
    * Read the instantaneous acceleration of an axis.
    * @param j axis number
    * @param acc pointer to the array that will contain the output
    */
    virtual bool getEncoderAcceleration(int j, double *acc) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->enc->getEncoderAcceleration(off+base,acc);
        }
        *acc=0.0;
        return false;
    }

    /**
    * Read the istantaneous acceleration of all axes.
    * @param accs pointer to the array that will contain the output
    * @return true if all goes well, false if anything bad happens.
    */
    virtual bool getEncoderAccelerations(double *accs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->enc)
            {
                ret=ret&&p->enc->getEncoderAcceleration(off+base, accs+l);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->amp->enableAmp(off+base);
        }
        return false;
    }

    /**
    * Disable the amplifier on a specific joint. All computations within the board
    * will be carried out normally, but the output will be disabled.
    * @return true/false on success/failure
    */
    virtual bool disableAmp(int j) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->amp->disableAmp(off+base);
        }
        return false;
    }

    /**
    * Read the electric current going to all motors.
    * @param vals pointer to storage for the output values
    * @return hopefully true, false in bad luck.
    */
    virtual bool getCurrents(double *vals) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->amp)
            {
                ret=ret&&p->amp->getCurrent(off+base, vals+l);
            }
            else
                ret=false;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->amp->getCurrent(off+base,val);
        }
        *val=0.0;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            return p->amp->setMaxCurrent(off+base,v);
        }
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
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (p->amp)
                {

                    st[l]=0;
                    //getAmpStatus for single joint does not exist!!
                    // AMP_STATUS TODO
                    //ret=ret&&p->amp->getAmpStatus(off+base, st+l);
                }
            else
                ret=false;
        }

        return ret;
    }

    virtual bool getAmpStatus(int j, int *v)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (p->amp)
            {
                return p->amp->getAmpStatus(off+base,v);
            }
        *v=0;
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
    virtual bool setLimits(int j, double min, double max) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->lim)
        {
            return p->lim->setLimits(off+base,min, max);
        }
        return false;
    }

    /**
    * Get the software limits for a particular axis.
    * @param axis joint number
    * @param min pointer to store the value of the lower limit
    * @param max pointer to store the value of the upper limit
    * @return true if everything goes fine, false if something bad happens (yes, sometimes life is tough)
    */
    virtual bool getLimits(int j, double *min, double *max) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            {
                *min=0.0;
                *max=0.0;
                return false;
            }

        if (p->lim)
            {
                return p->lim->getLimits(off+base,min, max);
            }
        *min=0.0;
        *max=0.0;
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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *s=device.getSubdevice(subIndex);
        if (!s)
            return false;

        if (s->calib)
        {
            return s->calib->calibrate(off+base, p);
        }
        return false;
    }

    virtual bool calibrate2(int j, unsigned int ui, double v1, double v2, double v3) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (p->calib2)
        {
            return p->calib2->calibrate2(off+base, ui,v1,v2,v3);
        }
        return false;
    }

    /**
    * Check whether the calibration has been completed.
    * @param j is the joint that has started a calibration procedure.
    * @return true/false on success/failure.
    */
    virtual bool done(int j) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->calib2)
        {
            return p->calib2->done(off+base);
        }
        return false;
    }

    virtual bool abortPark()
    {
        fprintf(stderr, "ControlBoardWrapper2::Calling abortPark -- not implemented\n");
        return false;
    }

    virtual bool abortCalibration()
    {
        fprintf(stderr, "ControlBoardWrapper2::Calling abortCalibration -- not implemented\n");
        return false;
    }

    /* IAxisInfo */
    virtual bool getAxisName(int j, yarp::os::ConstString& name) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->info)
        {
            return p->info->getAxisName(off+base, name);
        }
        return false;
    }

    virtual bool setTorqueMode()
    {
        bool ret=true;
        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);

            if (!p)
                return false;

            if (p->pos)
            {
                //calling iControlMode interface
                ret=ret&&p->iMode->setTorqueMode(off+base);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getRefTorques(double *refs)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->getRefTorque(off+base, refs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getRefTorque(int j, double *t)
    {

        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getRefTorque(off+base, t);
        }
        return false;
    }

    virtual bool setRefTorques(const double *t)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->setRefTorque(off+base, t[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool setRefTorque(int j, double t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->setRefTorque(off+base, t);
        }
        return false;
    }

    virtual bool setTorquePid(int j, const Pid &pid)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->setTorquePid(off+base, pid);
        }

        return false;
    }

    virtual bool setImpedance(int j, double stiff, double damp)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iImpedance)
        {
            return p->iImpedance->setImpedance(off+base, stiff, damp);
        }

        return false;
    }

    virtual bool setImpedanceOffset(int j, double offset)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iImpedance)
        {
            return p->iImpedance->setImpedanceOffset(off+base, offset);
        }

        return false;
    }

    virtual bool getTorque(int j, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getTorque(off+base, t);
        }

        return false;
    }

    virtual bool getTorques(double *t)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->getTorque(off+base, t+l);
            }
            else
                ret=false;
        }
        return ret;
     }

    virtual bool getTorqueRange(int j, double *min, double *max)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getTorqueRange(off+base, min, max);
        }

        return false;
    }

    virtual bool getTorqueRanges(double *min, double *max)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->getTorqueRange(off+base, min+l, max+l);
            }
            else
                ret=false;
        }
        return ret;
     }

    virtual bool setTorquePids(const Pid *pids)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->setTorquePid(off+base, pids[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool setTorqueErrorLimit(int j, double limit)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->setTorqueErrorLimit(off+base, limit);
        }

        return false;
    }

    virtual bool setTorqueErrorLimits(const double *limits)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->setTorqueErrorLimit(off+base, limits[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getTorqueError(int j, double *err)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getTorqueError(off+base, err);
        }

        return false;
    }

    virtual bool getTorqueErrors(double *errs)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->getTorqueError(off+base, errs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getTorquePidOutput(int j, double *out)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getTorquePidOutput(off+base, out);
        }

        return false;
    }

    virtual bool getTorquePidOutputs(double *outs)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->getTorquePidOutput(off+base, outs+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getTorquePid(int j, Pid *pid)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getTorquePid(off+base, pid);
        }

        return false;
    }

    virtual bool getImpedance(int j, double* stiff, double* damp)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iImpedance)
        {
            return p->iImpedance->getImpedance(off+base, stiff, damp);
        }

        return false;
    }

    virtual bool getImpedanceOffset(int j, double* offset)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iImpedance)
        {
            return p->iImpedance->getImpedanceOffset(off+base, offset);
        }

        return false;
    }

    virtual bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iImpedance)
        {
            return p->iImpedance->getCurrentImpedanceLimit(off+base, min_stiff, max_stiff, min_damp, max_damp);
        }

        return false;
    }

    virtual bool getTorquePids(Pid *pids)
    {
         bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->getTorquePid(off+base, pids+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getTorqueErrorLimit(int j, double *limit)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getTorqueErrorLimit(off+base, limit);
        }

        return false;
    }

    virtual bool getTorqueErrorLimits(double *limits)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iTorque)
            {
                ret=ret&&p->iTorque->getTorqueErrorLimit(off+base, limits+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool resetTorquePid(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->resetTorquePid(off+base);
        }

        return false;
    }

    virtual bool disableTorquePid(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->disableTorquePid(off+base);
        }

        return false;
    }

    virtual bool enableTorquePid(int j)
    {
         int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->enableTorquePid(off+base);
        }

        return false;
    }

    virtual bool setTorqueOffset(int j, double v)
    {
         int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->setTorqueOffset(off+base,v);
        }

        return false;
    }

    virtual bool setPositionMode(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            return p->iMode->setPositionMode(off+base);
        }

        return false;
    }

    virtual bool setTorqueMode(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            return p->iMode->setTorqueMode(off+base);
        }

        return false;
    }

    virtual bool setImpedancePositionMode(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            return p->iMode->setImpedancePositionMode(off+base);
        }

        return false;
    }

    virtual bool setImpedanceVelocityMode(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            return p->iMode->setImpedanceVelocityMode(off+base);
        }

        return false;
    }

    virtual bool setVelocityMode(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            return p->iMode->setVelocityMode(off+base);
        }

        return false;
    }

    virtual bool setOpenLoopMode(int j)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            return p->iMode->setOpenLoopMode(off+base);
        }

        return false;
    }

    virtual bool getControlMode(int j, int *mode)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            DEBUG_CW2("Calling GET_CONTROL_MODE\n");
            return p->iMode->getControlMode(off+base, mode);
        }
        return false;
    }

    virtual bool getControlModes(int *modes)
    {
       bool ret=true;

        DEBUG_CW2("Calling GET_CONTROL_MODES\n");
        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iMode)
            {
                ret=ret&&p->iMode->getControlMode(off+base, modes+l);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getParameter(int j, unsigned int type, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getParameter(off+base, type, t);
        }
        return false;
    }

    virtual bool setParameter(int j, unsigned int type, double t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->setParameter(off+base, type, t);
        }
        return false;
    }

    virtual bool getDebugParameter(int j, unsigned int index, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getDebugParameter(off+base, index, t);
        }
        return false;
    }

    virtual bool getDebugReferencePosition(int j, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getDebugReferencePosition(off+base, t);
        }
        return false;
    }

    virtual bool getRotorPosition(int j, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getRotorPosition(off+base, t);
        }
        return false;
    }

    virtual bool getRotorPositions(double *t)
    {
        return false;
        /*
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getRotorPosition(off+base, t);
        }
        return false;*/
    }

    virtual bool getRotorSpeed(int j, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getRotorSpeed(off+base, t);
        }
        return false;
    }

    virtual bool getRotorSpeeds(double *t)
    {
        return false;
    }

    virtual bool getRotorAcceleration(int j, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getRotorAcceleration(off+base, t);
        }
        return false;
    }

    virtual bool getRotorAccelerations(double *t)
    {
        return false;
    }

    virtual bool getJointPosition(int j, double *t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getJointPosition(off+base, t);
        }
        return false;
    }

    virtual bool getJointPositions(double *t)
    {
        return false;
        /*
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->getJointPosition(off+base, t);
        }
        return false;*/
    }

    virtual bool setDebugParameter(int j, unsigned int index, double t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->setDebugParameter(off+base, index, t);
        }
        return false;
    }

    virtual bool setDebugReferencePosition(int j, double t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iDbg)
        {
            return p->iDbg->setDebugReferencePosition(off+base, t);
        }
        return false;
    }

    virtual bool setOutput(int j, double v)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iOpenLoop)
        {
            return p->iOpenLoop->setOutput(off+base, v);
        }
        return false;
    }

    virtual bool setOutputs(const double *outs) {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->iOpenLoop)
            {
                ret=ret&&p->iOpenLoop->setOutput(off+base, outs[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual Stamp getLastInputStamp() {
        timeMutex.wait();
        Stamp ret=time;
        timeMutex.post();
        return ret;
    }
};

#endif
