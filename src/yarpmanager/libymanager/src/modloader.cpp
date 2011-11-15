/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>

#include "modloader.h"
#include "tinyxml.h"
#include "utility.h"
#include "ymm-dir.h"

using namespace std;

XmlModLoader::XmlModLoader(const char* szPath, const char* szName)
{
    module.clear();

    if(strlen(szPath))
    {
        strPath = szPath;
        if((strPath.rfind(PATH_SEPERATOR)==string::npos) || 
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPERATOR);
    }
    
    if(szName)
        strName = szName;
}

/**
 * load only one module indicated by its xml file name 
 */
XmlModLoader::XmlModLoader(const char* szFileName)
{
    module.clear();
    if(szFileName)
        strFileName = szFileName;   
}



XmlModLoader::~XmlModLoader()
{
    
}


bool XmlModLoader::init(void)
{
    module.clear();
    fileNames.clear();
    ErrorLogger* logger  = ErrorLogger::Instance();
    
    /**
     * loading single module indicated by its xml file name 
     */
    if(!strFileName.empty())
    {
        fileNames.push_back(strFileName);
        return true;
    }

    if(strPath.empty())
    {
        logger->addError("No module path is introduced.");
        return false; 
    }

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == NULL)
    {       
        ostringstream err;
        err<<"Cannot access "<<strPath;
        logger->addError(err);
        return false;
    }
    
    /* we need to load all xml modules */   
    while((entry = readdir(dir)))
    {
        string name = entry->d_name;
        if(name.size() > 3)
        {
            string ext = name.substr(name.size()-3,3);
            if(compareString(ext.c_str(), "xml"))
                fileNames.push_back(strPath+name);
        }
    }
    closedir(dir);  

    /*
    if(fileNames.empty())
    {
        ostringstream err;
        err<<"No xml module file found in "<<strPath;
        logger->addWarning(err);
        //return true;
    }   
    */
    return true;
}


void XmlModLoader::reset(void)
{
    fini();
    init();
}


void XmlModLoader::fini(void)
{
    fileNames.clear();
    module.clear();
}


Module* XmlModLoader::getNextModule(void)
{   
    if(strName.empty())
    {
        Module* mod = NULL;
        while(!mod)
        {
            if(fileNames.empty())
                return NULL;
                
            string fname = fileNames.back();
            fileNames.pop_back();
            mod = parsXml(fname.c_str());       
        }   
        return mod;
    }
    else
    {
        /**
         * we need to check for a single module 
         */
         vector<string>::iterator itr; 
         for(itr=fileNames.begin(); itr<fileNames.end(); itr++)
         {
             Module* mod = parsXml((*itr).c_str());
             if(mod && (string(mod->getName())==strName))
                return mod;
         }
    }
    return NULL;
}



