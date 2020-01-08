/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_XMLMODLOADER
#define YARP_MANAGER_XMLMODLOADER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/module.h>
#include <yarp/manager/data.h>
#include <yarp/manager/manifestloader.h>

namespace yarp {
namespace manager {

class TextParser;
/**
 * Class XmlModLoader
 */
class XmlModLoader : public ModuleLoader {

public:
    XmlModLoader(const char* szFileName);
    XmlModLoader(const char* szPath, const char* szModuleName);
    virtual ~XmlModLoader();
    bool init(void) override;
    void reset(void) override;
    void fini(void) override;
    Module* getNextModule(void) override;

protected:

private:
    std::string strName;
    std::string strPath;
    std::string strFileName;
    std::vector<std::string> fileNames;
    TextParser*    parser;
    Module module;
    Module* parsXml(const char* szFile);
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_XMLMODLOADER__
