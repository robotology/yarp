/*
 *  Yarp Modules Manager
 *  Copyright: Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "resource.h"


/**
 * Class ResYarpPort
 */
ResYarpPort::ResYarpPort(void) : Node(RESOURCE) 
{
	modOwner = NULL;
}


ResYarpPort::ResYarpPort(const char* szName) : Node(RESOURCE) 
{
	setName(szName);
}


ResYarpPort::ResYarpPort(const ResYarpPort &resource) : Node(resource)
{
	strName = resource.strName;
	strPort = resource.strPort; 
	strDescription = resource.strDescription;
	modOwner = resource.modOwner;
}


ResYarpPort::~ResYarpPort() { }


Node* ResYarpPort::clone(void)
{
	ResYarpPort* resource = new ResYarpPort(*this);
	return resource; 
}

