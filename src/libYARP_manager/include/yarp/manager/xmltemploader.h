/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_MANAGER_XMLTEMPLOADER
#define YARP_MANAGER_XMLTEMPLOADER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/manifestloader.h>
#include <yarp/manager/impl/textparser.h>

namespace yarp {
namespace manager {


/**
 * Class XmlTempLoader
 */
class XmlTempLoader : public TempLoader {

public:
    XmlTempLoader(const char* szFileName);
    XmlTempLoader(const char* szPath, const char* szAppName);
    virtual ~XmlTempLoader();
    bool init(void) YARP_OVERRIDE;
    void fini(void) YARP_OVERRIDE;
    void reset(void) YARP_OVERRIDE;
    AppTemplate* getNextAppTemplate(void) YARP_OVERRIDE;

protected:

private:
    std::string strAppName;
    std::string strPath;
    std::string strFileName;
    std::vector<std::string> fileNames;
    TextParser     parser;
    AppTemplate app;
    AppTemplate* parsXml(const char* szFile);
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_XMLTEMPLOADER__
