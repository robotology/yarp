// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "ControlBoardWrapper2.h"

#include <iostream>

// _AC_
//#include "../../libraries/icubmod/ethManager/iCubDeviceInterface.h"
//#include "../../libraries/icubmod/ethManager/ethManager.h"

using namespace std;

inline void appendTimeStamp(Bottle &bot, Stamp &st)
{
    int count=st.getCount();
    double time=st.getTime();
    bot.addVocab(VOCAB_TIMESTAMP);
    bot.addInt(count);
    bot.addDouble(time);
}

SubDevice::SubDevice()
{
    pid = 0;
    pos = 0;
    vel = 0;
    enc = 0;
    amp = 0;
    lim = 0;
    calib = 0;
    calib2 = 0;
    iTimed= 0;
    info = 0;
    iOpenLoop=0;
    iDbg = 0;

    iTorque=0;
    iImpedance=0;
    iMode=0;

    base=-1;
    top=-1;
    axes=0;

    subdevice=0;

    configuredF=false;
    attachedF=false;
}

bool SubDevice::configure(int b, int t, int n, const std::string &key)
{
    configuredF=false;

    base=b;
    top=t;
    axes=n;
    id=key;

    if (top<base)
        {
            cerr<<"check configuration file top<base."<<endl;
            return false;
        }

    if ((top-base+1)!=axes)
        {
            cerr<<"check configuration file, number of axes and top/base parameters do not match"<<endl;
            return false;
        }

    if (axes<=0)
        {
            cerr<<"check number of axes"<<endl;
            return false;
        }

    encoders.resize(axes);
    encodersTimes.resize(axes);

    configuredF=true;
    return true;
}

void SubDevice::detach()
{
    subdevice=0;

    pid=0;
    pos=0;
    vel=0;
    enc=0;
    lim=0;
    calib=0;
    calib2=0;
    info=0;
    iTorque=0;
    iImpedance=0;
    iMode=0;
    iTimed=0;
    iOpenLoop=0;
    iDbg = 0;

    configuredF=false;
    attachedF=false;
}

bool SubDevice::attach(yarp::dev::PolyDriver *d, const std::string &k)
{
    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- SubDevice::attach", Logger::get().log_files.f3);
    if (id!=k)
        {
            cerr<<"Wrong device sorry."<<endl;
            return false;
        }

    //configure first
    if (!configuredF)
        {
            cerr<<"You need to call configure before you can attach any device"<<endl;
            return false;
        }

    if (d==0)
        {
            cerr<<"Invalid device (null pointer)\n"<<endl;
            return false;
        }

    subdevice=d;

    if (subdevice->isValid())
        {
            subdevice->view(pid);
            subdevice->view(pos);
            subdevice->view(vel);
            subdevice->view(amp);
            subdevice->view(lim);
            subdevice->view(calib);
            subdevice->view(calib2);
            subdevice->view(info);
            subdevice->view(iTimed);
            subdevice->view(iTorque);
            subdevice->view(iImpedance);
            subdevice->view(iMode);
            subdevice->view(iOpenLoop);
            subdevice->view(iDbg);
            subdevice->view(enc);
        }
    else
        {
            cerr<<"Invalid device (isValid() returned false"<<endl;
            return false;
        }


    if (iTimed!=0)
        std::cout << id << ":using IPreciselyTimed interface"<<endl;

    if (iMode==0)
        DEBUG_CW2("--> Warning iMode not valid interface\n");

    if (iTorque==0)
        DEBUG_CW2("--> Warning iTorque not valid interface\n");

    if (iImpedance==0)
        DEBUG_CW2("--> Warning iImpedance not valid interface\n");

    if (iOpenLoop==0)
        DEBUG_CW2("--> Warning iOpenLoop not valid interface\n");

    if (iDbg==0)
        DEBUG_CW2("--> Warning iDebug not valid interface\n");

    int deviceJoints=0;

    if (pos!=0||vel!=0)
        {
            if (pos!=0)
                {
                    if (!pos->getAxes(&deviceJoints))
                        {
                            std::cerr<< "Error: attached device has 0 axes\n";
                            return false;
                        }
                }
            if (vel!=0)
                {
                    if (!vel->getAxes(&deviceJoints))
                        {
                            std::cerr<< "Error: attached device has 0 axes\n";
                            return false;
                        }
                }

            if (deviceJoints<axes)
                {
                    std::cerr<<"check device configuration, number of joints of attached device less than the one specified during configuration.\n";
                    return false;
                }

            attachedF=true;
            return true;
        }
    else
        {
            return false;
        }

    return false;
}


// d for the driver factory.
DriverCreator *createControlBoardWrapper2() {
    return new DriverCreatorOf<ControlBoardWrapper2>("controlboard",
                                                     "controlboard",
                                                     "ControlBoardWrapper2");
}

