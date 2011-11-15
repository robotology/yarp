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
    iRank = mod.iRank;
    strName = mod.strName;
    arguments = mod.arguments;
    strVersion = mod.strVersion;
    strDescription = mod.strDescription;
    strHost = mod.strHost;
    authors = mod.authors;
    platforms = mod.platforms; 
    outputs = mod.outputs;
    inputs = mod.inputs;
    strXmlFile = mod.strXmlFile;
    strParam = mod.strParam;
    strWorkDir = mod.strWorkDir;
    strStdio = mod.strStdio;
    strBroker = mod.strBroker;
    strPrefix = mod.strPrefix; 
}


Module::~Module() { }


Node* Module::clone(void)
{
    Module* mod = new Module(*this);
    return mod; 
}


bool Module::addPlatform(Platform &platform)
{
    //__CHECK_NULLPTR(platform);
    platforms.push_back(platform);  
    return true;
}


bool Module::removePlatform(Platform& platform)
{
    //__CHECK_NULLPTR(platform);
    
    PlatformIterator itr = findPlatform(platform);
    if(itr == platforms.end()) 
        return true;
    platforms.erase(itr);
    return true;
}


bool Module::addArgument(Argument &argument)
{
    //__CHECK_NULLPTR(platform);
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


PlatformIterator Module::findPlatform(Platform& platform) 
{
    PlatformIterator itr;
    for(itr=platforms.begin(); itr<platforms.end(); itr++) 
        if ((*itr) == platform)
            return itr;
    return platforms.end();
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

