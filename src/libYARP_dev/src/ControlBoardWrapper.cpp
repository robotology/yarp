// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/dev/ControlBoardWrapper.h>

#include <iostream>

using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createControlBoardWrapper() {
    return new DriverCreatorOf<ControlBoardWrapper>("controlboardwrapper2",
        "controlboardwrapper2",
        "ControlBoardWrapper2");
}



inline void appendTimeStamp(Bottle &bot, Stamp &st)
{
    int count=st.getCount();
    double time=st.getTime();
    bot.addVocab(VOCAB_TIMESTAMP);
    bot.addInt(count);
    bot.addDouble(time);
}

namespace CBW2_yarp_internal_namespace {
CBW_SubDevice::CBW_SubDevice()
{
    pid = 0;
    pos = 0;
    pos2 = 0;
    posDir = 0;
    vel = 0;
    vel2 = 0;
    enc = 0;
    amp = 0;
    lim2 = 0;
    calib = 0;
    calib2 = 0;
    iTimed= 0;
    info = 0;
    iOpenLoop=0;
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

bool CBW_SubDevice::configure(int b, int t, int n, const std::string &key)
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

    subDev_encoders.resize(axes);
    encodersTimes.resize(axes);

    configuredF=true;
    return true;
}

void CBW_SubDevice::detach()
{
    subdevice=0;

    pid=0;
    pos=0;
    pos2=0;
    posDir=0;
    vel=0;
    vel2=0;
    enc=0;
    lim2=0;
    calib=0;
    calib2=0;
    info=0;
    iTorque=0;
    iImpedance=0;
    iMode=0;
    iTimed=0;
    iOpenLoop=0;
    configuredF=false;
    attachedF=false;
}

bool CBW_SubDevice::attach(yarp::dev::PolyDriver *d, const std::string &k)
{
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
            subdevice->view(pos2);
            subdevice->view(posDir);
            subdevice->view(vel);
            subdevice->view(vel2);
            subdevice->view(amp);
            subdevice->view(lim2);
            subdevice->view(calib);
            subdevice->view(calib2);
            subdevice->view(info);
            subdevice->view(iTimed);
            subdevice->view(iTorque);
            subdevice->view(iImpedance);
            subdevice->view(iMode);
            subdevice->view(iOpenLoop);
            subdevice->view(enc);
        }
    else
        {
            cerr<<"Invalid device " << k << " (isValid() returned false)"<<endl;
            return false;
        }


    if ((iTimed!=0) && (_verb))
        std::cout << id << ":using IPreciselyTimed interface"<<endl;

    if ((iMode==0) && (_verb))
        std::cerr << "--> Warning iMode not valid interface\n";

    if ((iTorque==0) && (_verb))
        std::cerr << "--> Warning iTorque not valid interface\n";

    if ((iImpedance==0) && (_verb))
        std::cerr << "--> Warning iImpedance not valid interface\n";

    if ((iOpenLoop==0) && (_verb))
        std::cerr << "--> Warning iOpenLoop not valid interface\n";

    int deviceJoints=0;

    if ( pos!=0 || pos2!=0 || vel!=0)
        {
            if (pos!=0)
                {
                    if (!pos->getAxes(&deviceJoints))
                        {
                            std::cerr<< "Error: attached device has 0 axes\n";
                            return false;
                        }
                }

            if (pos2!=0)
                {
                    if (!pos2->getAxes(&deviceJoints))
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

}

ImplementCallbackHelper2::ImplementCallbackHelper2(ControlBoardWrapper *x) {
    controlledAxes = 0;
    pos = dynamic_cast<yarp::dev::IPositionControl *> (x);
    pos2 = dynamic_cast<yarp::dev::IPositionControl2 *> (x);
    posDir = dynamic_cast<yarp::dev::IPositionDirect *> (x);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (x);
    vel2 = dynamic_cast<yarp::dev::IVelocityControl2 *> (x);
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
                        Bottle *b = cmd.get(4).asList();
                        if (b!=NULL)
                        {
                            double stiff = b->get(0).asDouble();
                            double damp = b->get(1).asDouble();
                            *ok = iImpedance->setImpedance(cmd.get(3).asInt(),stiff,damp);
                            *rec=true;
                        }
                    }
                    break;
                case VOCAB_IMP_OFFSET:
                    {
                        Bottle *b = cmd.get(4).asList();
                        if (b!=NULL)
                        {
                            double offs = b->get(0).asDouble();
                            *ok = iImpedance->setImpedanceOffset(cmd.get(3).asInt(),offs);
                            *rec=true;
                        }
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
                int axis = cmd.get(3).asInt();
                int mode=cmd.get(2).asVocab();
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
                            Bottle *b = cmd.get(3).asList();
                            if (b==NULL)
                                break;

                            int i;
                            const int njs = b->size();
                            if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b->get(i).asDouble();
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
                            Bottle *b = cmd.get(3).asList();
                            if (b==NULL)
                                break;
                            int i;
                            const int njs = b->size();
                            if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b->get(i).asDouble();
                                    *ok = torque->setTorqueErrorLimits (p);
                                    delete[] p;
                                }
                        }
                        break;

