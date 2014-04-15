// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __CONTROLBOARDWRAPPER2__
#define __CONTROLBOARDWRAPPER2__

/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// ControlBoardWrapper
// A modified version of the remote control board class
// which remaps joints, it can also merge networks into a single part.
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
#include <vector>


#ifdef MSVC
    #pragma warning(disable:4355)
#endif


/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp {
    namespace dev {
        class ControlBoardWrapper;
        namespace impl {
            class CommandsHelper;
            class ImplementCallbackHelper;
            class SubDevice;
            class WrappedDevice;
        }
    }
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS


enum MAX_VALUES_FOR_ALLOCATION_TABLE_TMP_DATA { MAX_DEVICES=5, MAX_JOINTS_ON_DEVICE=32};

/* the control command message type
* head is a Bottle which contains the specification of the message type
* body is a Vector which move the robot accordingly
*/
typedef yarp::os::PortablePair<yarp::os::Bottle, yarp::sig::Vector> CommandMessage;


/**
* Helper object for reading config commands for the ControlBoardWrapper
* class.
*/
class  yarp::dev::impl::CommandsHelper : public yarp::dev::DeviceResponder
{
protected:
    yarp::dev::ControlBoardWrapper      *caller;
    yarp::dev::IPidControl              *pid;
    yarp::dev::IPositionControl         *pos;
    yarp::dev::IPositionControl2        *pos2;
    yarp::dev::IVelocityControl         *vel;
    yarp::dev::IVelocityControl2        *vel2;
    yarp::dev::IEncodersTimed           *enc;
    yarp::dev::IAmplifierControl        *amp;
    yarp::dev::IControlLimits2          *lim2;
    yarp::dev::ITorqueControl           *torque;
    yarp::dev::IControlMode             *iMode;
    yarp::dev::IAxisInfo                *info;
    yarp::dev::IControlCalibration2     *ical2;
    yarp::dev::IOpenLoopControl         *iOpenLoop;
    yarp::dev::IImpedanceControl        *iImpedance;
    yarp::sig::Vector                   vect;
    yarp::os::Stamp                     lastRpcStamp;
    yarp::os::Semaphore                 mutex;
    int                                 controlledJoints;

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
    void init(yarp::dev::ControlBoardWrapper *x);

    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);

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
class  yarp::dev::impl::ImplementCallbackHelper : public yarp::os::TypedReaderCallback<CommandMessage> {
protected:
    yarp::dev::IPositionControl     *pos;
    yarp::dev::IPositionControl2    *pos2;
    yarp::dev::IPositionDirect      *posDir;
    yarp::dev::IVelocityControl     *vel;
    yarp::dev::IVelocityControl2    *vel2;
    yarp::dev::IOpenLoopControl     *iOpenLoop;
    int                             controlledAxes;

public:
    /**
    * Constructor.
    */
    ImplementCallbackHelper();

    /**
    * Initialization.
    * @param x is the instance of the container class using the callback.
    */
    void init(yarp::dev::ControlBoardWrapper *x);

    /**
    * Callback function.
    * @param v is the Vector being received.
    */
    virtual void onRead(CommandMessage& v);

    bool initialize();
};


class  yarp::dev::impl::SubDevice
{
public:
    std::string id;
    int base;
    int top;
    int axes;

    bool configuredF;

    yarp::dev::PolyDriver            *subdevice;
    yarp::dev::IPidControl           *pid;
    yarp::dev::IPositionControl      *pos;
    yarp::dev::IPositionControl2     *pos2;
    yarp::dev::IVelocityControl      *vel;
    yarp::dev::IVelocityControl2     *vel2;
    yarp::dev::IEncodersTimed        *enc;
    yarp::dev::IAmplifierControl     *amp;
    yarp::dev::IControlLimits2       *lim2;
    yarp::dev::IControlCalibration   *calib;
    yarp::dev::IControlCalibration2  *calib2;
    yarp::dev::IPreciselyTimed       *iTimed;
    yarp::dev::ITorqueControl        *iTorque;
    yarp::dev::IImpedanceControl     *iImpedance;
    yarp::dev::IOpenLoopControl      *iOpenLoop;
    yarp::dev::IControlMode          *iMode;
    yarp::dev::IAxisInfo             *info;
    yarp::dev::IPositionDirect       *posDir;

    yarp::sig::Vector subDev_encoders;
    yarp::sig::Vector encodersTimes;

    SubDevice();

    bool attach(yarp::dev::PolyDriver *d, const std::string &id);
    void detach();
    inline void setVerbose(bool _verbose) {_subDevVerbose = _verbose; }

    bool configure(int base, int top, int axes, const std::string &id);

    inline void refreshEncoders()
    {
    	int idx = 0;
    	double tmp = 0;
        for(int j=base, idx=0; j<(base+axes); j++, idx++)
        {
            if(enc)
                enc->getEncoderTimed(j, &subDev_encoders[idx], &encodersTimes[idx]);
        }
    }

    bool isAttached()
    { return attachedF; }

private:
    bool _subDevVerbose;
    bool attachedF;
};

typedef std::vector<yarp::dev::impl::SubDevice> SubDeviceVector;

struct DevicesLutEntry
{
    int offset; //an offset, the device is mapped starting from this joint
    int deviceEntry; //index to the joint corresponding subdevice in the list
};


class yarp::dev::impl::WrappedDevice
{
public:
    SubDeviceVector subdevices;
    std::vector<DevicesLutEntry> lut;

    inline yarp::dev::impl::SubDevice *getSubdevice(unsigned int i)
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
class yarp::dev::ControlBoardWrapper:
                             public yarp::dev::DeviceDriver,
                             public yarp::os::RateThread,
                             public yarp::dev::IPidControl,
                             public yarp::dev::IPositionControl2,
                             public yarp::dev::IPositionDirect,
                             public yarp::dev::IVelocityControl2,
                             public yarp::dev::IEncodersTimed,
                             public yarp::dev::IAmplifierControl,
                             public yarp::dev::IControlLimits2,
                             public yarp::dev::IControlCalibration,
                             public yarp::dev::IControlCalibration2,
                             public yarp::dev::IOpenLoopControl,
                             public yarp::dev::ITorqueControl,
                             public yarp::dev::IImpedanceControl,
                             public yarp::dev::IControlMode,
                             public yarp::dev::IMultipleWrapper,
                             public yarp::dev::IAxisInfo,
                             public yarp::dev::IPreciselyTimed
{
private:
    yarp::dev::impl::WrappedDevice device;

    yarp::os::Port state_p;   // out port to read the state
    yarp::os::Port control_p; // in port to command the robot
    yarp::os::Port rpc_p;     // RPC to configure the robot
    yarp::os::Stamp time;     // envelope to attach to the state port
    yarp::os::Semaphore timeMutex;

    yarp::os::PortWriterBuffer<yarp::sig::Vector> state_buffer;
    yarp::os::PortReaderBuffer<CommandMessage> control_buffer;

    yarp::dev::impl::ImplementCallbackHelper callback_impl;

    yarp::dev::impl::CommandsHelper command_reader;

    // for new interface
    yarp::os::PortReaderBuffer<yarp::os::Bottle> command_buffer;

    yarp::sig::Vector            CBW_encoders;
    std::string       partName;

    int               controlledJoints;
    int               base;
    int               top;
    int               thread_period;
    bool              _verb;

    bool closeMain() {
        if (yarp::os::RateThread::isRunning()) {
            yarp::os::RateThread::stop();
        }

        // close the port connections here!
        rpc_p.close();
        control_p.close();
        state_p.close();

        return true;
    }

    yarp::os::Bottle getOptions();

    // Default usage
    // Open the wrapper only, the attach method needs to be called before using it
    bool openDeferredAttach(yarp::os::Property& prop);

    // For the simulator, if a subdevice parameter is given to the wrapper, it will
    // open it and and attach to it immediatly.
    yarp::dev::PolyDriver *subDeviceOwned;
    bool openAndAttachSubDevice(yarp::os::Property& prop);

public:
    /**
    * Constructor.
    */
    ControlBoardWrapper() :yarp::os::RateThread(20), control_buffer(4)
    {
        callback_impl.init(this);
        command_reader.init(this);
        ////YARP_TRACE(Logger::get(),"ControlBoardWrapper2::ControlBoardWrapper2()", Logger::get().log_files.f3);
        controlledJoints = 0;
        thread_period = 20; // ms.
        base = 0;
        top = 0;
        subDeviceOwned = NULL;
        _verb = false;
    }

    virtual ~ControlBoardWrapper() {
        //YARP_TRACE(Logger::get(),"ControlBoardWrapper2::~ControlBoardWrapper2()", Logger::get().log_files.f3);
        closeMain();
    }

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const { return _verb; }

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
        if(subDeviceOwned != NULL)
        {
            subDeviceOwned->close();
            delete subDeviceOwned;
            subDeviceOwned = NULL;
        }
        return closeMain();
    }


    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are:
    * - verbose or v to print diagnostic information while running..
    * - name to specify the prefix of the port names.
    * - subdevice [optional] if specified, the openAndAttachSubDevice will be
    *             called, otherwise openDeferredAttach is called.
    * and all parameters required by the wrapper.
    */
    virtual bool open(yarp::os::Searchable& prop);

    virtual bool detachAll()
    {
        yarp::os::RateThread::stop();

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
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
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
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->pid)
            {
                ret=ret&&p->pid->setPid(off+base, ps[l]);
            }
            else
                ret=false;
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
    virtual bool setReference(int j, double ref)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
//#warning "check for max number of joints!?!?!"
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
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
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    virtual bool positionMove(const double *refs)
    {
        bool ret = true;
        int j_wrap = 0;         // index of the wrapper joint

        int nDev = device.subdevices.size();
        for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
        {
            int subIndex=device.lut[j_wrap].deviceEntry;
            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

            int wrapped_joints=(p->top - p->base) + 1;
            int *joints = new int[wrapped_joints];

            if(!p)
                return false;

            if(p->pos2)   // Position Control 2
            {
                // verione comandi su subset di giunti
                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
                {
                    joints[j_dev] = p->base + j_dev;  // for all joints is equivalent to add offset term
                }

                ret = ret && p->pos2->positionMove(wrapped_joints, joints, &refs[j_wrap]);
                j_wrap+=wrapped_joints;
            }
            else   // Classic Position Control
            {
                if(p->pos)
                {

                    for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                    {
                        int off=device.lut[j_wrap].offset;
                        ret=ret && p->pos->positionMove(p->base+off, refs[j_wrap]);
                    }
                }
                else
                {
                    ret=false;
                }
            }

            if(joints!=0)
            { delete [] joints;
              joints = 0;}
        }

        return ret;
    }

    /** Set new reference point for a subset of axis.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifing the new reference points
     * @return true/false on success/failure
     */
    virtual bool positionMove(const int n_joints, const int *joints, const double *refs)
    {
        bool ret = true;

    /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
     * then it is optimizable by instantiating the table once and for all during the creation of the class.
     * TODO check if concurrency problems are real!!
     */

        int    nDev   = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double   XRefs[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice   *ps[MAX_DEVICES];

       for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }


       // Create a map of joints for each subDevice
       int subIndex = 0;
       for(int j=0; j<n_joints; j++)
       {
           subIndex = device.lut[joints[j]].deviceEntry;
           XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
           XRefs[subIndex][X_idx[subIndex]] = refs[j];
           X_idx[subIndex]++;
       }

       for(subIndex=0; subIndex<nDev; subIndex++)
       {
           if(ps[subIndex]->pos2)   // Position Control 2
           {
               ret= ret && ps[subIndex]->pos2->positionMove(X_idx[subIndex], XJoints[subIndex], XRefs[subIndex]);
           }
           else   // Classic Position Control
           {
               if(ps[subIndex]->pos)
               {
                   for(int i = 0; i < X_idx[subIndex]; i++)
                   {
                       ret=ret && ps[subIndex]->pos->positionMove(XJoints[subIndex][i], XRefs[subIndex][i]);
                   }
               }
               else
               {
                   ret=false;
               }
           }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    /** Set relative position for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param deltas pointer to the array of relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const int n_joints, const int *joints, const double *deltas)
    {
        bool ret = true;

    /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
     * then it is optimizable by instantiating the table once and for all during the creation of the class.
     * TODO check this!!
     */

        int    nDev   = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double   XRefs[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice   *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }


       // Create a map of joints for each subDevice
       int subIndex = 0;
       for(int j=0; j<n_joints; j++)
       {
           subIndex = device.lut[joints[j]].deviceEntry;
           XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
           XRefs[subIndex][X_idx[subIndex]] = deltas[j];
           X_idx[subIndex]++;
       }

       for(subIndex=0; subIndex<nDev; subIndex++)
       {
           if(ps[subIndex]->pos2)   // Position Control 2
           {
               ret= ret && ps[subIndex]->pos2->relativeMove(X_idx[subIndex], XJoints[subIndex], XRefs[subIndex]);
           }
           else   // Classic Position Control
           {
               if(ps[subIndex]->pos)
               {
                   for(int i = 0; i < X_idx[subIndex]; i++)
                   {
                       ret=ret && ps[subIndex]->pos->relativeMove(XJoints[subIndex][i], XRefs[subIndex][i]);
                   }
               }
               else
               {
                   ret=false;
               }
           }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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


    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flags  pointer to the array that will store the actual value of the checkMotionDone
     * @return true/false if network communication went well.
     */
    virtual bool checkMotionDone(const int n_joints, const int *joints, bool *flags)
    {
        bool ret = true;

    /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
     * then it is optimizable by instantiating the table once and for all during the creation of the class.
     * TODO check this!!
     */

        int    nDev   = device.subdevices.size();
        bool   XFlags = true;
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice   *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }


       // Create a map of joints for each subDevice
       int subIndex;
       for(int j=0; j<n_joints; j++)
       {
           subIndex = device.lut[joints[j]].deviceEntry;
           int tmp1= X_idx[subIndex];
           int tmp2 = joints[j];
           XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
           X_idx[subIndex]++;
       }

       for(subIndex=0; subIndex<nDev; subIndex++)
       {
           if(ps[subIndex]->pos2)   // Position Control 2
           {
               ret= ret && ps[subIndex]->pos2->checkMotionDone(X_idx[subIndex], XJoints[subIndex], &XFlags);
                *flags = flags && XFlags;
           }
           else   // Classic Position Control
           {
               if(ps[subIndex]->pos)
               {
                   for(int i = 0; i < X_idx[subIndex]; i++)
                   {
                       ret=ret && ps[subIndex]->pos->checkMotionDone(XJoints[subIndex][i], &XFlags);
                        *flags = flags && XFlags;
                   }
               }
               else
               {
                   ret=false;
               }
           }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    virtual bool setRefSpeeds(const double *spds)
    {
        bool ret = true;
        int j_wrap = 0;         // index of the wrapper joint

        int nDev = device.subdevices.size();
        for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
        {
            yarp::dev::impl::SubDevice *p=device.getSubdevice(subDev_idx);

            if(!p)
                return false;

            int wrapped_joints=(p->top - p->base) + 1;
            int *joints = new int[wrapped_joints];

            if(p->pos2)   // Position Control 2
            {
                // verione comandi su subset di giunti
                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
                {
                    joints[j_dev] = p->base + j_dev;
                }

                p->pos2->setRefSpeeds(wrapped_joints, joints, &spds[j_wrap]);
                j_wrap += wrapped_joints;
            }
            else   // Classic Position Control
            {
                if(p->pos)
                {
                    for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                    {
                        int off=device.lut[j_wrap].offset;
                        ret=ret && p->pos->setRefSpeed(p->base+off, spds[j_wrap]);
                    }
                }
                else
                {
                    ret=false;
                }
            }

            if(joints!=0)
            { delete [] joints;
              joints = 0;}
        }

        return ret;
    }


    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const int n_joints, const int *joints, const double *spds)
    {
        /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
         * then it is optimizable by instantiating the table once and for all during the creation of the class.
         * TODO check this!!
         */

        bool ret = true;
        int    nDev   = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double   XSpeds[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice   *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }


       // Create a map of joints for each subDevice
       int subIndex = 0;
       for(int j=0; j<n_joints; j++)
       {
           subIndex = device.lut[joints[j]].deviceEntry;
           XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
           XSpeds[subIndex][X_idx[subIndex]] = spds[j];
           X_idx[subIndex]++;
       }

       for(subIndex=0; subIndex<nDev; subIndex++)
       {
           if(ps[subIndex]->pos2)   // Position Control 2
           {
               ret= ret && ps[subIndex]->pos2->setRefSpeeds(X_idx[subIndex], XJoints[subIndex], XSpeds[subIndex]);
           }
           else   // Classic Position Control
           {
               if(ps[subIndex]->pos)
               {
                   for(int i = 0; i < X_idx[subIndex]; i++)
                   {
                       ret=ret && ps[subIndex]->pos->setRefSpeed(XJoints[subIndex][i], XSpeds[subIndex][i]);
                   }
               }
               else
               {
                   ret=false;
               }
           }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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
    virtual bool setRefAccelerations(const double *accs)
    {
        bool ret = true;
        int j_wrap = 0;    // index of the joint from the wrapper side (useful if wrapper joins 2 subdevices)

        // for all subdevices
        for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
        {
            yarp::dev::impl::SubDevice *p=device.getSubdevice(subDev_idx);

            if(!p)
                return false;

            int wrapped_joints=(p->top - p->base) + 1;
            int *joints = new int[wrapped_joints];  // to be defined once and for all?

            if(p->pos2)   // Position Control 2
            {
                // verione comandi su subset di giunti
                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
                {
                    joints[j_dev] = p->base + j_dev;
                }

                p->pos2->setRefAccelerations(wrapped_joints, joints, &accs[j_wrap]);
                j_wrap += wrapped_joints;
            }
            else        // Classic Position Control
            {
                if(p->pos)
                {
                    for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                    {
                        int off=device.lut[j_wrap].offset;
                        ret=ret && p->pos->setRefAcceleration(p->base+off, accs[j_wrap]);
                    }
                }
                else
                {
                    ret=false;
                }
            }

            if(joints!=0)
            { delete [] joints;
            joints = 0;}
        }

        return ret;
    }

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array with acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const int n_joints, const int *joints, const double *accs)
    {
        /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
         * then it is optimizable by instantiating the table once and for all during the creation of the class.
         * TODO check this!!
         */

        bool ret = true;
        int    nDev   = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double   XAccs[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice  *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }


       // Create a map of joints for each subDevice
       int subIndex = 0;
       for(int j=0; j<n_joints; j++)
       {
           subIndex = device.lut[joints[j]].deviceEntry;
           XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
           XAccs[subIndex][X_idx[subIndex]] = accs[j];
           X_idx[subIndex]++;
       }

       for(subIndex=0; subIndex<nDev; subIndex++)
       {
           if(ps[subIndex]->pos2)   // Position Control 2
           {
               ret= ret && ps[subIndex]->pos2->setRefAccelerations(X_idx[subIndex], XJoints[subIndex], XAccs[subIndex]);
           }
           else   // Classic Position Control
           {
               if(ps[subIndex]->pos)
               {
                   for(int i = 0; i < X_idx[subIndex]; i++)
                   {
                       ret=ret && ps[subIndex]->pos->setRefAcceleration(XJoints[subIndex][i], XAccs[subIndex][i]);
                   }
               }
               else
               {
                   ret=false;
               }
           }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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


    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array that will store the speed values.
     * @return true/false upon success/failure
     */
    virtual bool getRefSpeeds(const int n_joints, const int *joints, double *spds)
    {
        /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
         * then it is optimizable by instantiating the table once and for all during the creation of the class.
         * TODO check this!!
         */

        bool ret = true;
        int    nDev   = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double  XSpeds[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice  *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }

        // Create a map of joints for each subDevice
       int subIndex = 0;
       for(int j=0; j<n_joints; j++)
       {
           subIndex = device.lut[joints[j]].deviceEntry;
           XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
           X_idx[subIndex]++;
       }

        for(subIndex=0; subIndex<nDev; subIndex++)
        {
            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
            if(ps[subIndex]->pos2)   // Position Control 2
            {
                ret= ret && p->pos2->getRefSpeeds(X_idx[subIndex], XJoints[subIndex], &XSpeds[subIndex][0]);
            }
            else   // Classic Position Control
            {
                if(ps[subIndex]->pos)
                {
                    for(int i = 0; i < X_idx[subIndex]; i++)
                    {
                        ret=ret && ps[subIndex]->pos->getRefSpeed(XJoints[subIndex][i], &XSpeds[subIndex][i]);
                    }
                }
                else
                {
                    ret=false;
                }
            }
        }

        if(ret)
        {
            // ReMix values by user expectations
            for(int i=0; i<nDev; i++)
                X_idx[i]=0;       // reset index

            for(int j=0; j<n_joints; j++)
            {
                subIndex = device.lut[joints[j]].deviceEntry;
                spds[j] = XSpeds[subIndex][X_idx[subIndex]];
                X_idx[subIndex]++;
            }
        }
        else
        {
            for(int j=0; j<n_joints; j++)
            {
                spds[j] = 0;
            }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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


    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values
     * @return true/false on success/failure
     */
    virtual bool getRefAccelerations(const int n_joints, const int *joints, double *accs)
    {
        /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
         * then it is optimizable by instantiating the table once and for all during the creation of the class.
         * TODO check this!!
         */

        bool ret = true;
        int    nDev   = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double  XAccs[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice  *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }

        // Create a map of joints for each subDevice
        int subIndex = 0;
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
            X_idx[subIndex]++;
        }

        for(subIndex=0; subIndex<nDev; subIndex++)
        {
            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
            if(p->pos2)   // Position Control 2
            {
                ret= ret && p->pos2->getRefAccelerations(X_idx[subIndex], XJoints[subIndex], &XAccs[subIndex][0]);
            }
            else   // Classic Position Control
            {
                if(p->pos)
                {
                    for(int i = 0; i < X_idx[subIndex]; i++)
                    {
                        int tmp_jDev = XJoints[subIndex][i];
                        int off=device.lut[tmp_jDev].offset;
                        ret=ret && p->pos->getRefAcceleration(p->base+off, &XAccs[subIndex][i]);
                    }
                }
                else
                {
                    ret=false;
                }
            }
        }

        if(ret)
        {
            // ReMix values by user expectations
            for(int i=0; i<nDev; i++)
                X_idx[i]=0;       // reset index

            subIndex=0;
            for(int j=0; j<n_joints; j++)
            {
                subIndex = device.lut[joints[j]].deviceEntry;
                accs[j] = XAccs[subIndex][X_idx[subIndex]];
                X_idx[subIndex]++;
            }
        }
        else
        {
            for(int j=0; j<n_joints; j++)
            {
                accs[j] = 0;
            }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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


    /** Stop motion for subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success/failure
     */
    virtual bool stop(const int n_joints, const int *joints)
    {
     /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
         * then it is optimizable by instantiating the table once and for all during the creation of the class.
         * TODO check this!!
         */

        bool ret = true;
        int    nDev   = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice  *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
       {
           X_idx[i]=0;
           ps[i]=device.getSubdevice(i);
       }


       // Create a map of joints for each subDevice
       int subIndex = 0;
       for(int j=0; j<n_joints; j++)
       {
           subIndex = device.lut[joints[j]].deviceEntry;
           XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
           X_idx[subIndex]++;
       }

       for(subIndex=0; subIndex<nDev; subIndex++)
       {
           if(ps[subIndex]->pos2)   // Position Control 2
           {
               ret= ret && ps[subIndex]->pos2->stop(X_idx[subIndex], XJoints[subIndex]);
           }
           else   // Classic Position Control
           {
               if(ps[subIndex]->pos)
               {
                   for(int i = 0; i < X_idx[subIndex]; i++)
                   {
                       ret=ret && ps[subIndex]->pos->stop(XJoints[subIndex][i]);
                   }
               }
               else
               {
                   ret=false;
               }
           }
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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
    virtual bool velocityMove(const double *v)
    {
        bool ret = true;
        int j_wrap = 0;         // index of the wrapper joint

        int nDev = device.subdevices.size();
        for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
        {
            yarp::dev::impl::SubDevice *p=device.getSubdevice(subDev_idx);

            if(!p)
                return false;

            int wrapped_joints=(p->top - p->base) + 1;
            int *joints = new int[wrapped_joints];

            if(p->vel2)   // Velocity Control 2
            {
                // verione comandi su subset di giunti
                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
                {
                    joints[j_dev] = p->base + j_dev;
                }

                ret = ret && p->vel2->velocityMove(wrapped_joints, joints, &v[j_wrap]);
                j_wrap += wrapped_joints;
            }
            else   // Classic Position Control
            {
                if(p->vel)
                {
                    for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                    {
                        int off=device.lut[j_wrap].offset;
                        ret=ret && p->vel->velocityMove(p->base+off, v[j_wrap]);
                    }
                }
                else
                {
                    ret=false;
                }
            }

            if(joints!=0)
            { delete [] joints;
              joints = 0;}
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->lim2)
        {
            return p->lim2->setLimits(off+base,min, max);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
        {
            *min=0.0;
            *max=0.0;
            return false;
        }

        if (p->lim2)
        {
            return p->lim2->getLimits(off+base,min, max);
        }
        *min=0.0;
        *max=0.0;
        return false;
    }

    /**
    * Set the software velocity limits for a particular axis, the behavior of the
    * control card when these limits are exceeded, depends on the implementation.
    * @param axis joint number
    * @param min the value of the lower limit
    * @param max the value of the upper limit
    * @return true or false on success or failure
    */
    virtual bool setVelLimits(int j, double min, double max) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (!p->lim2)
        {
            return false;
        }
        return p->lim2->setVelLimits(off+base,min, max);
    }

    /**
    * Get the software velocity limits for a particular axis.
    * @param axis joint number
    * @param min pointer to store the value of the lower limit
    * @param max pointer to store the value of the upper limit
    * @return true if everything goes fine, false if something bad happens
    */
    virtual bool getVelLimits(int j, double *min, double *max) {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        *min=0.0;
        *max=0.0;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
        {
            return false;
        }

        if(!p->lim2)
        {
            return false;
        }
        return p->lim2->getVelLimits(off+base,min, max);
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
    virtual bool calibrate(int j, double p)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p = device.getSubdevice(subIndex);
        if (p && p->calib2)
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->setRefTorque(off+base, t);
        }
        return false;
    }

    virtual bool getBemfParam(int j, double *t)
    {

        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->getBemfParam(off+base, t);
        }
        return false;
    }

    virtual bool setBemfParam(int j, double t)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            return p->iTorque->setBemfParam(off+base, t);
        }
        return false;
    }

    virtual bool setTorquePid(int j, const Pid &pid)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            return p->iMode->getControlMode(off+base, mode);
        }
        return false;
    }

    virtual bool getControlModes(int *modes)
    {
       bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    virtual bool setOutput(int j, double v)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    virtual bool setPosition(int j, double ref)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->posDir)
        {
            return p->posDir->setPosition(off+base, ref);
        }

        return false;
    }

    virtual bool setPositions(const int n_joints, const int *joints, double *dpos)
    {
        bool ret = true;

        /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
         * then it is optimizable by instantiating the table once and for all during the creation of the class.
         * TODO check if concurrency problems are real!!
         */

        int    nDev  = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double   XRefs[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice  *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
        {
            X_idx[i]=0;
            ps[i]=device.getSubdevice(i);
        }


        // Create a map of joints for each subDevice
        int subIndex = 0;
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
            XRefs[subIndex][X_idx[subIndex]] = dpos[j];
            X_idx[subIndex]++;
        }

        for(subIndex=0; subIndex<nDev; subIndex++)
        {
            if(ps[subIndex]->posDir)
            {
                ret= ret && ps[subIndex]->posDir->setPositions(X_idx[subIndex], XJoints[subIndex], XRefs[subIndex]);
            }
            else
            {
                ret=false;
            }
        }
        return ret;
    }

    virtual bool setPositions(const double *refs)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->posDir)
            {
                ret = p->posDir->setPosition(off+base, refs[l]) && ret;
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual yarp::os::Stamp getLastInputStamp() {
        timeMutex.wait();
        yarp::os::Stamp ret=time;
        timeMutex.post();
        return ret;
    }

    //
    // IVelocityControl2 Interface
    //
    virtual bool velocityMove(const int n_joints, const int *joints, const double *spds)
    {
        bool ret = true;

        /* This table is created here each time to avoid concurrency problems... if this shall not be the case,
         * then it is optimizable by instantiating the table once and for all during the creation of the class.
         * TODO check if concurrency problems are real!!
         */

        int    nDev  = device.subdevices.size();
        int    XJoints[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        double   XRefs[MAX_DEVICES][MAX_JOINTS_ON_DEVICE];
        int      X_idx[MAX_DEVICES];
        yarp::dev::impl::SubDevice  *ps[MAX_DEVICES];

        for(int i=0; i<nDev; i++)
        {
            X_idx[i]=0;
            ps[i]=device.getSubdevice(i);
        }


        // Create a map of joints for each subDevice
        int subIndex = 0;
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            XJoints[subIndex][X_idx[subIndex]] = device.lut[joints[j]].offset + ps[subIndex]->base;
            XRefs[subIndex][X_idx[subIndex]] = spds[j];
            X_idx[subIndex]++;
        }

        for(subIndex=0; subIndex<nDev; subIndex++)
        {
            if(ps[subIndex]->vel2)   // Velocity Control 2
            {
                ret= ret && ps[subIndex]->vel2->velocityMove(X_idx[subIndex], XJoints[subIndex], XRefs[subIndex]);
            }
            else   // Classic Velocity Control
            {
                if(ps[subIndex]->vel)
                {
                    for(int i = 0; i < X_idx[subIndex]; i++)
                    {
                        ret=ret && ps[subIndex]->vel->velocityMove(XJoints[subIndex][i], XRefs[subIndex][i]);
                    }
                }
                else
                {
                    ret=false;
                }
            }
        }
        return ret;
    }

    virtual bool setVelPid(int j, const Pid &pid)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
        if (!s)
            return false;

        if (s->vel2)
        {
            return s->vel2->setVelPid(off+base, pid);
        }
        return false;
    }

    virtual bool setVelPids(const Pid *pids)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->vel2)
            {
                ret=ret&&p->vel2->setVelPid(off+base, pids[l]);
            }
            else
                ret=false;
        }
        return ret;
    }

    virtual bool getVelPid(int j, Pid *pid)
    {
        //#warning "check for max number of joints!?!?!"
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
        if (!s)
            return false;

        if (s->vel2)
        {
            return s->vel2->getVelPid(off+base, pid);
        }
        return false;
    }

    virtual bool getVelPids(Pid *pids)
    {
        bool ret=true;

        for(int l=0;l<controlledJoints;l++)
        {
            int off=device.lut[l].offset;
            int subIndex=device.lut[l].deviceEntry;

            yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
            if (!p)
                return false;

            if (p->vel2)
            {
                ret=ret&&p->vel2->getVelPid(off+base, pids+l);
            }
            else
                ret=false;
        }
        return ret;
    }
};

#endif
