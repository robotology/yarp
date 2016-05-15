/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Lorenzo Natale, Silvio Traversaro
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include "ControlBoardRemapperHelpers.h"
#include <iostream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <cassert>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace std;


RemappedSubControlBoard::RemappedSubControlBoard()
{
    id = "";

    pid = 0;
    pos2 = 0;
    posDir = 0;
    vel2 = 0;
    iJntEnc = 0;
    iMotEnc = 0;
    amp = 0;
    lim2 = 0;
    calib = 0;
    calib2 = 0;
    iTimed= 0;
    info = 0;
    iOpenLoop=0;
    iTorque=0;
    iImpedance=0;
    iMode2=0;
    iInteract=0;

    subdevice=0;

    attachedF=false;
    _subDevVerbose = false;
}


void RemappedSubControlBoard::detach()
{
    subdevice=0;

    pid=0;
    pos2=0;
    posDir=0;
    vel2=0;
    amp = 0;
    iJntEnc=0;
    iMotEnc=0;
    lim2=0;
    calib=0;
    calib2=0;
    info=0;
    iTorque=0;
    iImpedance=0;
    iMode2=0;
    iTimed=0;
    iOpenLoop=0;
    iInteract=0;
    iVar = 0;
    attachedF=false;
}

bool RemappedSubControlBoard::attach(yarp::dev::PolyDriver *d, const std::string &k)
{
    if (id!=k)
    {
        yError()<<"ControlBoardRemapper: Wrong device" << k.c_str();
        return false;
    }

    if (d==0)
    {
        yError()<<"ControlBoardRemapper: Invalid device (null pointer)";
        return false;
    }

    subdevice=d;

    if (subdevice->isValid())
    {
        subdevice->view(pid);
        subdevice->view(pos2);
        subdevice->view(posDir);
        subdevice->view(vel2);
        subdevice->view(amp);
        subdevice->view(lim2);
        subdevice->view(calib);
        subdevice->view(calib2);
        subdevice->view(remcalib);
        subdevice->view(info);
        subdevice->view(iTimed);
        subdevice->view(iTorque);
        subdevice->view(iImpedance);
        subdevice->view(iMode2);
        subdevice->view(iOpenLoop);
        subdevice->view(iJntEnc);
        subdevice->view(iMotEnc);
        subdevice->view(iInteract);
        subdevice->view(imotor);
        subdevice->view(iVar);
    }
    else
    {
        yError()<<"ControlBoardRemapper: Invalid device " << k << " (isValid() returned false)";
        return false;
    }

    if ((iTorque==0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning iTorque not valid interface";
    }

    if ((iImpedance==0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning iImpedance not valid interface";
    }

    if ((iOpenLoop==0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning iOpenLoop not valid interface";
    }

    if ((iInteract==0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning iInteractionMode not valid interface";
    }

    if ((iMotEnc==0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning IMotorEncoders not valid interface";
    }

    if ((imotor==0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning IMotor not valid interface";
    }

    if ((iVar == 0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning IRemoveVariables not valid interface";
    }

    if ((info == 0) && (_subDevVerbose))
    {
        yWarning() << "ControlBoardRemapper:  Warning IAxisInfo not valid interface";
    }


    // checking minimum set of intefaces required
    if( !(pos2) )
    {
        yError("ControlBoardRemapper: IPositionControl2 interface was not found in subdevice. Quitting");
        return false;
    }

    if( ! (vel2) )
    {
        yError("ControlBoardRemapper: IVelocityControl2 interface was not found in subdevice. Quitting");
        return false;
    }

    if(!iJntEnc)
    {
        yError("ControlBoardRemapper: IEncoderTimed interface was not found in subdevice, exiting.");
        return false;
    }

    if(!iMode2)
    {
        yError("ControlBoardRemapper: IControlMode2 interface was not found in subdevice, exiting.");
        return false;
    }

    int deviceJoints=0;
    if (pos2!=0)
    {
        if (!pos2->getAxes(&deviceJoints))
        {
            yError() << "ControlBoardRemapper: failed to get axes number for subdevice " << k.c_str();
            return false;
        }
        if(deviceJoints <= 0)
        {
            yError("ControlBoardRemapper: attached device has an invalid number of joints (%d)", deviceJoints);
            return false;
        }
    }

    attachedF=true;
    return true;
}

bool ControlBoardSubControlBoardAxesDecomposition::configure(const RemappedControlBoards& remappedControlBoards)
{
    // Resize buffers
    m_nrOfControlledAxesInRemappedCtrlBrd = remappedControlBoards.getNrOfRemappedAxes();

    size_t nrOfSubControlBoards = remappedControlBoards.getNrOfSubControlBoards();

    m_nJointsInSubControlBoard.resize(nrOfSubControlBoards,0);
    m_jointsInSubControlBoard.resize(nrOfSubControlBoards);

    m_bufferForSubControlBoard.resize(nrOfSubControlBoards);
    m_bufferForSubControlBoardControlModes.resize(nrOfSubControlBoards);
    m_bufferForSubControlBoardInteractionModes.resize(nrOfSubControlBoards);

    m_counterForControlBoard.resize(nrOfSubControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < nrOfSubControlBoards; ctrlBrd++)
    {
        m_nJointsInSubControlBoard[ctrlBrd] = 0;
        m_jointsInSubControlBoard[ctrlBrd].clear();
        m_bufferForSubControlBoard[ctrlBrd].clear();
        m_bufferForSubControlBoardControlModes[ctrlBrd].clear();
        m_bufferForSubControlBoardInteractionModes[ctrlBrd].clear();
    }

    // Fill buffers
    for(size_t j=0; j < remappedControlBoards.getNrOfRemappedAxes(); j++)
    {
        int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

        m_nJointsInSubControlBoard[subIndex]++;
        m_jointsInSubControlBoard[subIndex].push_back(off);
    }

    // Reserve enough space in buffers
    for(size_t ctrlBrd=0; ctrlBrd < nrOfSubControlBoards; ctrlBrd++)
    {
        m_bufferForSubControlBoard[ctrlBrd].reserve(m_nJointsInSubControlBoard[ctrlBrd]);
        m_bufferForSubControlBoardControlModes[ctrlBrd].reserve(m_nJointsInSubControlBoard[ctrlBrd]);
        m_bufferForSubControlBoardInteractionModes[ctrlBrd].reserve(m_nJointsInSubControlBoard[ctrlBrd]);

        m_counterForControlBoard[ctrlBrd] = 0;
    }

    return true;
}



void ControlBoardSubControlBoardAxesDecomposition::fillSubControlBoardBuffersFromCompleteJointVector(const double* full, const RemappedControlBoards & remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_bufferForSubControlBoard[ctrlBrd].clear();
    }

    for(int j=0; j < m_nrOfControlledAxesInRemappedCtrlBrd; j++)
    {
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

        m_bufferForSubControlBoard[subIndex].push_back(full[j]);
    }
}

void ControlBoardSubControlBoardAxesDecomposition::fillCompleteJointVectorFromSubControlBoardBuffers(double* full, const RemappedControlBoards& remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_counterForControlBoard[ctrlBrd] = 0;
    }

    for(int j=0; j < m_nrOfControlledAxesInRemappedCtrlBrd; j++)
    {
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;
        full[j] = m_bufferForSubControlBoard[subIndex][m_counterForControlBoard[subIndex]];
        m_counterForControlBoard[subIndex]++;
    }
}

void ControlBoardSubControlBoardAxesDecomposition::fillSubControlBoardBuffersFromCompleteJointVector(const int* full, const RemappedControlBoards & remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_bufferForSubControlBoardControlModes[ctrlBrd].clear();
    }

    for(int j=0; j < m_nrOfControlledAxesInRemappedCtrlBrd; j++)
    {
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

        m_bufferForSubControlBoardControlModes[subIndex].push_back(full[j]);
    }
}

void ControlBoardSubControlBoardAxesDecomposition::fillCompleteJointVectorFromSubControlBoardBuffers(int* full, const RemappedControlBoards& remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_counterForControlBoard[ctrlBrd] = 0;
    }

    for(int j=0; j < m_nrOfControlledAxesInRemappedCtrlBrd; j++)
    {
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;
        full[j] = m_bufferForSubControlBoardControlModes[subIndex][m_counterForControlBoard[subIndex]];
        m_counterForControlBoard[subIndex]++;
    }
}

void ControlBoardSubControlBoardAxesDecomposition::fillSubControlBoardBuffersFromCompleteJointVector(const InteractionModeEnum* full, const RemappedControlBoards & remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_bufferForSubControlBoardInteractionModes[ctrlBrd].clear();
    }

    for(int j=0; j < m_nrOfControlledAxesInRemappedCtrlBrd; j++)
    {
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

        m_bufferForSubControlBoardInteractionModes[subIndex].push_back(full[j]);
    }
}

void ControlBoardSubControlBoardAxesDecomposition::fillCompleteJointVectorFromSubControlBoardBuffers(InteractionModeEnum* full, const RemappedControlBoards& remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_counterForControlBoard[ctrlBrd] = 0;
    }

    for(int j=0; j < m_nrOfControlledAxesInRemappedCtrlBrd; j++)
    {
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;
        full[j] = m_bufferForSubControlBoardInteractionModes[subIndex][m_counterForControlBoard[subIndex]];
        m_counterForControlBoard[subIndex]++;
    }
}

bool ControlBoardArbitraryAxesDecomposition::configure(const RemappedControlBoards& remappedControlBoards)
{
    // Resize buffers
    size_t nrOfSubControlBoards = remappedControlBoards.getNrOfSubControlBoards();

    m_nJointsInSubControlBoard.resize(nrOfSubControlBoards,0);
    m_jointsInSubControlBoard.resize(nrOfSubControlBoards);
    m_bufferForSubControlBoard.resize(nrOfSubControlBoards);
    m_bufferForSubControlBoardControlModes.resize(nrOfSubControlBoards);
    m_bufferForSubControlBoardInteractionModes.resize(nrOfSubControlBoards);

    m_counterForControlBoard.resize(nrOfSubControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < nrOfSubControlBoards; ctrlBrd++)
    {
        m_jointsInSubControlBoard[ctrlBrd].clear();
        m_bufferForSubControlBoard[ctrlBrd].clear();
        m_bufferForSubControlBoardControlModes[ctrlBrd].clear();
        m_bufferForSubControlBoardInteractionModes[ctrlBrd].clear();

    }

    // Count the maximum number of joints
    for(size_t j=0; j < remappedControlBoards.getNrOfRemappedAxes(); j++)
    {
        size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

        m_nJointsInSubControlBoard[subIndex]++;
    }

    // Reserve enough space in buffers
    for(size_t ctrlBrd=0; ctrlBrd < nrOfSubControlBoards; ctrlBrd++)
    {
        m_bufferForSubControlBoard[ctrlBrd].reserve(m_nJointsInSubControlBoard[ctrlBrd]);
        m_bufferForSubControlBoardControlModes[ctrlBrd].reserve(m_nJointsInSubControlBoard[ctrlBrd]);
        m_bufferForSubControlBoardInteractionModes[ctrlBrd].reserve(m_nJointsInSubControlBoard[ctrlBrd]);

        m_counterForControlBoard[ctrlBrd] = 0;
        m_jointsInSubControlBoard[ctrlBrd].reserve(m_nJointsInSubControlBoard[ctrlBrd]);
    }

    return true;
}



void ControlBoardArbitraryAxesDecomposition::fillArbitraryJointVectorFromSubControlBoardBuffers(double* arbitraryVec,
                                                                                                const int n_joints,
                                                                                                const int *joints,
                                                                                                const RemappedControlBoards& remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_counterForControlBoard[ctrlBrd] = 0;
    }

    for(int j=0; j < n_joints; j++)
    {
        size_t subIndex=remappedControlBoards.lut[joints[j]].subControlBoardIndex;
        arbitraryVec[j] = m_bufferForSubControlBoard[subIndex][m_counterForControlBoard[subIndex]];
        m_counterForControlBoard[subIndex]++;
    }
}


