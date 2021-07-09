/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ROSHEADER_INC
#define ROSHEADER_INC

#include <string>
#include <map>

class RosHeader
{
public:
    std::map<std::string,std::string> data;

    std::string writeHeader();

    bool readHeader(const std::string& bin);

    std::string toString() const;

    static void appendInt32(char *&buf,int x);

    static void appendString(char *&buf,const std::string& str);

    static std::string showMessage(std::string s);
};

#endif
