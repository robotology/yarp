/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RPCMessagesParser.h"
#include "ControlBoardWrapper.h"

#include <iostream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;
using namespace std;


inline void appendTimeStamp(Bottle &bot, Stamp &st)
{
    int count=st.getCount();
    double time=st.getTime();
    bot.addVocab(VOCAB_TIMESTAMP);
    bot.addInt(count);
    bot.addDouble(time);
}

void RPCMessagesParser::handleProtocolVersionRequest(const yarp::os::Bottle& cmd,
                                           yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (cmd.get(0).asVocab()!=VOCAB_GET)
    {
        *rec=false;
        *ok=false;
        return;
    }

    response.addVocab(VOCAB_PROTOCOL_VERSION);
    response.addInt(PROTOCOL_VERSION_MAJOR);
    response.addInt(PROTOCOL_VERSION_MINOR);
    response.addInt(PROTOCOL_VERSION_TWEAK);

    *rec=true;
    *ok=true;
}

void RPCMessagesParser::handleImpedanceMsg(const yarp::os::Bottle& cmd,
                                           yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("Handling IImpedance message\n");
    if (!rpc_IImpedance)
    {
        yError("controlBoardWrapper: I do not have a valid interface");
        *ok=false;
        return;
    }

    int code = cmd.get(0).asVocab();
    *ok=false;
    switch(code)
    {
        case VOCAB_SET:
        {
            if (ControlBoardWrapper_p->verbose())
                yDebug("handleImpedanceMsg::VOCAB_SET command\n");
            switch (cmd.get(2).asVocab())
            {
                case VOCAB_IMP_PARAM:
                {
                    Bottle *b = cmd.get(4).asList();
                    if (b!=nullptr)
                    {
                        double stiff = b->get(0).asDouble();
                        double damp = b->get(1).asDouble();
                        *ok = rpc_IImpedance->setImpedance(cmd.get(3).asInt(),stiff,damp);
                        *rec=true;
                    }
                }
                break;
                case VOCAB_IMP_OFFSET:
                {
                    Bottle *b = cmd.get(4).asList();
                    if (b!=nullptr)
                    {
                        double offs = b->get(0).asDouble();
                        *ok = rpc_IImpedance->setImpedanceOffset(cmd.get(3).asInt(),offs);
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
            if (ControlBoardWrapper_p->verbose())
                yDebug("handleImpedanceMsg::VOCAB_GET command\n");

            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));
            switch (cmd.get(2).asVocab())
            {
                case VOCAB_IMP_PARAM:
                {
                    *ok = rpc_IImpedance->getImpedance(cmd.get(3).asInt(),&stiff, &damp);
                    Bottle& b = response.addList();
                    b.addDouble(stiff);
                    b.addDouble(damp);
                    *rec=true;
                }
                break;
                case VOCAB_IMP_OFFSET:
                {
                    *ok = rpc_IImpedance->getImpedanceOffset(cmd.get(3).asInt(),&offs);
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
                    *ok = rpc_IImpedance->getCurrentImpedanceLimit(cmd.get(3).asInt(),&min_stiff, &max_stiff, &min_damp, &max_damp);
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

void RPCMessagesParser::handleControlModeMsg(const yarp::os::Bottle& cmd,
                                             yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("Handling IControlMode message\n");
    if (! (rpc_iCtrlMode || rpc_iCtrlMode2) )
    {
        yError("ControlBoardWrapper: I do not have a valid iControlMode interface");
        *ok=false;
        return;
    }

    //handle here messages about  IControlMode interface
    int code = cmd.get(0).asVocab();
    *ok=true;
    *rec=true; //or false

    switch(code)
    {
        case VOCAB_SET:
        {
            if (ControlBoardWrapper_p->verbose())
                yDebug("handleControlModeMsg::VOCAB_SET command\n");

            int method = cmd.get(2).asVocab();

            switch(method)
            {
                case VOCAB_CM_CONTROL_MODE:
                {
                    int axis = cmd.get(3).asInt();
//                  yDebug() << "got VOCAB_CM_CONTROL_MODE " << std::endl;
                    if(rpc_iCtrlMode2)
                        *ok = rpc_iCtrlMode2->setControlMode(axis, cmd.get(4).asVocab());
                    else
                    {
                        yError() << "ControlBoardWrapper: Unable to handle setControlMode request! This should not happen!";
                        *rec = false;
                    }
                }
                break;

                case VOCAB_CM_CONTROL_MODE_GROUP:
                {
                    int n_joints = cmd.get(3).asInt();
                    Bottle& jList = *(cmd.get(4).asList());
                    Bottle& modeList= *(cmd.get(5).asList());

                    int *js = new int [n_joints];
                    int *modes = new int [n_joints];

                    for(int i=0; i<n_joints; i++)
                    {
                        js[i] = jList.get(i).asInt();
                    }

                    for(int i=0; i<n_joints; i++)
                    {
                        modes[i] = modeList.get(i).asVocab();
                    }
                    if(rpc_iCtrlMode2)
                        *ok = rpc_iCtrlMode2->setControlModes(n_joints, js, modes);
                    else
                    {
                        *rec = false;
                        *ok = false;
                    }
                    delete [] js;
                    delete [] modes;
                }
                break;

                case VOCAB_CM_CONTROL_MODES:
                {
                    yarp::os::Bottle *modeList;
                    modeList  = cmd.get(3).asList();

                    if(modeList->size() != controlledJoints)
                    {
                        yError("received an invalid setControlMode message. Size of vector doesn´t match the number of controlled joints\n");
                        *ok = false;
                        break;
                    }
                    int *modes  = new int [controlledJoints];
                    for( int i=0; i<controlledJoints; i++)
                    {
                        modes[i] = modeList->get(i).asVocab();
                    }
                    if(rpc_iCtrlMode2)
                        *ok = rpc_iCtrlMode2->setControlModes(modes);
                    else
                    {
                        *rec = false;
                        *ok = false;
                    }
                    delete [] modes;
                }
                break;

                default:
                {
                    // if I´m here, someone is probably sending command using the old interface.
                    // try to be compatible as much as I can

                    yError()  << " Error, received a set control mode message using a legacy version, trying to be handle the message anyway "
                              << " but please update your client to be compatible with the IControlMode2 interface";

                    // yDebug << " cmd.get(4).asVocab() is " << Vocab::decode(cmd.get(4).asVocab()).c_str() << std::endl;
                    int axis = cmd.get(3).asInt();

                    switch (cmd.get(4).asVocab())
                    {
                        case VOCAB_CM_POSITION:
                            if(rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_POSITION);
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
                            else
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
                                *ok = rpc_iCtrlMode->setPositionMode(axis);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
                        break;

                        case VOCAB_CM_POSITION_DIRECT:
                            if(rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_POSITION_DIRECT);
                            else
                            {
                                *rec = false;
                                *ok = false;
                            }
                        break;


                        case VOCAB_CM_VELOCITY:
                            if(rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_VELOCITY);
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
                            else
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
                                *ok = rpc_iCtrlMode->setVelocityMode(axis);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
                        break;

                        case VOCAB_CM_TORQUE:
                            if(rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_TORQUE);
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
                            else
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
                                *ok = rpc_iCtrlMode->setTorqueMode(axis);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
                        break;

                        case VOCAB_CM_IMPEDANCE_POS:
                            yError() << "The 'impedancePosition' control mode is deprecated. \nUse setInteractionMode(axis, VOCAB_IM_COMPLIANT) + setControlMode(axis, VOCAB_CM_POSITION) instead";

                            //                      Let´s propagate the legacy version as is until it will be removed
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
                            if(rpc_iCtrlMode)
                                *ok = rpc_iCtrlMode->setImpedancePositionMode(axis);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
                        break;

                        case VOCAB_CM_IMPEDANCE_VEL:
                            yError() << "The 'impedanceVelocity' control mode is deprecated. \nUse setInteractionMode(axis, VOCAB_IM_COMPLIANT) + setControlMode(axis, VOCAB_CM_VELOCITY) instead";

                            //                      Let´s propagate the legacy version as is until it will be removed
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
                            if(rpc_iCtrlMode)
                                *ok = rpc_iCtrlMode->setImpedanceVelocityMode(axis);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
                        break;

                        case VOCAB_CM_PWM:
                            if (rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_PWM);
                            else
                            {
                                *rec = false;
                                *ok = false;
                            }
                            break;

                        case VOCAB_CM_CURRENT:
                            if (rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_CURRENT);
                            else
                            {
                                *rec = false;
                                *ok = false;
                            }
                            break;

                        case VOCAB_CM_MIXED:
                            if(rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_MIXED);
                            else
                            {
                                *rec = false;
                                *ok = false;
                            }
                        break;

                        case VOCAB_CM_FORCE_IDLE:
                            if(rpc_iCtrlMode2)
                                *ok = rpc_iCtrlMode2->setControlMode(axis, VOCAB_CM_FORCE_IDLE);
                            else
                            {
                                *rec = false;
                                *ok = false;
                            }
                        break;

                        default:
                            //                        if (ControlBoardWrapper_p->verbose())
                            yError("SET unknown controlMode : %s \n", cmd.toString().c_str());
                            *ok = false;
                            *rec = false;
                        break;
                    }
                }
                break;  // close default case
            }
        }
        break;      // close SET case

        case VOCAB_GET:
        {
            if (ControlBoardWrapper_p->verbose())
                yDebug("GET command\n");

            int method = cmd.get(2).asVocab();

            switch(method)
            {
                case VOCAB_CM_CONTROL_MODES:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("getControlModes\n");
                    int *p = new int[controlledJoints];
                    for (int i = 0; i < controlledJoints; ++i) {
                        p[i] = -1;
                    }
                    if(rpc_iCtrlMode)
                        *ok = rpc_iCtrlMode->getControlModes(p);

                    response.addVocab(VOCAB_IS);
                    response.addVocab(VOCAB_CM_CONTROL_MODES);

                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                        b.addVocab(p[i]);
                    delete[] p;

                    *rec=true;
                }
                break;

                case VOCAB_CM_CONTROL_MODE:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("getControlMode\n");

                    int p=-1;
                    int axis = cmd.get(3).asInt();
                    if(rpc_iCtrlMode)
                        *ok = rpc_iCtrlMode->getControlMode(axis, &p);

                    response.addVocab(VOCAB_IS);
                    response.addInt(axis);
                    response.addVocab(p);

                    //yError("Returning %d\n", p);
                    *rec=true;
                }
                break;

                case VOCAB_CM_CONTROL_MODE_GROUP:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("getControlMode group\n");

                    int n_joints = cmd.get(3).asInt();
                    Bottle& lIn = *(cmd.get(4).asList());

                    int *js = new int [n_joints];
                    int *modes = new int [n_joints];
                    for(int i=0; i<n_joints; i++)
                    {
                        js[i] = lIn.get(i).asInt();
                        modes[i] = -1;
                    }
                    if(rpc_iCtrlMode2)
                        *ok = rpc_iCtrlMode2->getControlModes(n_joints, js, modes);
                    else
                    {
                        *rec = false;
                        *ok = false;
                    }

                    response.addVocab(VOCAB_IS);
                    response.addVocab(VOCAB_CM_CONTROL_MODE_GROUP);
                    Bottle& b = response.addList();
                    for(int i=0; i<n_joints; i++)
                    {
                        b.addVocab(modes[i]);
                    }

                    delete[] js;
                    delete[] modes;

                    //yDebugf("Returning %d\n", p);
                    *rec=true;
                }
                break;

                default:
                    yError("received a GET ICONTROLMODE command not understood");
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


void RPCMessagesParser::handleTorqueMsg(const yarp::os::Bottle& cmd,
                                        yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("Handling ITorqueControl message\n");

    if (!rpc_ITorque)
    {
        yError("Error, I do not have a valid ITorque interface");
        *ok=false;
        return;
    }

    int code = cmd.get(0).asVocab();
    switch (code)
    {
        case VOCAB_SET:
        {
            *rec = true;
            if (ControlBoardWrapper_p->verbose())
                yDebug("set command received\n");

            switch(cmd.get(2).asVocab())
            {
                case VOCAB_REF:
                {
                    *ok = rpc_ITorque->setRefTorque(cmd.get(3).asInt(), cmd.get(4).asDouble());
                }
                break;

                case VOCAB_BEMF:
                {
                    *ok = rpc_ITorque->setBemfParam(cmd.get(3).asInt(), cmd.get(4).asDouble());
                }
                break;

                case VOCAB_MOTOR_PARAMS:
                {
                    yarp::dev::MotorTorqueParameters params;
                    int joint = cmd.get(3).asInt();
                    Bottle *b = cmd.get(4).asList();

                    if (b==nullptr)
                        break;

                    if (b->size() != 4)
                    {
                        yError("received a SET VOCAB_MOTOR_PARAMS command not understood, size!=4");
                        break;
                    }

                    params.bemf         = b->get(0).asDouble();
                    params.bemf_scale   = b->get(1).asDouble();
                    params.ktau         = b->get(2).asDouble();
                    params.ktau_scale   = b->get(3).asDouble();

                    *ok = rpc_ITorque->setMotorTorqueParams(joint, params);
                }
                break;

                case VOCAB_REFS:
                {
                    Bottle *b = cmd.get(3).asList();
                    if (b==nullptr)
                        break;

                    int i;
                    const int njs = b->size();
                    if (njs==controlledJoints)
                    {
                        double *p = new double[njs];    // LATER: optimize to avoid allocation.
                        for (i = 0; i < njs; i++)
                            p[i] = b->get(i).asDouble();
                        *ok = rpc_ITorque->setRefTorques (p);
                        delete[] p;
                    }
                }
                break;

                case VOCAB_TORQUE_MODE:
                {
                    if(rpc_iCtrlMode2)
                    {
                        int *modes = new int[controlledJoints];
                        for(int i=0; i<controlledJoints; i++)
                            modes[i] = VOCAB_CM_TORQUE;
                        *ok = rpc_iCtrlMode2->setControlModes(modes);
                        delete [] modes;
                    }
                    else
                    {
                        *ok = false;
                    }
                }
                break;

            }
        }
        break;

        case VOCAB_GET:
        {
            *rec = true;
            if (ControlBoardWrapper_p->verbose())
                yDebug("get command received\n");
            double dtmp  = 0.0;
            double dtmp2 = 0.0;
            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));

            switch(cmd.get(2).asVocab())
            {
                case VOCAB_AXES:
                {
                    int tmp;
                    *ok = rpc_ITorque->getAxes(&tmp);
                    response.addInt(tmp);
                }
                break;

                case VOCAB_TRQ:
                {
                    *ok = rpc_ITorque->getTorque(cmd.get(3).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_BEMF:
                {
                    *ok = rpc_ITorque->getBemfParam(cmd.get(3).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_MOTOR_PARAMS:
                {
                    yarp::dev::MotorTorqueParameters params;
                    int joint = cmd.get(3).asInt();

                    // get the data
                    *ok = rpc_ITorque->getMotorTorqueParams(joint, &params);

                    // convert it back to yarp message
                    Bottle& b = response.addList();

                    b.addDouble(params.bemf);
                    b.addDouble(params.bemf_scale);
                    b.addDouble(params.ktau);
                    b.addDouble(params.ktau_scale);
                }
                break;
                case VOCAB_RANGE:
                {
                    *ok = rpc_ITorque->getTorqueRange(cmd.get(3).asInt(), &dtmp, &dtmp2);
                    response.addDouble(dtmp);
                    response.addDouble(dtmp2);
                }
                break;

                case VOCAB_TRQS:
                {
                    int i=0;
                    double *p = new double[controlledJoints];
                    *ok = rpc_ITorque->getTorques(p);
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
                    *ok = rpc_ITorque->getTorqueRanges(p1,p2);
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

                case VOCAB_REFERENCE:
                {
                    *ok = rpc_ITorque->getRefTorque(cmd.get(3).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_REFERENCES:
                {
                    double *p = new double[controlledJoints];
                    *ok = rpc_ITorque->getRefTorques(p);
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

void RPCMessagesParser::handleInteractionModeMsg(const yarp::os::Bottle& cmd,
                                                 yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("\nHandling IInteractionMode message\n");
    if (!rpc_IInteract)
    {
        yError("Error I do not have a valid IInteractionMode interface");
        *ok=false;
        return;
    }

    if (ControlBoardWrapper_p->verbose())
    {
        yDebug() << "received command: " << cmd.toString();
    }

    int action = cmd.get(0).asVocab();

    switch(action)
    {
        case VOCAB_SET:
        {
            switch (cmd.get(2).asVocab())
            {
                yarp::os::Bottle *jointList;
                yarp::os::Bottle *modeList;
                yarp::dev::InteractionModeEnum *modes;

                case VOCAB_INTERACTION_MODE:
                {
                    *ok = rpc_IInteract->setInteractionMode(cmd.get(3).asInt(), (yarp::dev::InteractionModeEnum) cmd.get(4).asVocab());
                }
                break;

                case VOCAB_INTERACTION_MODE_GROUP:
                {
//              yDebug() << "CBW.h set interactionMode GROUP" << std::endl;

                    int n_joints = cmd.get(3).asInt();
                    jointList = cmd.get(4).asList();
                    modeList  = cmd.get(5).asList();
                    if( (jointList->size() != n_joints) || (modeList->size() != n_joints) )
                    {
                        if (ControlBoardWrapper_p->verbose()) {
                            yError("Received an invalid setInteractionMode message. Size of vectors doesn´t match\n");
                        }
                        *ok = false;
                        break;
                    }
                    int *joints = new int[n_joints];
                    modes = new yarp::dev::InteractionModeEnum [n_joints];
                    for( int i=0; i<n_joints; i++)
                    {
                        joints[i] = jointList->get(i).asInt();
                        modes[i]  = (yarp::dev::InteractionModeEnum) modeList->get(i).asVocab();
//                  yDebug()  << "CBW.cpp received vocab " << yarp::os::Vocab::decode(modes[i]) << std::endl;
                    }
                    *ok = rpc_IInteract->setInteractionModes(n_joints, joints, modes);
                    delete [] joints;
                    delete [] modes;

                }
                break;

                case VOCAB_INTERACTION_MODES:
                {
//              yDebug()  << "CBW.c set interactionMode ALL" << std::endl;

                    modeList  = cmd.get(3).asList();
                    if(modeList->size() != controlledJoints)
                    {
                        if (ControlBoardWrapper_p->verbose())
                            yError("Received an invalid setInteractionMode message. Size of vector doesn´t match the number of controlled joints\n");
                        *ok = false;
                        break;
                    }
                    modes  = new yarp::dev::InteractionModeEnum [controlledJoints];
                    for( int i=0; i<controlledJoints; i++)
                    {
                        modes[i]  = (yarp::dev::InteractionModeEnum) modeList->get(i).asVocab();
                    }
                    *ok = rpc_IInteract->setInteractionModes(modes);
                    delete [] modes;
                }
                break;

                default:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yError("Error while Handling IInteractionMode message, SET command not understood %s\n", cmd.get(2).asString().c_str());
                    *ok = false;
                }
                break;
            }
            *rec=true; //or false
        }
        break;

        case VOCAB_GET:
        {
            yarp::os::Bottle *jointList;

            switch (cmd.get(2).asVocab())
            {
                case VOCAB_INTERACTION_MODE:
                {
                    yarp::dev::InteractionModeEnum mode;
                    *ok = rpc_IInteract->getInteractionMode(cmd.get(3).asInt(), &mode);
                    response.addVocab(mode);
                    if (ControlBoardWrapper_p->verbose())    yDebug()  << " resp is " << response.toString();
                }
                break;

                case VOCAB_INTERACTION_MODE_GROUP:
                {
                    yarp::dev::InteractionModeEnum* modes;

                    int n_joints = cmd.get(3).asInt();
                    jointList = cmd.get(4).asList();
                    if(jointList->size() != n_joints )
                    {
                    yError("Received an invalid getInteractionMode message. Size of vectors doesn´t match");
                        *ok = false;
                        break;
                    }
                    int *joints = new int[n_joints];
                    modes       = new yarp::dev::InteractionModeEnum [n_joints];
                    for( int i=0; i<n_joints; i++)
                    {
                        joints[i] = jointList->get(i).asInt();
                    }
                    *ok = rpc_IInteract->getInteractionModes(n_joints, joints, modes);

                    Bottle& c = response.addList();
                    for( int i=0; i<n_joints; i++)
                    {
                        c.addVocab(modes[i]);
                    }

                    if (ControlBoardWrapper_p->verbose())
                    {
                    yDebug("got response bottle\n");
                        response.toString();
                    }
                    delete [] joints;
                    delete [] modes;
                }
                break;

                case VOCAB_INTERACTION_MODES:
                {
                    yarp::dev::InteractionModeEnum* modes;
                    modes  = new yarp::dev::InteractionModeEnum [controlledJoints];

                    *ok = rpc_IInteract->getInteractionModes(modes);

                    Bottle& b = response.addList();
                    for( int i=0; i<controlledJoints; i++)
                    {
                        b.addVocab(modes[i]);
                    }
                    if (ControlBoardWrapper_p->verbose())
                    {
                        yDebug("got response bottle\n");
                        response.toString();
                    }
                    delete [] modes;
                }
                break;
            }
            lastRpcStamp.update();
            appendTimeStamp(response, lastRpcStamp);
        }
        break; // case VOCAB_GET

        default:
        yError("Error while Handling IInteractionMode message, command was not SET nor GET");
            *ok = false;
        break;

    }
}

void RPCMessagesParser::handleCurrentMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("Handling ICurrentControl message\n");

    if (!rpc_ICurrent)
    {
        yError("controlBoardWrapper: I do not have a valid ICurrentControl interface");
        *ok = false;
        return;
    }

    int code = cmd.get(0).asVocab();
    int action = cmd.get(2).asVocab();

    *ok = false;
    *rec = true;
    switch (code)
    {
    case VOCAB_SET:
    {
        switch (action)
        {
            case VOCAB_CURRENT_REF:
            {
                yError("VOCAB_CURRENT_REF methods is implemented as streaming");
                *ok = false;
            }
            break;

            case VOCAB_CURRENT_REFS:
            {
                yError("VOCAB_CURRENT_REFS methods is implemented as streaming");
                *ok = false;
            }
            break;

            case VOCAB_CURRENT_REF_GROUP:
            {
                yError("VOCAB_CURRENT_REF_GROUP methods is implemented as streaming");
                *ok = false;
            }
            break;

            default:
            {
                yError() << "Unknown handleCurrentMsg message received";
                *rec = false;
                *ok = false;
            }
            break;
        }
    }
    break;

    case VOCAB_GET:
    {
        *rec = true;
        if (ControlBoardWrapper_p->verbose())
            yDebug("get command received\n");
        double dtmp = 0.0;
        double dtmp2 = 0.0;
        response.addVocab(VOCAB_IS);
        response.add(cmd.get(1));

        switch (action)
        {
            case VOCAB_CURRENT_REF:
            {
                *ok = rpc_ICurrent->getRefCurrent(cmd.get(3).asInt(), &dtmp);
                response.addDouble(dtmp);
            }
            break;

            case VOCAB_CURRENT_REFS:
            {
                double *p = new double[controlledJoints];
                *ok = rpc_ICurrent->getRefCurrents(p);
                Bottle& b = response.addList();
                int i;
                for (i = 0; i < controlledJoints; i++)
                    b.addDouble(p[i]);
                delete[] p;
            }
            break;

            case VOCAB_CURRENT_RANGE:
            {

                *ok = rpc_ICurrent->getCurrentRange(cmd.get(3).asInt(), &dtmp, &dtmp2);
                response.addDouble(dtmp);
                response.addDouble(dtmp2);
            }
            break;

            case VOCAB_CURRENT_RANGES:
            {
                double *p1 = new double[controlledJoints];
                double *p2 = new double[controlledJoints];
                *ok = rpc_ICurrent->getCurrentRanges(p1,p2);
                Bottle& b1 = response.addList();
                Bottle& b2 = response.addList();
                int i;
                for (i = 0; i < controlledJoints; i++)
                {
                    b1.addDouble(p1[i]);
                }
                for (i = 0; i < controlledJoints; i++)
                {
                    b2.addDouble(p2[i]);
                }
                delete[] p1;
                delete[] p2;
            }
            break;

            default:
            {
                yError() << "Unknown handleCurrentMsg message received";
                *rec = false;
                *ok = false;
            }
            break;
        }
    }
    break;

    default:
    {
        yError() << "Unknown handleCurrentMsg message received";
        *rec = false;
        *ok = false;
    }
    break;
    }
}

void RPCMessagesParser::handlePidMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("Handling IPidControl message\n");

    if (!rpc_IPid)
    {
        yError("controlBoardWrapper: I do not have a valid IPidControl interface");
        *ok = false;
        return;
    }

    int code = cmd.get(0).asVocab();
    int action = cmd.get(2).asVocab();
    yarp::dev::PidControlTypeEnum pidtype = static_cast<yarp::dev::PidControlTypeEnum>(cmd.get(3).asVocab());

    *ok = false;
    *rec = true;
    switch (code)
    {
        case VOCAB_SET:
        {
            *rec = true;
            if (ControlBoardWrapper_p->verbose())
                yDebug("set command received\n");

            switch(action)
            {
                case VOCAB_OFFSET:
                {
                    double v;
                    int j = cmd.get(4).asInt();
                    v=cmd.get(5).asDouble();
                    *ok = rpc_IPid->setPidOffset(pidtype, j, v);
                }
                break;

                case VOCAB_PID:
                {
                    Pid p;
                    int j = cmd.get(4).asInt();
                    Bottle *b = cmd.get(5).asList();

                    if (b==nullptr)
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
                    p.kff = b->get(9).asDouble();
                    *ok = rpc_IPid->setPid(pidtype, j, p);
                }
                break;

                case VOCAB_PIDS:
                {
                    Bottle *b = cmd.get(4).asList();

                    if (b==nullptr)
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
                            if (c!=nullptr)
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
                                p[i].kff = c->get(9).asDouble();
                            }
                            else
                            {
                                allOK=false;
                            }
                        }
                        if (allOK)
                            *ok = rpc_IPid->setPids(pidtype, p);
                        else
                            *ok=false;

                        delete[] p;
                    }
                }
                break;

                case VOCAB_REF:
                {
                    *ok = rpc_IPid->setPidReference (pidtype, cmd.get(4).asInt(), cmd.get(5).asDouble());
                }
                break;

                case VOCAB_REFS:
                {
                    Bottle *b = cmd.get(4).asList();

                    if (b==nullptr)
                        break;

                    int i;
                    const int njs = b->size();
                    if (njs==controlledJoints)
                    {
                        double *p = new double[njs];    // LATER: optimize to avoid allocation.
                        for (i = 0; i < njs; i++)
                            p[i] = b->get(i).asDouble();
                        *ok = rpc_IPid->setPidReferences (pidtype, p);
                        delete[] p;
                    }
                }
                break;

                case VOCAB_LIM:
                {
                    *ok = rpc_IPid->setPidErrorLimit (pidtype, cmd.get(4).asInt(), cmd.get(5).asDouble());
                }
                break;

                case VOCAB_LIMS:
                {
                    Bottle *b = cmd.get(4).asList();
                    int i;

                    if (b==nullptr)
                        break;

                    const int njs = b->size();
                    if (njs==controlledJoints)
                    {
                        double *p = new double[njs];    // LATER: optimize to avoid allocation.
                        for (i = 0; i < njs; i++)
                            p[i] = b->get(i).asDouble();
                        *ok = rpc_IPid->setPidErrorLimits (pidtype, p);
                        delete[] p;
                    }
                }
                break;

                case VOCAB_RESET:
                {
                    *ok = rpc_IPid->resetPid (pidtype, cmd.get(4).asInt());
                }
                break;

                case VOCAB_DISABLE:
                {
                    *ok = rpc_IPid->disablePid (pidtype, cmd.get(4).asInt());
                }
                break;

                case VOCAB_ENABLE:
                {
                    *ok = rpc_IPid->enablePid (pidtype, cmd.get(4).asInt());
                }
                break;
            }
        }
        break;

        case VOCAB_GET:
        {
            *rec = true;
            if (ControlBoardWrapper_p->verbose())
                yDebug("get command received\n");
            double dtmp = 0.0;
            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));

            switch (action)
            {
                case VOCAB_LIMS:
                {
                    double *p = new double[controlledJoints];
                    *ok = rpc_IPid->getPidErrorLimits(pidtype, p);
                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                        b.addDouble(p[i]);
                    delete[] p;
                }
                break;

                case VOCAB_ENABLE:
                {
                    bool booltmp=false;
                    *ok = rpc_IPid->isPidEnabled(pidtype, cmd.get(4).asInt(), &booltmp);
                    response.addInt(booltmp);
                }
                break;

                case VOCAB_ERR:
                {
                    *ok = rpc_IPid->getPidError(pidtype, cmd.get(4).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_ERRS:
                {
                    double *p = new double[controlledJoints];
                    *ok = rpc_IPid->getPidErrors(pidtype, p);
                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                        b.addDouble(p[i]);
                    delete[] p;
                }
                break;

                case VOCAB_OUTPUT:
                {
                    *ok = rpc_IPid->getPidOutput(pidtype, cmd.get(4).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_OUTPUTS:
                {
                    double *p = new double[controlledJoints];
                    *ok = rpc_IPid->getPidOutputs(pidtype, p);
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
                    *ok = rpc_IPid->getPid(pidtype, cmd.get(4).asInt(), &p);
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
                    b.addDouble(p.kff);
                }
                break;

                case VOCAB_PIDS:
                {
                    Pid *p = new Pid[controlledJoints];
                    *ok = rpc_IPid->getPids(pidtype, p);
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
                        c.addDouble(p[i].kff);
                    }
                    delete[] p;
                }
                break;

                case VOCAB_REFERENCE:
                {
                    *ok = rpc_IPid->getPidReference(pidtype, cmd.get(4).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_REFERENCES:
                {
                    double *p = new double[controlledJoints];
                    *ok = rpc_IPid->getPidReferences(pidtype, p);
                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                        b.addDouble(p[i]);
                    delete[] p;
                }
                break;

                case VOCAB_LIM:
                {
                    *ok = rpc_IPid->getPidErrorLimit(pidtype, cmd.get(4).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;
            }
        }
        break;

        default:
        {
            yError() << "Unknown handlePWMMsg message received";
            *rec = false;
            *ok = false;
        }
        break;
    }
}

void RPCMessagesParser::handlePWMMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("Handling IPWMControl message\n");

    if (!rpc_IPWM)
    {
        yError("controlBoardWrapper: I do not have a valid IPWMControl interface");
        *ok = false;
        return;
    }

    int code = cmd.get(0).asVocab();
    int action = cmd.get(2).asVocab();

    *ok = false;
    *rec = true;
    switch (code)
    {
        case VOCAB_SET:
        {
            *rec = true;
            if (ControlBoardWrapper_p->verbose())
                yDebug("set command received\n");

            switch (action)
            {
                case VOCAB_PWMCONTROL_REF_PWM:
                {
                    //handled as streaming!
                    yError() << "VOCAB_PWMCONTROL_REF_PWM handled as straming";
                    *ok = false;
                }
                break;

                default:
                {
                    yError() << "Unknown handlePWMMsg message received";
                    *ok = false;
                }
                break;
            }
        }
        break;

        case VOCAB_GET:
        {
            *rec = true;
            if (ControlBoardWrapper_p->verbose())
                yDebug("get command received\n");
            double dtmp = 0.0;
            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));

            switch (action)
            {
                case VOCAB_PWMCONTROL_REF_PWM:
                {
                    *ok = rpc_IPWM->getRefDutyCycle(cmd.get(3).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_PWMCONTROL_REF_PWMS:
                {
                    double *p = new double[controlledJoints];
                    *ok = rpc_IPWM->getRefDutyCycles(p);
                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                        b.addDouble(p[i]);
                    delete[] p;
                }
                break;

                case VOCAB_PWMCONTROL_PWM_OUTPUT:
                {
                    *ok = rpc_IPWM->getDutyCycle(cmd.get(3).asInt(), &dtmp);
                    response.addDouble(dtmp);
                }
                break;

                case VOCAB_PWMCONTROL_PWM_OUTPUTS:
                {
                    double *p = new double[controlledJoints];
                    *ok = rpc_IPWM->getRefDutyCycles(p);
                    Bottle& b = response.addList();
                    int i;
                    for (i = 0; i < controlledJoints; i++)
                        b.addDouble(p[i]);
                    delete[] p;
                }
                break;

                default:
                {
                    yError() << "Unknown handlePWMMsg message received";
                    *ok = false;
                }
                break;
            }
        }
        break;

        default:
        {
            yError() << "Unknown handlePWMMsg message received";
            *rec = false;
            *ok = false;
        }
        break;
    }
}

void RPCMessagesParser::handleRemoteVariablesMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if (ControlBoardWrapper_p->verbose())
        yDebug("Handling IRemoteCalibrator message\n");

    if (!rpc_IRemoteCalibrator)
    {
        yError("controlBoardWrapper: I do not have a valid IRemoteCalibrator interface");
        *ok = false;
        return;
    }

    int code = cmd.get(0).asVocab();
    int action = cmd.get(2).asVocab();

    *ok = false;
    *rec = true;
    switch (code)
    {
        case VOCAB_SET:
        {
            switch (action)
            {
                case VOCAB_VARIABLE:
                {
                    Bottle btail = cmd.tail().tail().tail().tail(); // remove the first four elements
                    string s = btail.toString();
                    *ok = rpc_IVar->setRemoteVariable(cmd.get(3).asString(), btail);
                }
                break;

                default:
                {
                    *rec = false;
                    *ok = false;
                } break;
             }
        }
        break;

        case VOCAB_GET:
        {
            response.clear();
            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));
            Bottle btmp;

            if (ControlBoardWrapper_p->verbose())
                yDebug("get command received\n");

            switch (action)
            {
                case VOCAB_VARIABLE:
                {
                    *ok = rpc_IVar->getRemoteVariable(cmd.get(3).asString(), btmp);
                    Bottle& b = response.addList();
                    b = btmp;
                }
                break;

                case VOCAB_LIST_VARIABLES:
                {
                    *ok = rpc_IVar->getRemoteVariablesList(&btmp);
                    Bottle& b = response.addList();
                    b = btmp;
                }
                break;
            }
        }
    }   //end get/set switch
}

void RPCMessagesParser::handleRemoteCalibratorMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok)
{
    if(ControlBoardWrapper_p->verbose())
        yDebug("Handling IRemoteCalibrator message\n");

    if (!rpc_IRemoteCalibrator)
    {
        yError("controlBoardWrapper: I do not have a valid IRemoteCalibrator interface");
        *ok=false;
        return;
    }

    int code   = cmd.get(0).asVocab();
    int action = cmd.get(2).asVocab();

    *ok=false;
    *rec=true;
    switch(code)
    {
        case VOCAB_SET:
        {
            switch(action)
            {
                case VOCAB_CALIBRATE_SINGLE_JOINT:
                {
                    yDebug() << "cmd is " << cmd.toString() << " joint is " << cmd.get(3).asInt();
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling calibrate joint with no parameter\n");
                    *ok = rpc_IRemoteCalibrator->calibrateSingleJoint(cmd.get(3).asInt());
                } break;

                case VOCAB_CALIBRATE_WHOLE_PART:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling calibrate whole part\n");
                    *ok = rpc_IRemoteCalibrator->calibrateWholePart();
                } break;

                case VOCAB_HOMING_SINGLE_JOINT:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling calibrate joint with no parameter\n");
                    *ok = rpc_IRemoteCalibrator->homingSingleJoint(cmd.get(3).asInt());
                } break;

                case VOCAB_HOMING_WHOLE_PART:
                {
                    yDebug() << "Received homing whole part";
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling calibrate whole part\n");
                    *ok = rpc_IRemoteCalibrator->homingWholePart();
                } break;

                case VOCAB_PARK_SINGLE_JOINT:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling calibrate joint with no parameter\n");
                    *ok = rpc_IRemoteCalibrator->parkSingleJoint(cmd.get(3).asInt());
                } break;

                case VOCAB_PARK_WHOLE_PART:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling calibrate whole part\n");
                    *ok = rpc_IRemoteCalibrator->parkWholePart();
                } break;

                case VOCAB_QUIT_CALIBRATE:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling quit calibrate\n");
                    *ok = rpc_IRemoteCalibrator->quitCalibrate();
                } break;

                case VOCAB_QUIT_PARK:
                {
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling quit park\n");
                    *ok = rpc_IRemoteCalibrator->quitPark();
                } break;

                default:
                {
                    *rec = false;
                    *ok = false;
                } break;
            }
        }break;

        case VOCAB_GET:
        {
            response.clear();
            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));

            switch(action)
            {
                case VOCAB_IS_CALIBRATOR_PRESENT:
                {
                    bool tmp;
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling VOCAB_IS_CALIBRATOR_PRESENT\n");
                    *ok = rpc_IRemoteCalibrator->isCalibratorDevicePresent(&tmp);
                    response.addInt(tmp);
                } break;
            }
        }
    }   //end get/set switch
}


// rpc callback
bool RPCMessagesParser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ok  = false;
    bool rec = false;    // Tells if the command is recognized!

    if (ControlBoardWrapper_p->verbose())
        yDebug("command received: %s\n", cmd.toString().c_str());

    int code = cmd.get(0).asVocab();

    if(cmd.size() < 2)
    {
        ok = false;
    }
    else
    {
        switch (cmd.get(1).asVocab())
        {
            case VOCAB_PID:
                handlePidMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_TORQUE:
                handleTorqueMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_ICONTROLMODE:
                handleControlModeMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_IMPEDANCE:
                handleImpedanceMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_INTERFACE_INTERACTION_MODE:
                handleInteractionModeMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_PROTOCOL_VERSION:
                handleProtocolVersionRequest(cmd, response, &rec, &ok);
            break;

            case VOCAB_REMOTE_CALIBRATOR_INTERFACE:
                handleRemoteCalibratorMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_REMOTE_VARIABILE_INTERFACE:
                handleRemoteVariablesMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_CURRENTCONTROL_INTERFACE:
                handleCurrentMsg(cmd, response, &rec, &ok);
            break;

            case VOCAB_PWMCONTROL_INTERFACE:
                handlePWMMsg(cmd, response, &rec, &ok);
                break;

            default:
                // fallback for old interfaces with no specific name
                switch (code)
                {
                    case VOCAB_CALIBRATE_JOINT:
                    {
                        rec=true;
                        if (ControlBoardWrapper_p->verbose())
                            yDebug("Calling calibrate joint\n");

                        int j=cmd.get(1).asInt();
                        int ui=cmd.get(2).asInt();
                        double v1=cmd.get(3).asDouble();
                        double v2=cmd.get(4).asDouble();
                        double v3=cmd.get(5).asDouble();
                        if (rpc_Icalib2==nullptr)
                            yError("Sorry I don't have a IControlCalibration2 interface\n");
                        else
                            ok=rpc_Icalib2->calibrate2(j,ui,v1,v2,v3);
                    }
                    break;

                    case VOCAB_CALIBRATE_JOINT_PARAMS:
                    {
                        rec = true;
                        if (ControlBoardWrapper_p->verbose())
                            yDebug("Calling calibrate joint\n");

                        int j = cmd.get(1).asInt();
                        CalibrationParameters params;
                        params.type = cmd.get(2).asInt();
                        params.param1 = cmd.get(3).asDouble();
                        params.param2 = cmd.get(4).asDouble();
                        params.param3 = cmd.get(5).asDouble();
                        params.param4 = cmd.get(6).asDouble();
                        if (rpc_Icalib2 == nullptr)
                            yError("Sorry I don't have a IControlCalibration2 interface\n");
                        else
                            ok = rpc_Icalib2->setCalibrationParameters(j, params);
                    }
                    break;

                    case VOCAB_CALIBRATE:
                    {
                        rec=true;
                        if (ControlBoardWrapper_p->verbose())
                            yDebug("Calling calibrate\n");
                        ok=rpc_Icalib2->calibrate();
                    }
                    break;

                    case VOCAB_CALIBRATE_DONE:
                    {
                        rec=true;
                        if (ControlBoardWrapper_p->verbose())
                            yDebug("Calling calibrate done\n");
                        int j=cmd.get(1).asInt();
                        ok=rpc_Icalib2->done(j);
                    }
                    break;

                    case VOCAB_PARK:
                    {
                        rec=true;
                    if (ControlBoardWrapper_p->verbose())
                        yDebug("Calling park function\n");
                        int flag=cmd.get(1).asInt();
                        if (flag)
                            ok=rpc_Icalib2->park(true);
                        else
                            ok=rpc_Icalib2->park(false);
                        ok=true; //client would get stuck if returning false
                    }
                    break;

                    case VOCAB_SET:
                    {
                        rec = true;
                        if (ControlBoardWrapper_p->verbose())
                            yDebug("set command received\n");

                        switch(cmd.get(1).asVocab())
                        {
                            case VOCAB_POSITION_MOVE:
                            {
                                ok = rpc_IPosCtrl->positionMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                                // this operation is also available on "command" port
                            case VOCAB_POSITION_MOVES:
                            {
                                Bottle *b = cmd.get(2).asList();
                                int i;
                                if (b==nullptr)
                                    break;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                tmpVect.resize(njs);
                                for (i = 0; i < njs; i++)
                                    tmpVect[i] = b->get(i).asDouble();

                                if (rpc_IPosCtrl!=nullptr)
                                    ok = rpc_IPosCtrl->positionMove(&tmpVect[0]);
                            }
                            break;

                            case VOCAB_POSITION_MOVE_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle *jlut = cmd.get(3).asList();
                                Bottle *pos_val= cmd.get(4).asList();

                                if (rpc_IPosCtrl2 == nullptr)
                                    break;

                                if (jlut==nullptr || pos_val==nullptr)
                                    break;
                                if (len!=jlut->size() || len!=pos_val->size())
                                    break;

                                int *j_tmp=new int [len];
                                double *pos_tmp=new double [len];

                                for (int i = 0; i < len; i++)
                                    j_tmp[i] = jlut->get(i).asInt();

                                for (int i = 0; i < len; i++)
                                    pos_tmp[i] = pos_val->get(i).asDouble();

                                ok = rpc_IPosCtrl2->positionMove(len, j_tmp, pos_tmp);

                                delete [] j_tmp;
                                delete [] pos_tmp;
                            }
                            break;

                                // this operation is also available on "command" port
                            case VOCAB_VELOCITY_MOVES:
                            {
                                Bottle *b = cmd.get(2).asList();
                                int i;
                                if (b==nullptr)
                                    break;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                tmpVect.resize(njs);
                                for (i = 0; i < njs; i++)
                                    tmpVect[i] = b->get(i).asDouble();
                                if (rpc_IVelCtrl!=nullptr)
                                    ok = rpc_IVelCtrl->velocityMove(&tmpVect[0]);

                            }
                            break;

                            case VOCAB_RELATIVE_MOVE:
                            {
                                ok = rpc_IPosCtrl->relativeMove(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_RELATIVE_MOVE_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle *jBottle_p = cmd.get(3).asList();
                                Bottle *posBottle_p= cmd.get(4).asList();

                                if (rpc_IPosCtrl2 == nullptr)
                                    break;

                                if (jBottle_p==nullptr || posBottle_p==nullptr)
                                    break;
                                if (len!=jBottle_p->size() || len!=posBottle_p->size())
                                    break;

                                int *j_tmp=new int [len];
                                double *pos_tmp=new double [len];

                                for (int i = 0; i < len; i++)
                                    j_tmp[i] = jBottle_p->get(i).asInt();

                                for (int i = 0; i < len; i++)
                                    pos_tmp[i] = posBottle_p->get(i).asDouble();

                                ok = rpc_IPosCtrl2->relativeMove(len, j_tmp, pos_tmp);

                                delete [] j_tmp;
                                delete [] pos_tmp;
                            }
                            break;

                            case VOCAB_RELATIVE_MOVES:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==nullptr)
                                    break;

                                int i;
                                const int njs = b->size();
                                if(njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = rpc_IPosCtrl->relativeMove(p);
                                delete[] p;
                            }
                            break;

                            case VOCAB_REF_SPEED:
                            {
                                ok = rpc_IPosCtrl->setRefSpeed(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_REF_SPEED_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle *jBottle_p = cmd.get(3).asList();
                                Bottle *velBottle_p= cmd.get(4).asList();

                                if (rpc_IPosCtrl2 == nullptr)
                                    break;

                                if (jBottle_p==nullptr || velBottle_p==nullptr)
                                    break;
                                if (len!=jBottle_p->size() || len!=velBottle_p->size())
                                    break;

                                int *j_tmp=new int [len];
                                double *spds_tmp=new double [len];

                                for (int i = 0; i < len; i++)
                                    j_tmp[i] = jBottle_p->get(i).asInt();

                                for (int i = 0; i < len; i++)
                                    spds_tmp[i] = velBottle_p->get(i).asDouble();

                                ok = rpc_IPosCtrl2->setRefSpeeds(len, j_tmp, spds_tmp);
                                delete[] j_tmp;
                                delete[] spds_tmp;
                            }
                            break;

                            case VOCAB_REF_SPEEDS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==nullptr)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = rpc_IPosCtrl->setRefSpeeds(p);
                                delete[] p;
                            }
                            break;

                            case VOCAB_REF_ACCELERATION:
                            {
                                ok = rpc_IPosCtrl->setRefAcceleration(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_REF_ACCELERATION_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle *jBottle_p = cmd.get(3).asList();
                                Bottle *accBottle_p = cmd.get(4).asList();

                                if (rpc_IPosCtrl2 == nullptr)
                                    break;

                                if (jBottle_p==nullptr || accBottle_p==nullptr)
                                    break;
                                if (len!=jBottle_p->size() || len!=accBottle_p->size())
                                    break;

                                int *j_tmp = new int [len];
                                double *accs_tmp = new double [len];

                                for (int i = 0; i < len; i++)
                                    j_tmp[i] = jBottle_p->get(i).asInt();

                                for (int i = 0; i < len; i++)
                                    accs_tmp[i] = accBottle_p->get(i).asDouble();

                                ok = rpc_IPosCtrl2->setRefAccelerations(len, j_tmp, accs_tmp);
                                delete[] j_tmp;
                                delete[] accs_tmp;
                            }
                            break;

                            case VOCAB_REF_ACCELERATIONS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==nullptr)
                                    break;

                                int i;
                                const int njs = b->size();
                                if(njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = rpc_IPosCtrl->setRefAccelerations(p);
                                delete[] p;
                            }
                            break;

                            case VOCAB_STOP:
                            {
                                ok = rpc_IPosCtrl->stop(cmd.get(2).asInt());
                            }
                            break;

                            case VOCAB_STOP_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle *jBottle_p = cmd.get(3).asList();

                                if (rpc_IPosCtrl2 == nullptr)
                                    break;

                                if (jBottle_p==nullptr)
                                    break;
                                if (len!=jBottle_p->size())
                                    break;

                                int *j_tmp = new int [len];

                                for (int i = 0; i < len; i++)
                                    j_tmp[i] = jBottle_p->get(i).asInt();

                                ok = rpc_IPosCtrl2->stop(len, j_tmp);
                                delete[] j_tmp;
                            }
                            break;

                            case VOCAB_STOPS:
                            {
                                ok = rpc_IPosCtrl->stop();
                            }
                            break;

                            case VOCAB_E_RESET:
                            {
                                ok = rpc_IEncTimed->resetEncoder(cmd.get(2).asInt());
                            }
                            break;

                            case VOCAB_E_RESETS:
                            {
                                ok = rpc_IEncTimed->resetEncoders();
                            }
                            break;

                            case VOCAB_ENCODER:
                            {
                                ok = rpc_IEncTimed->setEncoder(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_ENCODERS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==nullptr)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = rpc_IEncTimed->setEncoders(p);
                                delete[] p;
                            }
                            break;

                            case VOCAB_MOTOR_CPR:
                            {
                                ok = rpc_IMotEnc->setMotorEncoderCountsPerRevolution(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_MOTOR_E_RESET:
                            {
                                ok = rpc_IMotEnc->resetMotorEncoder(cmd.get(2).asInt());
                            }
                            break;

                            case VOCAB_MOTOR_E_RESETS:
                            {
                                ok = rpc_IMotEnc->resetMotorEncoders();
                            }
                            break;

                            case VOCAB_MOTOR_ENCODER:
                            {
                                ok = rpc_IMotEnc->setMotorEncoder(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_MOTOR_ENCODERS:
                            {
                                Bottle *b = cmd.get(2).asList();

                                if (b==nullptr)
                                    break;

                                int i;
                                const int njs = b->size();
                                if (njs!=controlledJoints)
                                    break;
                                double *p = new double[njs];    // LATER: optimize to avoid allocation.
                                for (i = 0; i < njs; i++)
                                    p[i] = b->get(i).asDouble();
                                ok = rpc_IMotEnc->setMotorEncoders(p);
                                delete[] p;
                            }
                            break;

                            case VOCAB_AMP_ENABLE:
                            {
                                ok = rcp_IAmp->enableAmp(cmd.get(2).asInt());
                            }
                            break;

                            case VOCAB_AMP_DISABLE:
                            {
                                ok = rcp_IAmp->disableAmp(cmd.get(2).asInt());
                            }
                            break;

                            case VOCAB_AMP_MAXCURRENT:
                            {
                                ok = rcp_IAmp->setMaxCurrent(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_AMP_PEAK_CURRENT:
                            {
                                ok = rcp_IAmp->setPeakCurrent(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_AMP_PWM_LIMIT:
                            {
                                ok = rcp_IAmp->setPWMLimit(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_LIMITS:
                            {
                                ok = rcp_Ilim2->setLimits(cmd.get(2).asInt(), cmd.get(3).asDouble(), cmd.get(4).asDouble());
                            }
                            break;


                            case VOCAB_TEMPERATURE_LIMIT:
                            {
                                ok = rpc_IMotor->setTemperatureLimit(cmd.get(2).asInt(), cmd.get(3).asDouble());
                            }
                            break;

                            case VOCAB_VEL_LIMITS:
                            {
                                ok = rcp_Ilim2->setVelLimits(cmd.get(2).asInt(), cmd.get(3).asDouble(), cmd.get(4).asDouble());
                            }
                            break;

                            default:
                            {
                                yError("received an unknown command after a VOCAB_SET (%s)\n", cmd.toString().c_str());
                            }
                            break;
                        } //switch(cmd.get(1).asVocab()
                        break;
                    }

                    case VOCAB_GET:
                    {
                        rec = true;
                        if (ControlBoardWrapper_p->verbose())
                            yDebug("get command received\n");

                        double dtmp = 0.0;
                        Bottle btmp;
                        response.addVocab(VOCAB_IS);
                        response.add(cmd.get(1));

                        switch(cmd.get(1).asVocab())
                        {

                            case VOCAB_TEMPERATURE_LIMIT:
                            {
                                ok = rpc_IMotor->getTemperatureLimit(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_TEMPERATURE:
                            {
                                ok = rpc_IMotor->getTemperature(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_GEARBOX_RATIO:
                            {
                                ok = rpc_IMotor->getGearboxRatio(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_TEMPERATURES:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IMotor->getTemperatures(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_AMP_MAXCURRENT:
                            {
                                ok = rcp_IAmp->getMaxCurrent(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_POSITION_MOVE:
                            {
                                if (ControlBoardWrapper_p->verbose())
                                    yDebug("getTargetPosition\n");

                                ok = rpc_IPosCtrl2->getTargetPosition(cmd.get(2).asInt(), &dtmp);

                                response.addDouble(dtmp);
                                rec=true;
                            }
                            break;

                            case VOCAB_POSITION_MOVE_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle& in = *(cmd.get(3).asList());
                                int *jointList = new int[len];
                                double *refs = new double[len];

                                for(int j=0; j<len; j++)
                                {
                                    jointList[j] = in.get(j).asInt();
                                }
                                ok = rpc_IPosCtrl2->getTargetPositions(len, jointList, refs);

                                Bottle& b = response.addList();
                                for (int i = 0; i < len; i++)
                                    b.addDouble(refs[i]);

                                delete[] jointList;
                                delete[] refs;
                            }
                            break;

                            case VOCAB_POSITION_MOVES:
                            {
                                double *refs = new double[controlledJoints];
                                ok = rpc_IPosCtrl2->getTargetPositions(refs);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(refs[i]);
                                delete[] refs;
                            }
                            break;

                            case VOCAB_POSITION_DIRECT:
                            {
                                if (ControlBoardWrapper_p->verbose())
                                    yDebug("getRefPosition\n");

                                ok = rpc_IPosDirect->getRefPosition(cmd.get(2).asInt(), &dtmp);

                                response.addDouble(dtmp);
                                rec=true;
                            }
                            break;

                            case VOCAB_POSITION_DIRECT_GROUP:
                            {
                                int len = cmd.get(2).asInt();
                                Bottle& in = *(cmd.get(3).asList());
                                int *jointList = new int[len];
                                double *refs = new double[len];

                                for(int j=0; j<len; j++)
                                {
                                    jointList[j] = in.get(j).asInt();
                                }
                                ok = rpc_IPosDirect->getRefPositions(len, jointList, refs);

                                Bottle& b = response.addList();
                                for (int i = 0; i < len; i++)
                                    b.addDouble(refs[i]);

                                delete[] jointList;
                                delete[] refs;
                            }
                            break;

                            case VOCAB_POSITION_DIRECTS:
                            {
                                double *refs = new double[controlledJoints];
                                ok = rpc_IPosDirect->getRefPositions(refs);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(refs[i]);
                                delete[] refs;
                            }
                            break;

                            case VOCAB_VELOCITY_MOVE:
                            {
                                if (ControlBoardWrapper_p->verbose())
                                    yDebug("getVelocityMove - cmd: %s\n", cmd.toString().c_str());

                                ok = rpc_IVelCtrl2->getRefVelocity(cmd.get(2).asInt(), &dtmp);

                                response.addDouble(dtmp);
                                rec=true;
                            }
                            break;

                            case VOCAB_VELOCITY_MOVE_GROUP:
                            {
                                if (ControlBoardWrapper_p->verbose())
                                    yDebug("getVelocityMove_group - cmd: %s\n", cmd.toString().c_str());

                                int len = cmd.get(2).asInt();
                                Bottle& in = *(cmd.get(3).asList());
                                int *jointList = new int[len];
                                double *refs = new double[len];

                                for(int j=0; j<len; j++)
                                {
                                    jointList[j] = in.get(j).asInt();
                                }
                                ok = rpc_IVelCtrl2->getRefVelocities(len, jointList, refs);

                                Bottle& b = response.addList();
                                for (int i = 0; i < len; i++)
                                    b.addDouble(refs[i]);

                                delete[] jointList;
                                delete[] refs;
                            }
                            break;

                            case VOCAB_VELOCITY_MOVES:
                            {
                                if (ControlBoardWrapper_p->verbose())
                                    yDebug("getVelocityMoves - cmd: %s\n", cmd.toString().c_str());

                                double *refs = new double[controlledJoints];
                                ok = rpc_IVelCtrl2->getRefVelocities(refs);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(refs[i]);
                                delete[] refs;
                            }
                            break;

                            case VOCAB_MOTORS_NUMBER:
                            {
                                int tmp;
                                ok = rpc_IMotor->getNumberOfMotors(&tmp);
                                response.addInt(tmp);
                            }
                            break;

                            case VOCAB_AXES:
                            {
                                int tmp;
                                ok = rpc_IPosCtrl->getAxes(&tmp);
                                response.addInt(tmp);
                            }
                            break;

                            case VOCAB_MOTION_DONE:
                            {
                                bool x = false;;
                                ok = rpc_IPosCtrl->checkMotionDone(cmd.get(2).asInt(), &x);
                                response.addInt(x);
                            }
                            break;

                            case VOCAB_MOTION_DONE_GROUP:
                            {
                                bool x = false;
                                int len = cmd.get(2).asInt();
                                Bottle& in = *(cmd.get(3).asList());
                                int *jointList = new int[len];
                                for(int j=0; j<len; j++)
                                {
                                    jointList[j] = in.get(j).asInt();
                                }
                                if(rpc_IPosCtrl2!=nullptr)
                                    ok = rpc_IPosCtrl2->checkMotionDone(len, jointList, &x);
                                response.addInt(x);

                                delete[] jointList;
                            }
                            break;

                            case VOCAB_MOTION_DONES:
                            {
                                bool x = false;
                                ok = rpc_IPosCtrl->checkMotionDone(&x);
                                response.addInt(x);
                            }
                            break;

                            case VOCAB_REF_SPEED:
                            {
                                ok = rpc_IPosCtrl->getRefSpeed(cmd.get(2).asInt(), &dtmp);
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
                                ok = rpc_IPosCtrl2->getRefSpeeds(len, jointList, speeds);

                                Bottle& b = response.addList();
                                for (int i = 0; i < len; i++)
                                    b.addDouble(speeds[i]);

                                delete[] jointList;
                                delete[] speeds;
                            }
                            break;

                            case VOCAB_REF_SPEEDS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IPosCtrl->getRefSpeeds(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_REF_ACCELERATION:
                            {
                                ok = rpc_IPosCtrl->getRefAcceleration(cmd.get(2).asInt(), &dtmp);
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
                                ok = rpc_IPosCtrl2->getRefAccelerations(len, jointList, accs);

                                Bottle& b = response.addList();
                                for (int i = 0; i < len; i++)
                                    b.addDouble(accs[i]);

                            delete[] jointList;
                            delete[] accs;
                            }
                            break;

                            case VOCAB_REF_ACCELERATIONS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IPosCtrl->getRefAccelerations(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_ENCODER:
                            {
                                ok = rpc_IEncTimed->getEncoder(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_ENCODERS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IEncTimed->getEncoders(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_ENCODER_SPEED:
                            {
                                ok = rpc_IEncTimed->getEncoderSpeed(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_ENCODER_SPEEDS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IEncTimed->getEncoderSpeeds(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_ENCODER_ACCELERATION:
                            {
                                ok = rpc_IEncTimed->getEncoderAcceleration(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_ENCODER_ACCELERATIONS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IEncTimed->getEncoderAccelerations(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_MOTOR_CPR:
                            {
                                ok = rpc_IMotEnc->getMotorEncoderCountsPerRevolution(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_MOTOR_ENCODER:
                            {
                                ok = rpc_IMotEnc->getMotorEncoder(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_MOTOR_ENCODERS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IMotEnc->getMotorEncoders(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_MOTOR_ENCODER_SPEED:
                            {
                                ok = rpc_IMotEnc->getMotorEncoderSpeed(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_MOTOR_ENCODER_SPEEDS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IMotEnc->getMotorEncoderSpeeds(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_MOTOR_ENCODER_ACCELERATION:
                            {
                                ok = rpc_IMotEnc->getMotorEncoderAcceleration(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_MOTOR_ENCODER_ACCELERATIONS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rpc_IMotEnc->getMotorEncoderAccelerations(p);
                                Bottle& b = response.addList();
                                int i;
                                for (i = 0; i < controlledJoints; i++)
                                    b.addDouble(p[i]);
                                delete[] p;
                            }
                            break;

                            case VOCAB_MOTOR_ENCODER_NUMBER:
                            {
                                int num=0;
                                ok = rpc_IMotEnc->getNumberOfMotorEncoders(&num);
                                response.addInt(num);
                            }
                            break;

                            case VOCAB_AMP_CURRENT:
                            {
                                ok = rcp_IAmp->getCurrent(cmd.get(2).asInt(), &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_AMP_CURRENTS:
                            {
                                double *p = new double[controlledJoints];
                                ok = rcp_IAmp->getCurrents(p);
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
                                ok = rcp_IAmp->getAmpStatus(p);
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
                                ok = rcp_IAmp->getAmpStatus(j, &itmp);
                                response.addInt(itmp);
                            }
                            break;

                            case VOCAB_AMP_NOMINAL_CURRENT:
                            {
                                int m=cmd.get(2).asInt();
                                ok = rcp_IAmp->getNominalCurrent(m, &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_AMP_PEAK_CURRENT:
                            {
                                int m=cmd.get(2).asInt();
                                ok = rcp_IAmp->getPeakCurrent(m, &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_AMP_PWM:
                            {
                                int m=cmd.get(2).asInt();
                                ok = rcp_IAmp->getPWM(m, &dtmp);
                                //yDebug() << "RPC parser::getPWM: j" << m << " val " << dtmp;
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_AMP_PWM_LIMIT:
                            {
                                int m=cmd.get(2).asInt();
                                ok = rcp_IAmp->getPWMLimit(m, &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_AMP_VOLTAGE_SUPPLY:
                            {
                                int m=cmd.get(2).asInt();
                                ok = rcp_IAmp->getPowerSupplyVoltage(m, &dtmp);
                                response.addDouble(dtmp);
                            }
                            break;

                            case VOCAB_LIMITS:
                            {
                                double min = 0.0, max = 0.0;
                                ok = rcp_Ilim2->getLimits(cmd.get(2).asInt(), &min, &max);
                                response.addDouble(min);
                                response.addDouble(max);
                            }
                            break;

                            case VOCAB_VEL_LIMITS:
                            {
                                double min = 0.0, max = 0.0;
                                ok = rcp_Ilim2->getVelLimits(cmd.get(2).asInt(), &min, &max);
                                response.addDouble(min);
                                response.addDouble(max);
                            }
                            break;

                            case VOCAB_INFO_NAME:
                            {
                                ConstString name = "undocumented";
                                ok = rpc_AxisInfo->getAxisName(cmd.get(2).asInt(),name);
                                response.addString(name.c_str());
                            }
                            break;

                            case VOCAB_INFO_TYPE:
                            {
                                 yarp::dev::JointTypeEnum type;
                                 ok = rpc_AxisInfo->getJointType(cmd.get(2).asInt(), type);
                                 response.addInt(type);
                            }
                            break;

                            default:
                            {
                                 yError("received an unknown request after a VOCAB_GET: %s\n", yarp::os::Vocab::decode(cmd.get(1).asVocab()).c_str());
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

    // yDebug("--> [%X] done ret %d\n",self, ok);
    }

        return ok;
}

bool RPCMessagesParser::initialize()
{
    bool ok = false;
    if (rpc_IPosCtrl)
    {
        ok = rpc_IPosCtrl->getAxes(&controlledJoints);
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

RPCMessagesParser::RPCMessagesParser() :
        ControlBoardWrapper_p(nullptr),
        rpc_IPid(nullptr),
        rpc_IPosCtrl(nullptr),
        rpc_IPosCtrl2(nullptr),
        rpc_IPosDirect(nullptr),
        rpc_IVelCtrl(nullptr),
        rpc_IVelCtrl2(nullptr),
        rpc_IEncTimed(nullptr),
        rpc_IMotEnc(nullptr),
        rcp_IAmp(nullptr),
        rcp_Ilim2(nullptr),
        rpc_ITorque(nullptr),
        rpc_iCtrlMode(nullptr),
        rpc_iCtrlMode2(nullptr),
        rpc_AxisInfo(nullptr),
        rpc_IRemoteCalibrator(nullptr),
        rpc_Icalib2(nullptr),
        rpc_IImpedance(nullptr),
        rpc_IInteract(nullptr),
        rpc_IMotor(nullptr),
        rpc_IVar(nullptr),
        rpc_ICurrent(nullptr),
        rpc_IPWM(nullptr),
        controlledJoints(0)
{
}

void RPCMessagesParser::init(ControlBoardWrapper *x)
{
    ControlBoardWrapper_p = x;
    rpc_IPid              = dynamic_cast<yarp::dev::IPidControl *>          (ControlBoardWrapper_p);
    rpc_IPosCtrl          = dynamic_cast<yarp::dev::IPositionControl *>     (ControlBoardWrapper_p);
    rpc_IPosCtrl2         = dynamic_cast<yarp::dev::IPositionControl2 *>    (ControlBoardWrapper_p);
    rpc_IPosDirect        = dynamic_cast<yarp::dev::IPositionDirect *>      (ControlBoardWrapper_p);
    rpc_IVelCtrl          = dynamic_cast<yarp::dev::IVelocityControl *>     (ControlBoardWrapper_p);
    rpc_IVelCtrl2         = dynamic_cast<yarp::dev::IVelocityControl2 *>    (ControlBoardWrapper_p);
    rpc_IEncTimed         = dynamic_cast<yarp::dev::IEncodersTimed *>       (ControlBoardWrapper_p);
    rpc_IMotEnc           = dynamic_cast<yarp::dev::IMotorEncoders *>       (ControlBoardWrapper_p);
    rpc_IMotor            = dynamic_cast<yarp::dev::IMotor *>               (ControlBoardWrapper_p);
    rpc_IVar              = dynamic_cast<yarp::dev::IRemoteVariables *>     (ControlBoardWrapper_p);
    rcp_IAmp              = dynamic_cast<yarp::dev::IAmplifierControl *>    (ControlBoardWrapper_p);
    rcp_Ilim2             = dynamic_cast<yarp::dev::IControlLimits2 *>      (ControlBoardWrapper_p);
    rpc_AxisInfo          = dynamic_cast<yarp::dev::IAxisInfo *>            (ControlBoardWrapper_p);
    rpc_IRemoteCalibrator = dynamic_cast<yarp::dev::IRemoteCalibrator *>    (ControlBoardWrapper_p);
    rpc_Icalib2           = dynamic_cast<yarp::dev::IControlCalibration2 *> (ControlBoardWrapper_p);
    rpc_IImpedance        = dynamic_cast<yarp::dev::IImpedanceControl *>    (ControlBoardWrapper_p);
    rpc_ITorque           = dynamic_cast<yarp::dev::ITorqueControl *>       (ControlBoardWrapper_p);
    rpc_iCtrlMode         = dynamic_cast<yarp::dev::IControlMode *>         (ControlBoardWrapper_p);
    rpc_iCtrlMode2        = dynamic_cast<yarp::dev::IControlMode2 *>        (ControlBoardWrapper_p);
    rpc_IInteract         = dynamic_cast<yarp::dev::IInteractionMode *>     (ControlBoardWrapper_p);
    rpc_ICurrent          = dynamic_cast<yarp::dev::ICurrentControl *>      (ControlBoardWrapper_p);
    rpc_IPWM              = dynamic_cast<yarp::dev::IPWMControl *>          (ControlBoardWrapper_p);
    controlledJoints      = 0;
}
