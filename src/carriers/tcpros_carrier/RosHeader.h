/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef ROSHEADER_INC
#define ROSHEADER_INC

#include <string>
#include <map>

#include <tcpros_carrier_api.h>

class YARP_tcpros_carrier_API RosHeader {
public:
    std::map<std::string,std::string> data;

    std::string writeHeader();

    bool readHeader(const std::string& bin);

    std::string toString() const;

    static void appendInt(char *&buf,int x);

    static void appendString(char *&buf,const std::string& str);

    static std::string showMessage(std::string s);
};

#endif
