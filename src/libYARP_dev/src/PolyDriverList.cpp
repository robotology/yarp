/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/PolyDriverList.h>

#include <vector>


#define RES(v) ((std::vector<PolyDriverDescriptor> *)v)

using namespace yarp::dev;

PolyDriverList::PolyDriverList()
{
    descriptors=new std::vector<PolyDriverDescriptor>;
}

PolyDriverList::~PolyDriverList()
{
    delete RES(descriptors);
}

PolyDriverDescriptor *PolyDriverList::operator[](int k)
{
    return &(*RES(descriptors))[k];
}

const PolyDriverDescriptor *PolyDriverList::operator[] (int k) const
{
    return &(*RES(descriptors))[k];
}

int PolyDriverList::size() const
{
    return (int)RES(descriptors)->size();
}

void PolyDriverList::push(PolyDriver *p, const char *k)
{
    RES(descriptors)->push_back(PolyDriverDescriptor(p, k));
}

void PolyDriverList::push(PolyDriverDescriptor &v)
{
    RES(descriptors)->push_back(v);
}

const PolyDriverList &PolyDriverList::operator=(const PolyDriverList &l)
{
    *RES(descriptors)=*RES(l.descriptors);
    return *this;
}
