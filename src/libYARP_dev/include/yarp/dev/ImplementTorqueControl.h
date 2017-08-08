/*
 * Copyright (C) 2011 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMPLEMENTTORQUECONTROL_H
#define YARP_DEV_IMPLEMENTTORQUECONTROL_H

#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementTorqueControl;
    }
}


class YARP_dev_API yarp::dev::ImplementTorqueControl: public ITorqueControl
{
protected:
    yarp::dev::ITorqueControlRaw *iTorqueRaw;
    void *helper;
    double *temp;
    double *temp2;
    int    *temp_int;
    yarp::dev::Pid *tmpPids;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos, const double *nw);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementTorqueControl(yarp::dev::ITorqueControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementTorqueControl();

    virtual bool getAxes(int *ax) YARP_OVERRIDE;
    virtual bool getRefTorque(int j, double *) YARP_OVERRIDE;
    virtual bool getRefTorques(double *t) YARP_OVERRIDE;
    virtual bool setRefTorques(const double *t) YARP_OVERRIDE;
    virtual bool setRefTorque(int j, double t) YARP_OVERRIDE;
    virtual bool setRefTorques(const int n_joint, const int *joints, const double *t) YARP_OVERRIDE;
    virtual bool getTorques(double *t) YARP_OVERRIDE;
    virtual bool getTorque(int j, double *t) YARP_OVERRIDE;
    virtual bool getBemfParam(int j, double *bemf) YARP_OVERRIDE;
    virtual bool setBemfParam(int j, double bemf) YARP_OVERRIDE;
    virtual bool setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params) YARP_OVERRIDE;
    virtual bool getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters *params) YARP_OVERRIDE;
    virtual bool getTorqueRange(int j, double *min, double *max) YARP_OVERRIDE;
    virtual bool getTorqueRanges(double *min, double *max) YARP_OVERRIDE;
};

#endif // YARP_DEV_IMPLEMENTTORQUECONTROL_H
