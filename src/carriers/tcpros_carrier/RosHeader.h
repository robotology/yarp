/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
