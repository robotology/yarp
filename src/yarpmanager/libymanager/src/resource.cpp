/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/resource.h>

using namespace yarp::manager;


/**
 * Class GenericResource
 */

GenericResource::GenericResource(const char* szTypeName) : Node(RESOURCE)
{
    modOwner = NULL;
    bAvailable = true;
    bDisabled = false;
    if(szTypeName)
        strTypeName = szTypeName;
}


GenericResource::GenericResource(const GenericResource &resource) : Node(resource)
{
    bAvailable = resource.bAvailable;
    bDisabled = resource.bDisabled;
    strName = resource.strName;
    strTypeName = resource.strTypeName;
    strDescription = resource.strDescription;
    modOwner = resource.modOwner;
    strXmlFile = resource.strXmlFile;
}

GenericResource::~GenericResource() { }



/**
 * Class MultiResource
 */

MultiResource::MultiResource(void) : GenericResource("MultipleResource")
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
    GenericResource* newres = (GenericResource*) res.clone();
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


Node* MultiResource::clone(void)
{
    MultiResource* resource = new MultiResource(*this);
    return resource;
}

void MultiResource::swap(const MultiResource &res)
{
    clear();
    // deep copy
    for(int i=0; i<res.resourceCount(); i++)
        addResource(res.getResourceAt(i));
}

void MultiResource::clear(void)
{
    for(unsigned int i=0; i<resources.size(); i++)
    {
        delete resources[i];
        resources[i] = NULL;
    }
    resources.clear();
}


MultiResource::~MultiResource()
{
    clear();
}