Module* XmlModLoader::parsXml(const char* szFile)
{
    module.clear();
    ErrorLogger* logger  = ErrorLogger::Instance();
    
    TiXmlDocument doc(szFile);
    if(!doc.LoadFile()) 
    {
        ostringstream err;
        err<<"Syntax error while loading "<<szFile<<" at line "\
           <<doc.ErrorRow()<<": ";
        err<<doc.ErrorDesc();
        logger->addError(err);
        return NULL;
    }
    
    /* retrieving root module */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        ostringstream err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return NULL;
    }
    
    if(!compareString(root->Value(), "module"))
    {
        /*
        ostringstream msg;
        msg<<szFile<<" is not a module descriptor file.";
        logger->addWarning(msg);
        */
        return NULL;
    }
    
    /* retrieving name */
    TiXmlElement* name = (TiXmlElement*) root->FirstChild("name");
    if(!name || !name->GetText())
    {
        ostringstream err;
        err<<"Module from "<<szFile<<" has no name.";
        logger->addError(err);      
        //return NULL;
    }

    module.setXmlFile(szFile);
    
    if(name)
        module.setName(name->GetText());

    /* retrieving description */
    TiXmlElement* desc;
    if((desc = (TiXmlElement*) root->FirstChild("description")))
        module.setDescription(desc->GetText());

    /* retrieving version */
    TiXmlElement* ver;
    if((ver = (TiXmlElement*) root->FirstChild("version")))
        module.setVersion(ver->GetText());


    /* retrieving parameter */
    TiXmlElement* arguments;
    if((arguments = (TiXmlElement*) root->FirstChild("arguments")))
        for(TiXmlElement* param = arguments->FirstChildElement(); param;
                param = param->NextSiblingElement())
        {
            if(compareString(param->Value(), "param"))
            {           
                if(param->GetText())
                {   
                    bool brequired = false;
                    if(compareString(param->Attribute("required"), "yes"))
                        brequired = true; 
                    Argument arg(param->GetText(), 
                                 brequired,
                                 param->Attribute("desc"));
                    module.addArgument(arg);                 
                }
            }
            else
            {
                ostringstream war;
                war<<"Unrecognized tag from "<<szFile<<" at line "\
                   <<param->Row()<<".";
                logger->addWarning(war);                                
            }
            
        }
    

    /* retrieving rank */
    TiXmlElement* rank;
    if((rank = (TiXmlElement*) root->FirstChild("rank")) &&
        rank->GetText())
        module.setRank(atoi(rank->GetText()));


    /* retrieving authors information*/
    TiXmlElement* authors;
    if((authors = (TiXmlElement*) root->FirstChild("authors")))
        for(TiXmlElement* ath = authors->FirstChildElement(); ath;
                ath = ath->NextSiblingElement())
        {
            if(compareString(ath->Value(), "author"))
            {
            
                string info;
                if(ath->GetText())
                    info = ath->GetText();
                if(ath->Attribute("email"))
                {
                    info += string("/");
                    info += string(ath->Attribute("email"));
                }
                module.addAuthor(info.c_str());
            }
            else
            {
                ostringstream war;
                war<<"Unrecognized tag from "<<szFile<<" at line "\
                   <<ath->Row()<<".";
                logger->addWarning(war);                                
            }
            
        }


    /* retrieving platforms information*/
    TiXmlElement* platforms;
    if((platforms = (TiXmlElement*) root->FirstChild("platforms")))
        for(TiXmlElement* plat = platforms->FirstChildElement(); plat;
                plat = plat->NextSiblingElement())
        {
            if(compareString(plat->Value(), "os"))
            {
                Platform platform(plat->GetText(), plat->Attribute("dist"));
                module.addPlatform(platform);
            }
            else
            {
                ostringstream war;
                war<<"Unrecognized tag from "<<szFile<<" at line "\
                   <<plat->Row()<<".";
                logger->addWarning(war);                                
            }
        }


    /* retrieving data */
    if(root->FirstChild("data"))
        for(TiXmlElement* data = root->FirstChild("data")->FirstChildElement();
            data; data = data->NextSiblingElement())
        {       
            /* output data */               
            if(compareString(data->Value(), "output"))
            {
                OutputData output;
                TiXmlElement* element;
                if((element = (TiXmlElement*) data->FirstChild("type")))
                    output.setName(element->GetText());
                else
                {
                    ostringstream war;
                    war<<"Output data from "<<szFile<<" at line "\
                       <<data->Row()<<" has no type.";
                    logger->addWarning(war);                
                }
                
                if((element = (TiXmlElement*) data->FirstChild("port")))
                {
                    output.setPort(element->GetText());                 
                    output.setCarrier(strToCarrier(element->Attribute("carrier")));                     
                }
                else
                {
                    ostringstream war;
                    war<<"Output data from "<<szFile<<" at line "\
                       <<data->Row()<<" has no port.";
                    logger->addWarning(war);                
                }
                
                if((element = (TiXmlElement*) data->FirstChild("description")))
                    output.setDescription(element->GetText());

                module.addOutput(output);
            } // end of output data         

            /* input data */                
            if(compareString(data->Value(), "input"))
            {                   
                InputData input;
                
                TiXmlElement* element;
                if((element = (TiXmlElement*) data->FirstChild("type")))
                    input.setName(element->GetText());
                else
                {
                    ostringstream war;
                    war<<"Input data from "<<szFile<<" at line "\
                       <<data->Row()<<" has no type.";
                    logger->addWarning(war);                
                }
                
                if((element = (TiXmlElement*) data->FirstChild("port")))
                {
                    input.setPort(element->GetText());                  
                    input.setCarrier(strToCarrier(element->Attribute("carrier")));                      
                }
                else
                {
                    ostringstream war;
                    war<<"Input data from "<<szFile<<" at line "\
                       <<data->Row()<<" has no port.";
                    logger->addWarning(war);                
                }
                
                if((element = (TiXmlElement*) data->FirstChild("description")))
                    input.setDescription(element->GetText());

                if((element = (TiXmlElement*) data->FirstChild("required")))
                    if(compareString(element->GetText(), "yes"))
                        input.setRequired(true);

                if((element = (TiXmlElement*) data->FirstChild("priority")))
                    if(compareString(element->GetText(), "yes"))
                        input.setPriority(true);
                
                module.addInput(input);
            } // end of input data          

        }
        
    return &module;
}


