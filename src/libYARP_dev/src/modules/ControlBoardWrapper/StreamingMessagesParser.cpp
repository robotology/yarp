// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "ControlBoardWrapper.h"
#include "StreamingMessagesParser.h"
#include <iostream>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;
using namespace std;


StreamingMessagesParser::StreamingMessagesParser() {}

void StreamingMessagesParser::init(ControlBoardWrapper *x) {
    stream_nJoints = 0;
    stream_IPosCtrl  = dynamic_cast<yarp::dev::IPositionControl *> (x);
    stream_IPosCtrl2 = dynamic_cast<yarp::dev::IPositionControl2 *> (x);
    stream_IPosDirect = dynamic_cast<yarp::dev::IPositionDirect *> (x);
    stream_IVel = dynamic_cast<yarp::dev::IVelocityControl *> (x);
    stream_IVel2 = dynamic_cast<yarp::dev::IVelocityControl2 *> (x);
    stream_IOpenLoop=dynamic_cast<yarp::dev::IOpenLoopControl *> (x);
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
    //fprintf(stderr, "Received command %s, %s\n", b.toString().c_str(), cmdVector.toString().c_str());

    // some consistency checks
    if ((int)cmdVector.size() > stream_nJoints)
    {
        yarp::os::ConstString str = yarp::os::Vocab::decode(b.get(0).asVocab());
        fprintf(stderr, "Received command vector with number of elements bigger than axis controlled by this wrapper (cmd: %s requested jnts: %d received jnts: %d)\n",str.c_str(),stream_nJoints,(int)cmdVector.size());
        return;
    }
    if (cmdVector.data()==0)
    {
         fprintf(stderr, "Errors: received null command vector\n");
         return;
    }

    switch (b.get(0).asVocab())
    {
        // manage commands with interface name as first
        case VOCAB_OPENLOOP_INTERFACE:
        {
            switch(b.get(1).asVocab())
            {
                case VOCAB_OPENLOOP_REF_OUTPUT:
                {
                    if (stream_IOpenLoop)
                    {
                        bool ok = stream_IOpenLoop->setRefOutput(b.get(2).asVocab(), cmdVector[0]);
                        if (!ok)
                            fprintf(stderr, "Errors while trying to command an open loop message\n");
                    }
                    else
                        fprintf(stderr, "OpenLoop interface not valid\n");
                }
                break;

                case VOCAB_OPENLOOP_REF_OUTPUTS:
                {
                    if (stream_IOpenLoop)
                    {
                        bool ok=stream_IOpenLoop->setRefOutputs(cmdVector.data());
                        if (!ok)
                            fprintf(stderr, "Errors while trying to command an open loop message\n");
                    }
                    else
                        fprintf(stderr, "OpenLoop interface not valid\n");
                }
                break;
            }
            break;
        }
        break;

        // fallback to commands without interface name
        case VOCAB_POSITION_MODE:
            {
                fprintf(stderr, "Warning: received VOCAB_POSITION_MODE this is an send invalid message on streaming port\n");
                break;
            }
        case VOCAB_POSITION_MOVES:
            {
                if (stream_IPosCtrl)
                    {
                        bool ok = stream_IPosCtrl->positionMove(cmdVector.data());
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
                if (stream_IVel)
                    {
                        bool ok = stream_IVel->velocityMove(cmdVector.data());
                        if (!ok)
                            fprintf(stderr, "Errors while trying to start a velocity move\n");
                    }
            }
            break;

        case VOCAB_OUTPUTS:
            {
                std::cout << "DEPRECATED openloop setOutputS!! missing interface name! Check you are using the updated RemoteControlBoard class" << std::endl;
                std::cout << "Correct message should be [" << Vocab::decode(VOCAB_OPENLOOP_INTERFACE) << "] [" << Vocab::decode(VOCAB_OPENLOOP_REF_OUTPUTS) << "] list_if_values" << std::endl;
            }
            break;

        case VOCAB_POSITION_DIRECT:
        {
            if(stream_IPosDirect)
            {
                int temp_j = b.get(1).asInt();
                double temp_val = cmdVector.operator [](0);
                bool ok = stream_IPosDirect->setPosition(b.get(1).asInt(), cmdVector.operator [](0)); // cmdVector.data());
                if (!ok)
                {   fprintf(stderr, "Errors while trying to command an streaming position direct message on joint %d\n", b.get(1).asInt() ); }
            }
        }
        break;

        case VOCAB_POSITION_DIRECT_GROUP:
        {
            if(stream_IPosDirect)
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


                bool ok = stream_IPosDirect->setPositions(n_joints, joint_list, cmdVector.data());
                if (!ok)
                {   fprintf(stderr, "Error while trying to command a streaming position direct message on joint group\n" ); }

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
                {   fprintf(stderr, "Error while trying to command a streaming position direct message on all joints\n" ); }
            }
        }
        break;
        case VOCAB_VELOCITY_MOVE_GROUP:
        {
            if(stream_IVel2)
            {
                int n_joints = b.get(1).asInt();
                Bottle *jlut = b.get(2).asList();
                if( (jlut->size() != n_joints) && (cmdVector.size() != n_joints) )
                    fprintf(stderr, "Received VOCAB_VELOCITY_MOVE_GROUP size of joints vector or positions vector does not match the selected joint number\n" );

                int *joint_list = new int[n_joints];
                for (int i = 0; i < n_joints; i++)
                    joint_list[i] = jlut->get(i).asInt();

                bool ok = stream_IVel2->velocityMove(n_joints, joint_list, cmdVector.data());
                if (!ok)
                {   fprintf(stderr, "Error while trying to command a velocity move on joint group\n" ); }

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


