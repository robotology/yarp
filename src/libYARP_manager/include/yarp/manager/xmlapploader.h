/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Istituto Italiano di Tecnologia (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_MANAGER_XMLAPPLOADER
#define YARP_MANAGER_XMLAPPLOADER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/manifestloader.h>



namespace yarp {
namespace manager {
class TextParser;

/**
 * Class XmlAppLoader
 */
class XmlAppLoader : public AppLoader {

public:
    XmlAppLoader(const char* szFileName);
    XmlAppLoader(const char* szPath, const char* szAppName);
    virtual ~XmlAppLoader();

    bool         init(void) YARP_OVERRIDE;
    void         fini(void) YARP_OVERRIDE;
    void         reset(void) YARP_OVERRIDE;
    Application* getNextApplication(void) YARP_OVERRIDE;

protected:

private:
    std::string         strAppName;
    std::string         strPath;
    std::string         strFileName;
    std::vector<std::string> fileNames;
    Application    app;
    TextParser*    parser;
    Application*   parsXml(const char* szFile);
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_XMLAPPLOADER__
