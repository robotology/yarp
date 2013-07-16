/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "module.h"
#include <stdio.h>
#include <string>
using namespace std;
#include <string.h>

Module::Module(void) : Node(MODULE) { clear();}


Module::Module(const char* szName) : Node(MODULE)
{
    clear();
    setName(szName);
}


Module::Module(const Module &mod) : Node(mod)
{
    Module::swap(mod);
}


Module& Module::operator=(const Module& rhs)
{
    Node::operator=(rhs);
    Module::swap(rhs);
    return *this;
}


void Module::swap(const Module &mod)
{
    clear();
    iRank = mod.iRank;
    strName = mod.strName;
    arguments = mod.arguments;
    strVersion = mod.strVersion;
    strDescription = mod.strDescription;
    strHost = mod.strHost;
    bForced = mod.bForced;
    authors = mod.authors;
    outputs = mod.outputs;
    inputs = mod.inputs;
    strXmlFile = mod.strXmlFile;
    strParam = mod.strParam;
    strWorkDir = mod.strWorkDir;
    strStdio = mod.strStdio;
    strBroker = mod.strBroker;
    bNeedDeployer = mod.bNeedDeployer;
    strPrefix = mod.strPrefix;
    strBasePrefix = mod.strBasePrefix;
    modOwner = mod.modOwner;
    // deep copy
    for(int i=0; i<mod.resourceCount(); i++)
        addResource(mod.getResourceAt(i));
}


Module::~Module() { }


Node* Module::clone(void)
{
    Module* mod = new Module(*this);
    return mod;
}


bool Module::addArgument(Argument &argument)
{
    arguments.push_back(argument);
    return true;
}


bool Module::removeArgument(Argument& argument)
{
    ArgumentIterator itr = findArgument(argument);
    if(itr == arguments.end())
        return true;
    arguments.erase(itr);
    return true;
}


bool Module::addOutput(OutputData& output)
{
    //__CHECK_NULLPTR(output);
    outputs.push_back(output);
    return true;
}


bool Module::removeOutput(OutputData& output)
{
    //__CHECK_NULLPTR(output);

    OutputIterator itr = findOutput(output);
    if(itr == outputs.end())
        return true;
    outputs.erase(itr);
    return true;
}


bool Module::addInput(InputData& input)
{
    //__CHECK_NULLPTR(input);
    inputs.push_back(input);
    return true;
}


bool Module::removeInput(InputData& input)
{
    //__CHECK_NULLPTR(input);

    InputIterator itr = findInput(input);
    if(itr == inputs.end())
        return true;
    inputs.erase(itr);
    return true;
}


bool Module::addResource(GenericResource& res)
{
    GenericResource* newres = (GenericResource*) res.clone();
    newres->setOwner(this);
    resources.push_back(newres);
    return true;
}


bool Module::removeResource(GenericResource& res)
{
    ResourcePIterator itr = findResource(res);
    if(itr == resources.end())
        return true;
    resources.erase(itr);
    delete (*itr);
    return true;
}

bool Module::removeAuthor(Author& author)
{
    AuthorIterator itr;
    for(itr=authors.begin(); itr<authors.end(); itr++)
        if((*itr) == author)
        {
            authors.erase(itr);
            return true;
        }    
    return true;        
}


ArgumentIterator Module::findArgument(Argument& argument)
{
    ArgumentIterator itr;
    for(itr=arguments.begin(); itr<arguments.end(); itr++)
        if ((*itr) == argument)
            return itr;
    return arguments.end();
}


InputIterator Module::findInput(InputData& input)
{
    InputIterator itr;
    for(itr=inputs.begin(); itr<inputs.end(); itr++)
        if ((*itr) == input)
            return itr;
    return inputs.end();
}


OutputIterator Module::findOutput(OutputData& output)
{
    OutputIterator itr;
    for(itr=outputs.begin(); itr<outputs.end(); itr++)
        if ((*itr) == output)
            return itr;
    return outputs.end();
}

ResourcePIterator Module::findResource(GenericResource& res)
{
    ResourcePIterator itr;
    for(itr=resources.begin(); itr<resources.end(); itr++)
        if (*(*itr) == res)
            return itr;
    return resources.end();
}

void Module::clear(void)
{
    modOwner = NULL;
    iRank = 1;
    strName.clear();
    arguments.clear();
    strVersion.clear();
    strDescription.clear();
    strHost.clear();
    bForced = false;
    bNeedDeployer = false;
    authors.clear();
    outputs.clear();
    inputs.clear();
    strXmlFile.clear();
    strParam.clear();
    strWorkDir.clear();
    strStdio.clear();
    strBroker.clear();
    strPrefix.clear();
    strBasePrefix.clear();
    for(ResourcePIterator itr = resources.begin();
        itr != resources.end(); itr++)
    {
        delete (*itr);
        *itr = NULL;
    }
    resources.clear();
}

bool Module::setParam(const char* szParam)
{ 
    __CHECK_NULLPTR(szParam);

    bool bokay = true;
    strParam = szParam;
    ErrorLogger* logger  = ErrorLogger::Instance();
    ArgumentIterator itr;
    for(itr=arguments.begin(); itr<arguments.end(); itr++)
    {
        std::string strVal;
        bool ret = getParamValue((*itr).getParam(), (*itr).isSwitch(), strVal);
        if(!ret)
        {
            OSTRINGSTREAM msg;
            msg<<"Error in parsing parameters of "<<getName() \
                <<". ( '"<< (*itr).getParam()<<"' is not correct. )";
            logger->addWarning(msg);
            bokay = false;
        }
        else
        {
            if((*itr).isSwitch())
                (*itr).setValue(strVal.c_str());
            else
            {
                if(strVal != "off")
                    (*itr).setValue(strVal.c_str());
            }
        }
    }    
    return bokay;
}

bool Module::getParamValue(const char* key, bool bSwitch, std::string &param)
{
    if(!key)
        return false;

    //printf("\n\nparsing '%s' for %s (switch:%d)\n", strParam.c_str(), key, bSwitch);
    string strKey = string("--") + string(key);
    size_t pos = strParam.find(strKey.c_str());
    if(pos == string::npos)
    {
        param = "off";
        return true;
    }

    if(bSwitch)
    {
       param = "on";
       return true;
    }
    //printf("%s %d \n", __FILE__, __LINE__);
    
    pos += strKey.size(); 
    if((pos >= strParam.length()) || (strParam.at(pos) != ' '))
        return false;

    // skip all spaces
    while(strParam.at(pos++) == ' ')
    {
        if(pos >= strParam.length())
            return false;     
    }
    pos--;

    size_t pos2 = pos;
    while((pos2 < strParam.length()) && (strParam.at(pos2) != ' '))
        pos2++;
    param = strParam.substr(pos, pos2-pos).c_str();
    return true;
}

