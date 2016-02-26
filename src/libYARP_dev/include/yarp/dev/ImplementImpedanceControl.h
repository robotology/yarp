/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_IMPLEMENT_IMPEDANCE_CONTROL
#define YARP_IMPLEMENT_IMPEDANCE_CONTROL

#include <yarp/dev/IImpedanceControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementImpedanceControl;
    }
}

class YARP_dev_API yarp::dev::ImplementImpedanceControl: public IImpedanceControl
{
protected:
    yarp::dev::IImpedanceControlRaw *iImpedanceRaw;
    void *helper;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
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
    ImplementImpedanceControl(yarp::dev::IImpedanceControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementImpedanceControl();

    virtual bool getAxes(int *ax);
    virtual bool getImpedance(int j, double *stiffness, double *damping);
    virtual bool setImpedance(int j, double  stiffness, double  damping);
    virtual bool setImpedanceOffset(int j, double offset);
    virtual bool getImpedanceOffset(int j, double* offset);
    virtual bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp);

};

#endif
