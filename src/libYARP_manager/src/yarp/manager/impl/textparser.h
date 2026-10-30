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
#include <yarp/manager/impl/paramresolver.h>

namespace yarp::manager {


class TextParser
{
    typedef std::map<std::string, std::string> VarMap;

    VarMap        variables;
    ErrorLogger*  logger;

    // True if text contains a '$' that does not start an application parameter
    // placeholder ($PAR{...}), which is resolved later by ParamResolver
    static bool hasUnknownSymbol(const std::string& text)
    {
        for (size_t pos = text.find('$'); pos != std::string::npos; pos = text.find('$', pos + 1)) {
            if (text.compare(pos, ParamResolver::startKeyword.size(), ParamResolver::startKeyword) != 0) {
                return true;
            }
        }
        return false;
    }

public:
    TextParser(){logger = ErrorLogger::Instance();}

    bool addVariable(const std::string& key, const std::string& value)
    {
        if (key.empty())
        {
            OSTRINGSTREAM war;
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
            bool badSymbol    = hasUnknownSymbol(ret);
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
                OSTRINGSTREAM war;
                war << "use of symbol '$' detected but no keyword understood.. possible use: ${foo} for internal variable, $ENV{foo} for environment variable or $PAR{foo=default} for application parameter";
                if (logger) {
                    logger->addWarning(war);
                }
            }
        }

        return ret;

    }
};
} // namespace yarp::manager

#endif // TEXTPARSER_H
