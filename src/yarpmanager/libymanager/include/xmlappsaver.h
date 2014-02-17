// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2012 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Elena Ceseracciu, Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __XMLAPPSAVER__
#define __XMLAPPSAVER__

#include "ymm-types.h"
#include "manifestloader.h"

//namespace ymm {


/**
 * Class XmlAppSaver
 */
class XmlAppSaver : public AppSaver
{

public:
    XmlAppSaver(const char* szFileName=NULL);
    virtual bool save(Application* application);
    virtual ~XmlAppSaver();

protected:

private:
    bool serialXml(Application* app, const char* szFile);
    std::string strFileName;
};



//}

#endif //__XMLAPPSAVER__
