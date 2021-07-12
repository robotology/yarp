/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    StrStream();
    StrStream(const std::string str);
    ~StrStream();
    std::string str();
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
    static ErrorLogger* Instance();

    void addWarning(const char* szWarning);
    void addWarning(const std::string &str);
    void addWarning(OSTRINGSTREAM &stream);
    void addError(const char* szError);
    void addError(const std::string &str);
    void addError(OSTRINGSTREAM &stream);
    const char* getLastError();
    const char* getLastWarning();
    const char* getFormatedErrorString();
    const char* getFormatedWarningString();
    void clear();
    int errorCount();
    int warningCount();

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
