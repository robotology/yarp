/*
 * Author: Lorenzo Natale.
 * Copyright (C) 2009 The Robotcub consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/PolyDriverList.h>

#include <yarp/os/impl/PlatformVector.h>


#define RES(v) ((PlatformVector<PolyDriverDescriptor> *)v)

using namespace yarp::dev;

PolyDriverList::PolyDriverList()
{
    aceVector=new PlatformVector<PolyDriverDescriptor>;
}

PolyDriverList::~PolyDriverList()
{
    delete RES(aceVector);
}

PolyDriverDescriptor *PolyDriverList::operator[](int k)
{
    return &(*RES(aceVector))[k];
}

const PolyDriverDescriptor *PolyDriverList::operator[] (int k) const
{
    return &(*RES(aceVector))[k];
}

int PolyDriverList::size() const
{
    return (int)RES(aceVector)->size();
}

void PolyDriverList::push(PolyDriver *p, const char *k)
{
    RES(aceVector)->push_back(PolyDriverDescriptor(p, k));
}

void PolyDriverList::push(PolyDriverDescriptor &v)
{
    RES(aceVector)->push_back(v);
}

const PolyDriverList &PolyDriverList::operator=(const PolyDriverList &l)
{
  *RES(aceVector)=*RES(l.aceVector);
  return *this;
}
