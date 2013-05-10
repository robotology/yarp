// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_ROSTYPECODEGENYARP_INC
#define YARP2_ROSTYPECODEGENYARP_INC

#include <RosType.h>
#include <stdio.h>

class RosYarpType {
public:
    std::string rosType;
    std::string yarpType;
    std::string writer;
    std::string reader;
    int len;

    RosYarpType() {
        len = 0;
    }
};

class RosTypeCodeGenYarp : public RosTypeCodeGen {
private:
    std::string target;

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
                           RosTypeCodeGenState& state);

    virtual bool beginDeclare();
    virtual bool declareField(const RosField& field);
    virtual bool endDeclare();

    virtual bool beginRead();
    virtual bool readField(const RosField& field);
    virtual bool endRead();

    virtual bool beginWrite();
    virtual bool writeField(const RosField& field);
    virtual bool endWrite();

    virtual bool endType();

    virtual bool hasNativeTimeClass() const {
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
