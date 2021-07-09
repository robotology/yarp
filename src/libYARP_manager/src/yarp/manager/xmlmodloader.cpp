/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/xmlmodloader.h>
#include <yarp/conf/filesystem.h>
#include <yarp/manager/utility.h>
#include <dirent.h>
#include <yarp/manager/physicresource.h>
#include <yarp/manager/logicresource.h>
#include <yarp/manager/primresource.h>
#include <yarp/manager/impl/textparser.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>

#include <tinyxml.h>


using namespace std;
using namespace yarp::manager;


XmlModLoader::XmlModLoader(const char* szPath, const char* szName)
{
    parser = new(TextParser);
    module.clear();

    if(strlen(szPath))
    {
        const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};
        strPath = szPath;
        if((strPath.rfind(directorySeparator)==string::npos) ||
            (strPath.rfind(directorySeparator)!=strPath.size()-1))
            strPath = strPath + string(directorySeparator);
    }

    if(szName)
        strName = szName;
}

/**
 * load only one module indicated by its xml file name
 */
XmlModLoader::XmlModLoader(const char* szFileName)
{
    parser = new(TextParser);
    module.clear();
    if(szFileName)
        strFileName = szFileName;
}



XmlModLoader::~XmlModLoader()
{
    if(parser)
    {
        delete parser;
    }
}


bool XmlModLoader::init()
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
    if ((dir = opendir(strPath.c_str())) == nullptr)
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


void XmlModLoader::reset()
{
    fini();
    init();
}


void XmlModLoader::fini()
{
    fileNames.clear();
    module.clear();
}


