#ifndef TEXTPARSER_H
#define TEXTPARSER_H
#include <string>
#include <map>
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
    TextParser(){logger = logger->Instance();}

    bool addVariable(const std::string& key, const std::string& value)
    {
        if (key.empty())
        {
            war << "TextParser: empty key on variable setting..";
            if (logger) logger->addWarning(war);
            return false;
        }
        variables[key] = parseText(value.c_str());
        return true;
    }

    std::string    parseText(const char *element)
    {
        using namespace yarp::os;
        using namespace std;

        string ret, startKeyword, endKeyword;
        size_t s, e;
        bool   badSymbol;

        ret = "";

        if(element)
        {
            ret          = element;
            startKeyword = "$ENV{";
            endKeyword   = "}";
            badSymbol    = ret.find("$") != string::npos;
            s            = ret.find(startKeyword);
            e            = ret.find(endKeyword, s);

            if(s != string::npos && e != string::npos)
            {
                string envName, envValue;

                envName   = ret.substr(s + startKeyword.size(), e - s -startKeyword.size());
                envValue  = NetworkBase::getEnvironment(envName.c_str());
                ret       = ret.substr(0, s)+ envValue + ret.substr(e + endKeyword.size(), ret.size() - endKeyword.size());
                badSymbol = false;
                return parseText(ret.c_str());
            }

            ret = element;
            startKeyword = "${";
            endKeyword   = "}";
            s            = ret.find(startKeyword);
            e            = ret.find(endKeyword, s);

            if(s != string::npos && e != string::npos)
            {
                string envName, envValue;

                envName   = ret.substr(s + startKeyword.size(), e - s -startKeyword.size());
                envValue  = variables[envName];
                ret       = ret.substr(0, s)+ envValue + ret.substr(e + endKeyword.size(), ret.size() - endKeyword.size());
                badSymbol = false;
                return parseText(ret.c_str());
            }

            if(badSymbol)
            {
                war << "use of symbol '$' detected but no keyword understood.. possible use: ${foo} for internal variable or $ENV{foo} for environment variable";
                if (logger) logger->addWarning(war);
            }
        }

        return ret;

    }
};
}//manager
}//yarp

#endif // TEXTPARSER_H