ImplementCallbackHelper2::ImplementCallbackHelper2(ControlBoardWrapper2 *x) {
    pos = dynamic_cast<yarp::dev::IPositionControl *> (x);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (x);
    iOpenLoop=dynamic_cast<yarp::dev::IOpenLoopControl *> (x);
}

void CommandsHelper2::handleImpedanceMsg(const yarp::os::Bottle& cmd,
                                           yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (caller->verbose())
        fprintf(stderr, "Handling IImpedance message\n");
     if (!iImpedance)
        {
            fprintf(stderr, "Error I do not have a valid interface\n");
            *ok=false;
            return;
        }

    int code = cmd.get(0).asVocab();
    *ok=false;
    switch(code)
    {
    case VOCAB_SET:
        {
            if (caller->verbose())
                fprintf(stderr, "handleImpedanceMsg::VOCAB_SET command\n");
            switch (cmd.get(2).asVocab())
            {
                case VOCAB_IMP_PARAM:
                    {
                        Bottle& b = *(cmd.get(4).asList());
                        double stiff = b.get(0).asDouble();
                        double damp = b.get(1).asDouble();
                        *ok = iImpedance->setImpedance(cmd.get(3).asInt(),stiff,damp);
                        *rec=true;
                    }
                    break;
                case VOCAB_IMP_OFFSET:
                    {
                        Bottle& b = *(cmd.get(4).asList());
                        double offs = b.get(0).asDouble();
                        *ok = iImpedance->setImpedanceOffset(cmd.get(3).asInt(),offs);
                        *rec=true;
                    }
                    break;
            }
        }
        break;
    case VOCAB_GET:
        {
            double stiff = 0;
            double damp = 0;
            double offs = 0;
            if (caller->verbose())
                fprintf(stderr, "handleImpedanceMsg::VOCAB_GET command\n");

            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));
            switch (cmd.get(2).asVocab())
            {
                case VOCAB_IMP_PARAM:
                    {
                        *ok = iImpedance->getImpedance(cmd.get(3).asInt(),&stiff, &damp);
                        Bottle& b = response.addList();
                        b.addDouble(stiff);
                        b.addDouble(damp);
                        *rec=true;
                    }
                    break;
                case VOCAB_IMP_OFFSET:
                    {
                        *ok = iImpedance->getImpedanceOffset(cmd.get(3).asInt(),&offs);
                        Bottle& b = response.addList();
                        b.addDouble(offs);
                        *rec=true;
                    }
                    break;
                case VOCAB_LIMITS:
                    {
                        double min_stiff    = 0;
                        double max_stiff    = 0;
                        double min_damp     = 0;
                        double max_damp     = 0;
                        *ok = iImpedance->getCurrentImpedanceLimit(cmd.get(3).asInt(),&min_stiff, &max_stiff, &min_damp, &max_damp);
                        Bottle& b = response.addList();
                        b.addDouble(min_stiff);
                        b.addDouble(max_stiff);
                        b.addDouble(min_damp);
                        b.addDouble(max_damp);
                        *rec=true;
                    }
                    break;
            }
        }
        lastRpcStamp.update();
        appendTimeStamp(response, lastRpcStamp);
        break; // case VOCAB_GET
    default:
        {
            *rec=false;
        }
        break;
    }
}

void CommandsHelper2::handleControlModeMsg(const yarp::os::Bottle& cmd,
                                           yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (caller->verbose())
        fprintf(stderr, "Handling IControlMode message\n");
     if (!iMode)
        {
            fprintf(stderr, "Error I do not have a valid interface\n");
            *ok=false;
            return;
        }

    //TODO: handle here messages about  IControlMode interface
    int code = cmd.get(0).asVocab();
    *ok=true;

    switch(code)
    {
        case VOCAB_SET:
            {
                if (caller->verbose())
                    fprintf(stderr, "handleControlModeMsg::VOCAB_SET command\n");
                int p=-1;
                (void)p; // UNUSED

                int axis = cmd.get(3).asInt();
                int mode=cmd.get(2).asVocab();
                (void)mode; // UNUSED

                switch (cmd.get(2).asInt())
                {
                    case VOCAB_CM_POSITION:
                        *ok = iMode->setPositionMode(axis);
                        break;
                    case VOCAB_CM_VELOCITY:
                        *ok = iMode->setVelocityMode(axis);
                        break;
                    case VOCAB_CM_TORQUE:
                        *ok = iMode->setTorqueMode(axis);
                        break;
                    case VOCAB_CM_IMPEDANCE_POS:
                        *ok = iMode->setImpedancePositionMode(axis);
                        break;
                    case VOCAB_CM_IMPEDANCE_VEL:
                        *ok = iMode->setImpedanceVelocityMode(axis);
                        break;
                    case VOCAB_CM_OPENLOOP:
                        *ok = iMode->setOpenLoopMode(axis);
                        break;
                    default:
                        *ok = false;
                        break;
                }
                *rec=true; //or false
            }
            break;
        case VOCAB_GET:
            {
                if (caller->verbose())
                    fprintf(stderr, "GET command\n");
                if (cmd.get(2).asVocab()==VOCAB_CM_CONTROL_MODES)
                {
                    if (caller->verbose())
                        fprintf(stderr, "getControlModes\n");
                    int *p = new int[controlledJoints];
                    *ok = iMode->getControlModes(p);

                    response.addVocab(VOCAB_IS);
                    response.addVocab(VOCAB_CM_CONTROL_MODES);

                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                        b.addVocab(p[i]);
                    delete[] p;

                    *rec=true;
                }
                else if (cmd.get(2).asVocab()==VOCAB_CM_CONTROL_MODE)
                {
                    if (caller->verbose())
                        fprintf(stderr, "getControlMode\n");

                    int p=-1;
                    int axis = cmd.get(3).asInt();
                    *ok = iMode->getControlMode(axis, &p);

                    response.addVocab(VOCAB_IS);
                    response.addInt(axis);
                    response.addVocab(p);

                    //fprintf(stderr, "Returning %d\n", p);
                    *rec=true;
                }
            }
            lastRpcStamp.update();
            appendTimeStamp(response, lastRpcStamp);
            break; // case VOCAB_GET
        default:
            {
                *rec=false;
            }
            break;
    }
}


