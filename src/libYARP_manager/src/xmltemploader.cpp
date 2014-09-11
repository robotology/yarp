/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/xmltemploader.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/ymm-dir.h>

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
    if(szAppName)
        strAppName = szAppName;

    if(strlen(szPath))
    {
        strPath = szPath;
        if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPERATOR);
    }
}

/**
 * load only one application indicated by its xml file name
 */
XmlTempLoader::XmlTempLoader(const char* szFileName)
{
    if(szFileName)
        strFileName = szFileName;
}


XmlTempLoader::~XmlTempLoader()
{
}


bool XmlTempLoader::init(void)
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
    if ((dir = opendir(strPath.c_str())) == NULL)
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
            if(compareString(ext.c_str(), "xml.template"))
                fileNames.push_back(strPath+name);
        }
    }
    closedir(dir);
    return true;
}

void XmlTempLoader::reset(void)
{
    fini();
    init();
}


void XmlTempLoader::fini(void)
{
    fileNames.clear();
}


AppTemplate* XmlTempLoader::getNextAppTemplate(void)
{
    if(strAppName.empty())
    {
        AppTemplate* app = NULL;
        while(!app)
        {
            if(fileNames.empty())
                return NULL;
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
         if(app && (app->name==strAppName))
            return app;
        }
    }
 return NULL;
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
        return NULL;
    }

    /* retrieving root element */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return NULL;
    }

    if(!compareString(root->Value(), "application"))
    {
        return NULL;
    }

    app.tmpFileName = szFile;

    /* retrieving name */
    TiXmlElement* name = (TiXmlElement*) root->FirstChild("name");
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
        for(unsigned int i=0; i<strname.size(); i++)
            if(strname[i] == ' ')
                strname[i] = '_';
        app.name = strname;
    }

    return &app;
}
