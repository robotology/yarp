#ifndef TEXTPARSER_H
#define TEXTPARSER_H
#include <string>
#include <map>
#include <yarp/os/Network.h>
#include <iostream>

class TextParser
{
    typedef std::map<std::string, std::string> VarMap;

    VarMap variables;
public:
    TextParser(){}
    bool addVariable(const std::string& key, const std::string& value)
    {
        if (key.empty())
        {
            cout << "TextParser: empty key on variable setting..";
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

        ret = "";

        if(element)
        {
            ret          = element;
            startKeyword = "$ENV{";
            endKeyword   = "}";
            s            = ret.find(startKeyword);
            e            = ret.find(endKeyword, s);

            if(s != string::npos && e != string::npos)
            {
                string envName, envValue;

                envName  = ret.substr(s + startKeyword.size(), e - s -startKeyword.size());
                envValue = NetworkBase::getEnvironment(envName.c_str());
                ret      = ret.substr(0, s)+ envValue + ret.substr(e + endKeyword.size(), ret.size() - endKeyword.size());

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

                envName  = ret.substr(s + startKeyword.size(), e - s -startKeyword.size());
                envValue = variables[envName];
                ret      = ret.substr(0, s)+ envValue + ret.substr(e + endKeyword.size(), ret.size() - endKeyword.size());

                return parseText(ret.c_str());
            }
        }

        return ret;

    }
};

#endif // TEXTPARSER_H
