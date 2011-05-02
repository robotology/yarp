// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <RosTypeCodeGenYarp.h>

#include <stdio.h>

using namespace std;

bool RosTypeCodeGenYarp::beginType(const std::string& tname,
                                   RosTypeCodeGenState& state) {
    counter = state.getFreeVariable("i");
    len = state.getFreeVariable("len");
    printf("class %s : public yarp::os::Portable {\n", tname.c_str());
    return true;
}

bool RosTypeCodeGenYarp::beginDeclare() {
    printf("public:\n");
    return true;
}           

bool RosTypeCodeGenYarp::declareField(const RosField& field) {
    if (!field.isArray) {
        printf("  %s %s;\n", field.rosType.c_str(), field.rosName.c_str());
    } else {
        printf("  std::vector<%s> %s;\n", field.rosType.c_str(), field.rosName.c_str());
    }
    return true;
}

bool RosTypeCodeGenYarp::endDeclare() {
    printf("\n");
    return true;
}

bool RosTypeCodeGenYarp::beginRead() {
    printf("  bool read(yarp::os::ConnectionReader& connection) {\n");
    usedLen = false;
    return true;
}           

// pending issues
// * translate primitive types
// * deal with strings, which are a variable-sized primitive

bool RosTypeCodeGenYarp::readField(const RosField& field) {
    //printf("    // read %s\n", field.rosName.c_str());
    if (field.rosType=="string") {
        // strings are special; variable length primitive
        printf("// String not dealt with correctly yet for reading\n");
    }
    if (field.isPrimitive) {
        if (field.isArray) {
            printf("    %s%s = connection.expectInt();\n",
                   usedLen?"":"int ",
                   len.c_str());
            usedLen = true;
            printf("    fields.resize(%s);\n", len.c_str());
            printf("    if (!connection.readBlock(&%s[0],sizeof(%s)*%s) return false;\n",
                   field.rosName.c_str(),
                   field.rosType.c_str(),
                   len.c_str());
        } else {
            printf("    if (!connection.readPrimitive_%s(%s)) return false;\n",
                   field.rosType.c_str(),
                   field.rosName.c_str());
        }
    } else {
        if (field.isArray) {
            printf("    %s%s = connection.expectInt();\n",
                   usedLen?"":"int ",
                   len.c_str());
            usedLen = true;
            printf("    %s.resize(%s);\n", 
                   field.rosName.c_str(),
                   len.c_str());
            printf("    for (int %s=0; %s<%s; %s++) {\n", 
                   counter.c_str(),
                   counter.c_str(),
                   len.c_str(),
                   counter.c_str());
            printf("      if (!%s[%s].read(connection)) return false;\n",
                   field.rosName.c_str(),
                   counter.c_str());
            printf("    }\n");                  
        } else {
            printf("    if (!%s.read(connection)) return false;\n",
                   field.rosName.c_str());
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endRead() {
    printf("    return true;\n");
    printf("  }\n\n");
    return true;
}

bool RosTypeCodeGenYarp::beginWrite() {
    printf("  bool write(yarp::os::ConnectionWriter& connection) {\n");
    return true;
}

bool RosTypeCodeGenYarp::writeField(const RosField& field) {
    if (field.rosType=="string") {
        // strings are special; variable length primitive
        if (field.isArray) {
            printf("    if (!connection.appendInt(%s.size()) return false;\n",
                   field.rosName.c_str());
            printf("    for (int %s=0; %s<%s.size(); %s++) {\n", 
                   counter.c_str(),
                   counter.c_str(),
                   field.rosName.c_str(),
                   counter.c_str());
            printf("      if (!connection.appendInt(%s.length())) return false;\n",
                   field.rosName.c_str());
            printf("      if (!connection.appendExternalBlock(%s.c_str(),%s.length())) return false;\n",
                   field.rosName.c_str(),
                   field.rosName.c_str());
            printf("    }\n");                  
        } else {
            printf("    if (!connection.appendInt(%s.length())) return false;\n",
                   field.rosName.c_str());
            printf("    if (!connection.appendExternalBlock(%s.c_str(),%s.length())) return false;\n",
                   field.rosName.c_str(),
                   field.rosName.c_str());
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            printf("    if (!connection.appendInt(%s.size()) return false;\n",
                   field.rosName.c_str());
            printf("    if (!connection.appendExternalBlock(&%s[0],sizeof(%s)*%s.size()) return false;\n",
                   field.rosName.c_str(),
                   field.rosType.c_str(),
                   field.rosName.c_str());
        } else {
            printf("    if (!connection.writePrimitive_%s(%s)) return false;\n",
                   field.rosType.c_str(),
                   field.rosName.c_str());
        }
    } else {
        if (field.isArray) {
            printf("    if (!connection.appendInt(%s.size()) return false;\n",
                   field.rosName.c_str());
            printf("    for (int %s=0; %s<%s.size(); %s++) {\n", 
                   counter.c_str(),
                   counter.c_str(),
                   field.rosName.c_str(),
                   counter.c_str());
            printf("      if (!%s[%s].write(connection)) return false;\n",
                   field.rosName.c_str(),
                   counter.c_str());
            printf("    }\n");                  
        } else {
            printf("    if (!%s.write(connection)) return false;\n",
                   field.rosName.c_str());
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endWrite() {
    printf("    return true;\n");
    printf("  }\n");
    return true;
}


bool RosTypeCodeGenYarp::endType() {
    printf("};\n\n");
    return true;
}

