// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <RosTypeCodeGenYarp.h>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

bool RosTypeCodeGenYarp::beginType(const std::string& tname,
                                   RosTypeCodeGenState& state) {
    counter = state.getFreeVariable("i");
    len = state.getFreeVariable("len");
    len2 = state.getFreeVariable("len2");
    string safe_tname = tname;
    if (safe_tname.find(".")!=string::npos) {
        safe_tname = safe_tname.substr(0,safe_tname.find("."));
    }
    for (int i=0; i<(int)safe_tname.length(); i++) {
        if (safe_tname[i]=='/') {
            safe_tname[i] = '_';
        }
    }
    string fname = safe_tname + ".h";
    if (target!="") {
        string iname = target + "/" + safe_tname + "_indexALL.txt";
        FILE *index = fopen(iname.c_str(),"w");
        if (index!=NULL) {
            fprintf(index,"%s\n",fname.c_str());
            fclose(index);
            index = NULL;
        }
        fname = target + "/" + fname;
    }
    out = fopen(fname.c_str(),"w");
    if (!out) {
        fprintf(stderr,"Failed to open %s for writing\n", fname.c_str());
    }
    printf("Generating %s\n", fname.c_str());
    fprintf(out,"// This is an automatically generated file.\n");
    fprintf(out,"// Generated from this %s.msg definition:\n", safe_tname.c_str());
    fprintf(out,"%s", state.txt.c_str());
    fprintf(out,"// Instances of this class can be read and written with YARP ports,\n");
    fprintf(out,"// using a ROS-compatible format.\n");
    fprintf(out,"\n");
    fprintf(out,"#ifndef YARPMSG_TYPE_%s\n", safe_tname.c_str());
    fprintf(out,"#define YARPMSG_TYPE_%s\n\n", safe_tname.c_str());
    fprintf(out,"#include <string>\n");
    fprintf(out,"#include <vector>\n");
    fprintf(out,"#include <yarp/os/Portable.h>\n");
    fprintf(out,"#include <yarp/os/ConstString.h>\n");
    fprintf(out,"#include <yarp/os/NetInt32.h>\n");
    fprintf(out,"#include <yarp/os/NetInt64.h>\n");
    fprintf(out,"#include <yarp/os/NetFloat32.h>\n");
    fprintf(out,"#include <yarp/os/NetFloat64.h>\n");
    for (int i=0; i<(int)state.dependencies.size(); i++) {
        fprintf(out,"#include <%s.h>\n",state.dependencies[i].c_str());
    }
    fprintf(out,"\n");
    fprintf(out,"class %s : public yarp::os::Portable {\n", safe_tname.c_str());
    fprintf(out,"public:\n");
    fprintf(out,"  yarp::os::ConstString getTypeName() const {\n");
    fprintf(out,"    return \"%s\";\n", safe_tname.c_str());
    fprintf(out,"  }\n\n");
    return true;
}

bool RosTypeCodeGenYarp::beginDeclare() {
    return true;
}           

bool RosTypeCodeGenYarp::declareField(const RosField& field) {
    RosYarpType t = mapPrimitive(field);
    if (!field.isArray) {
        fprintf(out,"  %s %s;\n", t.yarpType.c_str(), field.rosName.c_str());
    } else {
        fprintf(out,"  std::vector<%s> %s;\n", t.yarpType.c_str(), 
               field.rosName.c_str());
    }
    return true;
}

bool RosTypeCodeGenYarp::endDeclare() {
    fprintf(out,"\n");
    return true;
}

bool RosTypeCodeGenYarp::beginRead() {
    fprintf(out,"  bool read(yarp::os::ConnectionReader& connection) {\n");
    usedLen = false;
    usedLen2 = false;
    first = true;
    return true;
}           

// pending issues
// * translate primitive types
// * deal with strings, which are a variable-sized primitive

