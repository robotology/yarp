/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "module.h"

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
    strPrefix = mod.strPrefix; 
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
    iRank = 1;
    strName.clear();
    arguments.clear();
    strVersion.clear();
    strDescription.clear();
    strHost.clear();
    bForced = false;
    authors.clear();
    outputs.clear();
    inputs.clear();
    strXmlFile.clear();
    strParam.clear();
    strWorkDir.clear();
    strStdio.clear();
    strBroker.clear();
    strPrefix.clear();

    for(ResourcePIterator itr = resources.begin(); 
        itr != resources.end(); itr++)
    {
        delete (*itr);
        *itr = NULL;
    }
    resources.clear();
}
    

