// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __RESOURCE__
#define __RESOURCE__

#include "ymm-types.h" 
#include "node.h"
#include "utility.h"

using namespace std; 

//namespace ymm {


class ResYarpPort : public Node{

public: 
	ResYarpPort(void);
	ResYarpPort(const char* szName);
	ResYarpPort(const ResYarpPort &res);
	~ResYarpPort();
	virtual Node* clone(void);
	void setName(const char* szName) { if(szName) strName = szName; }	
	const char* getName(void) { return strName.c_str(); }
	void setPort(const char* szPort) { if(szPort) strPort = szPort; }
	const char* getPort(void) { return strPort.c_str(); }
	void setDescription(const char* szDesc) { if(szDesc) strDescription = szDesc; }
	const char* getDescription(void) { return strDescription.c_str(); }
	void setOwner(Node* owner) { modOwner = owner; }
	const Node* owner(void) { return modOwner; }
	
	bool operator==(const ResYarpPort& res) {		
		return (strName == res.strName); 
	}
	
protected:

private:
	string strName;
	string strPort; 
	string strDescription;
	Node*  modOwner; 
};
 
 
//}

#endif //__RESOURCE__
