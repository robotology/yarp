/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "StreamingMessagesParser.h"

#include <yarp/os/LogStream.h>

#include "ControlBoardLogComponent.h"
#include <iostream>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;


void StreamingMessagesParser::init(yarp::dev::DeviceDriver* x)
{
    stream_nJoints = 0;
    x->view(stream_IPosCtrl);
    x->view(stream_IPosDirect);
    x->view(stream_IVelDirect);
    x->view(stream_IVel);
    x->view(stream_ITorque);
    x->view(stream_IPWM);
    x->view(stream_ICurrent);
    x->view(stream_IAxis);
    x->view(stream_IPid);
}

void StreamingMessagesParser::reset()
{
    stream_nJoints = 0;
    stream_IPosCtrl = nullptr;
    stream_IPosDirect = nullptr;
    stream_IVel = nullptr;
    stream_IVelDirect = nullptr;
    stream_ITorque = nullptr;
    stream_IPWM = nullptr;
    stream_ICurrent = nullptr;
    stream_IAxis = nullptr;
    stream_IPid = nullptr;
}

bool StreamingMessagesParser::initialize()
{
    stream_nJoints = 0;
    if (stream_IPosCtrl) { stream_IPosCtrl->getAxes(&stream_nJoints); return true; }
    if (stream_IVel)     { stream_IVel->getAxes(&stream_nJoints); return true; }
    if (stream_ITorque)  { stream_ITorque->getAxes(&stream_nJoints); return true; }
    if (stream_IAxis)    { stream_IAxis->getAxes(&stream_nJoints); return true; }

    yCError(CONTROLBOARD, "Unable to get number of joints");
    return false;
}

