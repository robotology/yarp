/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP2_ROSTYPE_INC
#define YARP2_ROSTYPE_INC

#include <string>
#include <vector>
#include <map>
#include <list>

class RosTypeSearch
{
private:
    bool find_service;
    std::string target_dir;
    std::string source_dir;
    std::string package_name;
    bool allow_ros;
    bool allow_web;
    bool abort_on_error;
    bool verbose;

    bool fetchFromRos(const std::string& target_file,
                      const std::string& type_name,
                      bool find_service);

    bool fetchFromWeb(const std::string& target_file,
                      const std::string& type_name,
                      bool find_service);
public:
    RosTypeSearch() :
            find_service(false),
            target_dir("."),
            source_dir(""),
            package_name(""),
            allow_ros(true),
            allow_web(false),
            abort_on_error(true),
            verbose(false)
    {
    }

    void lookForService(bool flag) {
        find_service = flag;
    }

    void disableRos() {
        allow_ros = false;
    }

    void enableWeb() {
        allow_web = true;
    }

    void softFail() {
        abort_on_error = false;
    }

    void setVerbose() {
        verbose = true;
    }

    std::string findFile(const char *tname);

    std::string readFile(const char *fname);

    void setTargetDirectory(const char *tname) {
        target_dir = tname;
    }

    std::string getTargetDirectory() {
        return target_dir;
    }
};

class RosTypeCodeGen;
class RosTypeCodeGenState;


class RosType
{
public:
    bool isValid;
    bool isArray;
    int arrayLength;
    bool isPrimitive;
    bool isRosPrimitive;
    bool isStruct;
    std::string rosType;
    std::string rosRawType;
    std::string rosName;
    std::string rosPath;
    std::string initializer;
    std::vector<RosType> subRosType;
    std::string txt;
    std::list<std::string> checksum_var_text;
    std::list<std::string> checksum_const_text;
    std::string checksum;
    std::string source;
    RosType *reply;
    std::string package;
    bool verbose;

    RosType() :
            isValid(false),
            isArray(false),
            arrayLength(-1),
            isPrimitive(false),
            isRosPrimitive(false),
            isStruct(false),
            reply(nullptr),
            verbose(false)
    {
    }

    void clear() {
        isValid = false;
        isArray = false;
        arrayLength = -1;
        isPrimitive = false;
        isRosPrimitive = false;
        isStruct = false;
        txt = "";
        rosType = "";
        rosRawType = "";
        rosName = "";
        initializer = "";
        subRosType.clear();
        if (reply) {
            delete reply;
            reply = nullptr;
        }
        checksum_var_text.clear();
        checksum_const_text.clear();
        checksum = "";
        source = "";
        //package = "";
    }

    virtual ~RosType() {
        clear();
    }

    bool read(const char *tname, RosTypeSearch& env, RosTypeCodeGen& gen,
              int nesting = 0);
    bool cache(const char *tname, RosTypeSearch& env, RosTypeCodeGen& gen);
    void show();

    bool emitType(RosTypeCodeGen& gen,
                  RosTypeCodeGenState& state);

    bool isConst() const {
        return initializer != "";
    }

    void setVerbose() {
        verbose = true;
    }
};

typedef RosType RosField;

class RosTypeCodeGenState
{
public:
    std::string directory;
    std::map<std::string, RosType *> generated;
    std::map<std::string, bool> usedVariables;
    std::map<std::string, std::string> checksums;
    std::map<std::string, std::vector<std::string>> dependencies;
    std::map<std::string, std::vector<std::string>> dependenciesAsPaths;
    std::string txt;
    std::vector<std::string> generatedFiles;

    std::string useVariable(const std::string& name) {
        usedVariables[name] = true;
        return name;
    }

    std::string getFreeVariable(std::string name) {
        while (usedVariables.find(name)!=usedVariables.end()) {
            name = name + "_";
        }
        return useVariable(name);
    }
};

class RosTypeCodeGen
{
protected:
    bool verbose;
public:
    RosTypeCodeGen() : verbose(false) {}
    virtual ~RosTypeCodeGen() {}

    virtual bool beginType(const std::string& tname,
                           RosTypeCodeGenState& state) = 0;

    virtual bool beginDeclare() { return true; }
    virtual bool declareField(const RosField& field) = 0;
    virtual bool endDeclare() { return true; }

    virtual bool beginConstruct() { return true; }
    virtual bool initField(const RosField& field, bool &isFirstToInit) { return true; }
    virtual bool endInitConstruct() { return true; }
    virtual bool constructField(const RosField& field) { return true; }
    virtual bool endConstruct() { return true; }

    virtual bool beginClear() { return true; }
    virtual bool clearField(const RosField& field) = 0;
    virtual bool endClear() { return true; }

    virtual bool beginRead(bool bare, int len) { return true; }
    virtual bool readField(bool bare, const RosField& field) = 0;
    virtual bool endRead(bool bare) { return true; }

    virtual bool beginWrite(bool bare, int len) { return true; }
    virtual bool writeField(bool bare, const RosField& field) = 0;
    virtual bool endWrite(bool bare) { return true; }

    virtual bool endType(const std::string& tname,
                         const RosField& field) = 0;

    virtual bool writeIndex(RosTypeCodeGenState& state) = 0;

    virtual bool hasNativeTimeClass() const {
        return false;
    }

    void setVerbose() {
        verbose = true;
    }
};


#endif
