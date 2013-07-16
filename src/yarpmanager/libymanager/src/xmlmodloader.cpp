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
#include "xmlmodloader.h"
#include "physicresource.h"
#include "logicresource.h"
#include "primresource.h"


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
        OSTRINGSTREAM err;
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
        OSTRINGSTREAM err;
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
        OSTRINGSTREAM err;
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
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return NULL;
    }
    
    if(!compareString(root->Value(), "module"))
    {
        /*
        OSTRINGSTREAM msg;
        msg<<szFile<<" is not a module descriptor file.";
        logger->addWarning(msg);
        */
        return NULL;
    }
    
    /* retrieving name */
    TiXmlElement* name = (TiXmlElement*) root->FirstChild("name");
    if(!name || !name->GetText())
    {
        OSTRINGSTREAM err;
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
                    arg.setDefault(param->Attribute("default"));
                    module.addArgument(arg);                 
                }
            }
            else
            if(compareString(param->Value(), "switch"))
            {           
                if(param->GetText())
                {   
                    bool brequired = false;
                    if(compareString(param->Attribute("required"), "yes"))
                        brequired = true; 
                    Argument arg(param->GetText(), 
                                 brequired,
                                 param->Attribute("desc"), true);
                    arg.setDefault(param->Attribute("default"));
                    module.addArgument(arg);                 
                }
            }                      
            else
            {
                OSTRINGSTREAM war;
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
                Author author;
                if(ath->GetText())
                    author.setName(ath->GetText());
                if(ath->Attribute("email"))
                    author.setEmail(ath->Attribute("email"));
                module.addAuthor(author);
            }
            else
            {
                OSTRINGSTREAM war;
                war<<"Unrecognized tag from "<<szFile<<" at line "\
                   <<ath->Row()<<".";
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
                    OSTRINGSTREAM war;
                    war<<"Output data from "<<szFile<<" at line "\
                       <<data->Row()<<" has no type.";
                    logger->addWarning(war);                
                }
                
                if((element = (TiXmlElement*) data->FirstChild("port")))
                {
                    output.setPort(element->GetText());                 
                    output.setCarrier(element->Attribute("carrier"));  
                }
                else
                {
                    OSTRINGSTREAM war;
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
                    OSTRINGSTREAM war;
                    war<<"Input data from "<<szFile<<" at line "\
                       <<data->Row()<<" has no type.";
                    logger->addWarning(war);                
                }
                
                if((element = (TiXmlElement*) data->FirstChild("port")))
                {
                    input.setPort(element->GetText());                  
                    input.setCarrier(element->Attribute("carrier"));
                }
                else
                {
                    OSTRINGSTREAM war;
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

    /* retrieving broker*/
    TiXmlElement* element;
    if((element = (TiXmlElement*) root->FirstChild("deployer")))
    {
        module.setBroker(element->GetText());
        module.setNeedDeployer(true);
    }

    /* retrieving dependencies*/
    if(root->FirstChild("dependencies"))
        for(TiXmlElement* restag = root->FirstChild("dependencies")->FirstChildElement();
            restag; restag = restag->NextSiblingElement())
        {
            Computer computer;
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

                    // platform
                    if(compareString(comptag->Value(), "platform"))
                    {
                        Platform os;
                        TiXmlElement* element;
                        if((element = (TiXmlElement*) comptag->FirstChild("name")))
                            os.setName(element->GetText());               
                        if((element = (TiXmlElement*) comptag->FirstChild("distribution")))
                            os.setDistribution(element->GetText());                
                        if((element = (TiXmlElement*) comptag->FirstChild("release")))
                            os.setRelease(element->GetText()); 
                        computer.setPlatform(os);
                    } // end of platform tag
                
                    /*
                    //multiplatform
                    if(compareString(comptag->Value(), "multiplatform"))
                    {
                        MultiPlatform mltPlatform;
                        for(TiXmlElement* mptag = comptag->FirstChild("multiplatform")->FirstChildElement();
                            mptag; mptag = mptag->NextSiblingElement())
                        {
                            // platform
                            if(compareString(mptag->Value(), "platform"))
                            {
                                Platform os;
                                TiXmlElement* element;
                                if((element = (TiXmlElement*) mptag->FirstChild("name")))
                                    os.setName(element->GetText());               
                                if((element = (TiXmlElement*) mptag->FirstChild("distribution")))
                                    os.setDistribution(element->GetText());            
                                if((element = (TiXmlElement*) mptag->FirstChild("release")))
                                    os.setDistribution(element->GetText());
                                mltPlatform.addPlatform(os);
                            } 
                        }
                        module.addResource(mltPlatform);
                    }
                    // end of multiplatform tag 
                    */
                    // memory
                    if(compareString(comptag->Value(), "memory"))
                    {
                        Memory mem;
                        TiXmlElement* element;
                        if((element = (TiXmlElement*) comptag->FirstChild("total_space")))
                            mem.setTotalSpace((Capacity)atol(element->GetText()));               
                        if((element = (TiXmlElement*) comptag->FirstChild("free_space")))
                            mem.setFreeSpace((Capacity)atol(element->GetText()));                   
                        computer.setMemory(mem);                    
                    } // end of memory tag

                    // storage
                    if(compareString(comptag->Value(), "storage"))
                    {
                        Storage stg;
                        TiXmlElement* element;
                        if((element = (TiXmlElement*) comptag->FirstChild("total_space")))
                            stg.setTotalSpace((Capacity)atol(element->GetText()));               
                        if((element = (TiXmlElement*) comptag->FirstChild("free_space")))
                            stg.setFreeSpace((Capacity)atol(element->GetText()));                   
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
                        if((element = (TiXmlElement*) comptag->FirstChild("siblings")))
                            proc.setSiblings((size_t)atoi(element->GetText()));                                           
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
                        module.addResource(net);
                        computer.setNetwork(net);
                    } // end of network tag

                    // yarp_port
                    if(compareString(comptag->Value(), "yarp_port"))
                    {
                        ResYarpPort yport;
                        TiXmlElement* element = (TiXmlElement*) comptag->FirstChild("name");
                        if(element && element->GetText())
                        {
                            yport.setName(element->GetText());
                            yport.setPort(element->GetText());
                            computer.addPeripheral(yport);
                        }
                        else
                        {
                            OSTRINGSTREAM war;
                            war<<"yarp_port from "<<szFile<<" at line " <<comptag->Row()<<" has no name.";
                            logger->addWarning(war);
                        }                            
                    }

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
                } // end of computer tag loop            
                module.addResource(computer);
            } //end of if computer tag
        }// end of dependecnies tag 

    return &module;
}


