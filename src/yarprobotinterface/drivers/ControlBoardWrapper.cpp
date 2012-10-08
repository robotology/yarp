#include "ControlBoardWrapper.h"

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <iostream>

inline void appendTimeStamp(Bottle &bot, Stamp &st)
{
    int count=st.getCount();
    double time=st.getTime();
    bot.addVocab(VOCAB_TIMESTAMP);
    bot.addInt(count);
    bot.addDouble(time);
}

// d for the driver factory.
DriverCreator *createControlBoardWrapper() {
    return new DriverCreatorOf<ControlBoardWrapper>("controlboard",
        "controlboard",
        "ControlBoardWrapper");
}

ImplementCallbackHelper::ImplementCallbackHelper(ControlBoardWrapper *x) {
    pos = dynamic_cast<yarp::dev::IPositionControl *> (x);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (x);
}

void CommandsHelper::handleControlModeMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (!iControlMode)
    {
        *ok=false;
        return;
    }

    //TODO: handle here messages about  IControlMode interface
    int code = cmd.get(1).asVocab();
    *ok=true;

    switch(code)
    {
        case VOCAB_SET:
            {
                //possible contro modes are: VOCAB_CM_TORQUE, VOCAB_CM_POSITION, VOCAB_CM_VELOCITY
                // iControlMode->...
                int p=-1;
                (void)p; // UNUSED
                int axis = cmd.get(3).asInt();
                switch (cmd.get(2).asInt())
                    {
                        case VOCAB_CM_POSITION:
                            p=1;
                            *ok = iControlMode->setPositionMode(axis);
                        break;
                        case VOCAB_CM_VELOCITY:
                            p=2;
                            //*ok = iControlMode->setVelocityMode(axis);
                        break;
                        case VOCAB_CM_TORQUE:
                            p=3;
                            *ok = iControlMode->setTorqueMode(axis);
                        break;
                        case VOCAB_CM_IMPEDANCE_POS:
                            p=4;
                            *ok = iControlMode->setImpedancePositionMode(axis);
                        break;
                        case VOCAB_CM_IMPEDANCE_VEL:
                            p=5;
                            *ok = iControlMode->setImpedanceVelocityMode(axis);
                        break;
                        case VOCAB_CM_OPENLOOP:
                            p=50;
                            *ok = iControlMode->setOpenLoopMode(axis);
                        break;
                        default:
                            p=-1;
                            *ok = false;
                        break;
                    }
                *rec=true; //or false
            }
            break;
        case VOCAB_GET:
            {
                if (cmd.get(2).asVocab()==VOCAB_CM_CONTROL_MODES)
                {
                    int *p = new int[controlledJoints];
                    *ok = iControlMode->getControlModes(p);
                    response.addVocab(VOCAB_IS);
                    response.addVocab(VOCAB_CM_CONTROL_MODES);
                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                    {
                        switch (p[i])
                        {
                            case 0: //IDLE
                                b.addVocab(VOCAB_CM_UNKNOWN);
                            break;
                            case 1:
                                b.addVocab(VOCAB_CM_POSITION);
                            break;
                            case 2:
                                b.addVocab(VOCAB_CM_VELOCITY);
                            break;
                            case 3:
                                b.addVocab(VOCAB_CM_TORQUE);
                            break;
                            case 4:
                                b.addVocab(VOCAB_CM_IMPEDANCE_POS);
                            break;
                            case 5:
                                b.addVocab(VOCAB_CM_IMPEDANCE_VEL);
                            break;
                            default:
                                b.addVocab(VOCAB_CM_UNKNOWN);
                            break;
                        }
                    }
                    delete[] p;
                    *rec=true;
                }

                else if (cmd.get(2).asVocab()==VOCAB_CM_CONTROL_MODE)
                {
                    int p=-1;
                    int axis = cmd.get(3).asInt();
                    *ok = iControlMode->getControlMode(axis, &p);

                    response.addVocab(VOCAB_IS);
                    response.addInt(axis);
                    response.addVocab(VOCAB_CM_CONTROL_MODE);

                    switch (p)
                    {
                        case 0: //IDLE
                            response.addVocab(VOCAB_CM_UNKNOWN);
                        break;
                        case 1:
                            response.addVocab(VOCAB_CM_POSITION);
                        break;
                        case 2:
                            response.addVocab(VOCAB_CM_VELOCITY);
                        break;
                        case 3:
                            response.addVocab(VOCAB_CM_TORQUE);
                        break;
                        case 4:
                            response.addVocab(VOCAB_CM_IMPEDANCE_POS);
                        break;
                        case 5:
                            response.addVocab(VOCAB_CM_IMPEDANCE_VEL);
                        break;
                        default:
                            response.addVocab(VOCAB_CM_UNKNOWN);
                        break;
                    }
                    *rec=true;
                 }
            }
            break;
        default:
            {
                *rec=false;
            }
            break;
    }
}

void CommandsHelper::handleTorqueMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (!torque)
    {
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
                double dtmp = 0.0;
                response.addVocab(VOCAB_IS);
                response.add(cmd.get(1));

                switch(cmd.get(2).asVocab())
                {
                    case VOCAB_AXES:
                        {
                            // int tmp; UNUSED
                            *ok = torque->getAxes(&tmp);
                            response.addInt(tmp);
                        }
                    break;

                    case VOCAB_TRQ:
                        {
                            *ok = torque->getTorque(cmd.get(3).asInt(), &dtmp);
                            response.addDouble(dtmp);
                        }

                    case VOCAB_TRQS:
                        {
                            double *p = new double[controlledJoints];
                            *ok = torque->getTorques(p);
                            Bottle& b = response.addList();
                            int i;
                            for (i = 0; i < controlledJoints; i++)
                                b.addDouble(p[i]);
                            delete[] p;
                        }

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
        break;
    }
    //rec --> true se il comando e' riconosciuto
    //ok --> contiene il return value della chiamata all'interfaccia
    // ...*ok=torque->setPid();
    //torque->
}

