/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