void ControlBoardArbitraryAxesDecomposition::fillSubControlBoardBuffersFromArbitraryJointVector(const double* arbitraryVec,
                                                                                                const int n_joints,
                                                                                                const int *joints,
                                                                                                const RemappedControlBoards& remappedControlBoards)
{
    this->createListOfJointsDecomposition(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_bufferForSubControlBoard[ctrlBrd].clear();
    }

    for(int j=0; j < n_joints; j++)
    {
        size_t subIndex=remappedControlBoards.lut[joints[j]].subControlBoardIndex;
        m_bufferForSubControlBoard[subIndex].push_back(arbitraryVec[j]);
    }
}


void ControlBoardArbitraryAxesDecomposition::fillArbitraryJointVectorFromSubControlBoardBuffers(int* arbitraryVec,
                                                                                                const int n_joints,
                                                                                                const int *joints,
                                                                                                const RemappedControlBoards& remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_counterForControlBoard[ctrlBrd] = 0;
    }

    for(int j=0; j < n_joints; j++)
    {
        size_t subIndex=remappedControlBoards.lut[joints[j]].subControlBoardIndex;
        arbitraryVec[j] = m_bufferForSubControlBoardControlModes[subIndex][m_counterForControlBoard[subIndex]];
        m_counterForControlBoard[subIndex]++;
    }
}