// streaming port callback
void StreamingMessagesParser::onRead(CommandMessage& v)
{
    Bottle& b = v.head;
    Vector& cmdVector = v.body;

    //Use the following only for debug, since it can heavily slow down the system
    yCTrace(CONTROLBOARD, "Received command %s, %s\n", b.toString().c_str(), cmdVector.toString().c_str());

    // some consistency checks
    if (static_cast<int>(cmdVector.size()) > stream_nJoints) {
        std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
        yCError(CONTROLBOARD, "Received command vector with number of elements bigger than axis controlled by this wrapper (cmd: %s requested jnts: %d received jnts: %d)\n", str.c_str(), stream_nJoints, (int)cmdVector.size());
        return;
    }
    if (cmdVector.data() == nullptr) {
        yCError(CONTROLBOARD, "Received null command vector");
        return;
    }

    switch (b.get(0).asVocab32()) {
    // manage commands with interface name as first...
    // PWM control
    case VOCAB_PWMCONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_PWMCONTROL_REF_PWM: {
                if (stream_IPWM) {
                    int joint = b.get(2).asInt32();
                    bool ok = stream_IPWM->setRefDutyCycle(joint, cmdVector[0]);
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to command an pwm message");
                    }
                } else {
                    yCError(CONTROLBOARD, "PWM interface not valid");
                }
            } break;
            case VOCAB_PWMCONTROL_REF_PWMS: {
                if (stream_IPWM) {
                    bool ok = stream_IPWM->setRefDutyCycles(cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to command an pwm message");
                    }
                } else {
                    yCError(CONTROLBOARD, "PWM interface not valid");
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // PID control
    case VOCAB_PIDCONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_PIDCONTROL_SET_OFFSET: {
                if (stream_IPid) {
                    PidControlTypeEnum pidtype = PidControlTypeEnum(b.get(2).asVocab32());
                    int joint = b.get(3).asInt32();
                    double off = cmdVector[0];
                    bool ok = stream_IPid->setPidOffset(pidtype, joint, off);
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to execute streaming command setPidOffset()\n");
                    }
                }
            } break;
            case VOCAB_PIDCONTROL_SET_FEEDFORWARD: {
                if (stream_IPid) {
                    PidControlTypeEnum pidtype = PidControlTypeEnum(b.get(2).asVocab32());
                    int joint = b.get(3).asInt32();
                    double ffd = cmdVector[0];
                    bool ok = stream_IPid->setPidFeedforward(pidtype, joint, ffd);
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to execute streaming command setPidFeedforward()\n");
                    }
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // Current control
    case VOCAB_CURRENTCONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_CURRENT_REF: {
                if (stream_ICurrent) {
                    int joint = b.get(2).asInt32();
                    bool ok = stream_ICurrent->setRefCurrent(joint, cmdVector[0]);
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to execute streaming command setRefCurrent()\n");
                    }
                }
            } break;
            case VOCAB_CURRENT_REFS: {
                if (stream_ICurrent) {
                    bool ok = stream_ICurrent->setRefCurrents(cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to execute streaming command setRefCurrents()\n");
                    }
                }
            } break;
            case VOCAB_CURRENT_REF_GROUP: {
                if (stream_ICurrent) {
                    int n_joints = b.get(2).asInt32();
                    Bottle* jlut = b.get(3).asList();
                    if ((static_cast<int>(jlut->size()) != n_joints) && (static_cast<int>(cmdVector.size()) != n_joints)) {
                        yCError(CONTROLBOARD, "Received VOCAB_CURRENT_REF_GROUP size of joints vector or currents vector does not match the selected joint number\n");
                    }

                    int* joint_list = new int[n_joints];
                    for (int i = 0; i < n_joints; i++) {
                        joint_list[i] = jlut->get(i).asInt32();
                    }


                    bool ok = stream_ICurrent->setRefCurrents(n_joints, joint_list, cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to execute streaming command setRefCurrents() group\n");
                    }

                    delete[] joint_list;
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // Torque control
    case VOCAB_TORQUECONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_TORQUES_DIRECT: {
                if (stream_ITorque) {
                    int joint = b.get(2).asInt32();
                    bool ok = stream_ITorque->setRefTorque(joint, cmdVector[0]);
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to command a streaming torque direct message on single joint\n");
                    }
                }
            } break;
            case VOCAB_TORQUES_DIRECTS: {
                if (stream_ITorque) {
                    bool ok = stream_ITorque->setRefTorques(cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to command a streaming torque direct message on all joints\n");
                    }
                }
            } break;
            case VOCAB_TORQUES_DIRECT_GROUP: {
                if (stream_ITorque) {
                    int n_joints = b.get(2).asInt32();
                    Bottle* jlut = b.get(3).asList();
                    if ((static_cast<int>(jlut->size()) != n_joints) && (static_cast<int>(cmdVector.size()) != n_joints)) {
                        yCError(CONTROLBOARD, "Received VOCAB_TORQUES_DIRECT_GROUP size of joints vector or torques vector does not match the selected joint number\n");
                    }

                    int* joint_list = new int[n_joints];
                    for (int i = 0; i < n_joints; i++) {
                        joint_list[i] = jlut->get(i).asInt32();
                    }
                    bool ok = stream_ITorque->setRefTorques(n_joints, joint_list, cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Error while trying to command a streaming toruqe direct message on joint group\n");
                    }
                    delete[] joint_list;
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // Position control
    case VOCAB_POSITIONCONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_POSITION_MOVES: {
                if (stream_IPosCtrl) {
                    bool ok = stream_IPosCtrl->positionMove(cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to start a position move");
                    }
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // Position direct control
    case VOCAB_POSITIONDIRECTCONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_POSITION_DIRECT: {
                if (stream_IPosDirect) {
                    int joint = b.get(2).asInt32();
                    bool ok = stream_IPosDirect->setPosition(joint, cmdVector[0]); // cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to command an streaming position direct message on joint %d\n", b.get(1).asInt32());
                    }
                }
            } break;
            case VOCAB_POSITION_DIRECT_GROUP: {
                if (stream_IPosDirect) {
                    int n_joints = b.get(2).asInt32();
                    Bottle* jlut = b.get(3).asList();
                    if ((static_cast<int>(jlut->size()) != n_joints) && (static_cast<int>(cmdVector.size()) != n_joints)) {
                        yCError(CONTROLBOARD, "Received VOCAB_POSITION_DIRECT_GROUP size of joints vector or positions vector does not match the selected joint number\n");
                    }
                    int* joint_list = new int[n_joints];
                    for (int i = 0; i < n_joints; i++) {
                        joint_list[i] = jlut->get(i).asInt32();
                    }
                    bool ok = stream_IPosDirect->setPositions(n_joints, joint_list, cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Error while trying to command a streaming position direct message on joint group\n");
                    }
                    delete[] joint_list;
                }
            } break;
            case VOCAB_POSITION_DIRECTS: {
                if (stream_IPosDirect) {
                    bool ok = stream_IPosDirect->setPositions(cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Error while trying to command a streaming position direct message on all joints\n");
                    }
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // Velocity control
    case VOCAB_VELOCITYCONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_VELOCITY_MOVE: {
                if (stream_IVel) {
                   int joint = b.get(2).asInt32();
                   bool ok = stream_IVel->velocityMove(joint, cmdVector[0]);
                   if (!ok) {
                       yCError(CONTROLBOARD, "Errors while trying to start a velocity move");
                   }
                }
            } break;

            case VOCAB_VELOCITY_MOVES: {
                if (stream_IVel) {
                    bool ok = stream_IVel->velocityMove(cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to start a velocity move");
                    }
                }
            } break;
            case VOCAB_VELOCITY_MOVE_GROUP: {
                if (stream_IVel) {
                    int n_joints = b.get(2).asInt32();
                    Bottle* jlut = b.get(3).asList();
                    if ((static_cast<int>(jlut->size()) != n_joints) && (static_cast<int>(cmdVector.size()) != n_joints)) {
                        yCError(CONTROLBOARD, "Received VOCAB_VELOCITY_MOVE_GROUP size of joints vector or positions vector does not match the selected joint number\n");
                    }

                    int* joint_list = new int[n_joints];
                    for (int i = 0; i < n_joints; i++) {
                        joint_list[i] = jlut->get(i).asInt32();
                    }

                    bool ok = stream_IVel->velocityMove(n_joints, joint_list, cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Error while trying to command a velocity move on joint group\n");
                    }

                    delete[] joint_list;
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // Velocity direct control
    case VOCAB_VELOCITYDIRECTCONTROL_INTERFACE: {
        switch (b.get(1).asVocab32()) {
            case VOCAB_VELOCITY_DIRECT_SET_ONE: {
                if (stream_IVelDirect) {
                    int joint = b.get(2).asInt32();
                    bool ok = stream_IVelDirect->setRefVelocity(joint, cmdVector[0]); // cmdVector.data());
                    if (!ok) {
                        yCError(CONTROLBOARD, "Errors while trying to command an streaming velocity direct message on joint %d\n", joint);
                    }
                }
            } break;
            case VOCAB_VELOCITY_DIRECT_SET_GROUP: {
                if (stream_IVelDirect) {
                    int n_joints = b.get(2).asInt32();
                    Bottle* jlut = b.get(3).asList();
                    if ((static_cast<int>(jlut->size()) != n_joints) &&
                        (static_cast<int>(cmdVector.size()) != n_joints)) {
                        yCError(CONTROLBOARD, "Received VOCAB_VELOCITY_DIRECT_SET_GROUP size of joints vector or positions vector does not match the selected joint number\n");
                    }
                    std::vector<int> joint_list(n_joints);
                    for (int i = 0; i < n_joints; i++) {
                        joint_list[i] = jlut->get(i).asInt32();
                    }
                    std::vector<double> vel_list(n_joints);
                    for (int i = 0; i < n_joints; i++) {
                        vel_list[i] = cmdVector[i];
                    }
                    bool ok = stream_IVelDirect->setRefVelocity(joint_list, vel_list);
                    if (!ok) {
                        yCError(CONTROLBOARD, "Error while trying to command a streaming position direct message on joint group\n");
                    }
                }
            } break;
            case VOCAB_VELOCITY_DIRECT_SET_ALL: {
                if (stream_IVelDirect) {
                    int n_joints = cmdVector.size();
                    std::vector<double> vel_list(n_joints);
                    for (int i = 0; i < n_joints; i++) {
                        vel_list[i] = cmdVector[i];
                    }
                    bool ok = stream_IVelDirect->setRefVelocity(vel_list);
                    if (!ok) {
                        yCError(CONTROLBOARD, "Error while trying to command a streaming position direct message on all joints\n");
                    }
                }
            } break;
            default:
            {
                std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
                yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
            } break;
        }
    } break;

    // fallback error
    default:
    {
        std::string str = yarp::os::Vocab32::decode(b.get(0).asVocab32());
        yCError(CONTROLBOARD, "Unrecognized message while receiving on command port (%s)\n", str.c_str());
    } break;
    }
}