void CommandsHelper2::handleTorqueMsg(const yarp::os::Bottle& cmd,
                                      yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (caller->verbose())
        fprintf(stderr, "Handling ITorqueControl message\n");

    if (!torque)
        {
            fprintf(stderr, "Error, I do not have a valid ITorque interface\n");
            *ok=false;
            return;
        }

    int code = cmd.get(0).asVocab();
    switch (code)
    {
        case VOCAB_SET:
            {
                *rec = true;
                if (caller->verbose())
                    printf("set command received\n");

                switch(cmd.get(2).asVocab())
                {
                    case VOCAB_REF:
                        {
                            *ok = torque->setRefTorque(cmd.get(3).asInt(), cmd.get(4).asDouble());
                        }
                        break;

                    case VOCAB_REFS:
                        {
                            Bottle& b = *(cmd.get(3).asList());
                            int i;
                            const int njs = b.size();
                            if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b.get(i).asDouble();
                                    *ok = torque->setRefTorques (p);
                                    delete[] p;
                                }
                        }
                        break;

                    case VOCAB_LIM:
                        {
                            *ok = torque->setTorqueErrorLimit (cmd.get(3).asInt(), cmd.get(4).asDouble());
                        }
                        break;

                    case VOCAB_LIMS:
                        {
                            Bottle& b = *(cmd.get(3).asList());
                            int i;
                            const int njs = b.size();
                            if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b.get(i).asDouble();
                                    *ok = torque->setTorqueErrorLimits (p);
                                    delete[] p;
                                }
                        }
                        break;

                    case VOCAB_PID:
                        {
                            Pid p;
                            int j = cmd.get(3).asInt();
                            Bottle& b = *(cmd.get(4).asList());
                            p.kp = b.get(0).asDouble();
                            p.kd = b.get(1).asDouble();
                            p.ki = b.get(2).asDouble();
                            p.max_int = b.get(3).asDouble();
                            p.max_output = b.get(4).asDouble();
                            p.offset = b.get(5).asDouble();
                            p.scale = b.get(6).asDouble();
                            *ok = torque->setTorquePid(j, p);
                        }
                        break;

                    case VOCAB_PIDS:
                        {
                            Bottle& b = *(cmd.get(3).asList());
                            int i;
                            const int njs = b.size();
                            if (njs==controlledJoints)
                                {
                                    Pid *p = new Pid[njs];
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
                                    *ok = torque->setTorquePids(p);
                                    delete[] p;
                                }
                        }
                        break;

                    case VOCAB_RESET:
                        {
                            *ok = torque->resetTorquePid (cmd.get(3).asInt());
                        }
                        break;

                    case VOCAB_DISABLE:
                        {
                            *ok = torque->disableTorquePid (cmd.get(3).asInt());
                        }
                        break;

                    case VOCAB_ENABLE:
                        {
                            *ok = torque->enableTorquePid (cmd.get(3).asInt());
                        }
                        break;

                    case VOCAB_TORQUE_MODE:
                        {
                            *ok = torque->setTorqueMode();
                        }
                        break;

                }
            }
            break;

        case VOCAB_GET:
            {
                *rec = true;
                if (caller->verbose())
                    printf("get command received\n");
                int tmp = 0;
                double dtmp  = 0.0;
                double dtmp2 = 0.0;
                response.addVocab(VOCAB_IS);
                response.add(cmd.get(1));

                switch(cmd.get(2).asVocab())
                {
                    case VOCAB_AXES:
                        {
                            //int tmp; UNUSED (using the wrong one?
                            *ok = torque->getAxes(&tmp);
                            response.addInt(tmp);
                        }
                        break;

                    case VOCAB_TRQ:
                        {
                            *ok = torque->getTorque(cmd.get(3).asInt(), &dtmp);
                            response.addDouble(dtmp);
                        }
                        break;

                    case VOCAB_RANGE:
                        {
                            *ok = torque->getTorqueRange(cmd.get(3).asInt(), &dtmp, &dtmp2);
                            response.addDouble(dtmp);
                            response.addDouble(dtmp2);
                        }
                        break;

                    case VOCAB_TRQS:
                        {
                            int i=0;
                            double *p = new double[controlledJoints];
                            *ok = torque->getTorques(p);
                            Bottle& b = response.addList();
                            for (i = 0; i < controlledJoints; i++)
                                b.addDouble(p[i]);
                            delete[] p;
                        }
                        break;

                    case VOCAB_RANGES:
                        {
                            double *p1 = new double[controlledJoints];
                            double *p2 = new double[controlledJoints];
                            *ok = torque->getTorqueRanges(p1,p2);
                            Bottle& b1 = response.addList();
                            int i;
                            for (i = 0; i < controlledJoints; i++)
                                b1.addDouble(p1[i]);
                            Bottle& b2 = response.addList();
                            for (i = 0; i < controlledJoints; i++)
                                b2.addDouble(p2[i]);
                            delete[] p1;
                            delete[] p2;
                        }
                        break;

                    case VOCAB_ERR:
                        {
                            *ok = torque->getTorqueError(cmd.get(3).asInt(), &dtmp);
                            response.addDouble(dtmp);
                        }
                        break;

                    case VOCAB_ERRS:
                        {
                            double *p = new double[controlledJoints];
                            *ok = torque->getTorqueErrors(p);
                            Bottle& b = response.addList();
                            int i;
                            for (i = 0; i < controlledJoints; i++)
                                b.addDouble(p[i]);
                            delete[] p;
                        }
                        break;

                    case VOCAB_OUTPUT:
                        {
                            *ok = torque->getTorquePidOutput(cmd.get(3).asInt(), &dtmp);
                            response.addDouble(dtmp);
                        }
                        break;

                    case VOCAB_OUTPUTS:
                        {
                            double *p = new double[controlledJoints];
                            *ok = torque->getTorquePidOutputs(p);
                            Bottle& b = response.addList();
                            int i;
                            for (i = 0; i < controlledJoints; i++)
                                b.addDouble(p[i]);
                            delete[] p;
                        }
                        break;

                    case VOCAB_PID:
                        {
                            Pid p;
                            *ok = torque->getTorquePid(cmd.get(3).asInt(), &p);
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

                    case VOCAB_PIDS:
                        {
                            Pid *p = new Pid[controlledJoints];
                            *ok = torque->getTorquePids(p);
                            Bottle& b = response.addList();
                            int i;
                            for (i = 0; i < controlledJoints; i++)
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

                    case VOCAB_REFERENCE:
                        {
                            *ok = torque->getRefTorque(cmd.get(3).asInt(), &dtmp);
                            response.addDouble(dtmp);
                        }
                        break;

                    case VOCAB_REFERENCES:
                        {
                            double *p = new double[controlledJoints];
                            *ok = torque->getRefTorques(p);
                            Bottle& b = response.addList();
                            int i;
                            for (i = 0; i < controlledJoints; i++)
                                b.addDouble(p[i]);
                            delete[] p;
                        }
                        break;

                    case VOCAB_LIM:
                        {
                            *ok = torque->getTorqueErrorLimit(cmd.get(3).asInt(), &dtmp);
                            response.addDouble(dtmp);
                        }
                        break;

                    case VOCAB_LIMS:
                        {
                            double *p = new double[controlledJoints];
                            *ok = torque->getTorqueErrorLimits(p);
                            Bottle& b = response.addList();
                            int i;
                            for (i = 0; i < controlledJoints; i++)
                                b.addDouble(p[i]);
                            delete[] p;
                        }
                        break;

                }
            }
            lastRpcStamp.update();
            appendTimeStamp(response, lastRpcStamp);
            break; // case VOCAB_GET
    }
}


