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
#include <chrono>
#include <iomanip>

#include <yarp/manager/ymm-types.h>

namespace yarp::manager {


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

/**
 * Singleton class For storing execution start time
 */
class ClockStart {
public:
    static ClockStart& getInstance() {
        static ClockStart instance;
        return instance;
    }

    ClockStart(const ClockStart&) = delete;
    ClockStart& operator=(const ClockStart&) = delete;

    /**
     * @brief Get the starting time as a string in HH:MM:SS format
     * @return the starting time as a string
     */
    std::string getStartTime() const {
        std::ostringstream oss;
        oss << std::put_time(&startTimeStruct, "%H:%M:%S");
        return oss.str();
    }

    /**
     * @brief Set the starting time from a string in HH:MM:SS format
     * @param startTimeStr the starting time as a string
     */
    void setStartTime(std::string startTimeStr) {
        std::istringstream iss(startTimeStr);
        std::tm tm = {};
        if (iss >> std::get_time(&tm, "%H:%M:%S")) {
            startTimeStruct = tm;
        }
    }

private:
    std::chrono::system_clock::time_point startTime;
    std::tm startTimeStruct;

    /**
     * @brief Private constructor to initialize the start time
     * @details The start time is initialized to the current time
     */
    ClockStart() {
        startTime = std::chrono::system_clock::now();
        std::time_t startTimeT = std::chrono::system_clock::to_time_t(startTime);

#ifdef _WIN32
        localtime_s(&startTimeStruct, &startTimeT);
#else
        localtime_r(&startTimeT, &startTimeStruct);
#endif
    }
};


bool compareString(const char* szFirst, const char* szSecond);
std::string getCurrentTimeString();
std::string getElapsedTimeString(const std::string& startTimeStr);
void trimString(std::string& str);
OS strToOS(const char* szOS);

class Graph;
bool exportDotGraph(Graph& graph, const char* szFileName);

} // namespace yarp::manager


#endif // __YARP_MANAGER_UTILITY__
