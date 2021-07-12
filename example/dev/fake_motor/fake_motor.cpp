/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Searchable.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <cstdio>

using yarp::dev::DeviceDriver;
using yarp::dev::DriverCreatorOf;
using yarp::dev::Drivers;
using yarp::dev::IPositionControl;
using yarp::dev::PolyDriver;
using yarp::os::Network;
using yarp::os::Searchable;

class FakeMotor :
        public DeviceDriver,
        public IPositionControl
{
public:
    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param ax pointer to storage
     * @return true/false.
     */
    bool getAxes(int* ax) override
    {
        *ax = 2;
        printf("FakeMotor reporting %d axes present\n", *ax);
        return true;
    }

    /** Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    bool positionMove(int j, double ref) override
    {
        YARP_UNUSED(j);
        YARP_UNUSED(ref);
        return true;
    }

    /** Set new reference point for all axes.
     * @param refs array, new reference points.
     * @return true/false on success/failure
     */
    bool positionMove(const double* refs) override
    {
        YARP_UNUSED(refs);
        return true;
    }

    bool positionMove(const int n_joint, const int* joints, const double* refs) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        YARP_UNUSED(refs);
        return true;
    }

    /** Set relative position. The command is relative to the
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    bool relativeMove(int j, double delta) override
    {
        YARP_UNUSED(j);
        YARP_UNUSED(delta);
        return true;
    }

    /** Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    bool relativeMove(const double* deltas) override
    {
        YARP_UNUSED(deltas);
        return true;
    }

    bool relativeMove(const int n_joint, const int* joints, const double* deltas) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        YARP_UNUSED(deltas);
        return true;
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    bool checkMotionDone(int j, bool* flag) override
    {
        YARP_UNUSED(j);
        YARP_UNUSED(flag);
        return true;
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    bool checkMotionDone(bool* flag) override
    {
        YARP_UNUSED(flag);
        return true;
    }

    bool checkMotionDone(const int n_joint, const int* joints, bool* flag) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        YARP_UNUSED(flag);
        return true;
    }

    /** Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    bool setRefSpeed(int j, double sp) override
    {
        YARP_UNUSED(j);
        YARP_UNUSED(sp);
        return true;
    }

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    bool setRefSpeeds(const double* spds) override
    {
        YARP_UNUSED(spds);
        return true;
    }

    bool setRefSpeeds(const int n_joint, const int* joints, const double* spds) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        YARP_UNUSED(spds);
        return true;
    }

    /** Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    bool setRefAcceleration(int j, double acc) override
    {
        YARP_UNUSED(j);
        YARP_UNUSED(acc);
        return true;
    }

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    bool setRefAccelerations(const double* accs) override
    {
        YARP_UNUSED(accs);
        return true;
    }

    bool setRefAccelerations(const int n_joint, const int* joints, const double* accs) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        YARP_UNUSED(accs);
        return true;
    }

    /** Get reference speed for a joint. Returns the speed used to
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    bool getRefSpeed(int j, double* ref) override
    {
        YARP_UNUSED(j);
        YARP_UNUSED(ref);
        return true;
    }

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    bool getRefSpeeds(double* spds) override
    {
        YARP_UNUSED(spds);
        return true;
    }

    bool getRefSpeeds(const int n_joint, const int* joints, double* spds) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        YARP_UNUSED(spds);
        return true;
    }

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    bool getRefAcceleration(int j, double* acc) override
    {
        YARP_UNUSED(j);
        YARP_UNUSED(acc);
        return true;
    }

    /** Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    bool getRefAccelerations(double* accs) override
    {
        YARP_UNUSED(accs);
        return true;
    }

    bool getRefAccelerations(const int n_joint, const int* joints, double* accs) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        YARP_UNUSED(accs);
        return true;
    }

    /** Stop motion, single joint
     * @param j joint number
     * @return true/false on success/failure
     */
    bool stop(int j) override
    {
        YARP_UNUSED(j);
        return true;
    }

    /** Stop motion, multiple joints
     * @return true/false on success/failure
     */
    bool stop() override
    {
        return true;
    }

    bool stop(const int n_joint, const int* joints) override
    {
        YARP_UNUSED(n_joint);
        YARP_UNUSED(joints);
        return true;
    }

    bool open(Searchable& config) override
    {
        YARP_UNUSED(config);
        return true;
    }

    bool close() override
    {
        return true;
    }
};


void testMotor(PolyDriver& driver)
{
    IPositionControl* pos;
    if (driver.view(pos)) {
        int ct = 0;
        pos->getAxes(&ct);
        printf("  number of axes is: %d\n", ct);
    } else {
        printf("  could not find IPositionControl interface\n");
    }
}

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::os::Network yarp;

    Drivers::factory().add(new DriverCreatorOf<FakeMotor>("motor",
                                                          "controlboard",
                                                          "FakeMotor"));

    printf("============================================================\n");
    printf("check our device can be instantiated directly\n");

    PolyDriver direct("motor");
    if (direct.isValid()) {
        printf("Direct instantiation worked\n");
        testMotor(direct);
    } else {
        printf("Direct instantiation failed\n");
    }
    direct.close();

    // check our device can be wrapped in the controlboard network wrapper
    printf("\n\n");
    printf("============================================================\n");
    printf("check our device can be wrapped in controlboard\n");


    PolyDriver indirect("(device controlboard) (subdevice motor)");
    if (indirect.isValid()) {
        printf("Indirect instantiation worked\n");
    } else {
        printf("Indirect instantiation failed\n");
    }
    indirect.close();

    // check our device can be wrapped in the controlboard network wrapper
    // and accessed remotely
    printf("\n\n");
    printf("============================================================\n");
    printf("check our device can be accessed via remote_controlboard\n");

    PolyDriver server("(device controlboard) (subdevice motor) (name /server)");
    if (server.isValid()) {
        printf("Server instantiation worked\n");

        PolyDriver client("(device clientcontrolboard) (local /client) (remote /server)");
        if (client.isValid()) {
            printf("Client instantiation worked\n");
            testMotor(client);
        } else {
            printf("Client instantiation failed\n");
        }
        client.close();
    }
    server.close();


    return 0;
}
