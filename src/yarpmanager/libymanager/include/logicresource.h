// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __LOGICRESOURCE__
#define __LOGICRESOURCE__

#include "ymm-types.h" 
#include "node.h"
#include "utility.h"
#include "resource.h"

using namespace std; 


class Platform : public GenericResource 
{
public: 
    Platform(void);
    Platform(const char* szName);
    Platform(const Platform &res);
    virtual ~Platform();
    virtual Node* clone(void);
    virtual bool satisfy(GenericResource* resource);

    void setDistribution(const char* str) { if(str) strDistrib = str; }
    void setRelease(const char* str) { if(str) strRelease = str; }
    const char* getDistribution(void) { return strDistrib.c_str(); }
    const char* getRelease(void) { return strRelease.c_str(); }

protected:

private:
    string strDistrib;
    string strRelease;
    bool satisfy_platform(Platform* os);
   
};
 

class ResYarpPort : public GenericResource 
{
public: 
    ResYarpPort(void);
    ResYarpPort(const char* szName);
    ResYarpPort(const ResYarpPort &res);
    virtual ~ResYarpPort();
    void setPort(const char* szPort) { if(szPort) strPort = szPort; }
    const char* getPort(void) { return strPort.c_str(); }
    void setTimeout(double t) { timeout = t; }
    double getTimeout(void) { return timeout; }
    virtual Node* clone(void);
    virtual bool satisfy(GenericResource* resource);
    
protected:

private:
    string strPort; 
    double timeout;
};
 
 
//}

#endif //__LOGICRESOURCE__

