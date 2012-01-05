// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __MODULE__
#define __MODULE__


#include "ymm-types.h" 
#include "node.h"
#include "data.h"
#include "utility.h"
#include "resource.h"

using namespace std; 

//namespace ymm {



/**
 * Class Argument  
 */
class Argument{

public: 
    Argument(const char* szParam, bool required=false, 
             const char* szDesc=NULL) {
        strParam = szParam;
        if(szDesc) strDesc = szDesc; 
        bRequired = required;
    }   
    virtual ~Argument(){}   
    const char* getParam(void) { return strParam.c_str(); }
    const char* getDescription(void) { return strDesc.c_str(); }    
    bool operator==(const Argument& alt) {      
        return (strParam == alt.strParam); 
    }
    
protected:

private:
    string strParam;
    string strDesc;
    bool bRequired;
};


typedef vector<InputData> InputContainer;
typedef vector<OutputData> OutputContainer;
typedef vector<InputData>::iterator InputIterator;
typedef vector<OutputData>::iterator OutputIterator;
typedef vector<string> AuthorContainer;
typedef vector<Argument> ArgumentContainer;
typedef vector<Argument>::iterator ArgumentIterator;



/**
 * Class Module  
 */
class Module : public Node{

public: 
    Module(void);
    Module(const char* szName);
    Module(const Module &mod);
    virtual ~Module();
    virtual Node* clone(void);        
    Module& operator=(const Module& rhs);

    void setName(const char* szName) { if(szName) strName = szName; }
    void setRank(int rank) {iRank = rank;}
    void setVersion(const char* szVersion) { if(szVersion) strVersion = szVersion; }
    void setDescription(const char* szDesc) { if(szDesc) strDescription = szDesc; }
    void setHost(const char* szHost) { if(szHost) strHost = szHost; }
    void setForced(bool flag) { bForced = flag; }
    
    void setParam(const char* szParam) { if(szParam) strParam = szParam; }
    void setXmlFile(const char* szFilename) { if(szFilename) strXmlFile = szFilename;}
    void setWorkDir(const char* szWDir) { if(szWDir) strWorkDir = szWDir; }
    void setStdio(const char* szStdio) { if(szStdio) strStdio = szStdio; }
    void setBroker(const char* szBroker) { if(szBroker) strBroker = szBroker; }
    void setPrefix(const char* szPrefix) { if(szPrefix) strPrefix = szPrefix; }

    int getRank(void) { return iRank; }
    const char* getName(void) { return strName.c_str(); }
    const char* getVersion(void) { return strVersion.c_str(); }
    const char* getDescription(void) { return strDescription.c_str(); }
    const char* getHost(void) { return strHost.c_str(); }   
    bool getForced(void) { return bForced; }
  
    const char* getParam(void) { return strParam.c_str(); } 
    const char* getXmlFile(void) { return strXmlFile.c_str(); }
    const char* getWorkDir(void) { return strWorkDir.c_str(); } 
    const char* getStdio(void) { return strStdio.c_str(); }
    const char* getBroker(void) { return strBroker.c_str(); }   
    const char* getPrefix(void) { return strPrefix.c_str(); }   

    int argumentCount(void) { return arguments.size(); }
    Argument& getArgumentAt(int index){ return arguments[index]; }
    int outputCount(void) { return outputs.size(); }
    OutputData& getOutputAt(int index) { return outputs[index]; }
    int inputCount(void) { return inputs.size(); }
    InputData& getInputAt(int index) { return inputs[index]; } 
    int resourceCount(void) const { return resources.size(); }
    GenericResource& getResourceAt(int index) const { return *(resources[index]); }

    void addAuthor(const char* szAuthor) { authors.push_back(szAuthor); }
    int authorCount(void) { return authors.size(); }
    const char* getAuthorAt(int index) { return authors[index].c_str(); }
    
    bool addArgument(Argument& arg);
    bool removeArgument(Argument& argument);
    bool addOutput(OutputData& output);
    bool removeOutput(OutputData& output);
    bool addInput(InputData& input);
    bool removeInput(InputData& input);
    bool addResource(GenericResource& res);
    bool removeResource(GenericResource& res);
   
    void clearInputs(void) { inputs.clear(); }
    void clearOutputs(void) { outputs.clear(); }
    void clearResources(void) { resources.clear(); }

    void clear(void);

protected:

private:
    string strName; 
    string strVersion;
    string strDescription;
    string strHost;
    bool bForced;
    int iRank;
    ArgumentContainer arguments;
    AuthorContainer authors;
    OutputContainer outputs;
    InputContainer inputs;
    ResourcePContainer resources;

    string strParam;
    string strXmlFile;
    string strWorkDir;
    string strStdio;
    string strBroker;
    string strPrefix;

    ArgumentIterator findArgument(Argument& argument);
    InputIterator findInput(InputData& input);
    OutputIterator findOutput(OutputData& output); 
    ResourcePIterator findResource(GenericResource& output); 
    void swap(const Module &mod);

};

typedef vector<Module*> ModulePContainer;
typedef vector<Module*>::iterator ModulePIterator;


#define PRINT_MODULE(m)\
        {\
        cout<<"Name     : "<<m->getName()<<endl;\
        cout<<"Desc     : "<<m->getDescription()<<endl;\
        cout<<"Version  : "<<m->getVersion()<<endl;\
        cout<<"Rank     : "<<m->getRank()<<endl;\
        cout<<"Param    : "<<endl;\
        for(int i=0; i<m->argumentCount(); i++)\
        {\
            cout<<"           "<<m->getArgumentAt(i).getParam()<<" (";\
            cout<<m->getArgumentAt(i).getDescription()<<")"<<endl;\
        }\
        cout<<"Authors  : "<<endl;\
        for(int i=0; i<m->authorCount(); i++)\
            cout<<"           "<<m->getAuthorAt(i)<<endl;\
        /*cout<<"Platforms: "<<endl;\
        for(int i=0; i<m->platformCount(); i++)\
            cout<<"           "<<m->getPlatformAt(i).getOS()\
            <<", "<<m->getPlatformAt(i).getDistribution()<<endl;*/\
        cout<<"Inputs   : "<<endl;\
        for(int i=0; i<m->inputCount(); i++)\
        {\
            cout<<"           "<<m->getInputAt(i).getName()\
            <<", "<<m->getInputAt(i).getPort();\
            if(m->getInputAt(i).isRequired())\
                cout<<", [REQUIRED]";\
            if(m->getInputAt(i).withPriority())\
                cout<<", [PRIORITY]";\
            cout<<endl;\
        }\
        cout<<"Outputs  : "<<endl;\
        for(int i=0; i<m->outputCount(); i++)\
            cout<<"           "<<m->getOutputAt(i).getName()\
            <<", "<<m->getOutputAt(i).getPort()<<endl;\
        cout<<endl;\
        }
//}

#endif //__MODULE__
