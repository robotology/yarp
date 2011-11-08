// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __UTILITY__
#define __UTILITY__

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

#include "ymm-types.h" 


bool compareString(const char* szFirst, const char* szSecond);
Carrier strToCarrier(const char* szCar);
const char* carrierToStr(Carrier cr);
OS strToOS(const char* szOS);


/**
 * Singleton class ErrorLogger
 */  
class ErrorLogger
{
public:
	static ErrorLogger* Instance(void);
	
	void addWarning(const char* szWarning) { 
		if(szWarning) 
			warnings.push_back(string(szWarning));
	}
	
	void addWarning(const string &str) { 
		warnings.push_back(str);
	}
	
	void addWarning(const ostringstream &stream) { 
		addWarning(stream.str());	
	}

	void addError(const char* szError) {
		if(szError) 
			errors.push_back(string(szError));
	}

	void addError(const string &str) {
		errors.push_back(str);
	}

	void addError(const ostringstream &stream) {
		addError(stream.str());
	}
	
	const char* getLastError(void) { 
		if(errors.empty())
			return NULL;
		static string msg;
		msg = errors.back();
		errors.pop_back();
		return msg.c_str(); 		
	}
	
	const char* getLastWarning(void) {
		if(warnings.empty())
			return NULL;
		static string msg;
		msg = warnings.back();
		warnings.pop_back();
		return msg.c_str(); 
	}
	void clear(void) { errors.clear(); warnings.clear(); }
	size_t errorCount(void){ return errors.size();}
	size_t warningCount(void){ return warnings.size();}
 
private:
	ErrorLogger(){};  
	ErrorLogger(ErrorLogger const&){};
	//ErrorLogger& operator=(ErrorLogger const&){};
	static ErrorLogger* pInstance;
	vector<string> errors;
	vector<string> warnings;
};
 
 
 
 

//}


#endif //__UTILITY__
