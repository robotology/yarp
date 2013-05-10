// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_ROSTYPE_INC
#define YARP2_ROSTYPE_INC

#include <string>
#include <vector>
#include <map>

class RosTypeSearch {
public:
    std::string findFile(const char *tname);
};

class RosTypeCodeGen;
class RosTypeCodeGenState;


class RosType {
public:

    // std::vector<RosType> subRosType; is awkward to export in a MSVC DLL
    // so we work around it
    class RosTypes {
    public:
        void *system_resource;

        RosTypes();

        virtual ~RosTypes();

        RosTypes(const RosTypes& alt);

        const RosTypes& operator=(const RosTypes& alt);

        void clear();

        void push_back(const RosType& t);

        size_t size();

        RosType& operator[](int i);
    };

public:
    bool isValid;
    bool isArray;
    bool isPrimitive;
    std::string rosType;
    std::string rosName;
    RosTypes subRosType;
    std::string txt;

    RosType() {
        isValid = false;
        isArray = false;
        isPrimitive = false;
        txt = "";
    }

    bool read(const char *tname, RosTypeSearch& env, RosTypeCodeGen& gen,
              int nesting = 0);
    void show();

    bool emitType(RosTypeCodeGen& gen, 
                  RosTypeCodeGenState& state);
};

typedef RosType RosField;

class RosTypeCodeGenState {
public:
    std::string directory;
    std::map<std::string, bool> generated;
    std::map<std::string, bool> usedVariables;
    std::vector<std::string> dependencies;
    std::string txt;

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

class RosTypeCodeGen {
public:
    virtual ~RosTypeCodeGen() {}

    virtual bool beginType(const std::string& tname,
                           RosTypeCodeGenState& state) = 0;

    virtual bool beginDeclare() { return true; }
    virtual bool declareField(const RosField& field) = 0;
    virtual bool endDeclare() { return true; }

    virtual bool beginRead() { return true; } 
    virtual bool readField(const RosField& field) = 0;
    virtual bool endRead() { return true; }

    virtual bool beginWrite() { return true; }
    virtual bool writeField(const RosField& field) = 0;
    virtual bool endWrite() { return true; }

    virtual bool endType() = 0;

    virtual bool hasNativeTimeClass() const {
        return false;
    }
};

class RosTypeCodeGenTest : public RosTypeCodeGen {
public:
    virtual bool beginType(const std::string& tname, 
                           RosTypeCodeGenState& state);
    virtual bool declareField(const RosField& field);
    virtual bool readField(const RosField& field);
    virtual bool writeField(const RosField& field);
    virtual bool endType();
};



#endif