void ControlBoardArbitraryAxesDecomposition::fillSubControlBoardBuffersFromArbitraryJointVector(const int* arbitraryVec,
                                                                                                const int n_joints,
                                                                                                const int *joints,
                                                                                                const RemappedControlBoards& remappedControlBoards)
{
    this->createListOfJointsDecomposition(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_bufferForSubControlBoardControlModes[ctrlBrd].clear();
    }

    for(int j=0; j < n_joints; j++)
    {
        size_t subIndex=remappedControlBoards.lut[joints[j]].subControlBoardIndex;
        m_bufferForSubControlBoardControlModes[subIndex].push_back(arbitraryVec[j]);
    }
}


void ControlBoardArbitraryAxesDecomposition::fillArbitraryJointVectorFromSubControlBoardBuffers(InteractionModeEnum* arbitraryVec,
                                                                                                const int n_joints,
                                                                                                const int *joints,
                                                                                                const RemappedControlBoards& remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_counterForControlBoard[ctrlBrd] = 0;
    }

    for(int j=0; j < n_joints; j++)
    {
        size_t subIndex=remappedControlBoards.lut[joints[j]].subControlBoardIndex;
        arbitraryVec[j] = m_bufferForSubControlBoardInteractionModes[subIndex][m_counterForControlBoard[subIndex]];
        m_counterForControlBoard[subIndex]++;
    }
}


