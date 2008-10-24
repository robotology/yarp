// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/

#ifndef __YARPATTACHABLEINTERFACE__
#define __YARPATTACHABLEINTERFACE__

#include <yarp/dev/PloyDriver.h>

/*! \file Attachable.h define the interface for an attachable object*/

namespace yarp {
    namespace dev {
        class IAttachable;
    }
}


/**
 * @ingroup dev_iattachable
 *
 * Interface for an object that can "attach" to another. This is useful
 * for example when an object refers internally to another device, but
 * you want to decouple the creation of the two objects. In this case
 * you first creates the two objects separately then you can attach one 
 * to the other.
 */
class yarp::dev::IAttachable
{
public:
    /**
     * Destructor.
     */
    virtual ~IAttachable() {}

    /*
    * Attach to another object.
    * @param poly: the polydriver that you want to attach to.
    * @return true/false on success failure.
    */
    virtual bool attach(PolyDriver *poly)=0;

    /*
    * Detach the object (you must have first called attach).
    * @return true/false on success failure.
    */
    virtual bool detach()=0;
};

#endif

