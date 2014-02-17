// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __UTILITY__
#define __UTILITY__

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <string.h>
#include <iostream>

#include "ymm-types.h"


using namespace std;

class StrStream
{
public:
    StrStream(void);
    StrStream(const std::string str);
    ~StrStream();
    std::string str(void);
    StrStream& operator<<(StrStream &oss);
    StrStream& operator<<(const std::string &str);
    StrStream& operator<<(int n);
    StrStream& operator<<(double n);
    StrStream& operator = (const char* sz);
    StrStream& operator = (char* sz);
    StrStream& operator = (const std::string &str);
    StrStream& operator = (StrStream &oss);
    StrStream& operator = (int n);
    friend std::ostream& operator << (std::ostream &os , StrStream& sstr);

private:
    std::string dummyStr;
};


#if defined(_MSC_VER) && (_MSC_VER == 1600)
    typedef StrStream OSTRINGSTREAM;
#else
    #include <sstream>
    typedef std::stringstream OSTRINGSTREAM;
#endif


/**
 * Singleton class ErrorLogger
 */
class ErrorLogger
{
public:
    static ErrorLogger* Instance(void);

    void addWarning(const char* szWarning);
    void addWarning(const string &str);
    void addWarning(OSTRINGSTREAM &stream);
    void addError(const char* szError);
    void addError(const string &str);
    void addError(OSTRINGSTREAM &stream);
    const char* getLastError(void);
    const char* getLastWarning(void);
    void clear(void);
    int errorCount(void);
    int warningCount(void);

private:
    ErrorLogger(){};
    ErrorLogger(ErrorLogger const&){};
    static ErrorLogger* pInstance;
    vector<string> errors;
    vector<string> warnings;
};


bool compareString(const char* szFirst, const char* szSecond);
void trimString(string& str);
OS strToOS(const char* szOS);

class Graph;
bool exportDotGraph(Graph& graph, const char* szFileName);
//}


#endif //__UTILITY__
