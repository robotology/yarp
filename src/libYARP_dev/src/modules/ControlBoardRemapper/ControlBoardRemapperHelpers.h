/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Lorenzo Natale, Silvio Traversaro
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARP_DEV_CONTROLBOARDREMAPPERHELPERS_H
#define YARP_DEV_CONTROLBOARDREMAPPERHELPERS_H


#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/dev/IControlLimits2.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PreciselyTimed.h>


#include <yarp/sig/Vector.h>
#include <yarp/os/Mutex.h>
#include <yarp/dev/Wrapper.h>

#include <string>
#include <vector>


#ifdef MSVC
    #pragma warning(disable:4355)
#endif

namespace yarp
{

namespace dev
{

/*
 * Helper class for the ControlBoardRemapper.
 * It contains all the data structure related
 * to a given controlboard used by the remapper.
 */
class RemappedSubControlBoard
{
public:
    std::string id;

    yarp::dev::PolyDriver            *subdevice;
    yarp::dev::IPidControl           *pid;
    yarp::dev::IPositionControl2     *pos2;
    yarp::dev::IVelocityControl2     *vel2;
    yarp::dev::IEncodersTimed        *iJntEnc;
    yarp::dev::IMotorEncoders        *iMotEnc;
    yarp::dev::IAmplifierControl     *amp;
    yarp::dev::IControlLimits2       *lim2;
    yarp::dev::IControlCalibration   *calib;
    yarp::dev::IControlCalibration2  *calib2;
    yarp::dev::IRemoteCalibrator     *remcalib;
    yarp::dev::IPreciselyTimed       *iTimed;
    yarp::dev::ITorqueControl        *iTorque;
    yarp::dev::IImpedanceControl     *iImpedance;
    yarp::dev::IOpenLoopControl      *iOpenLoop;
    yarp::dev::IControlMode2         *iMode2;
    yarp::dev::IAxisInfo             *info;
    yarp::dev::IPositionDirect       *posDir;
    yarp::dev::IInteractionMode      *iInteract;
    yarp::dev::IMotor                *imotor;
    yarp::dev::IRemoteVariables      *iVar;

    RemappedSubControlBoard();

    bool attach(yarp::dev::PolyDriver *d, const std::string &id);
    void detach();

    inline void setVerbose(bool _verbose) {_subDevVerbose = _verbose; }

    bool isAttached()
    { return attachedF; }

private:
    bool _subDevVerbose;
    bool attachedF;
};

/**
 * Information in how an axis is
 * remapped on an axis of a SubControlBoard.
 */
class RemappedAxis
{
public:
    /**
     * The index of the SubControlBoard of the remapped axis
     * in the RemappedControlBoards class.
     */
    size_t subControlBoardIndex;

    /**
     * The index of the remapped axis in the SubControlBoard.
     */
    size_t axisIndexInSubControlBoard;
};

class RemappedControlBoards
{
public:
    /**
     * Vector of dimension getNrOfSubControlBoards .
     */
    std::vector<RemappedSubControlBoard> subdevices;

    /**
     * Vector of dimension getNrOfRemappedAxes .
     */
    std::vector<RemappedAxis> lut;

    /**
     * Given a controlboard index between 0 and getNrOfSubControlBoards()-1, return
     * the relative SubControlBoard.
     *
     * @return a reference to the requests SubControlBoard.
     */
    inline yarp::dev::RemappedSubControlBoard* getSubControlBoard(size_t i)
    {
        return &(subdevices[i]);
    }

    size_t getNrOfSubControlBoards() const
    {
        return subdevices.size();
    }

