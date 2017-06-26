#include <yarp/os/Value.h>
#include <yarp/os/Searchable.h>
#include <string>
#include <vector>
#include <map>
#include <yarp/os/LogStream.h>
class ParamParser
{
public:

    typedef enum
    {
        TYPE_STRING = 0,
        TYPE_BOOL,
        TYPE_DOUBLE,
        TYPE_BLOB,
        TYPE_INT,
        TYPE_INT64,
        TYPE_LIST,
        TYPE_LEAF,
        TYPE_VOCAB
    }paramType;

    ParamParser(std::string name)
    {
        appName                    = name;
        err_msgs     [TYPE_STRING] = "a string";
        err_msgs     [TYPE_BOOL  ] = "a boolean type";
        err_msgs     [TYPE_DOUBLE] = "a real";
        err_msgs     [TYPE_BLOB  ] = "a blob";
        err_msgs     [TYPE_INT   ] = "an integer";
        err_msgs     [TYPE_INT64 ] = "a 64 bit integer";
        err_msgs     [TYPE_LIST  ] = "list";
        err_msgs     [TYPE_LEAF  ] = "a leaf";
        err_msgs     [TYPE_VOCAB ] = "a vocab";
        isFunctionMap[TYPE_STRING] = &yarp::os::Value::isString;
        isFunctionMap[TYPE_BOOL  ] = &yarp::os::Value::isBool;
        isFunctionMap[TYPE_DOUBLE] = &yarp::os::Value::isDouble;
        isFunctionMap[TYPE_BLOB  ] = &yarp::os::Value::isBlob;
        isFunctionMap[TYPE_INT   ] = &yarp::os::Value::isInt;
        isFunctionMap[TYPE_INT64 ] = &yarp::os::Value::isInt64;
        isFunctionMap[TYPE_LIST  ] = &yarp::os::Value::isList;
        isFunctionMap[TYPE_LEAF  ] = &yarp::os::Value::isLeaf;
        isFunctionMap[TYPE_VOCAB ] = &yarp::os::Value::isVocab;

    }

    bool parse(const yarp::os::Searchable& cfg, const std::string& param, const paramType& type)
    {
        if (!cfg.check(param) || !(cfg.find(param).*isFunctionMap[type])())
        {
            std::string err_type = err_msgs.find(type) == err_msgs.end() ? "[unknow type]" : err_msgs[type];
            yError() << appName+":" << "parameter" << param << "not found or not" << err_type << "in configuration file";
            return false;
        }
        return true;
    }

    bool parse(const yarp::os::Searchable& cfg, std::vector<std::pair<std::string, ParamParser::paramType> >& paramVec)
    {
        for (auto p : paramVec)
        {
            if(!parse(cfg, p.first, p.second))
            {
                return false;
            }
        }
        return true;
    }
private:

    typedef bool(yarp::os::Value::*valueIsType)(void) const;

    std::map<paramType, std::string> err_msgs;
    std::map<paramType, valueIsType> isFunctionMap;
    std::string                      appName;
    yarp::os::Searchable*            cfg;
};
