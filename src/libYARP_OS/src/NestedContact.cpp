/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NestedContact.h>

using namespace yarp::os;

class NestedContact::Private
{
public:
    bool fromString(const std::string& nFullName);

    std::string fullName;
    std::string nodeName;
    std::string nestedName;
    std::string category;
    std::string wireType;
};

bool NestedContact::Private::fromString(const std::string& nFullName)
{
    fullName = nFullName;
    std::string::size_type idx2 = fullName.find(":/");
    if (idx2 != std::string::npos) {
        fullName = fullName.substr(idx2 + 2, fullName.length());
    }
    nodeName = fullName;
    nestedName = "";
    category = "";
    std::string::size_type idx = fullName.find('~');
    if (idx != std::string::npos) {
        // We have a type name squeezed in here, into what promises
        // to be a very full port name.
        wireType = fullName.substr(idx + 1, fullName.length());
        fullName = fullName.substr(0, idx);
    }
    idx = fullName.find('@');
    if (idx != std::string::npos) {
        // Great!  Looks like we are using a new syntax suggested
        // by Lorenzo Natale, /topic@/node
        nestedName = fullName.substr(0, idx);
        nodeName = fullName.substr(idx + 1, fullName.length());
        char ch = nestedName[nestedName.length() - 1];
        if (ch == '-' || ch == '+' || ch == '1') {
            size_t offset = 1;
            bool ok = true;
            if (ch == '1') {
                ok = false;
                if (nestedName.length() >= 2) {
                    char ch0 = nestedName[nestedName.length() - 2];
                    if (ch0 == '-' || ch0 == '+') {
                        offset++;
                        category += ch0;
                        ok = true;
                    }
                }
            }
            if (ok) {
                category += ch;
                nestedName = nestedName.substr(0, nestedName.length() - offset);
            }
        }
        return true;
    }
    idx = fullName.find('=');
    if (idx != std::string::npos) {
        nodeName = fullName.substr(0, idx);
        nestedName = fullName.substr(idx + 1, fullName.length());
        idx = nestedName.find('/');
        if (idx != std::string::npos) {
            if (idx == 0) {
                return true;
            }
            category = nestedName.substr(0, idx);
            nestedName = nestedName.substr(idx, nestedName.length());
            return true;
        }
    }
    idx = fullName.find('#');
    if (idx != std::string::npos) {
        nodeName = fullName.substr(0, idx);
        nestedName = fullName.substr(idx + 1, fullName.length());
        char ch = nodeName[nodeName.length() - 1];
        if (ch == '-' || ch == '+') {
            category += ch;
            nodeName = nodeName.substr(0, nodeName.length() - 1);
        }
        return true;
    }
    return false;
}


NestedContact::NestedContact() :
        mPriv(new Private())
{
}

NestedContact::NestedContact(const std::string& fullName) :
        mPriv(new Private())
{
    fromString(fullName);
}

NestedContact::NestedContact(const NestedContact& rhs) :
        mPriv(new Private(*(rhs.mPriv)))
{
}

NestedContact::NestedContact(NestedContact&& rhs) noexcept :
        mPriv(rhs.mPriv)
{
    rhs.mPriv = nullptr;
}

NestedContact::~NestedContact()
{
    delete mPriv;
}

NestedContact& NestedContact::operator=(const NestedContact& rhs)
{
    if (&rhs != this) {
        *mPriv = *(rhs.mPriv);
    }
    return *this;
}

NestedContact& NestedContact::operator=(NestedContact&& rhs) noexcept
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

bool NestedContact::fromString(const std::string& fullName)
{
    return mPriv->fromString(fullName);
}

void NestedContact::setTypeName(const std::string& nWireType)
{
    mPriv->wireType = nWireType;
}

void NestedContact::setCategoryWrite()
{
    mPriv->category = "+";
}

void NestedContact::setCategoryRead()
{
    mPriv->category = "-";
}

std::string NestedContact::getFullName() const
{
    return mPriv->fullName;
}

std::string NestedContact::getNodeName() const
{
    return mPriv->nodeName;
}

std::string NestedContact::getNestedName() const
{
    return mPriv->nestedName;
}

std::string NestedContact::getCategory() const
{
    return mPriv->category;
}

std::string NestedContact::getTypeName() const
{
    return mPriv->wireType;
}

std::string NestedContact::getTypeNameStar() const
{
    return (!mPriv->wireType.empty()) ? mPriv->wireType : "*";
}

bool NestedContact::isNested() const
{
    return !mPriv->nestedName.empty();
}

std::string NestedContact::toString() const
{
    return mPriv->nestedName + mPriv->category + "@" + mPriv->nodeName;
}
