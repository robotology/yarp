/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SERVERSQL_IMPL_TRIPLE_H
#define YARP_SERVERSQL_IMPL_TRIPLE_H

#include <string>
#include <yarp/conf/compiler.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 * The basic unit of data the name server works with.  There are three
 * basic fields, all optional: a namespace (ns), a name, and a value.
 * For those keeping score, this representation is a little bit like
 * that of RDF, and a great deal like machine tags.
 */
class Triple
{
public:
    std::string ns;
    std::string name;
    std::string value;
    bool hasNs;
    bool hasName;
    bool hasValue;


    Triple()
    {
        reset();
    }

    Triple(const Triple& alt)
    {
        hasNs = alt.hasNs;
        hasName = alt.hasName;
        hasValue = alt.hasValue;
        ns = alt.ns;
        name = alt.name;
        value = alt.value;
    }

    void reset()
    {
        hasNs = hasName = hasValue = false;
        ns = name = value = "";
    }

    void split(const std::string& str)
    {
        hasNs = hasName = hasValue = false;
        ns = name = value = "";
        size_t start = 0;
        size_t stop = std::string::npos;
        size_t ins = str.find(':');
        if (ins!=std::string::npos) {
            ns = str.substr(0,ins);
            start = ins+1;
            hasNs = true;
        }
        size_t ine = str.find('=',start);
        if (ine!=std::string::npos) {
            value = str.substr(ine+1,std::string::npos);
            stop = ine;
            hasValue = true;
        }
        name = str.substr(start,stop-start);
        hasName = true;
    }

    const char* getNs()
    {
        if (!hasNs) {
            return nullptr;
        }
        return ns.c_str();
    }

    const char* getName()
    {
        if (!hasName) {
            return nullptr;
        }
        return name.c_str();
    }

    const char* getValue()
    {
        if (!hasValue) {
            return nullptr;
        }
        return value.c_str();
    }

    std::string toString() const
    {
        std::string r = "";
        if (hasName) {
            r = name;
        }
        if (hasValue) {
            r += "=";
            r += value;
        }
        if (hasNs) {
            r = std::string(ns) + ":" + r;
        }
        return r;
    }

    void setNsNameValue(const char* ns, const char* name, const char* value)
    {
        setNameValue(name,value);
        hasNs = true;
        this->ns = ns;
    }

    void setNameValue(const char* name, const char* value)
    {
        reset();
        hasName = true;
        this->name = name;
        hasValue = true;
        this->value = value;
    }
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_TRIPLE_H
