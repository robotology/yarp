/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "utility.h"


// Global static pointer used to ensure a single instance of the class.
ErrorLogger* ErrorLogger::pInstance = NULL;  
 
ErrorLogger* ErrorLogger::Instance(void)
{
	if (!pInstance)
	  pInstance = new ErrorLogger;
	return pInstance;
} 



Carrier strToCarrier(const char* szCar)
{
	if(szCar) 
	{
		if(compareString(szCar, "TCP"))
			return TCP;
		if(compareString(szCar, "UDP"))
			return UDP;
		if(compareString(szCar, "MCAST"))
			return MCAST;
		if(compareString(szCar, "SHMEM"))
			return SHMEM;
		if(compareString(szCar, "TEXT"))
			return TEXT;
	}	
	return UNKNOWN;	
}

const char* carrierToStr(Carrier cr)
{	
	switch(cr){
		case TCP:{return("tcp");}
		case UDP:{return("udp");}
		case MCAST:{return("mcast");}
		case SHMEM:{return("shmem");}
		case TEXT:{return("text");}
		default:{return("tcp");}
	 };	
}

OS strToOS(const char* szOS)
{
	if(szOS) 
	{
		if(compareString(szOS, "LINUX"))
			return LINUX;
		if (compareString(szOS, "WINDOWS"))
			return WINDOWS;
		if (compareString(szOS, "MAC"))
			return MAC;
	}	
	return OTHER;
}


bool compareString(const char* szFirst, const char* szSecond) 
{
	if(!szFirst && !szSecond)
		return true; 
	if(	!szFirst || !szSecond)
		return false;
		
	string strFirst(szFirst);
	string strSecond(szSecond);
	transform(strFirst.begin(), strFirst.end(), strFirst.begin(), 
			  (int(*)(int))toupper);
	transform(strSecond.begin(), strSecond.end(), strSecond.begin(),
			  (int(*)(int))toupper);
	if(strFirst == strSecond) 
		return true; 
	return false;
}
