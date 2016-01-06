// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPREMOTEVARIABLESIMPL
#define YARPREMOTEVARIABLESIMPL

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

    virtual bool getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle& val);

    virtual bool setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle& val);

    virtual bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys);
};

#endif