void ImplementCallbackHelper::onRead(CommandMessage& v)
{
    //printf("Data received on the control channel of size: %d\n", v.body.size());
    //    int i;

    Bottle& b = v.head;
    //    printf("bottle: %s\n", b.toString().c_str());

    switch (b.get(0).asVocab())
    {
    case VOCAB_POSITION_MODE:
    case VOCAB_POSITION_MOVES:
        {
            //printf("Received a position command\n");
            //for (int i = 0; i < v.body.size(); i++)
            //    printf("%.2f ", v.body[i]);
            //printf("\n");

            if (pos)
            {
                bool ok = pos->positionMove(&(v.body[0]));
                if (!ok)
                    printf("Issues while trying to start a position move\n");
            }

        }
        break;

    case VOCAB_VELOCITY_MODE:
    case VOCAB_VELOCITY_MOVES:
        {
            //            printf("Received a velocity command\n");
            //            for (i = 0; i < v.body.size(); i++)
            //                printf("%.2f ", v.body[i]);
            //            printf("\n");
            if (vel)
            {
                bool ok = vel->velocityMove(&(v.body[0]));
                if (!ok)
                    printf("Issues while trying to start a velocity move\n");
            }
        }
        break;
    default:
        {
            printf("Unrecognized message while receiving on command port\n");
        }
        break;
    }

    //    printf("v: ");
    //    int i <;
    //    for (i = 0; i < (int)v.size(); i++)
    //        printf("%.3f ", v[i]);
    //    printf("\n");
}

bool CommandsHelper::respond(const yarp::os::Bottle& cmd,
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
        // handleTorqueMsg(cmd, response, &rec, &ok);
    }
    else if ((cmd.size()>1) && (cmd.get(1).asVocab()==VOCAB_ICONTROLMODE))
    {
        // handleControlModeMsg(cmd, response, &rec, &ok);
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
                    ok = pid->setPid(j, p);
                }
                break;

            case VOCAB_PIDS:
                {
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
                    if (b==NULL) break;
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
                    if (b==NULL) break;
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
                   // int tmp;
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
                    ok = amp->getAmpStatus(&tmp);
                    response.addInt(tmp);
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
        {}
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

bool CommandsHelper::initialize() {
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
        //Removed to clean dependencies with yarp internal classes, Lorenzo.
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

CommandsHelper::CommandsHelper(ControlBoardWrapper *x) {
    caller = x;
    pid = dynamic_cast<yarp::dev::IPidControl *> (caller);
    pos = dynamic_cast<yarp::dev::IPositionControl *> (caller);
    vel = dynamic_cast<yarp::dev::IVelocityControl *> (caller);
    enc = dynamic_cast<yarp::dev::IEncoders *> (caller);
    amp = dynamic_cast<yarp::dev::IAmplifierControl *> (caller);
    lim = dynamic_cast<yarp::dev::IControlLimits *> (caller);
    info = dynamic_cast<yarp::dev::IAxisInfo *> (caller);
    ical2= dynamic_cast<yarp::dev::IControlCalibration2 *> (caller);
    torque= dynamic_cast<yarp::dev::ITorqueControl *> (caller);
    iControlMode= dynamic_cast<yarp::dev::IControlMode *> (caller);
    controlledJoints = 0;
}

bool ControlBoardWrapper::attachAll(const PolyDriverList &sub)
{
    if (sub.size()!=1)
    {
        std::cerr<<"ControlBoardWrapper: cannot attach more than one device\n";
        return false;
    }

    subdevice=sub[0]->poly;

    if (subdevice->isValid())
    {
        subdevice->view(pid);
        subdevice->view(pos);
        subdevice->view(vel);
        subdevice->view(enc);
        subdevice->view(amp);
        subdevice->view(lim);
        subdevice->view(calib);
        subdevice->view(calib2);
        subdevice->view(info);
        subdevice->view(iTimed);
        subdevice->view(torque);
        subdevice->view(iControlMode);
    }
    else
        return false;

    if (iTimed!=0)
        fprintf(stderr, "Ok, using IPreciselyTimed interface\n");

    if (pos!=0||vel!=0||torque!=0)
    {
        if (pos!=0)
        {
            if (!pos->getAxes(&deviceJoints))
            {
                printf ("Error: attached device has 0 axes\n");
                return false;
            }
        }
        if (vel!=0)
        {
            if (!vel->getAxes(&deviceJoints))
            {
                printf ("Error: attached device has 0 axes\n\n");
                return false;
            }
        }

        //handle default values for base and top
        if (base<0)
        {
            base=0;
        }
        if (top<0)
        {
            top=deviceJoints-1;
        }

        if (top>(deviceJoints-1))
        {
            fprintf(stderr,"check start/end parameters, wrapping more than the actual number of joints in the network\n");
            return false;
        }

        controlledJoints=top-base+1;

        if (controlledJoints<1)
        {
            fprintf(stderr,"check start/end parameters, wrapping less than 1 joints\n");
            return false;
        }

        printf ("Wrapper [%s]: controlling %d out of %d axes\n", partName.c_str(), controlledJoints, deviceJoints);

        encoders.resize(deviceJoints);

        // initialization.
        command_reader.initialize();

        RateThread::setRate(thread_period);
        RateThread::start();
        return true;
    }
    else
    {
        return false;
    }
}

bool ControlBoardWrapper::detachAll()
{
    RateThread::stop();
    return true;
}