                    case VOCAB_PID:
                        {
                            Pid p;
                            int j = cmd.get(3).asInt();
                            Bottle *b = cmd.get(4).asList();

                            if (b==NULL)
                                break;

                            p.kp = b->get(0).asDouble();
                            p.kd = b->get(1).asDouble();
                            p.ki = b->get(2).asDouble();
                            p.max_int = b->get(3).asDouble();
                            p.max_output = b->get(4).asDouble();
                            p.offset = b->get(5).asDouble();
                            p.scale = b->get(6).asDouble();
                            p.stiction_up_val = b->get(7).asDouble();
                            p.stiction_down_val = b->get(8).asDouble();
                            *ok = torque->setTorquePid(j, p);
                        }
                        break;

                    case VOCAB_PIDS:
                        {
                            Bottle *b = cmd.get(3).asList();
                            if (b==NULL)
                                break;

                            int i;
                            const int njs = b->size();
                            if (njs==controlledJoints)
                                {
                                    bool allOK=true;

                                    Pid *p = new Pid[njs];
                                    for (i = 0; i < njs; i++)
                                        {
                                            Bottle *c = b->get(i).asList();
                                            if (c!=NULL)
                                            {
                                                p[i].kp = c->get(0).asDouble();
                                                p[i].kd = c->get(1).asDouble();
                                                p[i].ki = c->get(2).asDouble();
                                                p[i].max_int = c->get(3).asDouble();
                                                p[i].max_output = c->get(4).asDouble();
                                                p[i].offset = c->get(5).asDouble();
                                                p[i].scale = c->get(6).asDouble();
                                                p[i].stiction_up_val = c->get(7).asDouble();
                                                p[i].stiction_down_val = c->get(8).asDouble();
                                            }
                                            else
                                            {
                                                allOK=false;
                                            }
                                        }
                                    if (allOK)
                                        *ok = torque->setTorquePids(p);
                                    else 
                                        *ok=false;

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
                            int tmp;
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
                            b.addDouble(p.stiction_up_val);
                            b.addDouble(p.stiction_down_val);
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
                                c.addDouble(p[i].stiction_up_val);
                                c.addDouble(p[i].stiction_down_val);
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

bool ImplementCallbackHelper2::initialize()
{
    controlledAxes=0;
    if (pos)
        pos->getAxes(&controlledAxes);
    
    return true;
}

void ImplementCallbackHelper2::onRead(CommandMessage& v)
{
    Bottle& b = v.head;
    Vector& cmdVector = v.body;

    //Use the following only for debug, since it can heavily slow down the system
    //fprintf(stderr, "Received command %s, %s\n", b.toString().c_str(), cmdVector.toString().c_str());

    // some consistency checks
    if ((int)cmdVector.size() > controlledAxes)
    {
        yarp::os::ConstString str = yarp::os::Vocab::decode(b.get(0).asVocab());
        fprintf(stderr, "Received command vector with number of elements bigger than axis controlled by this wrapper (cmd: %s requested jnts: %d received jnts: %d)\n",str.c_str(),controlledAxes,(int)cmdVector.size());
        return;
    }
    if (cmdVector.data()==0)
    {
         fprintf(stderr, "Errors: received null command vector\n");
         return;
    }

    switch (b.get(0).asVocab())
        {
        case VOCAB_POSITION_MODE:
            {
                fprintf(stderr, "Warning: received VOCAB_POSITION_MODE this is an send invalid message on streaming port\n");
                break;
            }
        case VOCAB_POSITION_MOVES:
            {
                //printf("Received a position command\n");
                //for (int i = 0; i < v.body.size(); i++)
                //    printf("%.2f ", v.body[i]);
                //printf("\n");

                if (pos)
                    {
                        bool ok = pos->positionMove(cmdVector.data());
                        if (!ok)
                            fprintf(stderr, "Errors while trying to start a position move\n");
                    }

            }
            break;

        case VOCAB_VELOCITY_MODE:
             {
                fprintf(stderr, "Warning: received VOCAB_VELOCITY_MODE this is an send invalid message on streaming port\n");
                break;
            }
        case VOCAB_VELOCITY_MOVES:
            {
                //            printf("Received a velocity command\n");
                //            for (i = 0; i < v.body.size(); i++)
                //                printf("%.2f ", v.body[i]);
                //            printf("\n");
                if (vel)
                    {
                        bool ok = vel->velocityMove(cmdVector.data());
                        if (!ok)
                            fprintf(stderr, "Errors while trying to start a velocity move\n");
                    }
            }
            break;
        case VOCAB_OUTPUTS:
            {
                if (iOpenLoop)
                    {
                        bool ok=iOpenLoop->setOutputs(cmdVector.data());
                        if (!ok)
                            fprintf(stderr, "Errors while trying to command an open loop message\n");
                    }
            }
            break;
        case VOCAB_POSITION_DIRECT:
        {
            if(posDir)
            {
                int temp_j = b.get(1).asInt();
                double temp_val = cmdVector.operator [](0);
                bool ok = posDir->setPosition(b.get(1).asInt(), cmdVector.operator [](0)); // cmdVector.data());
                if (!ok)
                {   fprintf(stderr, "Errors while trying to command an streaming position direct message on joint %d\n", b.get(1).asInt() ); }
            }
        }
        break;
        case VOCAB_POSITION_DIRECT_GROUP:
        {

            if(posDir)
            {
                int n_joints = b.get(1).asInt();
                Bottle *jlut = b.get(2).asList();
                if( (jlut->size() != n_joints) && (cmdVector.size() != n_joints) )
                {
                    fprintf(stderr, "Received VOCAB_POSITION_DIRECT_GROUP size of joints vector or positions vector does not match the selected joint number\n" );

                }

                int *joint_list = new int[n_joints];
                for (int i = 0; i < n_joints; i++)
                    joint_list[i] = jlut->get(i).asInt();


                bool ok = posDir->setPositions(n_joints, joint_list, cmdVector.data());
                if (!ok)
                {   fprintf(stderr, "Error while trying to command a streaming position direct message on joint group\n" ); }

                delete[] joint_list;
            }
        }
        break;

        case VOCAB_POSITION_DIRECTS:
        {
            if(posDir)
            {
                bool ok = posDir->setPositions(cmdVector.data());
                if (!ok)
                {   fprintf(stderr, "Error while trying to command a streaming position direct message on all joints\n" ); }
            }
        }
        break;
        case VOCAB_VELOCITY_MOVE_GROUP:
        {
            if(vel2)
            {
                int n_joints = b.get(1).asInt();
                Bottle *jlut = b.get(2).asList();
                if( (jlut->size() != n_joints) && (cmdVector.size() != n_joints) )
                    fprintf(stderr, "Received VOCAB_VELOCITY_MOVE_GROUP size of joints vector or positions vector does not match the selected joint number\n" );

                int *joint_list = new int[n_joints];
                for (int i = 0; i < n_joints; i++)
                    joint_list[i] = jlut->get(i).asInt();

                bool ok = vel2->velocityMove(n_joints, joint_list, cmdVector.data());
                if (!ok)
                {   fprintf(stderr, "Error while trying to command a streaming position direct message on joint group\n" ); }

                delete[] joint_list;
            }
        }
        break;

        default:
            {
                yarp::os::ConstString str = yarp::os::Vocab::decode(b.get(0).asVocab());
                fprintf(stderr, "Unrecognized message while receiving on command port (%s)\n",str.c_str());
            }
            break;
        }
}

bool CommandsHelper2::respond(const yarp::os::Bottle& cmd,
                              yarp::os::Bottle& response)
{

    //    ACE_thread_t self=ACE_Thread::self();
    //    fprintf(stderr, "--> [%X] starting responder\n",self);

    bool ok = false;
    bool rec = false; // is the command recognized?
     if (caller->verbose())
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
                                Bottle *b = cmd.get(2).asList();
                                if (b==NULL)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b->get(i).asDouble();
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
                                Pid p;
                                int j = cmd.get(2).asInt();
                                Bottle *b = cmd.get(3).asList();

                                if (b==NULL)
                                    break;

                                p.kp = b->get(0).asDouble();
                                p.kd = b->get(1).asDouble();
                                p.ki = b->get(2).asDouble();
                                p.max_int = b->get(3).asDouble();
                                p.max_output = b->get(4).asDouble();
                                p.offset = b->get(5).asDouble();
                                p.scale = b->get(6).asDouble();
                                p.stiction_up_val = b->get(7).asDouble();
                                p.stiction_down_val = b->get(8).asDouble();
                                ok = pid->setPid(j, p);
                            }
                            break;

                        case VOCAB_PIDS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==NULL)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs==controlledJoints)
                                {
                                    Pid *p = new Pid[njs];

                                    bool allOK=true;

                                    for (i = 0; i < njs; i++)
                                    {
                                        Bottle *c = b->get(i).asList();

                                        if (c!=NULL)
                                        {
                                            p[i].kp = c->get(0).asDouble();
                                            p[i].kd = c->get(1).asDouble();
                                            p[i].ki = c->get(2).asDouble();
                                            p[i].max_int = c->get(3).asDouble();
                                            p[i].max_output = c->get(4).asDouble();
                                            p[i].offset = c->get(5).asDouble();
                                            p[i].scale = c->get(6).asDouble();
                                            p[i].stiction_up_val = c->get(7).asDouble();
                                            p[i].stiction_down_val = c->get(8).asDouble();
                                        }
                                        else
                                        {
                                            allOK=false;
                                        }
                                    }
                                    if (allOK)
                                        ok = pid->setPids(p);
                                    else
                                        ok=false;

                                    delete[] p;
                                }
                            }
                            break;

                        case VOCAB_VEL_PID:
                        {
                            Pid p;
                            int j = cmd.get(2).asInt();
                            Bottle *b = cmd.get(3).asList();

                            if (b==NULL)
                                break;

                            p.kp = b->get(0).asDouble();
                            p.kd = b->get(1).asDouble();
                            p.ki = b->get(2).asDouble();
                            p.max_int = b->get(3).asDouble();
                            p.max_output = b->get(4).asDouble();
                            p.offset = b->get(5).asDouble();
                            p.scale = b->get(6).asDouble();
                            p.stiction_up_val = b->get(7).asDouble();
                            p.stiction_down_val = b->get(8).asDouble();
                            ok = vel2->setVelPid(j, p);
                        }

                        case VOCAB_VEL_PIDS:
                        {
                            Bottle *b = cmd.get(2).asList();

                            if (b==NULL)
                                break;

                            int i;
                            const int njs = b->size();
                            if (njs==controlledJoints)
                            {
                                Pid *p = new Pid[njs];

                                bool allOK=true;

                                for (i = 0; i < njs; i++)
                                {
                                    Bottle *c = b->get(i).asList();

                                    if (c!=NULL)
                                    {
                                        p[i].kp = c->get(0).asDouble();
                                        p[i].kd = c->get(1).asDouble();
                                        p[i].ki = c->get(2).asDouble();
                                        p[i].max_int = c->get(3).asDouble();
                                        p[i].max_output = c->get(4).asDouble();
                                        p[i].offset = c->get(5).asDouble();
                                        p[i].scale = c->get(6).asDouble();
                                        p[i].stiction_up_val = c->get(7).asDouble();
                                        p[i].stiction_down_val = c->get(8).asDouble();
                                    }
                                    else
                                    {
                                        allOK=false;
                                    }
                                }
                                if (allOK)
                                    ok = vel2->setVelPids(p);
                                else
                                    ok=false;

                                delete[] p;
                            }
                        }

                        case VOCAB_REF:
                            {
                                ok = pid->setReference (cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                        case VOCAB_REFS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==NULL)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b->get(i).asDouble();
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
                                Bottle *b = cmd.get(2).asList();
                                int i;

                                if (b==NULL)
                                    break;

                                const int njs = b->size();
                                if (njs==controlledJoints)
                                {
                                    double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                    for (i = 0; i < njs; i++)
                                        p[i] = b->get(i).asDouble();
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
                                vect.resize(njs);
                                for (i = 0; i < njs; i++)
                                    vect[i] = b->get(i).asDouble();

                                if (pos!=NULL)
                                    ok = pos->positionMove(&vect[0]);
                            }
                            break;

                        case VOCAB_POSITION_MOVE_GROUP:
                        {
                           int len = cmd.get(2).asInt();
                            Bottle *jlut = cmd.get(3).asList();
                            Bottle *pos_val= cmd.get(4).asList();

                            if (pos2 == NULL)
                                break;

                            if (jlut==NULL || pos_val==NULL)
                                break;
                            if (len!=jlut->size() || len!=pos_val->size())
                                    break;

                                int *j_tmp=new int [len];
                                double *pos_tmp=new double [len];

                                for (int i = 0; i < len; i++)
                                    j_tmp[i] = jlut->get(i).asInt();

                                for (int i = 0; i < len; i++)
                                    pos_tmp[i] = pos_val->get(i).asDouble();

                                ok = pos2->positionMove(len, j_tmp, pos_tmp);

                                delete [] j_tmp;
                                delete [] pos_tmp;
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
                                vect.resize(njs);
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

                        case VOCAB_RELATIVE_MOVE_GROUP:
                        {
                            int len = cmd.get(2).asInt();
                            Bottle *jBottle_p = cmd.get(3).asList();
                            Bottle *posBottle_p= cmd.get(4).asList();

                            if (pos2 == NULL)
                                break;

                            if (jBottle_p==NULL || posBottle_p==NULL)
                                break;
                            if (len!=jBottle_p->size() || len!=posBottle_p->size())
                                break;

                            int *j_tmp=new int [len];
                            double *pos_tmp=new double [len];

                            for (int i = 0; i < len; i++)
                                j_tmp[i] = jBottle_p->get(i).asInt();

                            for (int i = 0; i < len; i++)
                                pos_tmp[i] = posBottle_p->get(i).asDouble();

                            ok = pos2->relativeMove(len, j_tmp, pos_tmp);

                            delete [] j_tmp;
                            delete [] pos_tmp;
                        }
                        break;

                        case VOCAB_RELATIVE_MOVES:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==NULL)
                                    break;

                                int i;
                                const int njs = b->size();
                                if(njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = pos->relativeMove(p);
                                delete[] p;
                            }
                            break;

                        case VOCAB_REF_SPEED:
                            {
                                ok = pos->setRefSpeed(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                        case VOCAB_REF_SPEED_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle *jBottle_p = cmd.get(3).asList();
                                Bottle *velBottle_p= cmd.get(4).asList();

                                if (pos2 == NULL)
                                    break;

                                if (jBottle_p==NULL || velBottle_p==NULL)
                                    break;
                                if (len!=jBottle_p->size() || len!=velBottle_p->size())
                                    break;

                                int *j_tmp=new int [len];
                                double *spds_tmp=new double [len];

                                for (int i = 0; i < len; i++)
                                    j_tmp[i] = jBottle_p->get(i).asInt();

                                for (int i = 0; i < len; i++)
                                    spds_tmp[i] = velBottle_p->get(i).asDouble();

                                ok = pos2->setRefSpeeds(len, j_tmp, spds_tmp);
                                delete[] j_tmp, spds_tmp;
                            }
                            break;

                        case VOCAB_REF_SPEEDS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==NULL)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = pos->setRefSpeeds(p);
                                delete[] p;
                            }
                            break;

                        case VOCAB_REF_ACCELERATION:
                            {
                                ok = pos->setRefAcceleration(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                        case VOCAB_REF_ACCELERATION_GROUP:
                        {
                            int len = cmd.get(2).asInt();
                            Bottle *jBottle_p = cmd.get(3).asList();
                            Bottle *accBottle_p = cmd.get(4).asList();

                            if (pos2 == NULL)
                                break;

                            if (jBottle_p==NULL || accBottle_p==NULL)
                                break;
                            if (len!=jBottle_p->size() || len!=accBottle_p->size())
                                break;

                            int *j_tmp = new int [len];
                            double *accs_tmp = new double [len];

                            for (int i = 0; i < len; i++)
                                j_tmp[i] = jBottle_p->get(i).asInt();

                            for (int i = 0; i < len; i++)
                                accs_tmp[i] = accBottle_p->get(i).asDouble();

                            ok = pos2->setRefAccelerations(len, j_tmp, accs_tmp);
                            delete[] j_tmp, accs_tmp;
                        }
                        break;

                        case VOCAB_REF_ACCELERATIONS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==NULL)
                                    break;

                                int i;
                                const int njs = b->size();
                                if(njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = pos->setRefAccelerations(p);
                                delete[] p;
                            }
                            break;

                        case VOCAB_STOP:
                            {
                                ok = pos->stop(cmd.get(2).asInt());
                            }
                            break;

                        case VOCAB_STOP_GROUP:
                        {
                            int len = cmd.get(2).asInt();
                            Bottle *jBottle_p = cmd.get(3).asList();

                            if (pos2 == NULL)
                                break;

                            if (jBottle_p==NULL)
                                break;
                            if (len!=jBottle_p->size())
                                break;

                            int *j_tmp = new int [len];
                            double *accs_tmp = new double [len];

                            for (int i = 0; i < len; i++)
                                j_tmp[i] = jBottle_p->get(i).asInt();

                            ok = pos2->stop(len, j_tmp);
                            delete[] j_tmp;
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
                                Bottle *b = cmd.get(2).asList();

                                if (b==NULL)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
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
                                ok = lim2->setLimits(cmd.get(2).asInt(), cmd.get(3).asDouble(), cmd.get(4).asDouble());
                            }
                            break;


                        case VOCAB_VEL_LIMITS:
                            {
                                ok = lim2->setVelLimits(cmd.get(2).asInt(), cmd.get(3).asDouble(), cmd.get(4).asDouble());
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
                    double dtmp = 0.0;
                    response.addVocab(VOCAB_IS);
                    response.add(cmd.get(1));

                    switch(cmd.get(1).asVocab())
                    {
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
                                b.addDouble(p.stiction_up_val);
                                b.addDouble(p.stiction_down_val);
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
                                        c.addDouble(p[i].stiction_up_val);
                                        c.addDouble(p[i].stiction_down_val);
                                    }
                                delete[] p;
                            }
                            break;

                        case VOCAB_VEL_PID:
                        {
                            Pid p;
                            ok = vel2->getVelPid(cmd.get(2).asInt(), &p);
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

                        case VOCAB_VEL_PIDS:
                        {
                            Pid *p = new Pid[controlledJoints];
                            ok = vel2->getVelPids(p);
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
                                c.addDouble(p[i].stiction_up_val);
                                c.addDouble(p[i].stiction_down_val);
                            }
                            delete[] p;
                        }

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

/*                        case VOCAB_MOTION_DONE_GROUP:
                            {
                                bool x = false;
                                int len = cmd.get(2).asInt();
                                Bottle& in = *(cmd.get(3).asList());
                                int *jointList = new int[len];
                                for(int j=0; j<len; j++)
                                {
                                    jointList[j] = in.get(j).asInt();
                                }
                                if(pos2!=NULL)
                                    ok = pos2->checkMotionDone(len, jointList, &x);
                                response.addInt(x);

                                delete[] jointList;
                            }
                            break;
*/
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

                        case VOCAB_REF_SPEED_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle& in = *(cmd.get(3).asList());
                                int *jointList = new int[len];
                                double *speeds = new double[len];

                                for(int j=0; j<len; j++)
                                {
                                    jointList[j] = in.get(j).asInt();
                                }
                                ok = pos2->getRefSpeeds(len, jointList, speeds);

                                Bottle& b = response.addList();
                                for (int i = 0; i < len; i++)
                                    b.addDouble(speeds[i]);

                                delete[] jointList, speeds;
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

                        case VOCAB_REF_ACCELERATION_GROUP:
                        {
                            int len = cmd.get(2).asInt();
                            Bottle& in = *(cmd.get(3).asList());
                            int *jointList = new int[len];
                            double *accs = new double[len];

                            for(int j=0; j<len; j++)
                            {
                                jointList[j] = in.get(j).asInt();
                            }
                            ok = pos2->getRefAccelerations(len, jointList, accs);

                            Bottle& b = response.addList();
                            for (int i = 0; i < len; i++)
                                b.addDouble(accs[i]);

                            delete[] jointList, accs;
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
                            break;

                        case VOCAB_LIMITS:
                            {
                                double min = 0.0, max = 0.0;
                                ok = lim2->getLimits(cmd.get(2).asInt(), &min, &max);
                                response.addDouble(min);
                                response.addDouble(max);
                            }
                            break;

                        case VOCAB_VEL_LIMITS:
                            {
                                double min = 0.0, max = 0.0;
                                ok = lim2->getVelLimits(cmd.get(2).asInt(), &min, &max);
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


bool CommandsHelper2::initialize()
{
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

CommandsHelper2::CommandsHelper2(ControlBoardWrapper *x)
{
    caller = x;
    pid = dynamic_cast<yarp::dev::IPidControl *> (caller);
    pos = dynamic_cast<yarp::dev::IPositionControl *> (caller);
    pos2 = dynamic_cast<yarp::dev::IPositionControl2 *> (caller);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (caller);
    vel2 = dynamic_cast<yarp::dev::IVelocityControl2 *> (caller);
    enc = dynamic_cast<yarp::dev::IEncodersTimed *> (caller);
    amp = dynamic_cast<yarp::dev::IAmplifierControl *> (caller);
    lim2 = dynamic_cast<yarp::dev::IControlLimits2 *> (caller);
    info = dynamic_cast<yarp::dev::IAxisInfo *> (caller);
    ical2= dynamic_cast<yarp::dev::IControlCalibration2 *> (caller);
    iOpenLoop=dynamic_cast<yarp::dev::IOpenLoopControl *> (caller);
    iImpedance=dynamic_cast<yarp::dev::IImpedanceControl *> (caller);
    torque=dynamic_cast<yarp::dev::ITorqueControl *> (caller);
    iMode=dynamic_cast<yarp::dev::IControlMode *> (caller);
    controlledJoints = 0;
}


Bottle ControlBoardWrapper::getOptions()
{
    Bottle options;
    options.addString("robotName, mandatory");
    options.addString("deviceId, mandatory");
    options.addString("period");
    return options;
}

bool ControlBoardWrapper::open(Searchable& config)
{
    string str=config.toString().c_str();
    Property prop;
    prop.fromString(config.toString().c_str());

    _verb = (prop.check("verbose","if present, give detailed output"));
    if (_verb)
        cout<<"running with verbose output\n";

    thread_period = prop.check("threadrate", 20, "thread rate in ms. for streaming encoder data").asInt();

    std::cout<<"Using ControlBoardWrapper2\n";
    if(prop.check("subdevice"))
    {
        std::cout << "\nFound <" << prop.find("subdevice").asString() << "> subdevice, opening it\n";
        if(! openAndAttachSubDevice(prop))
        {
            printf("Error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        if(!openDeferredAttach(prop))
            return false;
    }

    /* const values MAX_JOINTS_ON_DEVICE and MAX_DEVICES are used while parsing group joints commands like
        virtual bool positionMove(const int n_joints, const int *joints, const double *refs)
        into ControlBoardWrapper2.h file to build a static table to prevent cocurrencu problem.
        It will suffice to build a table for the rpc port and another one the for streaming port and use the
        correct one because it is the only source of concurrency inside the object. To be done?
    */
    if(controlledJoints > MAX_JOINTS_ON_DEVICE)
    {
        cerr << " ERROR: number of subdevices for this wrapper (" << controlledJoints << ") is bigger than maximum currently handled ("  << MAX_JOINTS_ON_DEVICE << ").";
        cerr << " To help fixing this error, please send an email to robotcub-hackers@lists.sourceforge.net with this error message (ControlBoardWrapper2.cpp @ line " << __LINE__ << endl;
        return false;
    }

    // initialize callback
    if (!callback_impl.initialize())
    {
        cerr<<"Error could not initialize callback object"<<endl;
        return false;
    }

    std::string rootName = prop.check("rootName",Value("/"), "starting '/' if needed.").asString().c_str();
    partName=prop.check("name",Value("controlboard"), "prefix for port names").asString().c_str();

    // cout << " rootName " << rootName << " partName " << partName;

    rootName+=(partName);

    // attach readers.
    // rpc_p.setReader(command_reader);
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


// Default usage
// Open the wrapper only, the attach method needs to be called before using it
bool ControlBoardWrapper::openDeferredAttach(Property& prop)
{
    if (!prop.check("networks", "list of networks merged by this wrapper"))
        return false;

    Bottle *nets=prop.find("networks").asList();
    if(nets==0)
    {
       cerr<<"Error parsing parameters: \"networks\" should be followed by a list\n";
       return false;
    }

    if (!prop.check("joints", "number of joints of the part"))
        return false;

    controlledJoints=prop.find("joints").asInt();

    /*  const values MAX_JOINTS_ON_DEVICE and MAX_DEVICES are used while parsing group joints commands like
        virtual bool positionMove(const int n_joints, const int *joints, const double *refs)
        into ControlBoardWrapper2.h file to build a static table to prevent cocurrencu problem.
        It will suffice to build a table for the rpc port and another one the for streaming port and use the
        correct one because it is the only source of concurrency inside the object. To be done?
     */
    int nsubdevices=nets->size();

    if(nsubdevices > MAX_DEVICES)
    {
        cerr << " ERROR: number of subdevices for this wrapper (" << nsubdevices << ") is bigger than maximum currently handled ("  << MAX_DEVICES << ").";
        cerr << " To help fixing this error, please send an email to robotcub-hackers@lists.sourceforge.net with this error message (ControlBoardWrapper2.cpp @ line " << __LINE__ << endl;
        return false;
    }

    device.lut.resize(controlledJoints);
    device.subdevices.resize(nsubdevices);

    // configure the devices
    int totalJ=0;
    for(int k=0;k<nets->size();k++)
    {
        Bottle parameters;
        int wBase;
        int wTop;

        parameters=prop.findGroup(nets->get(k).asString().c_str());

        // cout<<"Net is "<< nets->get(k).asString().c_str()<<"\n";
        //cout<<parameters.toString().c_str();

        if(parameters.size()==2)
        {
            Bottle *bot=parameters.get(1).asList();
            Bottle tmpBot;
            if(bot==NULL)
            {
                // probably data are not passed in the correct way, try to read them as a string.
                ConstString bString(parameters.get(1).asString());
                tmpBot.fromString(bString);

                if(tmpBot.size() != 4)
                {
                    cerr << "Error: check network parameters in part description" << endl;
                    cerr << "--> I was expecting "<<nets->get(k).asString().c_str() << " followed by a list of four integers in parenthesis" << endl;
                    cerr << "Got: "<< parameters.toString().c_str() << "\n";
                    return false;
                }
                else
                {
                    bot = &tmpBot;
                }
            }

            // If I came here, bot is correct
            wBase=bot->get(0).asInt();
            wTop=bot->get(1).asInt();
            base=bot->get(2).asInt();
            top=bot->get(3).asInt();
        }
        else if (parameters.size()==5)
        {
            // cout<<"Parameter networks use deprecated syntax\n";
            wBase=parameters.get(1).asInt();
            wTop=parameters.get(2).asInt();
            base=parameters.get(3).asInt();
            top=parameters.get(4).asInt();
        }
        else
        {
            cerr<<"Error: check network parameters in part description"<<endl;
            cerr<<"--> I was expecting "<<nets->get(k).asString().c_str() << " followed by a list of four integers in parenthesis"<<endl;
            cerr<<"Got: "<< parameters.toString().c_str() << "\n";                 
            return false;
        }

        CBW_SubDevice *tmpDevice=device.getSubdevice(k);

        int axes=top-base+1;
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
        cerr<<"Error total number of mapped joints ("<< totalJ <<") does not correspond to part joints (" << controlledJoints << ")" << endl;
        return false;
    }

    prop.put("rootName", "/");
    return true;
}

// For the simulator, if a subdevice parameter is given to the wrapper, it will
// open it and and attach to immediatly.
bool ControlBoardWrapper::openAndAttachSubDevice(Property& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString().c_str());

    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring


    p.unput("device");
    p.put("device",prop.find("subdevice").asString());

    // if error occour during open, quit here.
    printf("opening controlBoardWrapper2 subdevice\n");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        printf("opening controlBoardWrapper2 subdevice... FAILED\n");
        return false;
    }

    printf("opening controlBoardWrapper2 subdevice... done\n");

    // getting parameters in simStyle
    if (!p.check("GENERAL","section for general motor control parameters"))
    {
        fprintf(stderr, "Cannot understand configuration parameters\n");
        return false;
    }

    controlledJoints = p.findGroup("GENERAL").check("TotalJoints",Value(1), "Number of total joints").asInt();
    device.lut.resize(controlledJoints);
    device.subdevices.resize(1);

    printf("Attaching controlBoardWrapper2 to subdevice\n");

    // configure the device
    base = 0;
    top  = controlledJoints-1;

    CBW_SubDevice *tmpDevice=device.getSubdevice(0);

    std::string subDevName ((partName + "_" + prop.find("subdevice").asString().c_str()));
    if (!tmpDevice->configure(base, top, controlledJoints, subDevName) )
    {
        cerr<<"configure of subdevice ret false"<<endl;
        return false;
    }

    for(int j=0; j<controlledJoints; j++)
    {
        device.lut[j].deviceEntry = 0;
        device.lut[j].offset = j;
    }


    if (!device.subdevices[0].attach(subDeviceOwned, subDevName))
        return false;

    CBW_encoders.resize(device.lut.size());

    // initialization.
    command_reader.initialize();

    RateThread::setRate(thread_period);
    RateThread::start();

    prop.put("rootName", "");
    return true;
}


bool ControlBoardWrapper::attachAll(const PolyDriverList &polylist)
{
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

    CBW_encoders.resize(device.lut.size());

    // initialization.
    command_reader.initialize();

    RateThread::setRate(thread_period);
    RateThread::start();

    return true;
}

void ControlBoardWrapper::run()
{
    std::string tmp(partName.c_str());

    yarp::sig::Vector& v = state_buffer.get();
    v.size(controlledJoints);

    //getEncoders for all subdevices
    double *encoders=v.data();
    double timeStamp=0.0;

    for(unsigned int k=0;k<device.subdevices.size();k++)
        {
            int axes=device.subdevices[k].axes;
            int base=device.subdevices[k].base;

            device.subdevices[k].refreshEncoders();

            for(int l=0;l<axes;l++)
            {
                encoders[l]=device.subdevices[k].subDev_encoders[l];
                timeStamp+=device.subdevices[k].encodersTimes[l];
            }
            encoders+=device.subdevices[k].axes; //jump to next group
        }

    timeMutex.wait();
    time.update(timeStamp/controlledJoints);
    timeMutex.post();

    state_p.setEnvelope(time);
    state_buffer.write();
}

