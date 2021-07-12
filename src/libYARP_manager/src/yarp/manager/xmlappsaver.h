/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    bool save(Application* application) override;
    virtual ~XmlAppSaver();

protected:

private:
    bool serialXml(Application* app, const char* szFile);
    std::string strFileName;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_XMLAPPSAVER__
