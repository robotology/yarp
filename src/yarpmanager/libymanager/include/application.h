// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __APPLICATION__
#define __APPLICATION__

#include <map>

#include "ymm-types.h" 
#include "node.h"
#include "module.h"
#include "utility.h"
#include "logicresource.h"
#include "primresource.h"
#include "arbitrator.h"

using namespace std; 

//namespace ymm {


/**
 * Class Portmap  
 */
class Portmap{

public: 
    Portmap(const char* szOld, const char* szNew) {
        if(szOld) strOld = szOld;
        if(szNew) strNew = szNew;
    }       
    virtual ~Portmap(){}
    const char* oldPort(void) { return strOld.c_str(); }
    const char* newPort(void) { return strNew.c_str(); }
    bool operator==(const Portmap& alt) {       
        return ((strOld == alt.strOld) && (strNew == alt.strNew)); 
    }
    
protected:

private:
    string strOld;
    string strNew;
};

typedef vector<Portmap> PortmapContainer;
typedef vector<Portmap>::iterator PortmapIterator;



/**
 * Class Connection  
 */
class Connection{

public:
    Connection(void) { Connection(NULL, NULL); } 
    Connection(const char* szFrom, const char* szTo, const char* szCr=NULL) {
        if(szFrom) strFrom = szFrom;
        if(szTo) strTo = szTo;
        if(szCr) strCarrier = szCr;        
        bWithPriority = false;
        appOwner = NULL;
        bExternalFrom = false;
        bExternalTo = false;
        bPersist = false;
        input = NULL;
        output = NULL;
        model = NULL;
    }
    virtual ~Connection(){}
    const char* from(void) { return strFrom.c_str();}
    const char* to(void) { return strTo.c_str();}
    void setFrom(const char* szFrom) { if(szFrom) strFrom = szFrom; }
    void setTo(const char* szTo) { if(szTo) strTo = szTo; }
    void setCarrier(const char* szCr) { if(szCr) strCarrier = szCr; }
    void setFromExternal(bool ext) { bExternalFrom = ext;}
    void setToExternal(bool ext) { bExternalTo = ext; }
    void setPersistent(bool per) { bPersist = per; }
    bool isExternalFrom(void) { return bExternalFrom; }
    bool isExternalTo(void) { return bExternalTo; }
    bool isPersistent(void) { return bPersist; }
    const char* carrier(void) { return strCarrier.c_str(); }
   
    void setId(const char* id) { if(id) strId = id; }
    const char* getId(void) { return strId.c_str(); }

    void setOwner(Node* owner){ appOwner = owner; }
    Node* owner(void) { return appOwner; }

    void setCorInputData(InputData* in) { input = in;}
    InputData* getCorInputData(void) { return input;}
    void setCorOutputData(OutputData* out) { output = out;}
    OutputData* getCorOutputData(void) { return output;}

    void setPriority(bool withprio) { bWithPriority = withprio; }
    bool withPriority(void) { return bWithPriority; }

    GraphicModel* getModel(void) { return model;}
    void setModel(GraphicModel* mdl) { model = mdl; }; 

    // modelBased is used to keep the graphic and geometric 
    // information which is directly loaded from application 
    // description file. 
    GraphicModel& getModelBase(void) { return modelBase;}
    void setModelBase(GraphicModel& mdl) { modelBase = mdl; }; 


    bool operator==(const Connection& alt) {        
        return ((strFrom == alt.strFrom) && (strTo == alt.strTo)); 
    }
    
protected:

private:
    string strFrom;
    string strTo;
    string strId;
    bool bExternalTo;
    bool bExternalFrom; 
    string strCarrier;
    bool bPersist; 
    Node* appOwner;
    bool bWithPriority; 
    InputData* input;
    OutputData* output;
    GraphicModel* model;
    GraphicModel modelBase;

};


typedef vector<Connection> CnnContainer;
typedef vector<Connection>::iterator CnnIterator;

typedef vector<ResYarpPort> ResourceContainer;
typedef vector<ResYarpPort>::iterator ResourceIterator;