void ControlBoardArbitraryAxesDecomposition::fillSubControlBoardBuffersFromArbitraryJointVector(const InteractionModeEnum* arbitraryVec,
                                                                                                const int n_joints,
                                                                                                const int *joints,
                                                                                                const RemappedControlBoards& remappedControlBoards)
{
    this->createListOfJointsDecomposition(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_bufferForSubControlBoardInteractionModes[ctrlBrd].clear();
    }

    for(int j=0; j < n_joints; j++)
    {
        size_t subIndex=remappedControlBoards.lut[joints[j]].subControlBoardIndex;
        m_bufferForSubControlBoardInteractionModes[subIndex].push_back(arbitraryVec[j]);
    }
}


void ControlBoardArbitraryAxesDecomposition::createListOfJointsDecomposition(const int n_joints, const int* joints, const RemappedControlBoards & remappedControlBoards)
{
    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        m_nJointsInSubControlBoard[ctrlBrd] = 0;
        m_jointsInSubControlBoard[ctrlBrd].clear();
    }

    // Fill buffers
    for(int j=0; j < n_joints; j++)
    {
        int off=(int)remappedControlBoards.lut[joints[j]].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[joints[j]].subControlBoardIndex;

        m_nJointsInSubControlBoard[subIndex]++;
        m_jointsInSubControlBoard[subIndex].push_back(off);
    }

}

void ControlBoardArbitraryAxesDecomposition::resizeSubControlBoardBuffers(const int n_joints, const int *joints, const RemappedControlBoards & remappedControlBoards)
{
    // Properly populate the m_nJointsInSubControlBoard and m_jointsInSubControlBoard methods
    createListOfJointsDecomposition(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        assert(m_nJointsInSubControlBoard[ctrlBrd] == m_jointsInSubControlBoard[ctrlBrd].size());
        m_bufferForSubControlBoard.resize(m_nJointsInSubControlBoard[ctrlBrd]);
    }
}




