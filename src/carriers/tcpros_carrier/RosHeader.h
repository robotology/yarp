// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef ROSHEADER_INC
#define ROSHEADER_INC

#include <string>
#include <map>

class RosHeader {
public:
    std::map<std::string,std::string> data;

    std::string writeHeader();

    bool readHeader(const std::string& bin);

    static void appendInt(char *&buf,int x);

    static void appendString(char *&buf,const std::string& str);

    static std::string showMessage(std::string s);
};

#endif
