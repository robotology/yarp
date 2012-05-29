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

#include "ymm-types.h" 
#include "node.h"
#include "module.h"
#include "utility.h"
#include "logicresource.h"
#include "primresource.h"

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
    Connection(const char* szFrom, const char* szTo, const char* szCr=NULL) {
        if(szFrom) strFrom = szFrom;
        if(szTo) strTo = szTo;
        if(szCr) strCarrier = szCr;
        bWithPriority = false;
        modOwner = NULL;
        bExternalFrom = false;
        bExternalTo = false;
        bPersist = false;
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
    void setOwner(Module* module){ modOwner = module; }
    Module* owner(void) { return modOwner; }
    void setPriority(bool withprio) { bWithPriority = withprio; }
    bool withPriority(void) { return bWithPriority; }
    bool operator==(const Connection& alt) {        
        return ((strFrom == alt.strFrom) && (strTo == alt.strTo)); 
    }
    
protected:

private:
    string strFrom;
    string strTo;
    bool bExternalTo;
    bool bExternalFrom; 
    string strCarrier;
    bool bPersist; 
    Module* modOwner;
    bool bWithPriority; 
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
    
    virtual ~ModuleInterface(){}
        
    void setHost(const char* szHost) { if(szHost) strHost = szHost; }
    void setParam(const char* szParam) { if(szParam) strParam = szParam; }
    void setRank(int irank) { iRank = irank;}
    void setWorkDir(const char* szWDir) { if(szWDir) strWorkDir = szWDir; }
    void setStdio(const char* szStdio) { if(szStdio) strStdio = szStdio; }
    void setBroker(const char* szBroker) { if(szBroker) strBroker = szBroker; }
    void setPrefix(const char* szPrefix) {if(szPrefix) strPrefix = szPrefix; }

    const char* getName(void) { return strName.c_str(); }
    const char* getHost(void) { return strHost.c_str(); }
    const char* getParam(void) { return strParam.c_str(); }
    int getRank(void) { return iRank; }
    const char* getWorkDir(void) { return strWorkDir.c_str(); } 
    const char* getStdio(void) { return strStdio.c_str(); }
    const char* getBroker(void) { return strBroker.c_str(); }   
    const char* getPrefix(void) { return strPrefix.c_str(); }   

    bool operator==(const ModuleInterface& modint) {        
        return (strName == modint.strName); 
    }
   
    void addResource(ResYarpPort &res) { resources.push_back(res); }
    ResourceContainer& getResources(void) { return resources; }
 
    int portmapCount(void) { return portmaps.size(); }
    Portmap& getPortmapAt(int index){ return portmaps[index]; }
    bool addPortmap(Portmap &portmap);
    bool removePortmap(Portmap& portmap);
    
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
    
protected:

private:
    string strName;
    string strPrefix;
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

    void addAuthor(const char* szAuthor) { authors.push_back(szAuthor); }
    int authorCount(void) { return authors.size(); }
    const char* getAuthorAt(int index) { return authors[index].c_str(); }   

    int imoduleCount(void) { return Imodules.size(); }
    ModuleInterface& getImoduleAt(int index){ return Imodules[index]; }
    bool addImodule(ModuleInterface &imod);
    bool removeImodule(ModuleInterface& imod);

    int iapplicationCount(void) { return Iapplications.size(); }
    ApplicationInterface& getIapplicationAt(int index){ return Iapplications[index]; }
    bool addIapplication(ApplicationInterface &iapp);
    bool removeIapplication(ApplicationInterface& iapp);
    
    int resourcesCount(void) { return resources.size(); }
    ResYarpPort& getResourceAt(int index){ return resources[index]; }
    bool addResource(ResYarpPort &res);
    bool removeResource(ResYarpPort& res);

    void setPrefix(const char* szPrefix) { if(szPrefix) strPrefix = szPrefix; } 
    const char* getPrefix(void) { return strPrefix.c_str(); }

    void setXmlFile(const char* szFilename) { if(szFilename) strXmlFile = szFilename;}
    const char* getXmlFile(void) { return strXmlFile.c_str(); }

    int connectionCount(void) { return connections.size(); }
    Connection& getConnectionAt(int index){ return connections[index]; }
    bool addConnection(Connection &cnn);
    bool removeConnecrion(Connection& cnn);
    //void updateConnectionPrefix(void);

    bool operator==(const Application& app) {       
        return (strName == app.strName); 
    }
    
    void clear(void) {
        strName.clear();
        strVersion.clear();
        strDescription.clear();
        strPrefix.clear();
        authors.clear();
        Imodules.clear();
        connections.clear();
        strXmlFile.clear();
        Iapplications.clear();
        resources.clear();
    }
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
    string strPrefix;   
    string strXmlFile;

    
    IModuleIterator findImodule(ModuleInterface& imod);
    IApplicationIterator findIapplication(ApplicationInterface& iapp);
    ResourceIterator findResource(ResYarpPort& res);
    CnnIterator findConnection(Connection& cnn);
};
 
typedef vector<Application*> ApplicaitonPContainer;
typedef vector<Application*>::iterator ApplicationPIterator;

//}

#endif //__APPLICATION__
