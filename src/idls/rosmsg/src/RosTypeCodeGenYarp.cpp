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

#include <yarp/os/Os.h>
#include <iostream>
using namespace std;

static std::string getPackageName(const std::string& name) {

    string tname = name;
    string pname = "";
    if (name == "Header") {
        tname = "std_msgs";
    } else {
        size_t at = tname.rfind("/");
        if (at==string::npos) {
            tname = "";
        } else {
            tname = pname = tname.substr(0,at);
            do {
                at = pname.rfind("/");
                if (at == string::npos) break;
                tname = pname.substr(at+1,pname.length());
                pname = pname.substr(0,at);
            } while (tname=="srv"||tname=="msg");
        }
        if (tname == ".") {
            tname = "";
        }
    }
    return tname;
}

static std::string getPartName(const std::string& tname) {
    string part_tname = tname;
    size_t at = tname.rfind("/");
    if (at!=string::npos) {
        part_tname = tname.substr(at+1,tname.length());
    }
    if (part_tname.find(".")!=string::npos) {
        part_tname = part_tname.substr(0,part_tname.rfind("."));
    }
    return part_tname;
}

static std::string getSafeName(const std::string& tname) {
    string pack = getPackageName(tname);
    string part = getPartName(tname);
    string safe_tname;
    if (pack!="") {
        safe_tname = pack + "_" + part;
    } else {
        safe_tname = part;
    }
    return safe_tname;
}

static std::string getDoubleName(const std::string& tname) {
    string package_name = getPackageName(tname);
    string part_name = getPartName(tname);
    if (package_name!="") {
        part_name = package_name + "/" + part_name;
    }
    return part_name;
}

bool RosTypeCodeGenYarp::beginType(const std::string& tname,
                                   RosTypeCodeGenState& state) {
    counter = state.getFreeVariable("i");
    len = state.getFreeVariable("len");
    len2 = state.getFreeVariable("len2");
    string safe_tname = getSafeName(tname);
    className = safe_tname;
    string fname = safe_tname + ".h";
    string pack = getPackageName(tname);
    string part = getPartName(tname);
    string root = "";
    if (target!="") {
        root = target + "/";
    }
    if (target!="") {
        string iname = target + "/" + getPartName(tname) + "_indexALL.txt";
        yarp::os::mkdir_p(iname.c_str(),1);
        FILE *index = fopen(iname.c_str(),"w");
        if (index!=NULL) {
            fprintf(index,"%s\n",fname.c_str());
            if (pack!="") {
                fprintf(index,"%s/%s.h\n", pack.c_str(), part.c_str());
            }
            for (int i=0; i<(int)state.dependencies.size(); i++) {
                fprintf(index,"%s.h\n",getSafeName(state.dependenciesAsPaths[i]).c_str());
                if(getPackageName(state.dependenciesAsPaths[i]) != "") {
                    fprintf(index,"%s.h\n", getDoubleName(state.dependenciesAsPaths[i]).c_str());
                }
            }
            fclose(index);
            index = NULL;
        }
        fname = root + fname;
    }

    yarp::os::mkdir_p(fname.c_str(),1);

    if (pack!=""&&target!="") {
        // Make header file names more sensible
        string alt_fname =  root + pack + "/" + part + ".h";
        yarp::os::mkdir_p(alt_fname.c_str(),1);
        out = fopen(alt_fname.c_str(),"w");
        if (!out) {
            fprintf(stderr,"Failed to open %s for writing\n",
                    alt_fname.c_str());
            exit(1);
        }
        if (verbose) {
            printf("Generating %s\n", alt_fname.c_str());
        }
        fprintf(out,"// This is an automatically generated file.\n");
        fprintf(out,"#ifndef YARPMSG_TYPE_wrap_%s\n", safe_tname.c_str());
        fprintf(out,"#define YARPMSG_TYPE_wrap_%s\n\n", safe_tname.c_str());
        fprintf(out,"#include <%s.h>\n\n",getSafeName(tname).c_str());
        fprintf(out,"namespace %s {\n", pack.c_str());
        fprintf(out,"  typedef %s %s;\n", safe_tname.c_str(), part.c_str());
        fprintf(out,"}\n\n");
        fprintf(out,"#endif\n\n");
        fclose(out);
    }

    out = fopen(fname.c_str(),"w");
    if (!out) {
        fprintf(stderr,"Failed to open %s for writing\n", fname.c_str());
        exit(1);
    }
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
    fprintf(out,"#include <yarp/os/Wire.h>\n");
    fprintf(out,"#include <yarp/os/idl/WireTypes.h>\n");
    for (int i=0; i<(int)state.dependencies.size(); i++) {
        fprintf(out,"#include \"%s.h\"\n",getSafeName(state.dependenciesAsPaths[i]).c_str());
    }
    fprintf(out,"\n");
    fprintf(out,"class %s : public yarp::os::idl::WirePortable {\n", safe_tname.c_str());
    fprintf(out,"public:\n");
    return true;
}

