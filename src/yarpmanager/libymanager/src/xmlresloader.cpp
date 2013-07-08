/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>

#include <tinyxml.h>

#include "utility.h"
#include "ymm-dir.h"
#include "xmlresloader.h"
#include "physicresource.h"
#include "logicresource.h"


using namespace std;

XmlResLoader::XmlResLoader(const char* szPath, const char* szName)
{
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
XmlResLoader::XmlResLoader(const char* szFileName) 
{
    if(szFileName)
        strFileName = szFileName;   
}



XmlResLoader::~XmlResLoader()
{
}


bool XmlResLoader::init(void)
{
    fileNames.clear();
    ErrorLogger* logger  = ErrorLogger::Instance();
    
    /**
     * loading single resource indicated by its xml file name 
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
        OSTRINGSTREAM err;
        err<<"Cannot access "<<strPath;
        logger->addError(err);
        return false;
    }
    
    /* we need to load all xml files */   
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
        OSTRINGSTREAM err;
        err<<"No xml resource file found in "<<strPath;
        logger->addWarning(err);
        //return true;
    }   
    */
    return true;
}


void XmlResLoader::reset(void)
{
    fini();
    init();
}


void XmlResLoader::fini(void)
{
    fileNames.clear();
}


GenericResource* XmlResLoader::getNextResource(void)
{  
    if(strName.empty())
    {
        if(computers.size())
        {
            dummyComputer = computers.back();
            computers.pop_back();
            return &dummyComputer;
        }
        else
        {
            bool ret = false;
            do
            {
                if(fileNames.empty())
                    return NULL;
                    
                string fname = fileNames.back();
                fileNames.pop_back();
                ret = parsXml(fname.c_str());       
            } while(!ret);
           
            dummyComputer = computers.back();
            computers.pop_back();
            return &dummyComputer;
        }
    }
    else
    {
        /**
         * we need to check for a single resource 
         */
         vector<string>::iterator itr; 
         for(itr=fileNames.begin(); itr<fileNames.end(); itr++)
         {
             if(parsXml((*itr).c_str()))
             {
                for(ComputerIterator itr = computers.begin(); itr != computers.end(); itr++)
                if(string((*itr).getName()) == strName)
                    return &(*itr);
             }
         }
    }
    return NULL;
}



