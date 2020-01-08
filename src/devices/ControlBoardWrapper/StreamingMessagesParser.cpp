/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "StreamingMessagesParser.h"
#include "ControlBoardWrapper.h"
#include <iostream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;
using namespace std;


StreamingMessagesParser::StreamingMessagesParser() :
        stream_IPosCtrl(nullptr),
        stream_IPosDirect(nullptr),
        stream_IVel(nullptr),
        stream_ITorque(nullptr),
        stream_IPWM(nullptr),
        stream_ICurrent(nullptr),
        stream_nJoints(0)
{
}

void StreamingMessagesParser::init(ControlBoardWrapper *x) {
    stream_nJoints = 0;
    stream_IPosCtrl  = dynamic_cast<yarp::dev::IPositionControl *> (x);
    stream_IPosDirect = dynamic_cast<yarp::dev::IPositionDirect *> (x);
    stream_IVel = dynamic_cast<yarp::dev::IVelocityControl *> (x);
    stream_ITorque=dynamic_cast<yarp::dev::ITorqueControl *> (x);
    stream_IPWM = dynamic_cast<yarp::dev::IPWMControl *> (x);
    stream_ICurrent = dynamic_cast<yarp::dev::ICurrentControl *> (x);
}


bool StreamingMessagesParser::initialize()
{
    stream_nJoints=0;
    if (stream_IPosCtrl)
        stream_IPosCtrl->getAxes(&stream_nJoints);

    return true;
}

