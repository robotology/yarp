/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <RosTypeCodeGenYarp.h>

#include <cstdio>
#include <cstdlib>

#include <yarp/os/Os.h>
#include <iostream>
using namespace std;

static std::string getPackageName(const std::string& name)
{

    string tname = name;
    string pname = "";
    if (name == "Header") {
        tname = "std_msgs";
    } else {
        size_t at = tname.rfind("/");
        if (at == string::npos) {
            tname = "";
        } else {
            tname = pname = tname.substr(0, at);
            do {
                at = pname.rfind("/");
                if (at == string::npos) {
                    tname = pname;
                    break;
                }
                tname = pname.substr(at+1, pname.length());
                pname = pname.substr(0, at);
            } while (tname == "srv" || tname == "msg");
        }
        if (tname == ".") {
            tname = "";
        }
    }
    return tname;
}

static std::string getPartName(const std::string& tname)
{
    string part_tname = tname;
    size_t at = tname.rfind("/");
    if (at != string::npos) {
        part_tname = tname.substr(at+1, tname.length());
    }
    if (part_tname.find(".") != string::npos) {
        part_tname = part_tname.substr(0, part_tname.rfind("."));
    }
    return part_tname;
}

static std::string getSafeName(const std::string& tname)
{
    string pack = getPackageName(tname);
    string part = getPartName(tname);
    string safe_tname;
    if (pack != "") {
        safe_tname = pack + "_" + part;
    } else {
        safe_tname = part;
    }
    return safe_tname;
}

static std::string getDoubleName(const std::string& tname, const std::string& separator = "/")
{
    string package_name = getPackageName(tname);
    string part_name = getPartName(tname);
    if (package_name != "") {
        part_name = package_name + separator + part_name;
    }
    return part_name;
}

