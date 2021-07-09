/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    string pname;
    if (name == "Header") {
        tname = "std_msgs";
    } else {
        size_t at = tname.rfind('/');
        if (at == string::npos) {
            tname = "";
        } else {
            tname = pname = tname.substr(0, at);
            do {
                at = pname.rfind('/');
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
    size_t at = tname.rfind('/');
    if (at != string::npos) {
        part_tname = tname.substr(at+1, tname.length());
    }
    if (part_tname.find('.') != string::npos) {
        part_tname = part_tname.substr(0, part_tname.rfind('.'));
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

static void writeLicenseHeader(FILE* out)
{
    fprintf(out, "/*\n");
    fprintf(out, " * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)\n");
    fprintf(out, " * SPDX-License-Identifier: BSD-3-Clause\n");
    fprintf(out, " */\n");
    fprintf(out, "\n");
    fprintf(out, "// This is an automatically generated file.\n");
    fprintf(out, "\n");
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

    writeLicenseHeader(out);
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

        writeLicenseHeader(out);
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

    writeLicenseHeader(out);
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
    fprintf(out, "#include <yarp/os/Type.h>\n");
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
                fprintf(out, "        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) {\n");
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        %s%s = connection.expectInt32();\n",
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
            fprintf(out, "            %s%s = connection.expectInt32();\n",
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
                fprintf(out, "        %s%s = connection.expectInt32();\n",
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
                fprintf(out, "        if (connection.expectInt32() != (BOTTLE_TAG_LIST|%s)) {\n",
                        t.yarpTag.c_str());
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        %s%s = connection.expectInt32();\n",
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
                fprintf(out, "        if (connection.expectInt32() != BOTTLE_TAG_LIST) {\n");
                fprintf(out, "            return false;\n");
                fprintf(out, "        }\n");
            }
            fprintf(out, "        %s%s = connection.expectInt32();\n",
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
    fprintf(out, "    bool write%s(yarp::os::ConnectionWriter& connection) const override\n",
            bare?"Bare":"Bottle");
    fprintf(out, "    {\n");
    if (!bare) {
        fprintf(out, "        connection.appendInt32(BOTTLE_TAG_LIST);\n");
        fprintf(out, "        connection.appendInt32(%d);\n\n", len);
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
                fprintf(out, "        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        connection.appendInt32(%s.size());\n",
                        field.rosName.c_str());
            }
            fprintf(out, "        for (size_t %s=0; %s<%s.size(); %s++) {\n",
                    counter.c_str(),
                    counter.c_str(),
                    field.rosName.c_str(),
                    counter.c_str());
            fprintf(out, "            connection.appendInt32(%s[%s].length());\n",
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
                fprintf(out, "        connection.appendInt32(BOTTLE_TAG_STRING);\n");
            }
            fprintf(out, "        connection.appendInt32(%s.length());\n",
                    field.rosName.c_str());
            fprintf(out, "        connection.appendExternalBlock((char*)%s.c_str(), %s.length());\n",
                    field.rosName.c_str(),
                    field.rosName.c_str());
        }
    } else if (field.isPrimitive) {
        if (field.isArray) {
            if (!bare) {
                fprintf(out, "        connection.appendInt32(BOTTLE_TAG_LIST|%s);\n",
                        t.yarpTag.c_str());
                fprintf(out, "        connection.appendInt32(%s.size());\n",
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
                    fprintf(out, "        connection.appendInt32(%s.size());\n",
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
                fprintf(out, "        connection.appendInt32(%s);\n",
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
                fprintf(out, "        connection.appendInt32(BOTTLE_TAG_LIST);\n");
            }
            if (field.arrayLength == -1 || !bare) {
                fprintf(out, "        connection.appendInt32(%s.size());\n",
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
        fprintf(out, "    bool write(yarp::os::ConnectionWriter& connection) const override\n");
        fprintf(out, "    {\n");
        fprintf(out, "        return (connection.isBareMode() ? writeBare(connection)\n");
        fprintf(out, "                                        : writeBottle(connection));\n");
        fprintf(out, "    }\n\n");
    }
    return true;
}


static bool output_type(FILE *out,
                        const RosField& field,
                        std::map<std::string, int>& processed)
{
    const std::string& name = getDoubleName(field.rosType);
    processed[name] = 1;
    const std::string& source = field.source;
    for (char ch : source) {
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            fprintf(out, "\\n");
            fprintf(out, "\\\n");
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
    bool ends_with_eol = false;
    if (source.size() > 0 && source[source.size() -1] == '\n') {
        ends_with_eol = true;
    }
    for (const auto& sub_field : field.subRosType) {
        if (!sub_field.isStruct) {
            continue;
        }
        const std::string& sub_name = sub_field.rosType;
        if (processed.find(sub_name) != processed.end()) {
            continue;
        }
        if (sub_name == "TickTime" || sub_name == "TickDuration") {
            continue;
        }
        fprintf(out, "\\n\\\n");
        fprintf(out, "================================================================================\\n\\\n");
        fprintf(out, "MSG: %s\\n\\\n", (sub_name == "Header" ? "std_msgs/Header" : sub_name.c_str()));
        ends_with_eol = output_type(out, sub_field, processed);
    }
    return ends_with_eol;
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

    fprintf(out, "    // The name for this message, ROS will need this\n");
    fprintf(out, "    static constexpr const char* typeName = \"%s\";\n", dbl_name.c_str());
    fprintf(out, "\n");

    fprintf(out, "    // The checksum for this message, ROS will need this\n");
    fprintf(out, "    static constexpr const char* typeChecksum = \"%s\";\n", field.checksum.c_str());
    fprintf(out, "\n");

    // See http://wiki.ros.org/roslib/gentools for details about the format
    fprintf(out, "    // The source text for this message, ROS will need this\n");
    fprintf(out, "    static constexpr const char* typeText = \"\\\n");
    std::map<std::string, int> processed;
    bool ends_with_eol = output_type(out, field, processed);
    if (!ends_with_eol) {
        fprintf(out, "\\n\\\n");
    }
    fprintf(out, "\";\n");
    fprintf(out, "\n");

    fprintf(out, "    yarp::os::Type getType() const override\n");
    fprintf(out, "    {\n");
    fprintf(out, "        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);\n");
    fprintf(out, "        typ.addProperty(\"md5sum\", yarp::os::Value(typeChecksum));\n");
    fprintf(out, "        typ.addProperty(\"message_definition\", yarp::os::Value(typeText));\n");
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
    string flavor;
    if (name == "bool" && !field.isArray) {
        ry.yarpType = "bool";
        ry.writer = "appendInt8";
        ry.reader = "expectInt8";
        ry.yarpTag = "BOTTLE_TAG_INT8";
        flavor = "int";
        ry.yarpDefaultValue = "false";
        ry.len = 1;
    } else if (name == "int8" || (name == "bool" && field.isArray) || name == "char") {
        ry.yarpType = "std::int8_t";
        ry.writer = "appendInt8";
        ry.reader = "expectInt8";
        ry.yarpTag = "BOTTLE_TAG_INT8";
        flavor = "int";
    } else if (name == "uint8"||name == "byte") {
        ry.yarpType = "std::uint8_t";
        ry.writer = "appendInt8";
        ry.reader = "expectInt8";
        ry.yarpTag = "BOTTLE_TAG_INT8";
        flavor = "int";
    } else if (name == "int16") {
        ry.yarpType = "std::int16_t";
        ry.writer = "appendInt16";
        ry.reader = "expectInt16";
        ry.yarpTag = "BOTTLE_TAG_INT16";
        flavor = "int";
    } else if (name == "uint16") {
        ry.yarpType = "std::uint16_t";
        ry.writer = "appendInt16";
        ry.reader = "expectInt16";
        ry.yarpTag = "BOTTLE_TAG_INT16";
        flavor = "int";
    } else if (name == "int32") {
        ry.yarpType = "std::int32_t";
        ry.writer = "appendInt32";
        ry.reader = "expectInt32";
        ry.yarpTag = "BOTTLE_TAG_INT32";
        flavor = "int";
    } else if (name == "uint32") {
        ry.yarpType = "std::uint32_t";
        ry.writer = "appendInt32";
        ry.reader = "expectInt32";
        ry.yarpTag = "BOTTLE_TAG_INT32";
        flavor = "int";
    } else if (name == "int64") {
        ry.yarpType = "std::int64_t";
        ry.writer = "appendInt64";
        ry.reader = "expectInt64";
        ry.yarpTag = "BOTTLE_TAG_INT64";
        flavor = "int";
    } else if (name == "uint64") {
        ry.yarpType = "std::uint64_t";
        ry.writer = "appendInt64";
        ry.reader = "expectInt64";
        ry.yarpTag = "BOTTLE_TAG_INT64";
        flavor = "int";
    } else if (name == "float32") {
        ry.yarpType = "yarp::conf::float32_t";
        ry.writer = "appendFloat32";
        ry.reader = "expectFloat32";
        ry.yarpTag = "BOTTLE_TAG_FLOAT32";
        ry.yarpDefaultValue = "0.0f";
        flavor = "double";
    } else if (name == "float64") {
        ry.yarpType = "yarp::conf::float64_t";
        ry.writer = "appendFloat64";
        ry.reader = "expectFloat64";
        ry.yarpTag = "BOTTLE_TAG_FLOAT64";
        ry.yarpDefaultValue = "0.0";
        flavor = "double";
    } else if (name == "string") {
        ry.yarpDefaultValue = "\"\"";
    } else {
        fprintf(stderr, "Please translate %s in RosTypeCodeGenYarp.cpp\n",
                name.c_str());
        std::exit(1);
    }
    if (flavor == "int") {
        ry.yarpWriter = ry.writer;
        ry.yarpReader = ry.reader;
        ry.yarpWriterCast = "";
        ry.yarpWireReader = ry.reader;
        if (ry.yarpDefaultValue == "") {
            ry.yarpDefaultValue = "0";
        }
    } else if (flavor == "double") {
        ry.yarpWriter = ry.writer;
        ry.yarpReader = ry.reader;
        ry.yarpWriterCast = "";
        ry.yarpWireReader = ry.reader;
    }
    return ry;
}

#include <iostream>
bool RosTypeCodeGenYarp::writeIndex(RosTypeCodeGenState& state)
{
    // Generate _index.txt file
    string root = (target != "") ? (target + "/") : "";
    string index_fname;
    if(!packageName.empty()) {
        index_fname = packageName + "_" + className + "_index.txt";
    } else {
        index_fname = className + "_index.txt";
    }

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
