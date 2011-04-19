// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_ROSTYPE_INC
#define YARP2_ROSTYPE_INC

#include <string>
#include <vector>

class RosTypeSearch {
public:
    void addDirectory(const char *physical, const char *logical);

    std::string findFile(const char *tname) {
        // stub
        return std::string(tname) + ".msg";
    }
};

class RosType {
public:
    bool isValid;
    bool isArray;
    bool isPrimitive;
    std::string rosType;
    std::string rosName;
    std::vector<RosType> subRosType;

    RosType() {
        isValid = false;
        isArray = false;
        isPrimitive = false;
    }

    bool read(const char *tname, RosTypeSearch& env);

    void show();
};

#endif

