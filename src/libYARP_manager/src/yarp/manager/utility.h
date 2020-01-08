/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_UTILITY
#define YARP_MANAGER_UTILITY

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>

#include <yarp/manager/ymm-types.h>

namespace yarp {
namespace manager {


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
    void addWarning(const std::string &str);
    void addWarning(OSTRINGSTREAM &stream);
    void addError(const char* szError);
    void addError(const std::string &str);
    void addError(OSTRINGSTREAM &stream);
    const char* getLastError(void);
    const char* getLastWarning(void);
    const char* getFormatedErrorString();
    const char* getFormatedWarningString();
    void clear(void);
    int errorCount(void);
    int warningCount(void);

private:
    ErrorLogger(){}
    ErrorLogger(ErrorLogger const&){}
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};


bool compareString(const char* szFirst, const char* szSecond);
void trimString(std::string& str);
OS strToOS(const char* szOS);

class Graph;
bool exportDotGraph(Graph& graph, const char* szFileName);

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_UTILITY__
