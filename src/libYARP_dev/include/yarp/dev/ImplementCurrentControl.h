/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMPLEMENTCURRENTCONTROL_H
#define YARP_DEV_IMPLEMENTCURRENTCONTROL_H

#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementCurrentControl;
    }
}

class YARP_dev_API yarp::dev::ImplementCurrentControl: public ICurrentControl
{
protected:
    yarp::dev::ICurrentControlRaw *iCurrentRaw;
    void *helper;
    double *temp;
    double *temp2;
    int    *temp_int;
    yarp::dev::Pid *tmpPids;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize(int size, const int *amap, const double* ampsToSens);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementCurrentControl(yarp::dev::ICurrentControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementCurrentControl();

    virtual bool getNumberOfMotors(int *ax) YARP_OVERRIDE;
    virtual bool getRefCurrent(int j, double *) YARP_OVERRIDE;
    virtual bool getRefCurrents(double *t) YARP_OVERRIDE;
    virtual bool setRefCurrents(const double *t) YARP_OVERRIDE;
    virtual bool setRefCurrent(int j, double t) YARP_OVERRIDE;
    virtual bool setRefCurrents(const int n_joint, const int *joints, const double *t) YARP_OVERRIDE;
    virtual bool getCurrents(double *t) YARP_OVERRIDE;
    virtual bool getCurrent(int j, double *t) YARP_OVERRIDE;
    virtual bool getCurrentRange(int j, double *min, double *max) YARP_OVERRIDE;
    virtual bool getCurrentRanges(double *min, double *max) YARP_OVERRIDE;
};

#endif // YARP_DEV_IMPLEMENTCURRENTCONTROL_H