/**
 * Class ModuleInterface  
 */
class ModuleInterface{

public: 
    ModuleInterface( const char* szName) {
        if(szName) strName = szName;
        iRank = -1;
    }

    ModuleInterface(Module* module);
   
    virtual ~ModuleInterface(){}
        
    void setHost(const char* szHost) { if(szHost) strHost = szHost; }
    void setParam(const char* szParam) { if(szParam) strParam = szParam; }
    void setRank(int irank) { iRank = irank;}
    void setWorkDir(const char* szWDir) { if(szWDir) strWorkDir = szWDir; }
    void setStdio(const char* szStdio) { if(szStdio) strStdio = szStdio; }
    void setBroker(const char* szBroker) { if(szBroker) strBroker = szBroker; }
    void setPrefix(const char* szPrefix) {if(szPrefix) strPrefix = szPrefix; }
    void setTag(const char* szTag) {if(szTag) strTag = szTag; }

    const char* getName(void) { return strName.c_str(); }
    const char* getHost(void) { return strHost.c_str(); }
    const char* getParam(void) { return strParam.c_str(); }
    int getRank(void) { return iRank; }
    const char* getWorkDir(void) { return strWorkDir.c_str(); } 
    const char* getStdio(void) { return strStdio.c_str(); }
    const char* getBroker(void) { return strBroker.c_str(); }   
    const char* getPrefix(void) { return strPrefix.c_str(); }   
    const char* getTag(void) { return strTag.c_str(); }   

    bool operator==(const ModuleInterface& modint) {        
        return (strName == modint.strName); 
    }
   
    void addResource(ResYarpPort &res) { resources.push_back(res); }
    ResourceContainer& getResources(void) { return resources; }
 
    int portmapCount(void) { return portmaps.size(); }
    Portmap& getPortmapAt(int index){ return portmaps[index]; }
    bool addPortmap(Portmap &portmap);
    bool removePortmap(Portmap& portmap);
    
    //void setModule(Module* mod) { module = mod; }
    //Module* getModule(void) { return module;}    

    GraphicModel& getModelBase(void) { return modelBase;}
    void setModelBase(GraphicModel& mdl) { modelBase = mdl; }; 


protected:

private:
    string strName;
    string strHost;
    string strParam;
    string strWorkDir;
    string strStdio;
    string strBroker;
    string strPrefix;
    int iRank;
    ResourceContainer resources;
    PortmapContainer portmaps;  
    PortmapIterator findPortmap(Portmap& portmap);
    string strTag;
    GraphicModel modelBase;
};


typedef vector<ModuleInterface> IModuleContainer;
typedef vector<ModuleInterface>::iterator IModuleIterator;


/**
 * Class ApplicationInterface
 */
class ApplicationInterface {

public: 
    ApplicationInterface(const char* szName) {
        if(szName) strName = szName;
    }       
    virtual ~ApplicationInterface(){}
    void setPrefix(const char* szPrefix) { if(szPrefix) strPrefix = szPrefix; } 
    const char* getName(void) { return strName.c_str(); }
    const char* getPrefix(void) { return strPrefix.c_str(); }
    bool operator==(const ApplicationInterface& alt) {      
        return (strName == alt.strName); 
    }

    // modelBased is used to keep the graphic and geometric 
    // information which is directly loaded from application 
    // description file. 
    GraphicModel& getModelBase(void) { return modelBase;}
    void setModelBase(GraphicModel& mdl) { modelBase = mdl; }; 


protected:

private:
    string strName;
    string strPrefix;
    GraphicModel modelBase;
};


typedef vector<ApplicationInterface> IApplicationContainer;
typedef vector<ApplicationInterface>::iterator IApplicationIterator;

/**
 * Class Application  
 */
