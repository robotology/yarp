/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    ~XmlModLoader() override;
    bool init() override;
    void reset() override;
    void fini() override;
    Module* getNextModule() override;

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
