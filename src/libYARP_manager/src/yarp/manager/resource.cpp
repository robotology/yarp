/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/manager/resource.h>

using namespace yarp::manager;


/**
 * Class GenericResource
 */

GenericResource::GenericResource(const char* szTypeName) : Node(RESOURCE)
{
    modOwner = nullptr;
    bAvailable = true;
    bDisabled = false;
    if(szTypeName)
        strTypeName = szTypeName;
}

GenericResource::~GenericResource() = default;



/**
 * Class MultiResource
 */

MultiResource::MultiResource() : GenericResource("MultipleResource")
{
}


MultiResource::MultiResource(const char* szName) : GenericResource("MultipleResource")
{
    setName(szName);
}


MultiResource::MultiResource(const MultiResource &resource) : GenericResource(resource)
{
    MultiResource::swap(resource);
}


MultiResource& MultiResource::operator=(const MultiResource& rhs)
{
    GenericResource::operator=(rhs);
    MultiResource::swap(rhs);
    return *this;
}


bool MultiResource::addResource(GenericResource& res)
{
    auto* newres = (GenericResource*) res.clone();
    resources.push_back(newres);
    return true;
}

bool MultiResource::satisfy(GenericResource* resource)
{
    // MultiResource is used as intermediate concept for
    // disjunctive resource dependencies and it never
    // can be a provider.
    return false;
}


Node* MultiResource::clone()
{
    auto* resource = new MultiResource(*this);
    return resource;
}

void MultiResource::swap(const MultiResource &res)
{
    clear();
    // deep copy
    for(int i=0; i<res.resourceCount(); i++)
        addResource(res.getResourceAt(i));
}

void MultiResource::clear()
{
    for(auto& resource : resources)
    {
        delete resource;
        resource = nullptr;
    }
    resources.clear();
}


MultiResource::~MultiResource()
{
    clear();
}
