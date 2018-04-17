/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Route.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Contact.h>


using yarp::os::Route;
using yarp::os::ConstString;
using yarp::os::Contact;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Route::Private
{
public:
    Private(const ConstString& fromName,
            const ConstString& toName,
            const Contact& toContact,
            const ConstString& carrierName) :
        fromName(fromName),
        toName(toName),
        toContact(toContact),
        carrierName(carrierName)
    {
    }

    ConstString fromName;
    ConstString toName;
    Contact toContact;
    ConstString carrierName;

};

#endif // DOXYGEN_SHOULD_SKIP_THIS






Route::Route() :
        mPriv(new Private(ConstString(),
                          ConstString(),
                          Contact(),
                          ConstString()))
{
}

Route::Route(const ConstString& fromName,
             const ConstString& toName,
             const ConstString& carrierName) :
        mPriv(new Private(fromName,
                          toName,
                          Contact(),
                          carrierName))
{
}

Route::Route(const Route& rhs) :
        mPriv(new Private(*(rhs.mPriv)))
{
}

Route::Route(Route&& rhs) :
        mPriv(rhs.mPriv)
{
    rhs.mPriv = nullptr;
}

Route::~Route()
{
    delete mPriv;
}

Route& Route::operator=(const Route& rhs)
{
    if (&rhs != this) {
        *mPriv = *(rhs.mPriv);
    }
    return *this;
}

Route& Route::operator=(Route&& rhs)
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

const ConstString& Route::getFromName() const
{
    return mPriv->fromName;
}

void Route::setFromName(const ConstString& fromName)
{
    mPriv->fromName = fromName;
}

const ConstString& Route::getToName() const
{
    return mPriv->toName;
}

void Route::setToName(const ConstString& toName)
{
    mPriv->toName = toName;
}

const Contact& Route::getToContact() const
{
    return mPriv->toContact;
}

void Route::setToContact(const Contact& toContact)
{
    mPriv->toContact = toContact;
}

const ConstString& Route::getCarrierName() const
{
    return mPriv->carrierName;
}

void Route::setCarrierName(const ConstString& carrierName)
{
    mPriv->carrierName = carrierName;
}

void Route::swapNames()
{
    mPriv->fromName.swap(mPriv->toName);
}

ConstString Route::toString() const
{
    return getFromName() + "->" + getCarrierName() + "->" + getToName();
}

#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70

Route Route::addFromName(const ConstString& fromName) const
{
    Route result(*this);
    result.mPriv->fromName = fromName;
    return result;
}

Route Route::addToName(const ConstString& toName) const
{
    Route result(*this);
    result.mPriv->toName = toName;
    return result;
}


Route Route::addToContact(const Contact& toContact) const
{
    Route result(*this);
    result.mPriv->toContact = toContact;
    return result;
}

Route Route::addCarrierName(const ConstString& carrierName) const
{
    Route result(*this);
    result.mPriv->carrierName = carrierName;
    return result;
}

#endif // YARP_NO_DEPRECATED
