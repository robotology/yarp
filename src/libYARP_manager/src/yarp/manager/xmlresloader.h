/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_XMLRESLOADER
#define YARP_MANAGER_XMLRESLOADER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/resource.h>
#include <yarp/manager/primresource.h>
#include <yarp/manager/manifestloader.h>

namespace yarp {
namespace manager {
class TextParser;

/**
 * Class XmlResLoader
 */
class XmlResLoader : public ResourceLoader
{
public:
    XmlResLoader(const char* szFileName);
    XmlResLoader(const char* szPath, const char* szResName);
    ~XmlResLoader() override;
    bool init() override;
    void reset() override;
    void fini() override;
    GenericResource* getNextResource() override;

protected:

private:
    std::string strName;
    std::string strPath;
    std::string strFileName;
    std::vector<std::string> fileNames;
    TextParser*    parser;
    ComputerContainer computers;
    Computer dummyComputer;
    bool parsXml(const char* szFile);
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_XMLRESLOADER__
