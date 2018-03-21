/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_MODULE_H
#define YARP_MANAGER_MODULE_H


#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/data.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/resource.h>
#include <yarp/conf/api.h>

namespace yarp {
namespace manager {


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
    std::string strName;
    std::string strEmail;
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
    std::string strParam;
    std::string strDesc;
    std::string strDefault;
    std::string strValue;
    bool bRequired;
    bool bSwitch;
};


typedef std::vector<InputData> InputContainer;
typedef std::vector<OutputData> OutputContainer;
typedef std::vector<InputData>::iterator InputIterator;
typedef std::vector<OutputData>::iterator OutputIterator;
typedef std::vector<Author> AuthorContainer;
typedef std::vector<Author>::iterator AuthorIterator;
typedef std::vector<Argument> ArgumentContainer;
typedef std::vector<Argument>::iterator ArgumentIterator;



/**
 * Class Module
 */
class Module : public Node{

public:
    Module(void);
    Module(const char* szName);
    Module(const Module &mod);
    virtual ~Module();
    virtual Node* clone(void) override;
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
    void setEnvironment(const char* szEnv) {if(szEnv) strEnvironment = szEnv; }
    void setBasePrefix(const char* szPrefix) { if(szPrefix) strBasePrefix = szPrefix; }
    void setNeedDeployer(bool need) { bNeedDeployer = need; }
    void setDisplay(const char* szDisplay) {if(szDisplay) strDisplay = szDisplay;}

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
    const char* getEnvironment(void) { return strEnvironment.c_str(); }
    const char* getBasePrefix(void) { return strBasePrefix.c_str(); }
    const char* getDisplay() { return strDisplay.c_str(); }


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

    void setPostExecWait(double t) { waitStart = t; }
    double getPostExecWait() { return waitStart; }
    void setPostStopWait(double t) { waitStop = t; }
    double getPostStopWait() { return waitStop; }


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
    std::string strName;
    std::string strVersion;
    std::string strDescription;
    std::string strHost;
    bool bForced;
    int iRank;
    ArgumentContainer arguments;
    AuthorContainer authors;
    OutputContainer outputs;
    InputContainer inputs;
    ResourcePContainer resources;

    std::string strParam;
    std::string strXmlFile;
    std::string strWorkDir;
    std::string strStdio;
    std::string strBroker;
    bool bNeedDeployer;
    std::string strPrefix;
    std::string strEnvironment;
    std::string strBasePrefix;
    double waitStart;
    double waitStop;
    std::string strDisplay;
    GraphicModel modelBase;

    ArgumentIterator findArgument(Argument& argument);
    InputIterator findInput(InputData& input);
    OutputIterator findOutput(OutputData& output);
    ResourcePIterator findResource(GenericResource& output);
    void swap(const Module &mod);
    bool getParamValue(const char* key, bool bSwitch, std::string &param);
};

typedef std::vector<Module*> ModulePContainer;
typedef std::vector<Module*>::iterator ModulePIterator;


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

} // namespace yarp
} // namespace manager


#endif // YARP_MANAGER_MODULE
