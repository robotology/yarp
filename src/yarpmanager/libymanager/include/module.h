// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __MODULE_H_
#define __MODULE_H_


#include "ymm-types.h" 
#include "node.h"
#include "data.h"
#include "utility.h"
#include "resource.h"

using namespace std;

//namespace ymm {


class Author {
public:
    Author(void) {}
    Author(const char* name, const char* email) {
        if(name) { strName = name;}
        if(email) { strEmail = email; }
    }
    ~Author() {}

    void setName(const char* name) { if(name) strName = name; }
    void setEmail(const char* email) { if(email) strEmail = email; }
    const char* getName(void) { return strName.c_str(); }
    const char* getEmail(void) { return strEmail.c_str(); }

    bool operator==(const Author& alt) {
        return (strName == alt.strName);
    }

private:
    string strName;
    string strEmail;
};

/**
 * Class Argument
 */
class Argument{

public:
    Argument(const char* szParam, bool required=false,
             const char* szDesc=NULL, bool _switch=false) {
        if(szParam) strParam = szParam;
        if(szDesc) strDesc = szDesc;
        bRequired = required;
        bSwitch = _switch;
        if(bSwitch) strDefault = "off";
        strValue = strDefault;
    }
    virtual ~Argument(){}

    void setParam(const char* szParam) { if(szParam) strParam = szParam; }
    void setDefault(const char* szDefault) {if(szDefault) strDefault = szDefault; }
    void setValue(const char* szValue) {if(szValue) strValue = szValue; }
    const char* getDefault(void) { return strDefault.c_str(); }
    const char* getParam(void) { return strParam.c_str(); }
    const char* getValue(void) { return strValue.c_str(); }
    const char* getDescription(void) { return strDesc.c_str(); }
    bool isSwitch(void) { return bSwitch; }
    bool isRequired(void) {return bRequired; }
    bool operator==(const Argument& alt) {
        return (strParam == alt.strParam);
    }

protected:

private:
    string strParam;
    string strDesc;
    string strDefault;
    string strValue;
    bool bRequired;
    bool bSwitch;
};


typedef vector<InputData> InputContainer;
typedef vector<OutputData> OutputContainer;
typedef vector<InputData>::iterator InputIterator;
typedef vector<OutputData>::iterator OutputIterator;
typedef vector<Author> AuthorContainer;
typedef vector<Author>::iterator AuthorIterator;
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

    bool setParam(const char* szParam);
    void setXmlFile(const char* szFilename) { if(szFilename) strXmlFile = szFilename;}
    void setWorkDir(const char* szWDir) { if(szWDir) strWorkDir = szWDir; }
    void setStdio(const char* szStdio) { if(szStdio) strStdio = szStdio; }
    void setBroker(const char* szBroker) { if(szBroker) strBroker = szBroker; }
    void setPrefix(const char* szPrefix) { if(szPrefix) strPrefix = szPrefix; }
    void setBasePrefix(const char* szPrefix) { if(szPrefix) strBasePrefix = szPrefix; }
    void setNeedDeployer(bool need) { bNeedDeployer = need; }

    int getRank(void) { return iRank; }
    const char* getName(void) { return strName.c_str(); }
    const char* getVersion(void) { return strVersion.c_str(); }
    const char* getDescription(void) { return strDescription.c_str(); }
    const char* getHost(void) { return strHost.c_str(); }
    bool getForced(void) { return bForced; }
    bool getNeedDeployer(void) { return bNeedDeployer; }

    const char* getParam(void) { return strParam.c_str(); }
    const char* getXmlFile(void) { return strXmlFile.c_str(); }
    const char* getWorkDir(void) { return strWorkDir.c_str(); }
    const char* getStdio(void) { return strStdio.c_str(); }
    const char* getBroker(void) { return strBroker.c_str(); }
    const char* getPrefix(void) { return strPrefix.c_str(); }
    const char* getBasePrefix(void) { return strBasePrefix.c_str(); }

    int argumentCount(void) { return arguments.size(); }
    Argument& getArgumentAt(int index){ return arguments[index]; }
    int outputCount(void) { return outputs.size(); }
    OutputData& getOutputAt(int index) { return outputs[index]; }
    int inputCount(void) { return inputs.size(); }
    InputData& getInputAt(int index) { return inputs[index]; }
    int resourceCount(void) const { return resources.size(); }
    GenericResource& getResourceAt(int index) const { return *(resources[index]); }

    void addAuthor(Author& author) { authors.push_back(author); }
    int authorCount(void) { return authors.size(); }
    Author& getAuthorAt(int index) { return authors[index]; }
    bool removeAuthor(Author& author);

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

    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner(void) { return modOwner; }

    void clear(void);
    // modelBased is used to keep the graphic and geometric
    // information which is directly loaded from application
    // description file.
    GraphicModel& getModelBase(void) { return modelBase;}
    void setModelBase(GraphicModel& mdl) { modelBase = mdl; };


    friend class ModuleInterface;

protected:

private:
    Node* modOwner;
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
    bool bNeedDeployer;
    string strPrefix;
    string strBasePrefix;

    GraphicModel modelBase;

    ArgumentIterator findArgument(Argument& argument);
    InputIterator findInput(InputData& input);
    OutputIterator findOutput(OutputData& output);
    ResourcePIterator findResource(GenericResource& output);
    void swap(const Module &mod);
    bool getParamValue(const char* key, bool bSwitch, std::string &param);
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
            cout<<"           "<<m->getAuthorAt(i).getName()<<" ("<<m->getAuthorAt(i).getEmail()<<")"<<endl;\
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
