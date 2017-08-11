/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2012 Istituto Italiano di Tecnologia (IIT)
 *  Authors: Elena Ceseracciu <elena.ceseracciu@iit.it>
 *           Ali Paikan <ali.paikan@iit.it>
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_MANAGER_XMLAPPSAVER
#define YARP_MANAGER_XMLAPPSAVER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/manifestloader.h>

namespace yarp {
namespace manager {


/**
 * Class XmlAppSaver
 */
class XmlAppSaver : public AppSaver
{

public:
    XmlAppSaver(const char* szFileName=NULL);
    virtual bool save(Application* application) override;
    virtual ~XmlAppSaver();

protected:

private:
    bool serialXml(Application* app, const char* szFile);
    std::string strFileName;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_XMLAPPSAVER__