void ImplementCallbackHelper2::onRead(CommandMessage& v)
{
    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- ImplementCallbackHelper2::onRead", Logger::get().log_files.f3);

    //printf("Data received on the control channel of size: %d\n", v.body.size());
    //    int i;

    Bottle& b = v.head;
    //    printf("bottle: %s\n", b.toString().c_str());

    switch (b.get(0).asVocab())
        {
        case VOCAB_POSITION_MODE:
        case VOCAB_POSITION_MOVES:
            {
                // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- ImplementCallbackHelper2::VOCAB_POSITION_MOVES", Logger::get().log_files.f3);
                //printf("Received a position command\n");
                //for (int i = 0; i < v.body.size(); i++)
                //    printf("%.2f ", v.body[i]);
                //printf("\n");

                if (pos)
                    {
                        bool ok = pos->positionMove(&(v.body[0]));
                        if (!ok)
                            fprintf(stderr, "Issues while trying to start a position move\n");
                    }

            }
            break;

        case VOCAB_VELOCITY_MODE:
        case VOCAB_VELOCITY_MOVES:
            {
                // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- ImplementCallbackHelper2::VOCAB_VELOCITY_MOVES", Logger::get().log_files.f3);
                //            printf("Received a velocity command\n");
                //            for (i = 0; i < v.body.size(); i++)
                //                printf("%.2f ", v.body[i]);
                //            printf("\n");
                if (vel)
                    {
                        bool ok = vel->velocityMove(&(v.body[0]));
                        if (!ok)
                            fprintf(stderr, "Issues while trying to start a velocity move\n");
                    }
            }
            break;
        case VOCAB_OUTPUTS:
            {
                // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- ImplementCallbackHelper2::VOCAB_OUTPUTS", Logger::get().log_files.f3);
                if (iOpenLoop)
                    {
                        bool ok=iOpenLoop->setOutputs(&(v.body[0]));
                        if (!ok)
                            fprintf(stderr, "Issues while trying to command an open loop message\n");
                    }
            }
        default:
            {
                // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- ImplementCallbackHelper2::default", Logger::get().log_files.f3);
                fprintf(stderr, "Unrecognized message while receiving on command port\n");
            }
            break;
        }

    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- ImplementCallbackHelper2::end", Logger::get().log_files.f3);

    //    printf("v: ");
    //    int i <;
    //    for (i = 0; i < (int)v.size(); i++)
    //        printf("%.3f ", v[i]);
    //    printf("\n");
}


