/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARPWRAPPERINTERFACE
#define YARPWRAPPERINTERFACE

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>

/*! \file Wrapper.h define the interface for an attachable object*/

namespace yarp {
    namespace dev {
        class IWrapper;
        class IMultipleWrapper;
    }
}

/**
 * @ingroup dev_iwrapper
 *
 * Interface for an object that can wrap/or "attach" to another. This is useful
 * for example when an object refers internally to another device, but
 * you want to decouple the creation of the two objects. In this case
 * you first creates the two objects separately then you can attach one
 * to the other.
 */
class YARP_dev_API yarp::dev::IWrapper
{
public:
    /**
     * Destructor.
     */
    virtual ~IWrapper() {}

    /**
     * Attach to another object.
     * @param poly the polydriver that you want to attach to.
     * @return true/false on success failure.
     */
    virtual bool attach(PolyDriver *poly)=0;

    /**
     * Detach the object (you must have first called attach).
     * @return true/false on success failure.
     */
    virtual bool detach()=0;
};

/**
 * @ingroup dev_imultiplewrapper
 *
 * Interface for an object that can wrap/attach to to another. This is useful
 * for example when an object refers internally to another device, but
 * you want to decouple the creation of the two objects. In this case
 * you first creates the two objects separately then you can attach one
 * to the other. This class is similar to IWrapper, but allows attaching
 * to more than one object by subsequent calls to attach.
 */
class YARP_dev_API yarp::dev::IMultipleWrapper
{
public:
    /**
     * Destructor.
     */
    virtual ~IMultipleWrapper() {}

    /**
     * Attach to a list of objects.
     * @param p the polydriver list that you want to attach to.
     * @return true/false on success failure.
     */
    virtual bool attachAll(const PolyDriverList &p)=0;

    /**
     * Detach the object (you must have first called attach).
     * @return true/false on success failure.
     */
    virtual bool detachAll()=0;
};

#endif