bool RosTypeCodeGenYarp::beginType(const std::string& tname,
                                   RosTypeCodeGenState& state)
{
    counter = state.getFreeVariable("i");
    len = state.getFreeVariable("len");
    len2 = state.getFreeVariable("len2");

    string safename = getSafeName(tname);
    string doublename = getDoubleName(tname);
    string hdr_name = "yarp/rosmsg/" + doublename + ".h";
    string fullClassName = "yarp::rosmsg::" + getDoubleName(tname, "::");
    packageName = getPackageName(tname);
    className = getPartName(tname);
    string root = (target != "") ? (target + "/") : "";

#ifndef YARP_NO_DEPRECATED // since YARP 3.0.0
    string deprecated_fname1 = safename + ".h";
    string deprecated_fname2 = doublename + ".h";

    // package_class.h
    yarp::os::mkdir_p((root + deprecated_fname1).c_str(), 1);
    out = fopen((root + deprecated_fname1).c_str(), "w");
    if (!out) {
        fprintf(stderr, "Failed to open %s for writing\n", deprecated_fname1.c_str());
        std::exit(1);
    }
    state.generatedFiles.push_back(deprecated_fname1);
    if (verbose) {
        printf("Generating %s\n", deprecated_fname1.c_str());
    }

    fprintf(out, "// This is an automatically generated file.\n\n");
    fprintf(out, "#ifndef YARP_ROSMSG_deprecated1_%s_h\n", safename.c_str());
    fprintf(out, "#define YARP_ROSMSG_deprecated1_%s_h\n\n", safename.c_str());
    fprintf(out, "#include <yarp/conf/system.h>\n\n");
    fprintf(out, "YARP_COMPILER_DEPRECATED_WARNING(<%s> header is deprecated. Use <%s> instead)\n\n", deprecated_fname1.c_str(), hdr_name.c_str());
    fprintf(out, "#include <%s>\n", hdr_name.c_str());
    for (const auto& dep : state.dependenciesAsPaths[tname]) {
        fprintf(out, "#include <%s.h>\n", getSafeName(dep).c_str());
    }
    fprintf(out, "#include <yarp/conf/api.h>\n\n");
    fprintf(out, "YARP_DEPRECATED_MSG(\"%s is deprecated, use %s instead\")\n", safename.c_str(), fullClassName.c_str());
    fprintf(out, "typedef %s %s;\n", fullClassName.c_str(), safename.c_str());
    fprintf(out, "\n");
    fprintf(out, "#endif // YARP_ROSMSG_deprecated1_%s_h\n", safename.c_str());
    fclose(out);
    out = nullptr;

    // package/class.h
    if (packageName != "" && target != "") {
        yarp::os::mkdir_p((root + deprecated_fname2).c_str(), 1);
        out = fopen((root + deprecated_fname2).c_str(), "w");
        if (!out) {
            fprintf(stderr, "Failed to open %s for writing\n", deprecated_fname1.c_str());
            std::exit(1);
        }
        state.generatedFiles.push_back(deprecated_fname2);
        if (verbose) {
            printf("Generating %s\n", deprecated_fname2.c_str());
        }

        fprintf(out, "// This is an automatically generated file.\n\n");
        fprintf(out, "#ifndef YARP_ROSMSG_deprecated2_%s_h\n", safename.c_str());
        fprintf(out, "#define YARP_ROSMSG_deprecated2_%s_h\n\n", safename.c_str());
        fprintf(out, "#include <yarp/conf/system.h>\n\n");
        fprintf(out, "YARP_COMPILER_DEPRECATED_WARNING(<%s> header is deprecated. Use <%s> instead)\n\n", deprecated_fname2.c_str(), hdr_name.c_str());
        fprintf(out, "#include <%s>\n", hdr_name.c_str());
        fprintf(out, "#include <%s>\n", deprecated_fname1.c_str());
        fprintf(out, "#include <yarp/conf/api.h>\n\n");
        fprintf(out, "namespace %s {\n\n", packageName.c_str());
        fprintf(out, "YARP_DEPRECATED typedef %s %s;\n\n", fullClassName.c_str(), className.c_str());
        fprintf(out, "} // namespace %s\n\n", packageName.c_str());
        fprintf(out, "#endif // YARP_ROSMSG_deprecated2_%s_h\n", safename.c_str());
        fclose(out);
        out = nullptr;
    }
#endif // YARP_NO_DEPRECATED

    // yarp/rosmsg/package/class.h
    yarp::os::mkdir_p((root + hdr_name).c_str(), 1);
    out = fopen((root + hdr_name).c_str(), "w");
    if (!out) {
        fprintf(stderr, "Failed to open %s for writing\n", hdr_name.c_str());
        std::exit(1);
    }
    state.generatedFiles.push_back(hdr_name);
    if (verbose) {
        printf("Generating %s\n", hdr_name.c_str());
    }

    fprintf(out, "// This is an automatically generated file.\n\n");
    if (className == "TickTime" || className == "TickDuration") {
        fprintf(out, "// Generated from the following \"%s\" native type definition:\n", ((className == "TickTime") ? "time" : "duration"));
    } else {
        fprintf(out, "// Generated from the following \"%s\" msg definition:\n", doublename.c_str());
    }
    fprintf(out, "%s", state.txt.c_str());
    fprintf(out, "// Instances of this class can be read and written with YARP ports,\n");
    fprintf(out, "// using a ROS-compatible format.\n");
    fprintf(out, "\n");
    fprintf(out, "#ifndef YARP_ROSMSG_%s_h\n", safename.c_str());
    fprintf(out, "#define YARP_ROSMSG_%s_h\n\n", safename.c_str());
    fprintf(out, "#include <yarp/os/Wire.h>\n");
    fprintf(out, "#include <yarp/os/idl/WireTypes.h>\n");
    fprintf(out, "#include <string>\n");
    fprintf(out, "#include <vector>\n");
    if (className == "TickTime" || className == "TickDuration") {
        fprintf(out, "#include <climits>\n");
        fprintf(out, "#include <cstdint>\n");
    }
    for (auto dep : state.dependenciesAsPaths[tname]) {
        fprintf(out, "#include <yarp/rosmsg/%s.h>\n", getDoubleName(dep).c_str());
    }
    fprintf(out, "\n");
    fprintf(out, "namespace yarp {\n");
    fprintf(out, "namespace rosmsg {\n");
    if (!packageName.empty()) {
        fprintf(out, "namespace %s {\n", packageName.c_str());
    }
    fprintf(out, "\n");
    fprintf(out, "class %s : public yarp::os::idl::WirePortable\n", className.c_str());
    fprintf(out, "{\n");
    fprintf(out, "public:\n");

    return true;
}

