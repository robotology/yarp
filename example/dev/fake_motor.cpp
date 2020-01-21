/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <stdio.h>

using namespace yarp::os;
using namespace yarp::dev;

class FakeMotor : public DeviceDriver, public IPositionControl {
public:
    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param ax pointer to storage
     * @return true/false.
     */
    virtual bool getAxes(int *ax) {
        *ax = 2;
        printf("FakeMotor reporting %d axes present\n", *ax);
        return true;
    }

    /** Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool positionMove(int j, double ref) {
        return true;
    }


    /** Set new reference point for all axes.
     * @param refs array, new reference points.
     * @return true/false on success/failure
     */
    virtual bool positionMove(const double *refs) {
        return true;
    }


    /** Set relative position. The command is relative to the
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    virtual bool relativeMove(int j, double delta) {
        return true;
    }


    /** Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const double *deltas) {
        return true;
    }


    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(int j, bool *flag) {
        return true;
    }


    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(bool *flag) {
        return true;
    }


    /** Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeed(int j, double sp) {
        return true;
    }


    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const double *spds) {
        return true;
    }


    /** Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual bool setRefAcceleration(int j, double acc) {
        return true;
    }


    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const double *accs) {
        return true;
    }


    /** Get reference speed for a joint. Returns the speed used to
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    virtual bool getRefSpeed(int j, double *ref) {
        return true;
    }


    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    virtual bool getRefSpeeds(double *spds) {
        return true;
    }


    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    virtual bool getRefAcceleration(int j, double *acc) {
        return true;
    }


    /** Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    virtual bool getRefAccelerations(double *accs) {
        return true;
    }


    /** Stop motion, single joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool stop(int j) {
        return true;
    }


    /** Stop motion, multiple joints
     * @return true/false on success/failure
     */
    virtual bool stop() {
        return true;
    }


    virtual bool open(Searchable& config) {
        return true;
    }

    virtual bool close() {
        return true;
    }
};


void testMotor(PolyDriver& driver) {
    IPositionControl *pos;
    if (driver.view(pos)) {
        int ct = 0;
        pos->getAxes(&ct);
        printf("  number of axes is: %d\n", ct);
    } else {
        printf("  could not find IPositionControl interface\n");
    }
}

int main(int argc, char *argv[]) {
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
