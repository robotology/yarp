	// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __MODLOADER__
#define __MODLOADER__

#include "ymm-types.h" 
#include "module.h"
#include "data.h"

//namespace ymm {



/**
 * Abstract Class ModuleLoader  
 */
class ModuleLoader {

public: 
	ModuleLoader(void) {}
	virtual ~ModuleLoader() {}
	virtual bool init(void) = 0; 
	virtual void reset(void) = 0;
	virtual void fini(void) = 0;
	virtual Module* getNextModule(void) = 0;
	
protected:

private:

};


/**
 * Class XmlModLoader  
 */
class XmlModLoader : public ModuleLoader {

public:
	XmlModLoader(const char* szFileName);
	XmlModLoader(const char* szPath, const char* szModuleName);
	~XmlModLoader();
	virtual bool init(void);
	virtual void reset(void);
	virtual void fini(void);
	virtual Module* getNextModule(void);
	
protected:

private:
	string strName;
	string strPath;
	string strFileName;
	vector<string> fileNames;
	Module module;
	Module* parsXml(const char* szFile);
};


 
//}

#endif //__MODLOADER__