bool RosTypeCodeGenYarp::readField(const RosField& field) {
    RosYarpType t = mapPrimitive(field);
    if (!first) {
        fprintf(out,"\n");
    }
    first = false;
    fprintf(out,"    // *** %s ***\n", field.rosName.c_str());
    if (field.rosType=="string") {
        // strings are special; variable length primitive
        if (field.isArray) {
            fprintf(out,"    %s%s = connection.expectInt();\n",
                   usedLen?"":"int ",
                   len.c_str());
            usedLen = true;
            fprintf(out,"    %s.resize(%s);\n", 
                   field.rosName.c_str(),
                   len.c_str());
            fprintf(out,"    for (int %s=0; %s<%s; %s++) {\n", 
                   counter.c_str(),
                   counter.c_str(),
                   len.c_str(),
                   counter.c_str());


            fprintf(out,"      %s%s = connection.expectInt();\n",
                   usedLen2?"":"int ",
                   len2.c_str());
            usedLen2 = true;
            fprintf(out,"      %s.resize(%s);\n", 
                   field.rosName.c_str(),
                   len2.c_str());
            fprintf(out,"      if (!connection.expectBlock((char*)%s[%s].c_str(),%s)) return false;\n",
                   field.rosName.c_str(),
                   counter.c_str(),
                   len2.c_str());
            fprintf(out,"    }\n");                  
        } else {
            fprintf(out,"    %s%s = connection.expectInt();\n",
                   usedLen?"":"int ",
                   len.c_str());
            usedLen = true;
            fprintf(out,"    %s.resize(%s);\n", 
                   field.rosName.c_str(),
                   len.c_str());
            fprintf(out,"    if (!connection.expectBlock((char*)%s.c_str(),%s)) return false;\n",
                   field.rosName.c_str(),
                   len.c_str());
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            fprintf(out,"    %s%s = connection.expectInt();\n",
                   usedLen?"":"int ",
                   len.c_str());
            usedLen = true;
            fprintf(out,"    %s.resize(%s);\n", 
                   field.rosName.c_str(),
                   len.c_str());
            fprintf(out,"    if (!connection.expectBlock((char*)&%s[0],sizeof(%s)*%s)) return false;\n",
                   field.rosName.c_str(),
                   t.yarpType.c_str(),
                   len.c_str());
        } else {
            if (t.len!=0) {
                fprintf(out,"    if (!connection.expectBlock((char*)&%s,%d)) return false;\n",
                       field.rosName.c_str(),
                       t.len);
            } else {
                fprintf(out,"    %s = connection.%s();\n",
                       field.rosName.c_str(),
                       t.reader.c_str());
            }
        }
    } else {
        if (field.isArray) {
            fprintf(out,"    %s%s = connection.expectInt();\n",
                   usedLen?"":"int ",
                   len.c_str());
            usedLen = true;
            fprintf(out,"    %s.resize(%s);\n", 
                   field.rosName.c_str(),
                   len.c_str());
            fprintf(out,"    for (int %s=0; %s<%s; %s++) {\n", 
                   counter.c_str(),
                   counter.c_str(),
                   len.c_str(),
                   counter.c_str());
            fprintf(out,"      if (!%s[%s].read(connection)) return false;\n",
                   field.rosName.c_str(),
                   counter.c_str());
            fprintf(out,"    }\n");                  
        } else {
            fprintf(out,"    if (!%s.read(connection)) return false;\n",
                   field.rosName.c_str());
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endRead() {
    fprintf(out,"    return !connection.isError();\n");
    fprintf(out,"  }\n\n");
    return true;
}

bool RosTypeCodeGenYarp::beginWrite() {
    fprintf(out,"  bool write(yarp::os::ConnectionWriter& connection) {\n");
    usedLen = false;
    usedLen2 = false;
    first = true;
    return true;
}

bool RosTypeCodeGenYarp::writeField(const RosField& field) {
    RosYarpType t = mapPrimitive(field);
    if (!first) {
        fprintf(out,"\n");
    }
    first = false;
    fprintf(out,"    // *** %s ***\n", field.rosName.c_str());
    if (field.rosType=="string") {
        // strings are special; variable length primitive
        if (field.isArray) {
            fprintf(out,"    connection.appendInt(%s.size());\n",
                   field.rosName.c_str());
            fprintf(out,"    for (int %s=0; %s<%s.size(); %s++) {\n", 
                   counter.c_str(),
                   counter.c_str(),
                   field.rosName.c_str(),
                   counter.c_str());
            fprintf(out,"      connection.appendInt(%s[%s].length());\n",
                   field.rosName.c_str(),
                   counter.c_str());
            fprintf(out,"      connection.appendExternalBlock((char*)%s[%s].c_str(),%s[%s].length());\n",
                    field.rosName.c_str(),
                    counter.c_str(),
                    field.rosName.c_str(),
                    counter.c_str());
            fprintf(out,"    }\n");                  
        } else {
            fprintf(out,"    connection.appendInt(%s.length());\n",
                   field.rosName.c_str());
            fprintf(out,"    connection.appendExternalBlock((char*)%s.c_str(),%s.length());\n",
                   field.rosName.c_str(),
                   field.rosName.c_str());
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            fprintf(out,"    connection.appendInt(%s.size());\n",
                   field.rosName.c_str());
            fprintf(out,"    connection.appendExternalBlock((char*)&%s[0],sizeof(%s)*%s.size());\n",
                   field.rosName.c_str(),
                   t.yarpType.c_str(),
                   field.rosName.c_str());
        } else {
            if (t.len!=0) {
                fprintf(out,"    connection.appendBlock((char*)&%s,%d);\n",
                       field.rosName.c_str(),
                       t.len);
            } else {
                fprintf(out,"    connection.%s(%s);\n",
                       t.writer.c_str(),
                       field.rosName.c_str());
            }
        }
    } else {
        if (field.isArray) {
            fprintf(out,"    connection.appendInt(%s.size());\n",
                   field.rosName.c_str());
            fprintf(out,"    for (int %s=0; %s<%s.size(); %s++) {\n", 
                   counter.c_str(),
                   counter.c_str(),
                   field.rosName.c_str(),
                   counter.c_str());
            fprintf(out,"      if (!%s[%s].write(connection)) return false;\n",
                   field.rosName.c_str(),
                   counter.c_str());
            fprintf(out,"    }\n");                  
        } else {
            fprintf(out,"    if (!%s.write(connection)) return false;\n",
                   field.rosName.c_str());
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endWrite() {
    fprintf(out,"    return !connection.isError();\n");
    fprintf(out,"  }\n");
    return true;
}


bool RosTypeCodeGenYarp::endType() {
    fprintf(out,"};\n\n");
    fprintf(out,"#endif\n");
    fclose(out);
    out = NULL;
    return true;
}


RosYarpType RosTypeCodeGenYarp::mapPrimitive(const RosField& field) {
    RosYarpType ry;
    ry.rosType = field.rosType;
    ry.yarpType = field.rosType;
    if (field.rosType=="string") {
        ry.yarpType = "std::string";
    }
    if (!field.isPrimitive) {
        return ry;
    }
    string name = field.rosType;
    if (name=="int8"||name=="uint8"||(name=="bool"&&field.isArray)) {
        ry.yarpType = "char";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        ry.len = 1;
    } else if (name=="bool") {
        ry.yarpType = "bool";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        ry.len = 1;
    } else if (name=="int16"||name=="uint16") {
        ry.yarpType = "yarp::os::NetInt16";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        ry.len = 2;
    } else if (name=="int32"||name=="uint32") {
        ry.yarpType = "yarp::os::NetInt32";
        ry.writer = "appendInt";
        ry.reader = "expectInt";
    } else if (name=="int64"||name=="uint64") {
        ry.yarpType = "yarp::os::NetInt64";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        ry.len = 8;
    } else if (name=="float32") {
        ry.yarpType = "yarp::os::NetFloat32";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        ry.len = 4;
    } else if (name=="float64") {
        ry.yarpType = "yarp::os::NetFloat64";
        ry.writer = "appendDouble";
        ry.reader = "expectDouble";
    } else if (name=="string") {
        // ignore
    } else {
        fprintf(stderr, "Please translate %s in RosTypeCodeGenYarp.cpp\n",
                name.c_str());
        exit(1);
    }
    return ry;
}


