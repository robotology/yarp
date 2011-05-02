// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_ROSTYPECODEGENYARP_INC
#define YARP2_ROSTYPECODEGENYARP_INC

#include <RosType.h>

class RosTypeCodeGenYarp : public RosTypeCodeGen {
public:
    std::string counter;
    std::string len;
    bool usedLen;

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
};

#endif
