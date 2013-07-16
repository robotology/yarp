    // -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __XMLRESLOADER__
#define __XMLRESLOADER__

#include "ymm-types.h" 
#include "resource.h"
#include "primresource.h"
#include "manifestloader.h"

//namespace ymm {


/**
 * Class XmlResLoader  
 */
class XmlResLoader : public ResourceLoader 
{
public:
    XmlResLoader(const char* szFileName);
    XmlResLoader(const char* szPath, const char* szResName);
    virtual ~XmlResLoader();
    bool init(void);
    void reset(void);
    void fini(void);
    GenericResource* getNextResource(void);
    
protected:

private:
    string strName;
    string strPath;
    string strFileName;
    vector<string> fileNames;
    ComputerContainer computers;
    Computer dummyComputer; 
    bool parsXml(const char* szFile);
};


 
//}

#endif //__XMLRESLOADER__
