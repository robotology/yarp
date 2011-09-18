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

using namespace std; 

//namespace ymm {


/**
 * Class Platform  
 */
class Platform{

public: 
	Platform(OS _os, const char* szDist) {
		os = _os;
		if(szDist) strDist = szDist; 
	}
	
	Platform(const char* szOS, const char* szDist) {
		if(szDist) strDist = szDist; 
		os = strToOS(szOS);
	}

	virtual ~Platform(){}
	
	OS getOS(void) { return os; }
	const char* getDistribution(void) { return strDist.c_str(); } 
	
	bool operator==(const Platform& plat) {		
		return ((os == plat.os) && (strDist == plat.strDist)); 
	}
	
protected:

private:
	OS os;
	string strDist;
};


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


typedef vector<Platform> PlatformContainer;
typedef vector<InputData> InputContainer;
typedef vector<OutputData> OutputContainer;
typedef vector<Platform>::iterator PlatformIterator;
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
	void setName(const char* szName) { 
		if(szName){
			strName = szName; 
		}
	}
	void setRank(int rank) {iRank = rank;}
	void setVersion(const char* szVersion) { if(szVersion) strVersion = szVersion; }
	void setDescription(const char* szDesc) { if(szDesc) strDescription = szDesc; }
	void setHost(const char* szHost) { if(szHost) strHost = szHost; }

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
	
	const char* getParam(void) { return strParam.c_str(); }	
	const char* getXmlFile(void) { return strXmlFile.c_str(); }
	const char* getWorkDir(void) { return strWorkDir.c_str(); }	
	const char* getStdio(void) { return strStdio.c_str(); }
	const char* getBroker(void) { return strBroker.c_str(); }	
	const char* getPrefix(void) { return strPrefix.c_str(); }	

	int argumentCount(void) { return arguments.size(); }
	Argument& getArgumentAt(int index){ return arguments[index]; }
	int platformCount(void) { return platforms.size(); }
	Platform& getPlatformAt(int index){ return platforms[index]; }
	int outputCount(void) { return outputs.size(); }
	OutputData& getOutputAt(int index) { return outputs[index]; }
	int inputCount(void) { return inputs.size(); }
	InputData& getInputAt(int index) { return inputs[index]; } 

	void addAuthor(const char* szAuthor) { authors.push_back(szAuthor); }
	int authorCount(void) { return authors.size(); }
	const char* getAuthorAt(int index) { return authors[index].c_str(); }
	
	bool addArgument(Argument& arg);
	bool removeArgument(Argument& argument);
	bool addPlatform(Platform& platform);
	bool removePlatform(Platform& platform);	
	bool addOutput(OutputData& output);
	bool removeOutput(OutputData& output);
	bool addInput(InputData& input);
	bool removeInput(InputData& input);
	
	void clearInputs(void) { inputs.clear(); }
	void clearOutputs(void) { outputs.clear(); }
	void clearPlatforms(void) { platforms.clear(); }	
	void clear(void){
		iRank = 0;
		strName.clear();
		arguments.clear();
		strVersion.clear();
		strDescription.clear();
		strHost.clear();
		authors.clear();
		platforms.clear();
		outputs.clear();
		inputs.clear();
		strXmlFile.clear();
		strParam.clear();
		strWorkDir.clear();
		strStdio.clear();
		strBroker.clear();
		strPrefix.clear();
	}
	
protected:

private:
	string strName;	
	string strVersion;
	string strDescription;
	string strHost;
	int iRank;
	ArgumentContainer arguments;
	AuthorContainer authors;
	PlatformContainer platforms; 
	OutputContainer outputs;
	InputContainer inputs;

	string strParam;
	string strXmlFile;
	string strWorkDir;
	string strStdio;
	string strBroker;
	string strPrefix;

	PlatformIterator findPlatform(Platform& platform);
	ArgumentIterator findArgument(Argument& argument);
	InputIterator findInput(InputData& input);
	OutputIterator findOutput(OutputData& output); 
};



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
		cout<<"Platforms: "<<endl;\
		for(int i=0; i<m->platformCount(); i++)\
			cout<<"           "<<m->getPlatformAt(i).getOS()\
			<<", "<<m->getPlatformAt(i).getDistribution()<<endl;\
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
