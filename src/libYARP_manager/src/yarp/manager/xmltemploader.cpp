/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/xmltemploader.h>
#include <yarp/conf/filesystem.h>
#include <yarp/manager/utility.h>
#include <dirent.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>

#include <tinyxml.h>


using namespace std;
using namespace yarp::manager;


/**
 * if szAppName is NULL, XmlTempLoader will load all the applications
 * found in szPath otherwise only one application named szAppname
 * will be loaded.
 */
XmlTempLoader::XmlTempLoader(const char* szPath, const char* szAppName)
{
    if (szAppName) {
        strAppName = szAppName;
    }

    if(strlen(szPath))
    {
        const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};
        strPath = szPath;
        if ((strPath.rfind(directorySeparator) == string::npos) || (strPath.rfind(directorySeparator) != strPath.size() - 1)) {
            strPath = strPath + string(directorySeparator);
        }
    }
}

/**
 * load only one application indicated by its xml file name
 */
XmlTempLoader::XmlTempLoader(const char* szFileName)
{
    if (szFileName) {
        strFileName = szFileName;
    }
}


XmlTempLoader::~XmlTempLoader() = default;


bool XmlTempLoader::init()
{
    fileNames.clear();
    ErrorLogger* logger  = ErrorLogger::Instance();

    /**
     * loading single applicaition template indicated by its xml file name
     */
    if(!strFileName.empty())
    {
        fileNames.push_back(strFileName);
        return true;
    }

    if(strPath.empty())
    {
        logger->addError("No application template path is introduced.");
        return false;
    }

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == nullptr)
    {
        OSTRINGSTREAM err;
        err<<"Cannot access "<<strPath;
        logger->addError(err);
        return false;
    }

    /* we need to load all xml app templates */
    while((entry = readdir(dir)))
    {
        string name = entry->d_name;
        if(name.size() > 12)
        {
            string ext = name.substr(name.size()-12,12);
            if (compareString(ext.c_str(), "xml.template")) {
                fileNames.push_back(strPath + name);
            }
        }
    }
    closedir(dir);
    return true;
}

void XmlTempLoader::reset()
{
    fini();
    init();
}


void XmlTempLoader::fini()
{
    fileNames.clear();
}


AppTemplate* XmlTempLoader::getNextAppTemplate()
{
    if(strAppName.empty())
    {
        AppTemplate* app = nullptr;
        while(!app)
        {
            if (fileNames.empty()) {
                return nullptr;
            }
            string fname = fileNames.back();
            fileNames.pop_back();
            app = parsXml(fname.c_str());
        }
        return app;
    }
    else
    {
        vector<string>::iterator itr;
        for(itr=fileNames.begin(); itr<fileNames.end(); itr++)
        {
         AppTemplate* app = parsXml((*itr).c_str());
         if (app && (app->name == strAppName)) {
             return app;
         }
        }
    }
 return nullptr;
}



AppTemplate* XmlTempLoader::parsXml(const char* szFile)
{
    app.name.clear();
    app.tmpFileName.clear();

    ErrorLogger* logger  = ErrorLogger::Instance();

    TiXmlDocument doc(szFile);
    if(!doc.LoadFile())
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" at line "\
           <<doc.ErrorRow()<<": ";
        err<<doc.ErrorDesc();
        logger->addError(err);
        return nullptr;
    }

    /* retrieving root element */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return nullptr;
    }

    if(!compareString(root->Value(), "application"))
    {
        return nullptr;
    }

    app.tmpFileName = szFile;

    /* retrieving name */
    auto* name = (TiXmlElement*) root->FirstChild("name");
    if(!name || !name->GetText())
    {
        OSTRINGSTREAM err;
        err<<"Application from "<<szFile<<" has no name.";
        logger->addError(err);
        //return NULL;
    }
    else
    {
        string strname = name->GetText();
        for (char& i : strname) {
            if (i == ' ') {
                i = '_';
            }
        }
        app.name = strname;
    }

    return &app;
}
