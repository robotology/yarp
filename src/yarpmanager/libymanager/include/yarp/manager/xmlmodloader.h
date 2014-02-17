    // -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __XMLMODLOADER__
#define __XMLMODLOADER__

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/module.h>
#include <yarp/manager/data.h>
#include <yarp/manager/manifestloader.h>

//namespace ymm {


/**
 * Class XmlModLoader
 */
class XmlModLoader : public ModuleLoader {

public:
    XmlModLoader(const char* szFileName);
    XmlModLoader(const char* szPath, const char* szModuleName);
    virtual ~XmlModLoader();
    bool init(void);
    void reset(void);
    void fini(void);
    Module* getNextModule(void);

protected:

private:
    string strName;
    string strPath;
    string strFileName;
    vector<string> fileNames;
    Module module;
    Module* parsXml(const char* szFile);
};



//}

#endif //__XMLMODLOADER__