bool RosTypeCodeGenYarp::beginDeclare()
{
    return true;
}

bool RosTypeCodeGenYarp::declareField(const RosField& field)
{
    RosYarpType t = mapPrimitive(field);
    if (!field.isArray) {
        if (field.isConst()) {
            fprintf(out, "    static const %s %s = %s;\n", t.yarpType.c_str(), field.rosName.c_str(), field.initializer.c_str());
        } else {
            fprintf(out, "    %s %s;\n", t.yarpType.c_str(), field.rosName.c_str());
        }
    } else {
        fprintf(out, "    std::vector<%s> %s;\n", t.yarpType.c_str(),
                field.rosName.c_str());
    }
    return true;
}

bool RosTypeCodeGenYarp::endDeclare()
{
    fprintf(out, "\n");
    return true;
}

bool RosTypeCodeGenYarp::beginConstruct()
{
    fprintf(out, "    %s()", className.c_str());
    return true;
}

bool RosTypeCodeGenYarp::initField(const RosField& field, bool &isFirstToInit)
{
    if (!field.isConst()) {
        if (isFirstToInit) {
            fprintf(out, " :\n");
            isFirstToInit = false;
        } else {
            fprintf(out, ",\n");
        }

        if (field.isArray || !field.isPrimitive) {
            fprintf(out, "            %s()", field.rosName.c_str());
        } else {
            RosYarpType t = mapPrimitive(field);
            fprintf(out, "            %s(%s)", field.rosName.c_str(), t.yarpDefaultValue.c_str());
        }
    }
    return true;
}


bool RosTypeCodeGenYarp::endInitConstruct()
{
   fprintf(out, "\n    {\n");
   return true;
}

