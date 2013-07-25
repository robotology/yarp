// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Assif Mirza
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include <yarp/dev/ControlBoardInterfacesImpl.inl>
#include "DimaxU2C.h"

#define DEFAULT_NUM_MOTORS 16

DimaxU2C::DimaxU2C():
    ImplementPositionControl<DimaxU2C, IPositionControl>(this)
{
    //printf("DimaxU2C: constructor\n");
    //printf("Construct Servo object\n");
    servos = new Servo();
    if (!servos) {
        ACE_OS::fprintf(stderr,"DimaxU2C: Failed to create a Servo object\n");
    }
}

DimaxU2C::~DimaxU2C() {
    //printf("DimaxU2C: destructor\n");
    delete(servos);
    delete(speeds);
    delete(accels);
}

bool DimaxU2C::open(yarp::os::Searchable& config) {
    printf("DimaxU2C: open\n");

    numJoints = config.check("axes",
                             yarp::os::Value(DEFAULT_NUM_MOTORS),
                             "number of motors").asInt();

    speeds = new double[numJoints];
    accels = new double[numJoints];
    int *amap = new int[numJoints];
    double *enc = new double[numJoints];
    double *zos = new double[numJoints];

    for (int i=0;i<numJoints;i++) {
        speeds[i]=10;
        accels[i]=0;

        // for now we are mapping one to one so you pass
        // the raw motor position in range 800-2200
        // TODO: convert from angle to joint value
        amap[i]=i;
        enc[i]=1.0;
        zos[i]=0.0;
    }

    printf("Calling initialize: numJoints %d\n",numJoints);
    initialize(numJoints,      // number of joints/axes
               amap,           // axes map
               enc,            // encoder to angles conversion factors
               zos             // zeros of encoders
              );
    if (servos) {
        printf("Initialise Servo object\n");
        servos->init();
        return true;
    } else {
        ACE_OS::fprintf(stderr,"DimaxU2C: No Servo object created\n");
        return false;
    }
    return true;
}

bool DimaxU2C::close() {
    printf("DimaxU2C: close\n");
    // unallocate all memory ...

    return true; // easy
}


bool DimaxU2C::getAxes(int *ax) {
    *ax = numJoints;
    return true;
}
bool DimaxU2C::setPositionMode() {
    return true;
}

bool DimaxU2C::positionMoveRaw(int j, double ref) {
    printf("DimaxU2C::positionMoveRaw(%d,%f)\n",j,ref);
    servos->setPosition(j*3, (int) ref, (int) speeds[j]);
    return true;
}

bool DimaxU2C::positionMoveRaw(const double *refs) {
    for (int i=0;i<numJoints;i++) {
        positionMoveRaw(i,refs[i]);
    }
    return true;
}

bool DimaxU2C::relativeMoveRaw(int j, double delta) {
    // not implemented yet
    return true;
}

bool DimaxU2C::relativeMoveRaw(const double *deltas) {
    // not implemented yet
    return true;
}

bool DimaxU2C::checkMotionDoneRaw(bool *flag) {
    // not implemented yet
    return true;
}

bool DimaxU2C::checkMotionDoneRaw(int j, bool *flag) {
    // not implemented yet
    return true;
}

bool DimaxU2C::setRefSpeedRaw(int j, double sp) {
    speeds[j] = sp;
    return true;
}

bool DimaxU2C::setRefSpeedsRaw(const double *spds) {
    for (int i=0;i<numJoints;i++) {
        speeds[i]=spds[i];
    }
    return true;
}

bool DimaxU2C::setRefAccelerationRaw(int j, double acc) {
    accels[j]=acc;
    return true;
}

bool DimaxU2C::setRefAccelerationsRaw(const double *accs) {
    for (int i=0;i<numJoints;i++) {
        accels[i]=accs[i];
    }
    return true;
}

bool DimaxU2C::getRefSpeedRaw(int j, double *ref) {
    // not implemented yet
    return true;
}

bool DimaxU2C::getRefSpeedsRaw(double *spds) {
    // not implemented yet
    return true;
}

bool DimaxU2C::getRefAccelerationRaw(int j, double *acc) {
    // not implemented yet
    return true;
}

bool DimaxU2C::getRefAccelerationsRaw(double *accs) {
    // not implemented yet
    return true;
}

bool DimaxU2C::stopRaw(int j) {
    // not implemented yet
    return true;
}

bool DimaxU2C::stopRaw() {
    // not implemented yet
    return true;
}
