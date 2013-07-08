// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __XMLTEMPLOADER__
#define __XMLTEMPLOADER__

#include "ymm-types.h" 
#include "manifestloader.h"

//namespace ymm {

/**
 * Class XmlTempLoader  
 */
class XmlTempLoader : public TempLoader {

public:
    XmlTempLoader(const char* szFileName);
    XmlTempLoader(const char* szPath, const char* szAppName);
    virtual ~XmlTempLoader();
    bool init(void);
    void fini(void);
    void reset(void);
    AppTemplate* getNextAppTemplate(void);
    
protected:

private:
    string strAppName;
    string strPath;
    string strFileName;
    vector<string> fileNames;
    AppTemplate app;
    AppTemplate* parsXml(const char* szFile);
};


 
//}

#endif //__XMLTEMPLOADER__
