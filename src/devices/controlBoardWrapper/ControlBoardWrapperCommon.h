/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCOMMON_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCOMMON_H

#include "MultiJointData.h"

constexpr int PROTOCOL_VERSION_MAJOR = 1;
constexpr int PROTOCOL_VERSION_MINOR = 9;
constexpr int PROTOCOL_VERSION_TWEAK = 0;

class ControlBoardWrapperCommon
{
public:
// COMMON MEMBERS
    WrappedDevice device;
    size_t controlledJoints {0};
    std::string partName; // to open ports and print more detailed debug messages

    // RPC calls are concurrent from multiple clients, data used inside the calls has to be protected
    std::mutex rpcDataMutex; // mutex to avoid concurrency between more clients using rppc port
    MultiJointData rpcData;  // Structure used to re-arrange data from "multiple_joints" calls.

    std::mutex timeMutex; // mutex to protect access to time member
    yarp::os::Stamp time; // envelope to attach to the state port

// METHODS SHARED BY MULTIPLE INTERFACES
    /*
     * IEncodersTimed
     * IImpedanceControl
     * IPositionControl
     * IPositionDirect
     * ITorqueControl
     * IVelocityControl
     */
    bool getAxes(int* ax);

    /*
     * IPositionControl
     * IVelocityControl
     */
    bool setRefAcceleration(int j, double acc);
    bool setRefAccelerations(const double* accs);
    bool setRefAccelerations(const int n_joints, const int* joints, const double* accs);
    bool getRefAcceleration(int j, double* acc);
    bool getRefAccelerations(double* accs);
    bool getRefAccelerations(const int n_joints, const int* joints, double* accs);
    bool stop(int j);
    bool stop();
    bool stop(const int n_joint, const int* joints);

    /*
     * IMotor
     * IPWMControl
     */
    bool getNumberOfMotors(int* num);

    /*
     * IAmplifierControl
     * ICurrentControl
     */
    bool getCurrent(int m, double* curr);
    bool getCurrents(double* currs);

// UTILITIES
    inline void printError(const std::string& func_name, const std::string& info, bool result)
    {
        // FIXME: Check if it is still required.
        //        This method was commented out by these commits:
        //          afc039962f3667cc954e7a50ce6963ec60886611
        //          0c0de4a9331b9b843ac4b3d3746c074dd0427249

        // If result is false, this means that en error occurred in function named func_name, otherwise means that the device doesn't implement the interface to witch func_name belongs to.
        // if(false == result) {
        //     yCError(CONTROLBOARDREMAPPER) << "CBW(" << partName << "): " << func_name.c_str() << " on device" << info.c_str() << " returns false";
        // } else {
        // Commented in order to maintain the old behaviour (none message appear if device desn't implement the interface)
        // yCError(CONTROLBOARDREMAPPER) << "CBW(" << partName << "): " << func_name.c_str() << " on device" << info.c_str() << ": the interface is not available.";
        // }
    }
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCOMMON_H