    size_t getNrOfRemappedAxes() const
    {
        return lut.size();
    }
};

class ControlBoardRemapperBuffers
{
public:
    yarp::os::Mutex mutex;
    std::vector<int> controlBoardModes;
    std::vector<double> dummyBuffer;
    yarp::os::Stamp stamp;

};

/**
 * Class storing the decomposition of all the axes
 * in the Remapped ControlBoard in the SubControlBoard,
 * with buffer reading to use to simplify MultiJoint
 * methods implementation.
 *
 */
class ControlBoardSubControlBoardAxesDecomposition
{
public:
    /**
     * Resize the buffers using the information in
     * the RemappedControlBoards
     */
    bool configure(const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill buffers for the SubControlBoard from
     * a vector of joints of the RemappedControlBoards
     */
    void fillSubControlBoardBuffersFromCompleteJointVector(const double * full, const RemappedControlBoards & remappedControlBoards);


    /**
      * Fill buffers for the SubControlBoard from
      * a vector of joints of the RemappedControlBoard
      * (Version for ControlModes methods)
      */
    void fillSubControlBoardBuffersFromCompleteJointVector(const int * full,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill buffers for the SubControlBoard from
     * a vector of joints of the RemappedControlBoard
     *
     * (Version for InteractionModes methods)
     */
    void fillSubControlBoardBuffersFromCompleteJointVector(const InteractionModeEnum * full,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill a vector of joints of the ControlBoardRemapper from
     * the buffers of the SubControlBoard .
     */
    void fillCompleteJointVectorFromSubControlBoardBuffers(double * full, const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill a vector of joints of the ControlBoardRemapper from
     * the buffers of the SubControlBoard .
     *
     * Before calling this method you should have called the resizeSubControlBoardBuffers method.
     * (Version for ControlModes methods)
     */
    void fillCompleteJointVectorFromSubControlBoardBuffers(int * full,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill a vector of joints of the ControlBoardRemapper from
     * the buffers of the SubControlBoard .
     *
     * Before calling this method you should have called the resizeSubControlBoardBuffers method.
     * (Version for InteractionModes methods)
     */
    void fillCompleteJointVectorFromSubControlBoardBuffers(InteractionModeEnum * full,
                                                           const RemappedControlBoards & remappedControlBoards);


    /**
     * Mutex to grab to use this class.
     */
    yarp::os::Mutex mutex;

    // Buffer to be used in MultiJoint version of the
    int m_nrOfControlledAxesInRemappedCtrlBrd;
    std::vector<int> m_nJointsInSubControlBoard;
    std::vector< std::vector<int> > m_jointsInSubControlBoard;


    std::vector< std::vector<double> > m_bufferForSubControlBoard;
    std::vector< std::vector<int>    > m_bufferForSubControlBoardControlModes;
    std::vector< std::vector<InteractionModeEnum>  > m_bufferForSubControlBoardInteractionModes;

    std::vector<int> m_counterForControlBoard;
};

/**
 * Class storing the decomposition of a subset ot the total
 * remapped axes of the remapped controlboard in the
 * corresponding subsets of the axes of the SubControlBoard.
 *
 * This class is meant to be used when implementing multiple joint
 * methods (the one with the signature const int n_joints, const int *joints, double *dpos )
 *
 */
class ControlBoardArbitraryAxesDecomposition
{
    /**
     * Fill the buffer containing the structure of the decomposition between the
     * desired list of joints and and their mapping in the subControlboards.
     */
    void createListOfJointsDecomposition(const int n_joints, const int *joints, const RemappedControlBoards & remappedControlBoards);

public:
    /**
     * Resize the buffers using the information in
     * the RemappedControlBoards
     */
    bool configure(const RemappedControlBoards & remappedControlBoards);


    /**
     * Fill buffers for the SubControlBoard from
     * a vector of joints of the RemappedControlBoards
     */
    void fillSubControlBoardBuffersFromArbitraryJointVector(const double * arbitraryVec,
                                                            const int n_joints,
                                                            const int *joints,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill buffers for the SubControlBoard from
     * a vector of joints of the RemappedControlBoards
     * (Version for ControlModes methods)
     */
    void fillSubControlBoardBuffersFromArbitraryJointVector(const int * arbitraryVec,
                                                            const int n_joints,
                                                            const int *joints,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill buffers for the SubControlBoard from
     * a vector of joints of the RemappedControlBoards
     * (Version for InteractionModes methods)
     */
    void fillSubControlBoardBuffersFromArbitraryJointVector(const InteractionModeEnum * arbitraryVec,
                                                            const int n_joints,
                                                            const int *joints,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Resize buffers to have the dimension of specified by the method
     * (used for multi joint methods that read the data from the subcontrolboards).
     */
    void resizeSubControlBoardBuffers(const int n_joints,
                                      const int *joints,
                                      const RemappedControlBoards & remappedControlBoards);

    /**
    * Fill a vector of joints of the ControlBoardRemapper from
    * the buffers of the SubControlBoard .
    *
    * Before calling this method you should have called the resizeSubControlBoardBuffers method.
    */
    void fillArbitraryJointVectorFromSubControlBoardBuffers(double * arbitraryVec,
                                                            const int n_joints, const int *joints,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill a vector of joints of the ControlBoardRemapper from
     * the buffers of the SubControlBoard .
     *
     * Before calling this method you should have called the resizeSubControlBoardBuffers method.
     * (Version for ControlModes methods)
     */
    void fillArbitraryJointVectorFromSubControlBoardBuffers(int * arbitraryVec,
                                                            const int n_joints, const int *joints,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Fill a vector of joints of the ControlBoardRemapper from
     * the buffers of the SubControlBoard .
     *
     * Before calling this method you should have called the resizeSubControlBoardBuffers method.
     * (Version for InteractionModes methods)
     */
    void fillArbitraryJointVectorFromSubControlBoardBuffers(InteractionModeEnum * arbitraryVec,
                                                            const int n_joints, const int *joints,
                                                            const RemappedControlBoards & remappedControlBoards);

    /**
     * Mutex to grab to use this class.
     */
    yarp::os::Mutex mutex;

    // Total number of axes in the remapped controlboard
    int m_nrOfControlledAxesInRemappedCtrlBrd;

    // Vector of size getNrOfSubControlBoards
    std::vector<int> m_nJointsInSubControlBoard;
    std::vector< std::vector<int> > m_jointsInSubControlBoard;

    // Buffers for the control board (the size of each one should
    // match the size of m_nJointsInSubControlBoard[ctrlBoard] and
    // the size of m_jointsInSubControlBoard[ctrlBoard].size()
    std::vector< std::vector<double> > m_bufferForSubControlBoard;
    std::vector< std::vector<int>    > m_bufferForSubControlBoardControlModes;
    std::vector< std::vector<InteractionModeEnum>  > m_bufferForSubControlBoardInteractionModes;


    // Counter used when converting a full vector to
    // the subcontrolboard buffers
    std::vector<int> m_counterForControlBoard;
};

}

}

#endif  //YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H
