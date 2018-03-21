/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTREMOTEVARIABLES_H
#define YARP_DEV_IMPLEMENTREMOTEVARIABLES_H

#include <yarp/dev/IRemoteVariables.h>

namespace yarp {
    namespace dev {
        class ImplementRemoteVariables;
    }
}

class YARP_dev_API yarp::dev::ImplementRemoteVariables: public IRemoteVariables
{
protected:
    IRemoteVariablesRaw *ivar;
    void *helper;
    double *temp1;
    double *temp2;


    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap);

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
    ImplementRemoteVariables(yarp::dev::IRemoteVariablesRaw *y);

    virtual ~ImplementRemoteVariables();

    virtual bool getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle& val) override;

    virtual bool setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle& val) override;

    virtual bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys) override;
};

#endif // YARP_DEV_IMPLEMENTREMOTEVARIABLES_H