bool XmlResLoader::parsXml(const char* szFile)
{
    computers.clear();

    ErrorLogger* logger  = ErrorLogger::Instance();
    
    TiXmlDocument doc(szFile);
    if(!doc.LoadFile()) 
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" at line "\
           <<doc.ErrorRow()<<": ";
        err<<doc.ErrorDesc();
        logger->addError(err);
        return false;
    }
    /* retrieving root module */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return false;
    }
        
    if(!compareString(root->Value(), "resources"))
    {
        /*
        OSTRINGSTREAM msg;
        msg<<szFile<<" is not a resource descriptor file.";
        logger->addWarning(msg);
        */
        return false;
    }
    
    /* retrieving all computers descriptions */
    for(TiXmlElement* restag = root->FirstChildElement();
            restag; restag = restag->NextSiblingElement())
    {
        /* retrieving a computer resource */    
        if(compareString(restag->Value(), "computer"))
        {
            Computer computer;
            computer.setXmlFile(szFile);

            for(TiXmlElement* comptag = restag->FirstChildElement();
                comptag; comptag = comptag->NextSiblingElement())
            {       
                 /* retrieving name */
                if(compareString(comptag->Value(), "name"))                               
                    computer.setName(comptag->GetText());

                /* retrieving description */
                 if(compareString(comptag->Value(), "description"))                  
                    computer.setDescription(comptag->GetText());

                /* retrieving disablility */
                if(compareString(comptag->Value(), "disable"))
                {
                    if(compareString(comptag->GetText(), "yes"))
                        computer.setDisable(true);
                }

                // platform
                if(compareString(comptag->Value(), "platform"))
                {
                    Platform os;
                    TiXmlElement* element;
                    if((element = (TiXmlElement*) comptag->FirstChild("name")))
                        os.setName(element->GetText());
                    else
                    {
                        OSTRINGSTREAM war;
                        war<<"Platform from "<<szFile<<" at line "\
                           <<comptag->Row()<<" has no name.";
                        logger->addWarning(war);                
                    }
                    
                    if((element = (TiXmlElement*) comptag->FirstChild("distribution")))
                        os.setDistribution(element->GetText());
                    
                    if((element = (TiXmlElement*) comptag->FirstChild("release")))
                        os.setRelease(element->GetText()); 

                    computer.setPlatform(os);
                } // end of platform tag

                // memory
                if(compareString(comptag->Value(), "memory"))
                {
                    Memory mem;
                    TiXmlElement* element;
                    if((element = (TiXmlElement*) comptag->FirstChild("total_space")))
                        mem.setTotalSpace((Capacity)atol(element->GetText()));               
                   computer.setMemory(mem);
                } // end of memory tag

                // storage
                if(compareString(comptag->Value(), "storage"))
                {
                    Storage stg;
                    TiXmlElement* element;
                    if((element = (TiXmlElement*) comptag->FirstChild("total_space")))
                        stg.setTotalSpace((Capacity)atol(element->GetText()));               
                   computer.setStorage(stg);
                } // end of storage tag

                // processor
                if(compareString(comptag->Value(), "processor"))
                {
                    Processor proc;
                    TiXmlElement* element;
                    if((element = (TiXmlElement*) comptag->FirstChild("architecture")))
                        proc.setArchitecture(element->GetText());
                    if((element = (TiXmlElement*) comptag->FirstChild("model")))
                        proc.setModel(element->GetText());
                    if((element = (TiXmlElement*) comptag->FirstChild("cores")))
                        proc.setCores((size_t)atoi(element->GetText()));               
                    if((element = (TiXmlElement*) comptag->FirstChild("frequency")))
                        proc.setFrequency(atof(element->GetText()));
                   computer.setProcessor(proc);
                } // end of processor tag

                // network
                if(compareString(comptag->Value(), "network"))
                {
                    Network net;
                    TiXmlElement* element;
                    if((element = (TiXmlElement*) comptag->FirstChild("ip4")))
                        net.setIP4(element->GetText());
                    if((element = (TiXmlElement*) comptag->FirstChild("ip6")))
                        net.setIP6(element->GetText());
                    if((element = (TiXmlElement*) comptag->FirstChild("mac")))
                        net.setMAC(element->GetText());
                    computer.setNetwork(net);
                } // end of network tag


                // gpu
                if(compareString(comptag->Value(), "gpu"))
                {
                    GPU gpu;
                    TiXmlElement* element;
                    if((element = (TiXmlElement*) comptag->FirstChild("name")))
                        gpu.setName(element->GetText());
                    if((element = (TiXmlElement*) comptag->FirstChild("capability")))
                        gpu.setCompCompatibility(element->GetText());
                    if((element = (TiXmlElement*) comptag->FirstChild("cores")))
                        gpu.setCores((size_t)atoi(element->GetText()));
                    if((element = (TiXmlElement*) comptag->FirstChild("frequency")))
                        gpu.setFrequency(atof(element->GetText()));
                    if((element = (TiXmlElement*) comptag->FirstChild("register_block")))
                        gpu.setResgisterPerBlock((size_t)atoi(element->GetText()));
                    if((element = (TiXmlElement*) comptag->FirstChild("thread_block")))
                        gpu.setThreadPerBlock((size_t)atoi(element->GetText()));
                    if((element = (TiXmlElement*) comptag->FirstChild("overlap")))
                    {
                        if(compareString(element->GetText(), "yes"))
                            gpu.setOverlap(true);
                        else
                            gpu.setOverlap(false);
                    }
                 
                    // global memory
                    if(comptag->FirstChild("global_memory"))
                    {
                        TiXmlElement* element;
                        element = (TiXmlElement*) comptag->FirstChild("global_memory");
                        if((element = (TiXmlElement*) element->FirstChild("total_space")))
                            gpu.setGlobalMemory((Capacity)atol(element->GetText()));            
                    } // end of global memory tag

                    // shared memory
                    if(comptag->FirstChild("shared_memory"))
                    {
                        TiXmlElement* element;
                        element = (TiXmlElement*) comptag->FirstChild("shared_memory");
                        if((element = (TiXmlElement*) element->FirstChild("total_space")))
                            gpu.setSharedMemory((Capacity)atol(element->GetText()));            
                    } // end of shared memory tag

                    // constant memory
                    if(comptag->FirstChild("constant_memory"))
                    {
                        TiXmlElement* element;
                        element = (TiXmlElement*) comptag->FirstChild("constant_memory");
                        if((element = (TiXmlElement*) element->FirstChild("total_space")))
                            gpu.setConstantMemory((Capacity)atol(element->GetText()));            
                    } // end of shared memory tag

                   computer.addPeripheral(gpu);
                } // end of gpu tag
            } // end of computer loop 

            computers.push_back(computer);
        } // end of if computer
    } // end of resources 
    return true;
}


