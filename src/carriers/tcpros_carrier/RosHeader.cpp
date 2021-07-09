/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RosHeader.h"
#include "TcpRosLogComponent.h"

#include <cstdio>
#include <cstring>
#include <yarp/os/Bytes.h>
#include <yarp/os/NetType.h>

using namespace yarp::os;
using namespace std;

void RosHeader::appendInt32(char *&buf,int x) {
    Bytes bytes(buf,4);
    NetType::netInt(x,bytes);
    buf += 4;
}

void RosHeader::appendString(char *&buf,const string& str) {
    memcpy(buf,str.c_str(),str.length());
    buf += str.length();
}


string RosHeader::showMessage(string s) {
    string result;
    for (char ch : s) {
        char buf[256];
        sprintf(buf, "%c (%#x) ", (ch>=' ')?ch:'.', *reinterpret_cast<unsigned char*>(&ch));
        result += buf;
    }
    return result;
}

string RosHeader::writeHeader() {
    size_t len = 0;
    for (auto& it : data) {
        string key = it.first;
        string val = it.second;
        len += 4 + key.length() + 1 + val.length();
    }
    string result(len,'\0');
    char *buf = (char *)result.c_str();
    for (auto& it : data) {
        string key = it.first;
        string val = it.second;
        appendInt32(buf,key.length()+1+val.length());
        appendString(buf,key);
        appendString(buf,string("="));
        appendString(buf,val);
    }
    return result;
}


bool RosHeader::readHeader(const string& bin) {
    data.clear();

    unsigned int len = bin.length();
    char *at = (char*) bin.c_str();

    while (len>0) {
        Bytes bytes(at,4);
        int slen = NetType::netInt(bytes);
        at += 4;
        len -= 4;
        string keyval(at,slen);
        size_t delim = keyval.find_first_of('=',0);
        if (delim == string::npos) {
            yCWarning(TCPROSCARRIER, "Corrupt ROS header");
        }
        string key = keyval.substr(0,delim);
        string val = keyval.substr(delim+1);
        yCTrace(TCPROSCARRIER, "key %s => val %s\n", key.c_str(), val.c_str());
        data[key] = val;
        at += slen;
        len -= slen;
    }
    return true;
}


std::string RosHeader::toString() const {
    string result;
    for (const auto& it : data) {
        string key = it.first;
        string val = it.second;
        result += key;
        result += "->";
        result += val;
        result += " ";
    }
    return result;
}