Module* XmlModLoader::getNextModule()
{
    if(strName.empty())
    {
        Module* mod = nullptr;
        while(!mod)
        {
            if(fileNames.empty())
                return nullptr;

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
    return nullptr;
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
        return nullptr;
    }

    /* retrieving root module */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return nullptr;
    }

    if(!compareString(root->Value(), "module"))
    {
        /*
        OSTRINGSTREAM msg;
        msg<<szFile<<" is not a module descriptor file.";
        logger->addWarning(msg);
        */
        return nullptr;
    }

    /* retrieving name */
    auto* name = (TiXmlElement*) root->FirstChild("name");
    if(!name || !name->GetText())
    {
        OSTRINGSTREAM err;
        err<<"Module from "<<szFile<<" has no name.";
        logger->addError(err);
        //return NULL;
    }

    for(TiXmlElement* var = root->FirstChildElement("var"); var; var = var->NextSiblingElement())
    {
        if(var->Attribute("name") && var->GetText())
        {
            parser->addVariable(var->Attribute("name"), var->GetText());
        }
    }

    module.setXmlFile(szFile);

    if(name)
        module.setName(parser->parseText(name->GetText()).c_str());

    /* retrieving description */
    TiXmlElement* desc;
    if((desc = (TiXmlElement*) root->FirstChild("description")))
        module.setDescription(parser->parseText(desc->GetText()).c_str());

    /* retrieving version */
    TiXmlElement* ver;
    if((ver = (TiXmlElement*) root->FirstChild("version")))
        module.setVersion(parser->parseText(ver->GetText()).c_str());


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
                    Argument arg(parser->parseText(param->GetText()).c_str(),
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
                    Argument arg(parser->parseText(param->GetText()).c_str(),
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
        module.setRank(atoi(parser->parseText(rank->GetText()).c_str()));


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
                    author.setName(parser->parseText(ath->GetText()).c_str());
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

                if(compareString(data->Attribute("port_type"), "stream") || !data->Attribute("port_type"))
                    output.setPortType(STREAM_PORT);
                else if(compareString(data->Attribute("port_type"), "event"))
                    output.setPortType(EVENT_PORT);
                else if(compareString(data->Attribute("port_type"), "service"))
                    output.setPortType(SERVICE_PORT);
                else
                {
                    OSTRINGSTREAM war;
                    war<<"Unknown port type \'"<<data->Attribute("port_type")<<"\' from "<<szFile<<" at line "\
                       <<data->Row()<<". Available types : stream, event, service";
                    logger->addWarning(war);
                }


                TiXmlElement* element;
                if(output.getPortType() != SERVICE_PORT )
                {
                    if((element = (TiXmlElement*) data->FirstChild("type")))
                        output.setName(parser->parseText(element->GetText()).c_str());
                    else
                    {
                        OSTRINGSTREAM war;
                        war<<"Output data from "<<szFile<<" at line "\
                           <<data->Row()<<" has no type.";
                        logger->addWarning(war);
                    }
                }
                else
                    output.setName("*");

                if((element = (TiXmlElement*) data->FirstChild("port")))
                {
                    output.setPort(parser->parseText(element->GetText()).c_str());
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
                    output.setDescription(parser->parseText(element->GetText()).c_str());

                module.addOutput(output);
            } // end of output data

            /* input data */
            if(compareString(data->Value(), "input"))
            {
                InputData input;

                if(compareString(data->Attribute("port_type"), "stream") || !data->Attribute("port_type"))
                    input.setPortType(STREAM_PORT);
                else if(compareString(data->Attribute("port_type"), "event"))
                    input.setPortType(EVENT_PORT);
                else if(compareString(data->Attribute("port_type"), "service"))
                    input.setPortType(SERVICE_PORT);
                else
                {
                    OSTRINGSTREAM war;
                    war<<"Unknown port type \'"<<data->Attribute("port_type")<<"\' from "<<szFile<<" at line "\
                       <<data->Row()<<". Available types : stream, event, service";
                    logger->addWarning(war);
                }

                TiXmlElement* element;
                if(input.getPortType() != SERVICE_PORT )
                {

                    if((element = (TiXmlElement*) data->FirstChild("type")))
                        input.setName(parser->parseText(element->GetText()).c_str());
                    else
                    {
                        OSTRINGSTREAM war;
                        war<<"Input data from "<<szFile<<" at line "\
                           <<data->Row()<<" has no type.";
                        logger->addWarning(war);
                    }
                }
                else
                    input.setName("rpc");

                if((element = (TiXmlElement*) data->FirstChild("port")))
                {
                    input.setPort(parser->parseText(element->GetText()).c_str());
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
                    input.setDescription(parser->parseText(element->GetText()).c_str());

                if((element = (TiXmlElement*) data->FirstChild("required")))
                    if(compareString(parser->parseText(element->GetText()).c_str(), "yes"))
                        input.setRequired(true);

                if((element = (TiXmlElement*) data->FirstChild("priority")))
                    if(compareString(parser->parseText(element->GetText()).c_str(), "yes"))
                        input.setPriority(true);

                module.addInput(input);
            } // end of input data

        }

    if(root->FirstChild("services")) {
        for(TiXmlElement* services = root->FirstChild("services")->FirstChildElement();
            services; services = services->NextSiblingElement())
        {
            /* server */
            if(compareString(services->Value(), "server"))
            {
                InputData input;
                input.setPortType(SERVICE_PORT);
                TiXmlElement* element;
                if((element = (TiXmlElement*) services->FirstChild("port"))) {
                    input.setPort(parser->parseText(element->GetText()).c_str());
                    input.setCarrier("tcp");
                }
                if((element = (TiXmlElement*) services->FirstChild("description")))
                    input.setDescription(parser->parseText(element->GetText()).c_str());
                if((element = (TiXmlElement*) services->FirstChild("type")))
                    input.setName(parser->parseText(element->GetText()).c_str());
                else
                    input.setName("rpc");
                module.addInput(input);
            }
            /* client */
            if(compareString(services->Value(), "client"))
            {
                OutputData output;
                output.setPortType(SERVICE_PORT);
                TiXmlElement* element;
                if((element = (TiXmlElement*) services->FirstChild("port"))) {
                    output.setPort(parser->parseText(element->GetText()).c_str());
                    output.setCarrier("tcp");
                }
                if((element = (TiXmlElement*) services->FirstChild("description")))
                    output.setDescription(parser->parseText(element->GetText()).c_str());
                if((element = (TiXmlElement*) services->FirstChild("type")))
                    output.setName(parser->parseText(element->GetText()).c_str());
                else
                    output.setName("rpc");
                module.addOutput(output);
            }
        }

    }

    /* retrieving broker*/
    TiXmlElement* element;
    if((element = (TiXmlElement*) root->FirstChild("deployer")))
    {
        module.setBroker(parser->parseText(element->GetText()).c_str());
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
                        computer.setName(parser->parseText(comptag->GetText()).c_str());

                    /* retrieving description */
                     if(compareString(comptag->Value(), "description"))
                        computer.setDescription(parser->parseText(comptag->GetText()).c_str());

                    // platform
                    if(compareString(comptag->Value(), "platform"))
                    {
                        Platform os;
                        TiXmlElement* element;
                        if((element = (TiXmlElement*) comptag->FirstChild("name")))
                            os.setName(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("distribution")))
                            os.setDistribution(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("release")))
                            os.setRelease(parser->parseText(element->GetText()).c_str());
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
                            mem.setTotalSpace((Capacity)atol(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("free_space")))
                            mem.setFreeSpace((Capacity)atol(parser->parseText(element->GetText()).c_str()));
                        computer.setMemory(mem);
                    } // end of memory tag

                    // storage
                    if(compareString(comptag->Value(), "storage"))
                    {
                        Storage stg;
                        TiXmlElement* element;
                        if((element = (TiXmlElement*) comptag->FirstChild("total_space")))
                            stg.setTotalSpace((Capacity)atol(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("free_space")))
                            stg.setFreeSpace((Capacity)atol(parser->parseText(element->GetText()).c_str()));
                        computer.setStorage(stg);
                    } // end of storage tag

                    // processor
                    if(compareString(comptag->Value(), "processor"))
                    {
                        Processor proc;
                        TiXmlElement* element;
                        if((element = (TiXmlElement*) comptag->FirstChild("architecture")))
                            proc.setArchitecture(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("model")))
                            proc.setModel(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("cores")))
                            proc.setCores((size_t)atoi(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("siblings")))
                            proc.setSiblings((size_t)atoi(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("frequency")))
                            proc.setFrequency(atof(parser->parseText(element->GetText()).c_str()));
                        computer.setProcessor(proc);
                    } // end of processor tag

                    // network
                    if(compareString(comptag->Value(), "network"))
                    {
                        Network net;
                        TiXmlElement* element;
                        if((element = (TiXmlElement*) comptag->FirstChild("ip4")))
                            net.setIP4(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("ip6")))
                            net.setIP6(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("mac")))
                            net.setMAC(parser->parseText(element->GetText()).c_str());
                        module.addResource(net);
                        computer.setNetwork(net);
                    } // end of network tag

                    // yarp_port
                    if(compareString(comptag->Value(), "yarp_port"))
                    {
                        ResYarpPort yport;
                        auto* element = (TiXmlElement*) comptag->FirstChild("name");
                        if(element && element->GetText())
                        {
                            yport.setName(parser->parseText(element->GetText()).c_str());
                            yport.setPort(parser->parseText(element->GetText()).c_str());
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
                            gpu.setName(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("capability")))
                            gpu.setCompCompatibility(parser->parseText(element->GetText()).c_str());
                        if((element = (TiXmlElement*) comptag->FirstChild("cores")))
                            gpu.setCores((size_t)atoi(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("frequency")))
                            gpu.setFrequency(atof(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("register_block")))
                            gpu.setResgisterPerBlock((size_t)atoi(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("thread_block")))
                            gpu.setThreadPerBlock((size_t)atoi(parser->parseText(element->GetText()).c_str()));
                        if((element = (TiXmlElement*) comptag->FirstChild("overlap")))
                        {
                            if(compareString(parser->parseText(element->GetText()).c_str(), "yes"))
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
                                gpu.setGlobalMemory((Capacity)atol(parser->parseText(element->GetText()).c_str()));
                        } // end of global memory tag

                        // shared memory
                        if(comptag->FirstChild("shared_memory"))
                        {
                            TiXmlElement* element;
                            element = (TiXmlElement*) comptag->FirstChild("shared_memory");
                            if((element = (TiXmlElement*) element->FirstChild("total_space")))
                                gpu.setSharedMemory((Capacity)atol(parser->parseText(element->GetText()).c_str()));
                        } // end of shared memory tag

                        // constant memory
                        if(comptag->FirstChild("constant_memory"))
                        {
                            TiXmlElement* element;
                            element = (TiXmlElement*) comptag->FirstChild("constant_memory");
                            if((element = (TiXmlElement*) element->FirstChild("total_space")))
                                gpu.setConstantMemory((Capacity)atol(parser->parseText(element->GetText()).c_str()));
                        } // end of shared memory tag
                        computer.addPeripheral(gpu);
                    } // end of gpu tag
                } // end of computer tag loop
                module.addResource(computer);
            } //end of if computer tag
        }// end of dependecnies tag

    return &module;
}
