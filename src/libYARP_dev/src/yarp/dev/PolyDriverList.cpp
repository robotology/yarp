/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
