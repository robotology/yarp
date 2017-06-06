/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_ROSTYPECODEGENYARP_INC
#define YARP2_ROSTYPECODEGENYARP_INC

#include <RosType.h>
#include <cstdio>

class RosYarpType {
public:
    std::string rosType;
    std::string yarpType;
    std::string writer;
    std::string reader;
    std::string yarpWriter;
    std::string yarpWriterCast;
    std::string yarpTag;
    std::string yarpReader;
    std::string yarpWireReader;
    std::string yarpDefaultValue;
    int len;

    RosYarpType() {
        len = 0;
    }
};

class RosTypeCodeGenYarp : public RosTypeCodeGen {
private:
    std::string target;
    std::string className;

public:
    std::string counter;
    std::string len;
    std::string len2;
    bool usedLen;
    bool usedLen2;
    bool first;
    FILE *out;

    RosYarpType mapPrimitive(const RosField& field);

    virtual bool beginType(const std::string& tname,
                           RosTypeCodeGenState& state) override;

    virtual bool beginDeclare() override;
    virtual bool declareField(const RosField& field) override;
    virtual bool endDeclare() override;

    virtual bool beginConstruct() override;
    virtual bool constructField(const RosField& field) override;
    virtual bool endConstruct() override;

    virtual bool beginClear() override;
    virtual bool clearField(const RosField& field) override;
    virtual bool endClear() override;

    virtual bool beginRead(bool bare, int len) override;
    virtual bool readField(bool bare, const RosField& field) override;
    virtual bool endRead(bool bare) override;

    virtual bool beginWrite(bool bare, int len) override;
    virtual bool writeField(bool bare,const RosField& field) override;
    virtual bool endWrite(bool bare) override;

    virtual bool endType(const std::string& tname,
                         const RosField& field) override;

    virtual bool hasNativeTimeClass() const override {
        return false;
    }

    void setTargetDirectory(const char *tname) {
        target = tname;
    }

    std::string getTargetDirectory() {
        return target;
    }
};

#endif
