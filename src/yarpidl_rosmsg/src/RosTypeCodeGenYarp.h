/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    std::string packageName;

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

    bool beginDeclare() override;
    bool declareField(const RosField& field) override;
    bool endDeclare() override;

    bool beginConstruct() override;
    bool initField(const RosField& field, bool &isFirstToInit) override;
    bool endInitConstruct() override;
    bool constructField(const RosField& field) override;
    bool endConstruct() override;

    bool beginClear() override;
    bool clearField(const RosField& field) override;
    bool endClear() override;

    bool beginRead(bool bare, int len) override;
    bool readField(bool bare, const RosField& field) override;
    bool endRead(bool bare) override;

    bool beginWrite(bool bare, int len) override;
    bool writeField(bool bare,const RosField& field) override;
    bool endWrite(bool bare) override;

    virtual bool endType(const std::string& tname,
                         const RosField& field) override;

    bool writeIndex(RosTypeCodeGenState& state) override;
    bool hasNativeTimeClass() const override {
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