bool RosTypeCodeGenYarp::constructField(const RosField& field)
{
    if (field.isArray && field.arrayLength != -1) {
        RosYarpType t = mapPrimitive(field);
        if (t.yarpDefaultValue != "") {
            fprintf(out, "        %s.resize(%d, %s);\n",
                    field.rosName.c_str(), field.arrayLength,
                    t.yarpDefaultValue.c_str());
        } else {
            fprintf(out, "        %s.resize(%d);\n",
                    field.rosName.c_str(), field.arrayLength);
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endConstruct()
{
    fprintf(out, "    }\n\n");

    // Add special constructor, assignment operator, and cast operator
    // operator to TickTime and TickDuration to support timestamps as a double
    // These function has been took from ROS source file
    // http://docs.ros.org/diamondback/api/rostime/html/time_8h_source.html#l00095
    // and modified a bit to cope with yarp time handling in double
    if (className == "TickTime" || className == "TickDuration") {
        // Constructor
        fprintf(out,
"    %s(double timestamp) :\n"
"            sec(0),\n"
"            nsec(0)\n"
"    {\n"
"        uint64_t time = (uint64_t) (timestamp * 1000000000UL);\n"
"        uint64_t sec_part = (time / 1000000000UL);\n"
"        uint64_t nsec_part = (time %% 1000000000UL);\n"
"        if (sec > UINT32_MAX) {\n"
"            yWarning(\"%s::%s(): Timestamp exceeded the 32 bit representation, resetting it to 0\");\n"
"            sec = 0;\n"
"        }\n"
"        sec = static_cast<yarp::os::NetUint32>(sec_part);\n"
"        nsec = static_cast<yarp::os::NetUint32>(nsec_part);\n"
"    }\n\n", className.c_str(), className.c_str(), className.c_str());

        // Assignment operator
        fprintf(out,
"    %s& operator=(const double timestamp)\n"
"    {\n"
"        uint64_t time = (uint64_t) (timestamp * 1000000000UL);\n"
"        uint64_t sec_part = (time / 1000000000UL);\n"
"        uint64_t nsec_part = (time %% 1000000000UL);\n"
"        if (sec > UINT32_MAX) {\n"
"            yWarning(\"%s::operator=(): Timestamp exceeded the 32 bit representation, resetting it to 0\");\n"
"            sec = 0;\n"
"        }\n"
"        sec = static_cast<yarp::os::NetUint32>(sec_part);\n"
"        nsec = static_cast<yarp::os::NetUint32>(nsec_part);\n"
"        return *this;\n"
"    }\n\n", className.c_str(), className.c_str());

        // Cast operator
        fprintf(out,
"    operator double()\n"
"    {\n"
"        if (nsec > 1000000000UL) {\n"
"            yWarning(\"%s::operator double(): Check on nsec > 1000000000UL failed\");\n"
"        }\n"
"        return sec + nsec * 1000000000.0;\n"
"    }\n\n", className.c_str());

    }

    return true;
}


bool RosTypeCodeGenYarp::beginClear()
{
    fprintf(out, "    void clear()\n");
    fprintf(out, "    {\n");
    first = true;
    return true;
}

bool RosTypeCodeGenYarp::clearField(const RosField& field)
{
    if (!first) {
        fprintf(out, "\n");
    }
    first = false;
    fprintf(out, "        // *** %s ***\n", field.rosName.c_str());

    if (!field.isConst()) {
        if (field.isArray || !field.isPrimitive) {
            fprintf(out, "        %s.clear();\n", field.rosName.c_str());
        } else {
            RosYarpType t = mapPrimitive(field);
            fprintf(out, "        %s = %s;\n", field.rosName.c_str(), t.yarpDefaultValue.c_str());
        }
    }
    constructField(field);
    return true;
}

bool RosTypeCodeGenYarp::endClear()
{
    fprintf(out, "    }\n\n");
    return true;
}



bool RosTypeCodeGenYarp::beginRead(bool bare, int len)
{
    fprintf(out, "    bool read%s(yarp::os::ConnectionReader& connection) override\n",
            bare?"Bare":"Bottle");
    fprintf(out, "    {\n");
    if (!bare) {
        fprintf(out, "        connection.convertTextMode();\n");
        fprintf(out, "        yarp::os::idl::WireReader reader(connection);\n");
        fprintf(out, "        if (!reader.readListHeader(%d)) {\n",
                len);
        fprintf(out, "            return false;\n");
        fprintf(out, "        }\n\n");
    }
    usedLen = false;
    usedLen2 = false;
    first = true;
    return true;
}

// pending issues
// * translate primitive types
// * deal with strings, which are a variable-sized primitive

bool RosTypeCodeGenYarp::readField(bool bare, const RosField& field)
{
    if (field.isConst()) return true;
    RosYarpType t = mapPrimitive(field);
    if (!first) {
        fprintf(out, "\n");
    }
    first = false;
    fprintf(out, "        // *** %s ***\n", field.rosName.c_str());
    if (field.rosType == "string") {
        // strings are special; variable length primitive
        if (field.isArray) {
            if (!bare) {
                fprintf(out, "        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) {\n");
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        %s%s = connection.expectInt();\n",
                        usedLen?"":"int ",
                        len.c_str());
            } else {
                fprintf(out, "        %s%s = %d;\n",
                        usedLen?"":"int ", len.c_str(), field.arrayLength);
            }
            usedLen = true;
            fprintf(out, "        %s.resize(%s);\n",
                    field.rosName.c_str(),
                    len.c_str());
            fprintf(out, "        for (int %s=0; %s<%s; %s++) {\n",
                    counter.c_str(),
                    counter.c_str(),
                    len.c_str(),
                    counter.c_str());
            fprintf(out, "            %s%s = connection.expectInt();\n",
                    "int ",
                    len2.c_str());
            fprintf(out, "            %s[i].resize(%s);\n",
                    field.rosName.c_str(),
                    len2.c_str());
            fprintf(out, "            if (!connection.expectBlock((char*)%s[%s].c_str(), %s)) {\n",
                    field.rosName.c_str(),
                    counter.c_str(),
                    len2.c_str());
            fprintf(out, "                return false;\n");
            fprintf(out, "            }\n");
            fprintf(out, "        }\n");
        } else {
            if (!bare) {
                fprintf(out, "        if (!reader.readString(%s)) {\n",
                        field.rosName.c_str());
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");

            } else {
                fprintf(out, "        %s%s = connection.expectInt();\n",
                        usedLen?"":"int ",
                        len.c_str());
                usedLen = true;
                fprintf(out, "        %s.resize(%s);\n",
                        field.rosName.c_str(),
                        len.c_str());
                fprintf(out, "        if (!connection.expectBlock((char*)%s.c_str(), %s)) {\n",
                        field.rosName.c_str(),
                        len.c_str());
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            if (!bare) {
                fprintf(out, "        if (connection.expectInt() != (BOTTLE_TAG_LIST|%s)) {\n",
                        t.yarpTag.c_str());
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        %s%s = connection.expectInt();\n",
                        usedLen?"":"int ",
                        len.c_str());
            } else {
                fprintf(out, "        %s%s = %d;\n",
                        usedLen?"":"int ", len.c_str(), field.arrayLength);
            }
            usedLen = true;
            fprintf(out, "        %s.resize(%s);\n",
                   field.rosName.c_str(),
                   len.c_str());
            if (!bare) {
                fprintf(out, "        for (int i=0; i<%s; i++) {\n", len.c_str());
                fprintf(out, "            %s[i] = (%s)connection.%s();\n",
                        field.rosName.c_str(),
                        t.yarpType.c_str(),
                        t.yarpReader.c_str());
                fprintf(out, "        }\n");
            } else {
                fprintf(out, "        if (len > 0 && !connection.expectBlock((char*)&%s[0], sizeof(%s)*%s)) {\n",
                        field.rosName.c_str(),
                        t.yarpType.c_str(),
                        len.c_str());
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
        } else {
            if (!bare) {
                fprintf(out, "        %s = reader.%s();\n",
                        field.rosName.c_str(),
                        t.yarpWireReader.c_str());
            } else if (t.len != 0) {
                fprintf(out, "        if (!connection.expectBlock((char*)&%s, %d)) {\n",
                        field.rosName.c_str(),
                        t.len);
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            } else {
                fprintf(out, "        %s = connection.%s();\n",
                        field.rosName.c_str(),
                        t.reader.c_str());
            }
        }
    } else {
        if (field.isArray) {
            if (!bare) {
                fprintf(out, "        if (connection.expectInt() != BOTTLE_TAG_LIST) {\n");
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
            fprintf(out, "        %s%s = connection.expectInt();\n",
                   usedLen?"":"int ",
                   len.c_str());
            usedLen = true;
            fprintf(out, "        %s.resize(%s);\n",
                    field.rosName.c_str(),
                    len.c_str());
            fprintf(out, "        for (int %s=0; %s<%s; %s++) {\n",
                    counter.c_str(),
                    counter.c_str(),
                    len.c_str(),
                    counter.c_str());
            fprintf(out, "            if (!%s[%s].read(connection)) {\n",
                    field.rosName.c_str(),
                    counter.c_str());
            fprintf(out, "                return false;\n");
            fprintf(out, "            }\n");
            fprintf(out, "        }\n");
        } else {
            fprintf(out, "        if (!%s.read(connection)) {\n",
                   field.rosName.c_str());
            fprintf(out, "            return false;\n");
            fprintf(out, "        }\n");
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endRead(bool bare)
{
    fprintf(out, "\n");
    fprintf(out, "        return !connection.isError();\n");
    fprintf(out, "    }\n\n");
    if (!bare) {
        fprintf(out, "    using yarp::os::idl::WirePortable::read;\n");
        fprintf(out, "    bool read(yarp::os::ConnectionReader& connection) override\n");
        fprintf(out, "    {\n");
        fprintf(out, "        return (connection.isBareMode() ? readBare(connection)\n");
        fprintf(out, "                                        : readBottle(connection));\n");
        fprintf(out, "    }\n\n");
    }
    return true;
}

bool RosTypeCodeGenYarp::beginWrite(bool bare, int len)
{
    fprintf(out, "    bool write%s(yarp::os::ConnectionWriter& connection) override\n",
            bare?"Bare":"Bottle");
    fprintf(out, "    {\n");
    if (!bare) {
        fprintf(out, "        connection.appendInt(BOTTLE_TAG_LIST);\n");
        fprintf(out, "        connection.appendInt(%d);\n\n", len);
    }
    usedLen = false;
    usedLen2 = false;
    first = true;
    return true;
}

bool RosTypeCodeGenYarp::writeField(bool bare, const RosField& field)
{
    if (field.isConst()) return true;
    RosYarpType t = mapPrimitive(field);
    if (!first) {
        fprintf(out, "\n");
    }
    first = false;
    fprintf(out, "        // *** %s ***\n", field.rosName.c_str());
    if (field.rosType == "string") {
        // strings are special; variable length primitive
        if (field.isArray) {
            if (!bare) {
                fprintf(out, "        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        connection.appendInt(%s.size());\n",
                        field.rosName.c_str());
            }
            fprintf(out, "        for (size_t %s=0; %s<%s.size(); %s++) {\n",
                    counter.c_str(),
                    counter.c_str(),
                    field.rosName.c_str(),
                    counter.c_str());
            fprintf(out, "            connection.appendInt(%s[%s].length());\n",
                    field.rosName.c_str(),
                    counter.c_str());
            fprintf(out, "            connection.appendExternalBlock((char*)%s[%s].c_str(), %s[%s].length());\n",
                    field.rosName.c_str(),
                    counter.c_str(),
                    field.rosName.c_str(),
                    counter.c_str());
            fprintf(out, "        }\n");
        } else {
            if (!bare) {
                fprintf(out, "        connection.appendInt(BOTTLE_TAG_STRING);\n");
            }
            fprintf(out, "        connection.appendInt(%s.length());\n",
                    field.rosName.c_str());
            fprintf(out, "        connection.appendExternalBlock((char*)%s.c_str(), %s.length());\n",
                    field.rosName.c_str(),
                    field.rosName.c_str());
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            if (!bare) {
                fprintf(out, "        connection.appendInt(BOTTLE_TAG_LIST|%s);\n",
                        t.yarpTag.c_str());
                fprintf(out, "        connection.appendInt(%s.size());\n",
                        field.rosName.c_str());
                fprintf(out, "        for (size_t i=0; i<%s.size(); i++) {\n",
                        field.rosName.c_str());
                fprintf(out, "            connection.%s(%s%s[i]);\n",
                        t.yarpWriter.c_str(),
                        t.yarpWriterCast.c_str(),
                        field.rosName.c_str());
                fprintf(out, "        }\n");
            } else {
                if (field.arrayLength == -1) {
                    fprintf(out, "        connection.appendInt(%s.size());\n",
                            field.rosName.c_str());
                }
                fprintf(out, "        if (%s.size()>0) {\n",
                        field.rosName.c_str());
                fprintf(out, "            connection.appendExternalBlock((char*)&%s[0], sizeof(%s)*%s.size());\n",
                        field.rosName.c_str(),
                        t.yarpType.c_str(),
                        field.rosName.c_str());
                fprintf(out, "        }\n");
            }
        } else {
            if (!bare) {
                fprintf(out, "        connection.appendInt(%s);\n",
                        t.yarpTag.c_str());
                fprintf(out, "        connection.%s(%s%s);\n",
                        t.yarpWriter.c_str(),
                        t.yarpWriterCast.c_str(),
                        field.rosName.c_str());
            } else if (t.len != 0) {
                fprintf(out, "        connection.appendBlock((char*)&%s, %d);\n",
                       field.rosName.c_str(),
                       t.len);
            } else {
                fprintf(out, "        connection.%s(%s);\n",
                       t.writer.c_str(),
                       field.rosName.c_str());
            }
        }
    } else {
        if (field.isArray) {
            if (!bare) {
                fprintf(out, "        connection.appendInt(BOTTLE_TAG_LIST);\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        connection.appendInt(%s.size());\n",
                        field.rosName.c_str());
            }
            fprintf(out, "        for (size_t %s=0; %s<%s.size(); %s++) {\n",
                   counter.c_str(),
                   counter.c_str(),
                   field.rosName.c_str(),
                   counter.c_str());
            fprintf(out, "            if (!%s[%s].write(connection)) {\n",
                   field.rosName.c_str(),
                   counter.c_str());
            fprintf(out, "                return false;\n");
            fprintf(out, "            }\n");
            fprintf(out, "        }\n");
        } else {
            fprintf(out, "        if (!%s.write(connection)) {\n",
                   field.rosName.c_str());
            fprintf(out, "            return false;\n");
            fprintf(out, "        }\n");
        }
    }
    return true;
}

bool RosTypeCodeGenYarp::endWrite(bool bare)
{
    fprintf(out, "\n");
    if (!bare) {
        fprintf(out, "        connection.convertTextMode();\n");
    }
    fprintf(out, "        return !connection.isError();\n");
    fprintf(out, "    }\n\n");
    if (!bare) {
        fprintf(out, "    using yarp::os::idl::WirePortable::write;\n");
        fprintf(out, "    bool write(yarp::os::ConnectionWriter& connection) override\n");
        fprintf(out, "    {\n");
        fprintf(out, "        return (connection.isBareMode() ? writeBare(connection)\n");
        fprintf(out, "                                        : writeBottle(connection));\n");
        fprintf(out, "    }\n\n");
    }
    return true;
}


static void output_type(FILE *out,
                        const RosField& field,
                        std::map<std::string, int>& processed)
{
    const std::string& name = field.rosType;
    processed[name] = 1;
    const std::string& source = field.source;
    bool need_newline = false;
    for (size_t i=0; i<source.length(); i++) {
        char ch = source[i];
        if (ch == '\r') continue;
        if (need_newline) {
            fprintf(out, "\\n");
            fprintf(out, "\\\n");
            need_newline = false;
        }
        if (ch == '\n') {
            need_newline = true;
            continue;
        }
        if (ch == '\\') {
            fprintf(out, "\\\\");
            continue;
        }
        if (ch == '\"') {
            fprintf(out, "\\\"");
            continue;
        }
        fprintf(out, "%c", ch);
    }
    size_t len = field.subRosType.size();
    for (size_t i=0; i<len; i++) {
        const RosField& sub_field = field.subRosType[i];
        if (!sub_field.isStruct) {
            continue;
        }
        const std::string& sub_name = sub_field.rosType;
        if (processed.find(sub_name) != processed.end()) {
            continue;
        }
        fprintf(out, "\\n================================================================================\\n\\\n");
        fprintf(out, "MSG: %s\\n\\\n", (sub_name == "Header"?"std_msgs/Header":sub_name.c_str()));
        output_type(out, sub_field, processed);
    }
}

bool RosTypeCodeGenYarp::endType(const std::string& tname,
                                 const RosField& field)
{
    string safename = getSafeName(tname);
    string dbl_name = getDoubleName(tname);
    string fullClassName = "yarp::rosmsg::" + getDoubleName(tname, "::");
    fprintf(out, "    // This class will serialize ROS style or YARP style depending on protocol.\n");
    fprintf(out, "    // If you need to force a serialization style, use one of these classes:\n");
    fprintf(out, "    typedef yarp::os::idl::BareStyle<%s> rosStyle;\n", fullClassName.c_str());
    fprintf(out, "    typedef yarp::os::idl::BottleStyle<%s> bottleStyle;\n\n", fullClassName.c_str());

    fprintf(out, "    // Give source text for class, ROS will need this\n");
    fprintf(out, "    yarp::os::ConstString getTypeText()\n");
    fprintf(out, "    {\n");
    fprintf(out, "        return \"");
    std::map<std::string, int> processed;
    output_type(out, field, processed);
    fprintf(out, "\";\n");
    fprintf(out, "    }\n");
    fprintf(out, "\n");

    fprintf(out, "    // Name the class, ROS will need this\n");
    fprintf(out, "    yarp::os::Type getType() override\n");
    fprintf(out, "    {\n");
    fprintf(out, "        yarp::os::Type typ = yarp::os::Type::byName(\"%s\", \"%s\");\n", dbl_name.c_str(), dbl_name.c_str());
    fprintf(out, "        typ.addProperty(\"md5sum\", yarp::os::Value(\"%s\"));\n", field.checksum.c_str());
    fprintf(out, "        typ.addProperty(\"message_definition\", yarp::os::Value(getTypeText()));\n");
    fprintf(out, "        return typ;\n");
    fprintf(out, "    }\n");
    fprintf(out, "};\n\n");

    if (!packageName.empty()) {
        fprintf(out, "} // namespace %s\n", packageName.c_str());
    }
    fprintf(out, "} // namespace rosmsg\n");
    fprintf(out, "} // namespace yarp\n\n");
    fprintf(out, "#endif // YARP_ROSMSG_%s_h\n", safename.c_str());

    fclose(out);
    out = nullptr;

    return true;
}


RosYarpType RosTypeCodeGenYarp::mapPrimitive(const RosField& field)
{
    RosYarpType ry;
    ry.rosType = field.rosType;
    ry.yarpType = field.rosType;
    size_t pos = ry.yarpType.find('/');
    if(pos != std::string::npos) {
        ry.yarpType.replace(pos, 1, "::");
    }
    ry.yarpType = "yarp::rosmsg::" + ry.yarpType;

    if (field.rosType == "string") {
        ry.yarpType = "std::string";
    }
    if (!field.isPrimitive) {
        return ry;
    }
    string name = field.rosType;
    string flavor = "";
    if (name == "int8" || (name == "bool" && field.isArray) || name == "char") {
        ry.yarpType = "char";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 1;
    } else if (name == "uint8"||name == "byte") {
        ry.yarpType = "unsigned char";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 1;
    } else if (name == "bool") {
        ry.yarpType = "bool";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.yarpDefaultValue = "false";
        ry.len = 1;
    } else if (name == "int16") {
        ry.yarpType = "yarp::os::NetInt16";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 2;
    } else if (name == "uint16") {
        ry.yarpType = "yarp::os::NetUint16";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 2;
    } else if (name == "int32") {
        ry.yarpType = "yarp::os::NetInt32";
        ry.writer = "appendInt";
        ry.reader = "expectInt";
        flavor = "int";
    } else if (name == "uint32") {
        ry.yarpType = "yarp::os::NetUint32";
        ry.writer = "appendInt";
        ry.reader = "expectInt";
        flavor = "int";
    } else if (name == "int64") {
        ry.yarpType = "yarp::os::NetInt64";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 8;
    } else if (name == "uint64") {
        ry.yarpType = "yarp::os::NetUint64";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "int";
        ry.len = 8;
    } else if (name == "float32") {
        ry.yarpType = "yarp::os::NetFloat32";
        ry.writer = "appendBlock";
        ry.reader = "expectBlock";
        flavor = "double";
        ry.len = 4;
    } else if (name == "float64") {
        ry.yarpType = "yarp::os::NetFloat64";
        ry.writer = "appendDouble";
        ry.reader = "expectDouble";
        flavor = "double";
    } else if (name == "string") {
        ry.yarpDefaultValue = "\"\"";
    } else {
        fprintf(stderr, "Please translate %s in RosTypeCodeGenYarp.cpp\n",
                name.c_str());
        std::exit(1);
    }
    if (flavor == "int") {
        ry.yarpWriter = "appendInt";
        ry.yarpReader = "expectInt";
        ry.yarpWriterCast = "(int)";
        ry.yarpTag = "BOTTLE_TAG_INT";
        ry.yarpWireReader = "expectInt";
        if (ry.yarpDefaultValue == "") {
            ry.yarpDefaultValue = "0";
        }
    } else if (flavor == "double") {
        ry.yarpWriter = "appendDouble";
        ry.yarpReader = "expectDouble";
        ry.yarpWriterCast = "(double)";
        ry.yarpTag = "BOTTLE_TAG_DOUBLE";
        ry.yarpWireReader = "expectDouble";
        if (ry.yarpDefaultValue == "") {
            ry.yarpDefaultValue = "0.0";
        }
    }
    return ry;
}


bool RosTypeCodeGenYarp::writeIndex(RosTypeCodeGenState& state)
{
    // Generate _indexALL.txt file
    string root = (target != "") ? (target + "/") : "";
    string index_fname = className + "_indexALL.txt";

    yarp::os::mkdir_p((root + index_fname).c_str(), 1);
    out = fopen((root + index_fname).c_str(), "w");
    if (!out) {
        fprintf(stderr, "Failed to open %s for writing\n", index_fname.c_str());
        return false;
    }
    if (verbose) {
        printf("Generating %s\n", index_fname.c_str());
    }

    for (const auto& str : state.generatedFiles) {
        fprintf(out, "%s\n", str.c_str());
    }

    fclose(out);
    out = nullptr;

    return true;
}
