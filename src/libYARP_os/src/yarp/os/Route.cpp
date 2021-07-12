/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Route.h>

#include <yarp/os/Contact.h>

#include <string>
#include <utility>


using yarp::os::Contact;
using yarp::os::Route;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Route::Private
{
public:
    Private(std::string fromName,
            std::string toName,
            Contact toContact,
            std::string carrierName) :
            fromName(std::move(fromName)),
            toName(std::move(toName)),
            toContact(std::move(toContact)),
            carrierName(std::move(carrierName))
    {
    }

    std::string fromName;
    std::string toName;
    Contact toContact;
    std::string carrierName;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS


Route::Route() :
        mPriv(new Private(std::string(),
                          std::string(),
                          Contact(),
                          std::string()))
{
}

Route::Route(const std::string& fromName,
             const std::string& toName,
             const std::string& carrierName) :
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

Route::Route(Route&& rhs) noexcept :
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

Route& Route::operator=(Route&& rhs) noexcept
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

const std::string& Route::getFromName() const
{
    return mPriv->fromName;
}

void Route::setFromName(const std::string& fromName)
{
    mPriv->fromName = fromName;
}

const std::string& Route::getToName() const
{
    return mPriv->toName;
}

void Route::setToName(const std::string& toName)
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

const std::string& Route::getCarrierName() const
{
    return mPriv->carrierName;
}

void Route::setCarrierName(const std::string& carrierName)
{
    mPriv->carrierName = carrierName;
}

void Route::swapNames()
{
    mPriv->fromName.swap(mPriv->toName);
}

std::string Route::toString() const
{
    return getFromName() + "->" + getCarrierName() + "->" + getToName();
}
