/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardRemapperHelpers.h"
#include "ControlBoardRemapperLogComponent.h"

#include <yarp/os/LogStream.h>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace std;


RemappedSubControlBoard::RemappedSubControlBoard()
{
    id = "";

    pid = nullptr;
    pos = nullptr;
    posDir = nullptr;
    vel = nullptr;
    iJntEnc = nullptr;
    iMotEnc = nullptr;
    amp = nullptr;
    lim = nullptr;
    calib = nullptr;
    remcalib = nullptr;
    iTimed= nullptr;
    info = nullptr;
    iTorque=nullptr;
    iImpedance=nullptr;
    iMode=nullptr;
    iInteract=nullptr;
    imotor=nullptr;
    iVar = nullptr;
    iPwm = nullptr;
    iCurr = nullptr;

    subdevice=nullptr;


    attachedF=false;
    _subDevVerbose = false;
}


void RemappedSubControlBoard::detach()
{
    subdevice=nullptr;

    pid=nullptr;
    pos=nullptr;
    posDir=nullptr;
    vel=nullptr;
    amp = nullptr;
    iJntEnc=nullptr;
    iMotEnc=nullptr;
    lim=nullptr;
    calib=nullptr;
    info=nullptr;
    iTorque=nullptr;
    iImpedance=nullptr;
    iMode=nullptr;
    iTimed=nullptr;
    iInteract=nullptr;
    imotor=nullptr;
    iVar = nullptr;
    iPwm = nullptr;
    iCurr = nullptr;

    attachedF=false;
}

bool RemappedSubControlBoard::attach(yarp::dev::PolyDriver *d, const std::string &k)
{
    if (id!=k)
    {
        yCError(CONTROLBOARDREMAPPER) << "Wrong device" << k.c_str();
        return false;
    }

    if (d==nullptr)
    {
        yCError(CONTROLBOARDREMAPPER) << "Invalid device (null pointer)";
        return false;
    }

    subdevice=d;

    if (subdevice->isValid())
    {
        subdevice->view(pid);
        subdevice->view(pos);
        subdevice->view(posDir);
        subdevice->view(vel);
        subdevice->view(amp);
        subdevice->view(lim);
        subdevice->view(calib);
        subdevice->view(remcalib);
        subdevice->view(info);
        subdevice->view(iTimed);
        subdevice->view(iTorque);
        subdevice->view(iImpedance);
        subdevice->view(iMode);
        subdevice->view(iJntEnc);
        subdevice->view(iMotEnc);
        subdevice->view(iInteract);
        subdevice->view(imotor);
        subdevice->view(iVar);
        subdevice->view(iPwm);
        subdevice->view(iCurr);
    }
    else
    {
        yCError(CONTROLBOARDREMAPPER) << "Invalid device" << k << "(isValid() returned false)";
        return false;
    }

    if ((iTorque==nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "iTorque not valid interface";
    }

    if ((iImpedance==nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "iImpedance not valid interface";
    }

    if ((iInteract==nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "iInteractionMode not valid interface";
    }

    if ((iMotEnc==nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "IMotorEncoders not valid interface";
    }

    if ((imotor==nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "IMotor not valid interface";
    }

    if ((iVar == nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "IRemoveVariables not valid interface";
    }

    if ((info == nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "IAxisInfo not valid interface";
    }

    if ((iPwm == nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "IPWMControl not valid interface";
    }

    if ((iCurr == nullptr) && (_subDevVerbose))
    {
        yCWarning(CONTROLBOARDREMAPPER) << "ICurrentControl not valid interface";
    }


    // checking minimum set of intefaces required
    if( !(pos) )
    {
        yCError(CONTROLBOARDREMAPPER, "IPositionControl interface was not found in subdevice. Quitting");
        return false;
    }

    if( ! (vel) )
    {
        yCError(CONTROLBOARDREMAPPER, "IVelocityControl interface was not found in subdevice. Quitting");
        return false;
    }

    if(!iJntEnc)
    {
        yCError(CONTROLBOARDREMAPPER, "IEncoderTimed interface was not found in subdevice, exiting.");
        return false;
    }

    if(!iMode)
    {
        yCError(CONTROLBOARDREMAPPER, "IControlMode interface was not found in subdevice, exiting.");
        return false;
    }

    int deviceJoints=0;
    if (pos!=nullptr)
    {
        if (!pos->getAxes(&deviceJoints))
        {
            yCError(CONTROLBOARDREMAPPER) << "failed to get axes number for subdevice" << k.c_str();
            return false;
        }
        if(deviceJoints <= 0)
        {
            yCError(CONTROLBOARDREMAPPER, "attached device has an invalid number of joints (%d)", deviceJoints);
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
        yCAssert(CONTROLBOARDREMAPPER, (unsigned)m_nJointsInSubControlBoard[ctrlBrd] == m_jointsInSubControlBoard[ctrlBrd].size());
        m_bufferForSubControlBoard.resize(m_nJointsInSubControlBoard[ctrlBrd]);
    }
}