class Application : public Node{

public: 
    Application(void);
    Application(const char* szName);
    Application(const Application &app);
    virtual ~Application();
    void setName(const char* szName) { 
        if(szName){
            strName = szName; 
        }
    }
    void setVersion(const char* szVersion) { if(szVersion) strVersion = szVersion; }
    void setDescription(const char* szDesc) { if(szDesc) strDescription = szDesc; }
    const char* getName(void) { return strName.c_str(); }
    const char* getVersion(void) { return strVersion.c_str(); }
    const char* getDescription(void) { return strDescription.c_str(); } 
    virtual Node* clone(void);

    void addAuthor(Author& author) { authors.push_back(author); }
    int authorCount(void) { return authors.size(); }
    Author& getAuthorAt(int index) { return authors[index]; }
    bool removeAuthor(Author& author);

    int imoduleCount(void) { return Imodules.size(); }
    ModuleInterface& getImoduleAt(int index){ return Imodules[index]; }
    bool addImodule(ModuleInterface &imod);
    bool removeImodule(ModuleInterface& imod);
    void removeAllImodules(void) { Imodules.clear(); }

    int iapplicationCount(void) { return Iapplications.size(); }
    ApplicationInterface& getIapplicationAt(int index){ return Iapplications[index]; }
    bool addIapplication(ApplicationInterface &iapp);
    bool removeIapplication(ApplicationInterface& iapp);
    void removeAllIapplications(void) { Iapplications.clear(); }
    
    int resourcesCount(void) { return resources.size(); }
    ResYarpPort& getResourceAt(int index){ return resources[index]; }
    bool addResource(ResYarpPort &res);
    bool removeResource(ResYarpPort& res);

    void setPrefix(const char* szPrefix) { if(szPrefix) strPrefix = szPrefix; } 
    const char* getPrefix(void) { return strPrefix.c_str(); }
    void setBasePrefix(const char* szPrefix) { if(szPrefix) strBasePrefix = szPrefix; } 
    const char* getBasePrefix(void) { return strBasePrefix.c_str(); }


    void setXmlFile(const char* szFilename) { if(szFilename) strXmlFile = szFilename;}
    const char* getXmlFile(void) { return strXmlFile.c_str(); }

    int connectionCount(void) { return connections.size(); }
    Connection& getConnectionAt(int index){ return connections[index]; }
    Connection& addConnection(Connection &cnn);
    bool removeConnection(Connection& cnn);
    //void updateConnectionPrefix(void);
    
    int arbitratorCount(void) { return arbitrators.size(); }
    Arbitrator& getArbitratorAt(int index){ return arbitrators[index]; }
    Arbitrator& addArbitrator(Arbitrator &arb);
    bool removeArbitrator(Arbitrator& arb);

    void setOwner(Node* owner){ appOwner = owner; }
    Node* owner(void) { return appOwner; }


    bool operator==(const Application& app) {       
        return (strName == app.strName); 
    }
    
    void clear(void) {
        strName.clear();
        strVersion.clear();
        strDescription.clear();
        strPrefix.clear();
        strBasePrefix.clear();
        authors.clear();
        Imodules.clear();
        connections.clear();
        strXmlFile.clear();
        Iapplications.clear();
        resources.clear();
    }

    GraphicModel& getModelBase(void) { return modelBase;}
    void setModelBase(GraphicModel& mdl) { modelBase = mdl; }; 

    map<string, int> modList;

protected:

private:
    string strName;
    string strVersion;
    string strDescription;
    AuthorContainer authors;
    IModuleContainer Imodules;
    IApplicationContainer Iapplications;
    ResourceContainer resources;
    CnnContainer connections;
    ArbContainer arbitrators;
    string strPrefix;  
    string strBasePrefix;
    string strXmlFile;
    Node* appOwner;

    GraphicModel modelBase;
    
    IModuleIterator findImodule(ModuleInterface& imod);
    IApplicationIterator findIapplication(ApplicationInterface& iapp);
    ResourceIterator findResource(ResYarpPort& res);
    CnnIterator findConnection(Connection& cnn);
    ArbIterator findArbitrator(Arbitrator& arb);

};
 
typedef vector<Application*> ApplicaitonPContainer;
typedef vector<Application*>::iterator ApplicationPIterator;

//}

#endif //__APPLICATION__