bool RosTypeCodeGenYarp::beginDeclare() {
    return true;
}

bool RosTypeCodeGenYarp::declareField(const RosField& field) {
    RosYarpType t = mapPrimitive(field);
    if (!field.isArray) {
        if (field.isConst()) {
            fprintf(out,"  static const %s %s = %s;\n", t.yarpType.c_str(), field.rosName.c_str(), field.initializer.c_str());
        } else {
            fprintf(out,"  %s %s;\n", t.yarpType.c_str(), field.rosName.c_str());
        }
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

bool RosTypeCodeGenYarp::beginConstruct() {
    fprintf(out,"  %s() {\n", className.c_str());
    return true;
}

bool RosTypeCodeGenYarp::constructField(const RosField& field) {
    if (field.isArray && field.arrayLength!=-1) {
        RosYarpType t = mapPrimitive(field);
        if (t.yarpDefaultValue!="") {
            fprintf(out,"    %s.resize(%d,%s);\n",
                    field.rosName.c_str(), field.arrayLength,
                    t.yarpDefaultValue.c_str());
        } else {
            fprintf(out,"    %s.resize(%d);\n",
                    field.rosName.c_str(), field.arrayLength);
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endConstruct() {
    fprintf(out,"  }\n\n");
    return true;
}

bool RosTypeCodeGenYarp::beginRead(bool bare, int len) {
    fprintf(out,"  bool read%s(yarp::os::ConnectionReader& connection) {\n",
            bare?"Bare":"Bottle");
    if (!bare) {
        fprintf(out,"    connection.convertTextMode();\n");
        fprintf(out,"    yarp::os::idl::WireReader reader(connection);\n");
        fprintf(out,"    if (!reader.readListHeader(%d)) return false;\n\n",
                len);
    }
    usedLen = false;
    usedLen2 = false;
    first = true;
    return true;
}

// pending issues
// * translate primitive types
// * deal with strings, which are a variable-sized primitive

bool RosTypeCodeGenYarp::readField(bool bare, const RosField& field) {
    if (field.isConst()) return true;
    RosYarpType t = mapPrimitive(field);
    if (!first) {
        fprintf(out,"\n");
    }
    first = false;
    fprintf(out,"    // *** %s ***\n", field.rosName.c_str());
    if (field.rosType=="string") {
        // strings are special; variable length primitive
        if (field.isArray) {
            if (!bare) {
                fprintf(out,"    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) return false;\n");
            }
            if (field.arrayLength==-1||!bare) {
                fprintf(out,"    %s%s = connection.expectInt();\n",
                        usedLen?"":"int ",
                        len.c_str());
            } else {
                fprintf(out,"    %s%s = %d;\n",
                        usedLen?"":"int ",len.c_str(),field.arrayLength);
            }
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
                   "int ",
                   len2.c_str());
            fprintf(out,"      %s[i].resize(%s);\n",
                   field.rosName.c_str(),
                   len2.c_str());
            fprintf(out,"      if (!connection.expectBlock((char*)%s[%s].c_str(),%s)) return false;\n",
                   field.rosName.c_str(),
                   counter.c_str(),
                   len2.c_str());
            fprintf(out,"    }\n");
        } else {
            if (!bare) {
                fprintf(out,"    if (!reader.readString(%s)) return false;\n",
                        field.rosName.c_str());
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
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            if (!bare) {
                fprintf(out,"    if (connection.expectInt()!=(BOTTLE_TAG_LIST|%s)) return false;\n",
                        t.yarpTag.c_str());
            }
            if (field.arrayLength==-1||!bare) {
                fprintf(out,"    %s%s = connection.expectInt();\n",
                        usedLen?"":"int ",
                        len.c_str());
            } else {
                fprintf(out,"    %s%s = %d;\n",
                        usedLen?"":"int ",len.c_str(),field.arrayLength);
            }
            usedLen = true;
            fprintf(out,"    %s.resize(%s);\n",
                   field.rosName.c_str(),
                   len.c_str());
            if (!bare) {
                fprintf(out,"    for (int i=0; i<%s; i++) {\n", len.c_str());
                fprintf(out,"      %s[i] = (%s)connection.%s();\n",
                        field.rosName.c_str(),
                        t.yarpType.c_str(),
                        t.yarpReader.c_str());
                fprintf(out,"    }\n");
            } else {
                fprintf(out,"    if (!connection.expectBlock((char*)&%s[0],sizeof(%s)*%s)) return false;\n",
                        field.rosName.c_str(),
                        t.yarpType.c_str(),
                        len.c_str());
            }
        } else {
            if (!bare) {
                fprintf(out,"    %s = reader.%s();\n",
                        field.rosName.c_str(),
                        t.yarpWireReader.c_str());
            } else if (t.len!=0) {
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
            if (!bare) {
                fprintf(out,"    if (connection.expectInt()!=BOTTLE_TAG_LIST) return false;\n");
            }
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

bool RosTypeCodeGenYarp::endRead(bool bare) {
    fprintf(out,"    return !connection.isError();\n");
    fprintf(out,"  }\n\n");
    if (!bare) {
        fprintf(out,"  using yarp::os::idl::WirePortable::read;\n");
        fprintf(out,"  bool read(yarp::os::ConnectionReader& connection) {\n");
        fprintf(out,"    if (connection.isBareMode()) return readBare(connection);\n");
        fprintf(out,"    return readBottle(connection);\n");
        fprintf(out,"  }\n\n");
    }
    return true;
}

bool RosTypeCodeGenYarp::beginWrite(bool bare, int len) {
    fprintf(out,"  bool write%s(yarp::os::ConnectionWriter& connection) {\n",
            bare?"Bare":"Bottle");
    if (!bare) {
        fprintf(out,"    connection.appendInt(BOTTLE_TAG_LIST);\n");
        fprintf(out,"    connection.appendInt(%d);\n\n", len);
    }
    usedLen = false;
    usedLen2 = false;
    first = true;
    return true;
}

bool RosTypeCodeGenYarp::writeField(bool bare, const RosField& field) {
    if (field.isConst()) return true;
    RosYarpType t = mapPrimitive(field);
    if (!first) {
        fprintf(out,"\n");
    }
    first = false;
    fprintf(out,"    // *** %s ***\n", field.rosName.c_str());
    if (field.rosType=="string") {
        // strings are special; variable length primitive
        if (field.isArray) {
            if (!bare) {
                fprintf(out,"    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);\n");
            }
            if (field.arrayLength==-1 || !bare) {
                fprintf(out,"    connection.appendInt(%s.size());\n",
                        field.rosName.c_str());
            }
            fprintf(out,"    for (size_t %s=0; %s<%s.size(); %s++) {\n",
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
            if (!bare) {
                fprintf(out,"    connection.appendInt(BOTTLE_TAG_STRING);\n");
            }
            fprintf(out,"    connection.appendInt(%s.length());\n",
                    field.rosName.c_str());
            fprintf(out,"    connection.appendExternalBlock((char*)%s.c_str(),%s.length());\n",
                    field.rosName.c_str(),
                    field.rosName.c_str());
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            if (!bare) {
                fprintf(out,"    connection.appendInt(BOTTLE_TAG_LIST|%s);\n",
                        t.yarpTag.c_str());
                fprintf(out,"    connection.appendInt(%s.size());\n",
                        field.rosName.c_str());
                fprintf(out,"    for (size_t i=0; i<%s.size(); i++) {\n",
                        field.rosName.c_str());
                fprintf(out,"      connection.%s(%s%s[i]);\n",
                        t.yarpWriter.c_str(),
                        t.yarpWriterCast.c_str(),
                        field.rosName.c_str());
                fprintf(out,"    }\n");
            } else {
                if (field.arrayLength==-1) {
                    fprintf(out,"    connection.appendInt(%s.size());\n",
                            field.rosName.c_str());
                }
                fprintf(out,"    connection.appendExternalBlock((char*)&%s[0],sizeof(%s)*%s.size());\n",
                        field.rosName.c_str(),
                        t.yarpType.c_str(),
                        field.rosName.c_str());
            }
        } else {
            if (!bare) {
                fprintf(out,"    connection.appendInt(%s);\n",
                        t.yarpTag.c_str());
                fprintf(out,"    connection.%s(%s%s);\n",
                        t.yarpWriter.c_str(),
                        t.yarpWriterCast.c_str(),
                        field.rosName.c_str());
            } else if (t.len!=0) {
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
            if (!bare) {
                fprintf(out,"    connection.appendInt(BOTTLE_TAG_LIST);\n");
            }
            if (field.arrayLength==-1 || !bare) {
                fprintf(out,"    connection.appendInt(%s.size());\n",
                        field.rosName.c_str());
            }
            fprintf(out,"    for (size_t %s=0; %s<%s.size(); %s++) {\n",
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

bool RosTypeCodeGenYarp::endWrite(bool bare) {
    if (!bare) {
        fprintf(out,"    connection.convertTextMode();\n");
    }
    fprintf(out,"    return !connection.isError();\n");
    fprintf(out,"  }\n\n");
    if (!bare) {
        fprintf(out,"  using yarp::os::idl::WirePortable::write;\n");
        fprintf(out,"  bool write(yarp::os::ConnectionWriter& connection) {\n");
        fprintf(out,"    if (connection.isBareMode()) return writeBare(connection);\n");
        fprintf(out,"    return writeBottle(connection);\n");
        fprintf(out,"  }\n\n");
    }
    return true;
}


static void output_type(FILE *out,
                        const RosField& field,
                        std::map<std::string,int>& processed) {
    const std::string& name = field.rosType;
    processed[name] = 1;
    const std::string& source = field.source;
    bool need_newline = false;
    for (size_t i=0; i<source.length(); i++) {
        char ch = source[i];
        if (ch=='\r') continue;
        if (need_newline) {
            fprintf(out,"\\n");
            fprintf(out,"\\\n");
            need_newline = false;
        }
        if (ch=='\n') {
            need_newline = true;
            continue;
        }
        if (ch=='\\') {
            fprintf(out,"\\\\");
            continue;
        }
        if (ch=='\"') {
            fprintf(out,"\\\"");
            continue;
        }
        fprintf(out,"%c",ch);
    }
    size_t len = field.subRosType.size();
    for (size_t i=0; i<len; i++) {
        const RosField& sub_field = field.subRosType[i];
        if (!sub_field.isStruct) continue;
        const std::string& sub_name = sub_field.rosType;
        if (processed.find(sub_name)!=processed.end()) continue;
        fprintf(out,"\\n================================================================================\\n\\\n");
        fprintf(out,"MSG: %s\\n\\\n", (sub_name=="Header"?"std_msgs/Header":sub_name.c_str()));
        output_type(out,sub_field,processed);
    }
}

bool RosTypeCodeGenYarp::endType(const std::string& tname,
                                 const RosField& field) {
    string safe_tname = getSafeName(tname);
    string dbl_name = getDoubleName(tname);
    fprintf(out,"  // This class will serialize ROS style or YARP style depending on protocol.\n");
    fprintf(out,"  // If you need to force a serialization style, use one of these classes:\n");
    fprintf(out,"  typedef yarp::os::idl::BareStyle<%s> rosStyle;\n", safe_tname.c_str());
    fprintf(out,"  typedef yarp::os::idl::BottleStyle<%s> bottleStyle;\n\n", safe_tname.c_str());

    fprintf(out,"  // Give source text for class, ROS will need this\n");
    fprintf(out,"  yarp::os::ConstString getTypeText() {\n");
    fprintf(out,"    return \"");
    std::map<std::string,int> processed;
    output_type(out,field,processed);
    fprintf(out,"\";\n");
    fprintf(out,"  }\n");
    fprintf(out,"\n");

    fprintf(out,"  // Name the class, ROS will need this\n");
    fprintf(out,"  yarp::os::Type getType() {\n");
    fprintf(out,"    yarp::os::Type typ = yarp::os::Type::byName(\"%s\",\"%s\");\n", dbl_name.c_str(), dbl_name.c_str());
    fprintf(out,"    typ.addProperty(\"md5sum\",yarp::os::Value(\"%s\"));\n", field.checksum.c_str());
    fprintf(out,"    typ.addProperty(\"message_definition\",yarp::os::Value(getTypeText()));\n");
    fprintf(out,"    return typ;\n");
    fprintf(out,"  }\n");
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
    for (int i=0; i<(int)ry.yarpType.length(); i++) {
        if (ry.yarpType[i]=='/') {
            ry.yarpType[i] = '_';
        }
    }
    if (field.rosType=="string") {
        ry.yarpType = "std::string";
    }
    if (!field.isPrimitive) {
        return ry;
    }
    string name = field.rosType;
    string flavor = "";
    if (name=="int8"||(name=="bool"&&field.isArray)||name=="char") {
        ry.yarpType = "char";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 1;
    } else if (name=="uint8"||name=="byte") {
        ry.yarpType = "unsigned char";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 1;
    } else if (name=="bool") {
        ry.yarpType = "bool";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.yarpDefaultValue = "false";
        ry.len = 1;
    } else if (name=="int16") {
        ry.yarpType = "yarp::os::NetInt16";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 2;
    } else if (name=="uint16") {
        ry.yarpType = "yarp::os::NetUint16";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 2;
    } else if (name=="int32") {
        ry.yarpType = "yarp::os::NetInt32";
        ry.writer = "appendInt";
        ry.reader = "expectInt";
        flavor = "int";
    } else if (name=="uint32") {
        ry.yarpType = "yarp::os::NetUint32";
        ry.writer = "appendInt";
        ry.reader = "expectInt";
        flavor = "int";
    } else if (name=="int64") {
        ry.yarpType = "yarp::os::NetInt64";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 8;
    } else if (name=="uint64") {
        ry.yarpType = "yarp::os::NetUint64";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 8;
    } else if (name=="float32") {
        ry.yarpType = "yarp::os::NetFloat32";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "double";
        ry.len = 4;
    } else if (name=="float64") {
        ry.yarpType = "yarp::os::NetFloat64";
        ry.writer = "appendDouble";
        ry.reader = "expectDouble";
        flavor = "double";
    } else if (name=="string") {
        // ignore
    } else {
        fprintf(stderr, "Please translate %s in RosTypeCodeGenYarp.cpp\n",
                name.c_str());
        exit(1);
    }
    if (flavor=="int") {
        ry.yarpWriter = "appendInt";
        ry.yarpReader = "expectInt";
        ry.yarpWriterCast = "(int)";
        ry.yarpTag = "BOTTLE_TAG_INT";
        ry.yarpWireReader = "expectInt";
        if (ry.yarpDefaultValue=="") {
            ry.yarpDefaultValue = "0";
        }
    } else if (flavor=="double") {
        ry.yarpWriter = "appendDouble";
        ry.yarpReader = "expectDouble";
        ry.yarpWriterCast = "(double)";
        ry.yarpTag = "BOTTLE_TAG_DOUBLE";
        ry.yarpWireReader = "expectDouble";
        if (ry.yarpDefaultValue=="") {
            ry.yarpDefaultValue = "0.0";
        }
    }
    return ry;
}