// streaming port callback
void StreamingMessagesParser::onRead(CommandMessage& v)
{
    Bottle& b = v.head;
    Vector& cmdVector = v.body;

    //Use the following only for debug, since it can heavily slow down the system
    //yDebug("Received command %s, %s\n", b.toString().c_str(), cmdVector.toString().c_str());

    // some consistency checks
    if ((int)cmdVector.size() > stream_nJoints)
    {
        std::string str = yarp::os::Vocab::decode(b.get(0).asVocab());
        yError("Received command vector with number of elements bigger than axis controlled by this wrapper (cmd: %s requested jnts: %d received jnts: %d)\n",str.c_str(),stream_nJoints,(int)cmdVector.size());
        return;
    }
    if (cmdVector.data()==nullptr)
    {
         yError("Received null command vector");
         return;
    }

    switch (b.get(0).asVocab())
    {
        // manage commands with interface name as first
        case VOCAB_PWMCONTROL_INTERFACE:
        {
            switch (b.get(1).asVocab())
            {
                case VOCAB_PWMCONTROL_REF_PWM:
                {
                    if (stream_IPWM)
                    {
                        bool ok = stream_IPWM->setRefDutyCycle(b.get(2).asInt32(), cmdVector[0]);
                        if (!ok)
                            yError("Errors while trying to command an pwm message");
                    }
                    else
                        yError("PWM interface not valid");
                }
                break;
                case VOCAB_PWMCONTROL_REF_PWMS:
                {
                    if (stream_IPWM)
                    {
                        bool ok = stream_IPWM->setRefDutyCycles(cmdVector.data());
                        if (!ok)
                            yError("Errors while trying to command an pwm message");
                    }
                    else
                        yError("PWM interface not valid");
                }
                break;
            }
        }
        break;

        case VOCAB_CURRENTCONTROL_INTERFACE:
        {
            switch (b.get(1).asVocab())
            {
                case VOCAB_CURRENT_REF:
                {
                    if (stream_ICurrent)
                    {
                        bool ok = stream_ICurrent->setRefCurrent(b.get(2).asInt32(), cmdVector[0]);
                        if (!ok)
                        {
                            yError("Errors while trying to command a streaming current message on single joint\n");
                        }
                    }
                }
                break;
                case VOCAB_CURRENT_REFS:
                {
                    if (stream_ICurrent)
                    {
                        bool ok = stream_ICurrent->setRefCurrents(cmdVector.data());
                        if (!ok)
                        {
                            yError("Errors while trying to command a streaming current message on all joints\n");
                        }
                    }
                }
                break;
                case VOCAB_CURRENT_REF_GROUP:
                {
                    if (stream_ICurrent)
                    {
                        int n_joints = b.get(2).asInt32();
                        Bottle *jlut = b.get(3).asList();
                        if (((int)jlut->size() != n_joints) && ((int)cmdVector.size() != n_joints))
                        {
                            yError("Received VOCAB_CURRENT_REF_GROUP size of joints vector or currents vector does not match the selected joint number\n");
                        }

                        int *joint_list = new int[n_joints];
                        for (int i = 0; i < n_joints; i++)
                            joint_list[i] = jlut->get(i).asInt32();


                        bool ok = stream_ICurrent->setRefCurrents(n_joints, joint_list, cmdVector.data());
                        if (!ok)
                        {
                            yError("Error while trying to command a streaming current message on joint group\n");
                        }

                        delete[] joint_list;
                    }
                }
                break;
                default:
                {
                    std::string str = yarp::os::Vocab::decode(b.get(0).asVocab());
                    yError("Unrecognized message while receiving on command port (%s)\n", str.c_str());
                }
                break;
            }
        }
        break;

        // fallback to commands without interface name
        case VOCAB_POSITION_MODE:
            {
                yError("Received VOCAB_POSITION_MODE this is an send invalid message on streaming port");
                break;
            }
        case VOCAB_POSITION_MOVES:
            {
                if (stream_IPosCtrl)
                    {
                        bool ok = stream_IPosCtrl->positionMove(cmdVector.data());
                        if (!ok)
                            yError("Errors while trying to start a position move");
                    }

            }
            break;

        case VOCAB_VELOCITY_MODE:
             {
                yError("Received VOCAB_VELOCITY_MODE this is an send invalid message on streaming port");
                break;
            }

        case VOCAB_VELOCITY_MOVE:
            {
                stream_IVel->velocityMove(b.get(1).asInt32(), cmdVector[0]);
            }
        break;

        case VOCAB_VELOCITY_MOVES:
            {
                if (stream_IVel)
                    {
                        bool ok = stream_IVel->velocityMove(cmdVector.data());
                        if (!ok)
                            yError("Errors while trying to start a velocity move");
                    }
            }
            break;

        case VOCAB_POSITION_DIRECT:
        {
            if(stream_IPosDirect)
            {
                bool ok = stream_IPosDirect->setPosition(b.get(1).asInt32(), cmdVector[0]); // cmdVector.data());
                if (!ok)
                {   yError("Errors while trying to command an streaming position direct message on joint %d\n", b.get(1).asInt32() ); }
            }
        }
        break;

        case VOCAB_TORQUES_DIRECT:
        {
            if (stream_ITorque)
            {
                bool ok = stream_ITorque->setRefTorque(b.get(1).asInt32(), cmdVector[0]);
                if (!ok)
                {   yError("Errors while trying to command a streaming torque direct message on single joint\n"); }
            }
        }
        break;

        case VOCAB_TORQUES_DIRECTS:
        {
            if (stream_ITorque)
            {
                bool ok = stream_ITorque->setRefTorques(cmdVector.data());
                if (!ok)
                {   yError("Errors while trying to command a streaming torque direct message on all joints\n"); }
            }
        }
        break;

        case VOCAB_TORQUES_DIRECT_GROUP:
        {
            if (stream_ITorque)
            {
                int n_joints = b.get(1).asInt32();
                Bottle *jlut = b.get(2).asList();
                if( ((int)jlut->size() != n_joints) && ((int)cmdVector.size() != n_joints) )
                {
                    yError("Received VOCAB_TORQUES_DIRECT_GROUP size of joints vector or torques vector does not match the selected joint number\n" );
                }

                int *joint_list = new int[n_joints];
                for (int i = 0; i < n_joints; i++)
                    joint_list[i] = jlut->get(i).asInt32();


                bool ok = stream_ITorque->setRefTorques(n_joints, joint_list, cmdVector.data());
                if (!ok)
                {   yError("Error while trying to command a streaming toruqe direct message on joint group\n" ); }

                delete[] joint_list;
            }
        }
        break;

        case VOCAB_POSITION_DIRECT_GROUP:
        {
            if(stream_IPosDirect)
            {
                int n_joints = b.get(1).asInt32();
                Bottle *jlut = b.get(2).asList();
                if( ((int)jlut->size() != n_joints) && ((int)cmdVector.size() != n_joints) )
                {
                    yError("Received VOCAB_POSITION_DIRECT_GROUP size of joints vector or positions vector does not match the selected joint number\n" );
                }

                int *joint_list = new int[n_joints];
                for (int i = 0; i < n_joints; i++)
                    joint_list[i] = jlut->get(i).asInt32();


                bool ok = stream_IPosDirect->setPositions(n_joints, joint_list, cmdVector.data());
                if (!ok)
                {   yError("Error while trying to command a streaming position direct message on joint group\n" ); }

                delete[] joint_list;
            }
        }
        break;

        case VOCAB_POSITION_DIRECTS:
        {
            if(stream_IPosDirect)
            {
                bool ok = stream_IPosDirect->setPositions(cmdVector.data());
                if (!ok)
                {   yError("Error while trying to command a streaming position direct message on all joints\n" ); }
            }
        }
        break;
        case VOCAB_VELOCITY_MOVE_GROUP:
        {
            if(stream_IVel)
            {
                int n_joints = b.get(1).asInt32();
                Bottle *jlut = b.get(2).asList();
                if( ((int)jlut->size() != n_joints) && ((int)cmdVector.size() != n_joints) )
                    yError("Received VOCAB_VELOCITY_MOVE_GROUP size of joints vector or positions vector does not match the selected joint number\n" );

                int *joint_list = new int[n_joints];
                for (int i = 0; i < n_joints; i++)
                    joint_list[i] = jlut->get(i).asInt32();

                bool ok = stream_IVel->velocityMove(n_joints, joint_list, cmdVector.data());
                if (!ok)
                {   yError("Error while trying to command a velocity move on joint group\n" ); }

                delete[] joint_list;
            }
        }
        break;

        default:
            {
                std::string str = yarp::os::Vocab::decode(b.get(0).asVocab());
                yError("Unrecognized message while receiving on command port (%s)\n",str.c_str());
            }
            break;
        }
}
