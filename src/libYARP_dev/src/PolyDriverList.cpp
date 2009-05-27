// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/dev/PolyDriverList.h>

#include <ace/config.h>
#include <ace/Vector_T.h>

#define RES(v) ((ACE_Vector<PolyDriverDescriptor> *)v)

using namespace yarp::dev;

PolyDriverList::PolyDriverList()
{
    aceVector=new ACE_Vector<PolyDriverDescriptor>;
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
    return RES(aceVector)->size();
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