bool CommandsHelper2::respond(const yarp::os::Bottle& cmd,
                              yarp::os::Bottle& response)
{
    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- CommandsHelper2::respond", Logger::get().log_files.f3);

    //    ACE_thread_t self=ACE_Thread::self();
    //    fprintf(stderr, "--> [%X] starting responder\n",self);

    bool ok = false;
    bool rec = false; // is the command recognized?
    // if (caller->verbose())
        printf("command received: %s\n", cmd.toString().c_str());
    int code = cmd.get(0).asVocab();

    if ((cmd.size()>1) && (cmd.get(1).asVocab()==VOCAB_TORQUE))
    {
        handleTorqueMsg(cmd, response, &rec, &ok);
    }
    else if ((cmd.size()>1) && (cmd.get(1).asVocab()==VOCAB_ICONTROLMODE))
    {
        handleControlModeMsg(cmd, response, &rec, &ok);
    }
    else if ((cmd.size()>1) && (cmd.get(1).asVocab()==VOCAB_IMPEDANCE))
    {
        handleImpedanceMsg(cmd, response, &rec, &ok);
    }
    else
    {
        switch (code)
        {
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
                {
                    rec = true;
                    if (caller->verbose())
                        printf("set command received\n");

                    switch(cmd.get(1).asVocab())
                    {
                        case VOCAB_DEBUG_PARAMETER:
                            {
                                int j     = cmd.get(2).asInt();
                                int index = cmd.get(3).asInt();
                                double val   = cmd.get(4).asDouble();
                                ok = iDbg->setDebugParameter(j, index, val);
                            }
                            break;

                        case VOCAB_DEBUG_DESIRED_POS:
                            {
                                int j     = cmd.get(2).asInt();
                                double val   = cmd.get(3).asDouble();
                                ok = iDbg->setDebugReferencePosition(j, val);
                            }
                            break;

                        case VOCAB_GENERIC_PARAMETER:
                            {
                                int j     = cmd.get(2).asInt();
                                int param = cmd.get(3).asInt();
                                double val   = cmd.get(4).asDouble();
                                ok = iDbg->setParameter(j, param, val);
                            }
                            break;

                        case VOCAB_OUTPUT:
                            {
                                double v;
                                int j = cmd.get(2).asInt();
                                v=cmd.get(3).asDouble();
                                ok = iOpenLoop->setOutput(j, v);
                            }
                            break;

                        case VOCAB_OUTPUTS:
                            {
                                Bottle& b = *(cmd.get(2).asList());
                                int i;
                                const int njs = b.size();
                                if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b.get(i).asDouble();
                                    ok = iOpenLoop->setOutputs(p);
                                    delete[] p;
                                }
                            }
                            break;

                        case VOCAB_OFFSET:
                            {
                                double v;
                                int j = cmd.get(2).asInt();
                                v=cmd.get(3).asDouble();
                                ok = pid->setOffset(j, v);
                            }
                            break;

                        case VOCAB_PID:
                            {
                                // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- CommandsHelper2::respond-> setpid", Logger::get().log_files.f3);

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

                        case VOCAB_PIDS:
                            {
                                // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper -- CommandsHelper2::respond-> setpids", Logger::get().log_files.f3);

                                Bottle& b = *(cmd.get(2).asList());
                                int i;
                                const int njs = b.size();
                                if (njs==controlledJoints)
                                {
                                    Pid *p = new Pid[njs];
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
                                if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b.get(i).asDouble();
                                    ok = pid->setReferences (p);
                                    delete[] p;
                                }
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
                                if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b.get(i).asDouble();
                                    ok = pid->setErrorLimits (p);
                                    delete[] p;
                                }
                            }
                            break;

                        case VOCAB_RESET:
                            {
                                ok = pid->resetPid (cmd.get(2).asInt());
                            }
                            break;

                        case VOCAB_DISABLE:
                            {
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
                                if (b==NULL)
                                    break;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                vect.size(njs);
                                for (i = 0; i < njs; i++)
                                    vect[i] = b->get(i).asDouble();

                                if (pos!=NULL)
                                    ok = pos->positionMove(&vect[0]);
                            }
                            break;

                        // this operation is also available on "command" port
                        case VOCAB_VELOCITY_MOVES:
                            {
                                Bottle *b = cmd.get(2).asList();
                                int i;
                                if (b==NULL)
                                    break;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                vect.size(njs);
                                for (i = 0; i < njs; i++)
                                    vect[i] = b->get(i).asDouble();
                                if (vel!=NULL)
                                    ok = vel->velocityMove(&vect[0]);

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
                                if(njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
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
                                if (njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
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
                                if(njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
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
                                if (njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
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
                            {
                                printf("received an unknown command after a VOCAB_SET\n");
                            }
                            break;
                    } //switch(cmd.get(1).asVocab()
                    break;
                }

            case VOCAB_GET:
                {
                    rec = true;
                    if (caller->verbose())
                        printf("get command received\n");
                    int tmp = 0;
                    (void) tmp; // UNUSED
                    double dtmp = 0.0;
                    response.addVocab(VOCAB_IS);
                    response.add(cmd.get(1));

                    switch(cmd.get(1).asVocab())
                    {
                        case VOCAB_DEBUG_PARAMETER:
                            {
                                int j     = cmd.get(2).asInt();
                                int index = cmd.get(3).asInt();
                                ok = iDbg->getDebugParameter(j, index, &dtmp);
                                response.addInt(j);
                                response.addInt(index);
                                response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_DESIRED_POS:
                            {
                                int j     = cmd.get(2).asInt();
                                ok = iDbg->getDebugReferencePosition(j, &dtmp);
                                response.addInt(j);
                                response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_ROTOR_POS:
                            {
                                int j     = cmd.get(2).asInt();
                                ok = iDbg->getRotorPosition(j, &dtmp);
                                response.addInt(j);
                                response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_ROTOR_POSS:
                            {
                                //ok = iDbg->getRotorPositions(&dtmp);
                                //response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_ROTOR_SPEED:
                            {
                                int j     = cmd.get(2).asInt();
                                ok = iDbg->getRotorSpeed(j, &dtmp);
                                response.addInt(j);
                                response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_ROTOR_SPEEDS:
                            {
                                //ok = iDbg->getRotorSpeeds(&dtmp);
                                //response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_ROTOR_ACCEL:
                            {
                                int j     = cmd.get(2).asInt();
                                ok = iDbg->getRotorAcceleration(j, &dtmp);
                                response.addInt(j);
                                response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_ROTOR_ACCELS:
                            {
                                //ok = iDbg->getRotorSpeeds(&dtmp);
                                //response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_JOINT_POS:
                            {
                                int j     = cmd.get(2).asInt();
                                ok = iDbg->getJointPosition(j, &dtmp);
                                response.addInt(j);
                                response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_DEBUG_JOINT_POSS:
                            {
                                //ok = iDbg->getJointPositions(&dtmp);
                                //response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_GENERIC_PARAMETER:
                            {
                                int j     = cmd.get(2).asInt();
                                int param = cmd.get(3).asInt();
                                ok = iDbg->getParameter(j, param, &dtmp);
                                response.addInt(j);
                                response.addInt(param);
                                response.addDouble(dtmp);
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
                                double *p = new double[controlledJoints];
                                ok = pid->getErrors(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                                double *p = new double[controlledJoints];
                                ok = pid->getOutputs(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                            }
                            break;

                        case VOCAB_PIDS:
                            {
                                Pid *p = new Pid[controlledJoints];
                                ok = pid->getPids(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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

                        case VOCAB_REFERENCE:
                            {
                                ok = pid->getReference(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                        case VOCAB_REFERENCES:
                            {
                                double *p = new double[controlledJoints];
                                ok = pid->getReferences(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                                double *p = new double[controlledJoints];
                                ok = pid->getErrorLimits(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                        case VOCAB_AXES:
                            {
                                int tmp;
                                ok = pos->getAxes(&tmp);
                                response.addInt(tmp);
                            }
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
                                double *p = new double[controlledJoints];
                                ok = pos->getRefSpeeds(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                                double *p = new double[controlledJoints];
                                ok = pos->getRefAccelerations(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                                double *p = new double[controlledJoints];
                                ok = enc->getEncoders(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                                double *p = new double[controlledJoints];
                                ok = enc->getEncoderSpeeds(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                                double *p = new double[controlledJoints];
                                ok = enc->getEncoderAccelerations(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
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
                                double *p = new double[controlledJoints];
                                ok = amp->getCurrents(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                        case VOCAB_AMP_STATUS:
                            {
                                int *p = new int[controlledJoints];
                                ok = amp->getAmpStatus(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addInt(p[i]);
                                delete[] p;
                            }
                            break;

                        case VOCAB_AMP_STATUS_SINGLE:
                            {
                                int j=cmd.get(2).asInt();
                                int itmp;
                                ok=amp->getAmpStatus(j, &itmp);
                                response.addInt(itmp);
                            }

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
                            {
                                printf("received an unknown request after a VOCAB_GET\n");
                            }
                            break;
                    } //switch cmd.get(1).asVocab())

                    lastRpcStamp.update();
                    appendTimeStamp(response, lastRpcStamp);
                } // case VOCAB_GET
            default:
                break;
        } //switch code

        if (!rec)
        {
            ok = DeviceResponder::respond(cmd,response);
        }
    }

    if (!ok)
    {
        // failed thus send only a VOCAB back.
        response.clear();
        response.addVocab(VOCAB_FAILED);
    }
    else
        response.addVocab(VOCAB_OK);

    // fprintf(stderr, "--> [%X] done ret %d\n",self, ok);
    return ok;
}


bool CommandsHelper2::initialize() {
    // AC_YARP_INFO(Logger::get(),"CommandsHelper2::initialize() ", Logger::get().log_files.f3);
    bool ok = false;
    if (pos)
        {
            ok = pos->getAxes(&controlledJoints);
        }

    DeviceResponder::makeUsage();
    addUsage("[get] [axes]", "get the number of axes");
    addUsage("[get] [name] $iAxisNumber", "get a human-readable name for an axis, if available");
    addUsage("[set] [pos] $iAxisNumber $fPosition", "command the position of an axis");
    addUsage("[set] [rel] $iAxisNumber $fPosition", "command the relative position of an axis");
    addUsage("[set] [vmo] $iAxisNumber $fVelocity", "command the velocity of an axis");
    addUsage("[get] [enc] $iAxisNumber", "get the encoder value for an axis");

    std::string args;
    for (int i=0; i<controlledJoints; i++) {
        if (i>0) {
            args += " ";
        }
        // removed dependency from yarp internals
        //args = args + "$f" + yarp::NetType::toString(i);
    }
    addUsage((std::string("[set] [poss] (")+args+")").c_str(),
             "command the position of all axes");
    addUsage((std::string("[set] [rels] (")+args+")").c_str(),
             "command the relative position of all axes");
    addUsage((std::string("[set] [vmos] (")+args+")").c_str(),
             "command the velocity of all axes");

    addUsage("[set] [aen] $iAxisNumber", "enable (amplifier for) the given axis");
    addUsage("[set] [adi] $iAxisNumber", "disable (amplifier for) the given axis");
    addUsage("[get] [acu] $iAxisNumber", "get current for the given axis");
    addUsage("[get] [acus]", "get current for all axes");

    return ok;
}

CommandsHelper2::CommandsHelper2(ControlBoardWrapper2 *x) {
    // AC_YARP_INFO(Logger::get(),"CommandsHelper2::CommandsHelper2(ControlBoardWrapper2 *x)", Logger::get().log_files.f3);
    caller = x;
    pid = dynamic_cast<yarp::dev::IPidControl *> (caller);
    pos = dynamic_cast<yarp::dev::IPositionControl *> (caller);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (caller);
    enc = dynamic_cast<yarp::dev::IEncodersTimed *> (caller);
    amp = dynamic_cast<yarp::dev::IAmplifierControl *> (caller);
    lim = dynamic_cast<yarp::dev::IControlLimits *> (caller);
    info = dynamic_cast<yarp::dev::IAxisInfo *> (caller);
    ical2= dynamic_cast<yarp::dev::IControlCalibration2 *> (caller);
    iOpenLoop=dynamic_cast<yarp::dev::IOpenLoopControl *> (caller);
    iDbg=dynamic_cast<yarp::dev::IDebugInterface *> (caller);
    iImpedance=dynamic_cast<yarp::dev::IImpedanceControl *> (caller);
    torque=dynamic_cast<yarp::dev::ITorqueControl *> (caller);
    iMode=dynamic_cast<yarp::dev::IControlMode *> (caller);
    controlledJoints = 0;
}



bool ControlBoardWrapper2::open(Searchable& prop)
{
    //debug
    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::open()", Logger::get().log_files.f3);

    string str=prop.toString().c_str();

    cout << str << endl << endl;

    verb = (prop.check("verbose","if present, give detailed output"));
    if (verb)
        cout<<"running with verbose output\n";

    thread_period = prop.check("threadrate", 20, "thread rate in ms. for streaming encoder data").asInt();

    int totalJ=0;

    std::cout<<"Using ControlBoardWrapper2\n";

    if (!prop.check("networks", "list of networks merged by this wrapper"))
        return false;

    Bottle *nets=prop.find("networks").asList();

    if (!prop.check("joints", "number of joints of the part"))
        return false;

    controlledJoints=prop.find("joints").asInt();

    int nsubdevices=nets->size();
    device.lut.resize(controlledJoints);
    device.subdevices.resize(nsubdevices);

    for(int k=0;k<nets->size();k++)
        {
            Bottle parameters=prop.findGroup(nets->get(k).asString().c_str());

            if (parameters.size()!=5)    // mapping joints using the paradigm: part from - to / network from - to
                {
                    cerr<<"Error: check network parameters in part description"<<endl;
                    cerr<<"--> I was expecting "<<nets->get(k).asString().c_str() << " followed by four integers"<<endl;
                    return false;
                }

            int wBase=parameters.get(1).asInt();
            int wTop=parameters.get(2).asInt();
            base=parameters.get(3).asInt();
            top=parameters.get(4).asInt();

            //cout<<"--> "<<wBase<<" "<<wTop<<" "<<base<<" "<<top<<endl;

            //TODO check consistenty
            int axes=top-base+1;

            SubDevice *tmpDevice=device.getSubdevice(k);

            if (!tmpDevice->configure(base, top, axes, nets->get(k).asString().c_str()))
                {
                    cerr<<"configure of subdevice ret false"<<endl;
                    return false;
                }

            for(int j=wBase;j<=wTop;j++)
                {
                    device.lut[j].deviceEntry=k;
                    device.lut[j].offset=j-wBase;
                }

            totalJ+=axes;
        }

    if (totalJ!=controlledJoints)
        {
            cerr<<"Error total number of mapped joints does not correspond to part joints"<<endl;
            return false;
        }

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

bool ControlBoardWrapper2::attachAll(const PolyDriverList &polylist)
{
    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::attachAll()", Logger::get().log_files.f3);
    for(int p=0;p<polylist.size();p++)
        {
            // find appropriate entry in list of subdevices
            // and attach
            unsigned int k=0;
            for(k=0; k<device.subdevices.size(); k++)
                {
                    std::string tmpKey=polylist[p]->key.c_str();
                    if (device.subdevices[k].id==tmpKey)
                        {
                            if (!device.subdevices[k].attach(polylist[p]->poly, tmpKey))
                                return false;
                        }
                }
        }

    //check if all devices are attached to the driver
    bool ready=true;
    for(unsigned int k=0; k<device.subdevices.size(); k++)
        {
            if (!device.subdevices[k].isAttached())
                {
                    ready=false;
                }
        }

    if (!ready)
        return false;

    encoders.resize(device.lut.size());

    // initialization.
    command_reader.initialize();

    RateThread::setRate(thread_period);
    RateThread::start();

    return true;
}

void ControlBoardWrapper2::run()
{
    String tmp(partName.c_str());
    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::run() - " + tmp, Logger::get().log_files.f3);

    yarp::sig::Vector& v = state_buffer.get();
    v.size(controlledJoints);

    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::run2()", Logger::get().log_files.f3);

    //getEncoders for all subdevices
    double *encoders=v.data();
    double timeStamp=0.0;

    for(unsigned int k=0;k<device.subdevices.size();k++)
        {
        // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::run3()", Logger::get().log_files.f3);
            int axes=device.subdevices[k].axes;
            int base=device.subdevices[k].base;

            device.subdevices[k].refreshEncoders();

            // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::run3.5()", Logger::get().log_files.f3);
            for(int l=0;l<axes;l++)
            {
                encoders[l]=device.subdevices[k].encoders[l+base];
                timeStamp+=device.subdevices[k].encodersTimes[l+base];
            }

            encoders+=device.subdevices[k].axes; //jump to next group
        }

    timeMutex.wait();
    time.update(timeStamp/controlledJoints);
    timeMutex.post();

    state_p.setEnvelope(time);
    state_buffer.write();
    // AC_YARP_INFO(Logger::get(),"ControlBoardWrapper2::run5() - end loop", Logger::get().log_files.f3);
}

