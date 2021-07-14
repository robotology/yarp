/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TEXTPARSER_H
#define TEXTPARSER_H
#include <string>
#include <map>
#include <yarp/conf/environment.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <iostream>
#include <yarp/manager/utility.h>

namespace yarp{
namespace manager{


class TextParser
{
    typedef std::map<std::string, std::string> VarMap;

    VarMap        variables;
    ErrorLogger*  logger;
    OSTRINGSTREAM war;
public:
    TextParser(){logger = ErrorLogger::Instance();}

    bool addVariable(const std::string& key, const std::string& value)
    {
        if (key.empty())
        {
            war << "TextParser: empty key on variable setting..";
            if (logger) {
                logger->addWarning(war);
            }
            return false;
        }
        variables[key] = parseText(value.c_str());
        return true;
    }

    std::string    parseText(const char *element)
    {

        std::string ret, startKeyword, endKeyword;
        size_t s, e;

        ret = "";

        if(element)
        {
            ret          = element;
            startKeyword = "$ENV{";
            endKeyword   = "}";
            bool badSymbol    = ret.find("$") != std::string::npos;
            s            = ret.find(startKeyword);
            e            = ret.find(endKeyword, s);

            if(s != std::string::npos && e != std::string::npos)
            {
                std::string envName, envValue;

                envName   = ret.substr(s + startKeyword.size(), e - s -startKeyword.size());
                envValue  = yarp::conf::environment::get_string(envName);
                ret       = ret.substr(0, s)+ envValue + ret.substr(e + endKeyword.size(), ret.size() - endKeyword.size());
                return parseText(ret.c_str());
            }

            ret = element;
            startKeyword = "${";
            endKeyword   = "}";
            s            = ret.find(startKeyword);
            e            = ret.find(endKeyword, s);

            if(s != std::string::npos && e != std::string::npos)
            {
                std::string envName, envValue;

                envName   = ret.substr(s + startKeyword.size(), e - s -startKeyword.size());
                envValue  = variables[envName];
                ret       = ret.substr(0, s)+ envValue + ret.substr(e + endKeyword.size(), ret.size() - endKeyword.size());
                return parseText(ret.c_str());
            }

            if(badSymbol)
            {
                war << "use of symbol '$' detected but no keyword understood.. possible use: ${foo} for internal variable or $ENV{foo} for environment variable";
                if (logger) {
                    logger->addWarning(war);
                }
            }
        }

        return ret;

    }
};
}//manager
}//yarp

#endif // TEXTPARSER_H
