// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

// $Id: Vector.cpp,v 1.28 2009-06-15 17:47:37 eshuy Exp $
#include <yarp/sig/Vector.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetFloat64.h>

#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::sig;
using namespace yarp::os;

#define RES(v) ((PlatformVector<T> *)v)
#define RES_ITERATOR(v) ((PLATFORM_VECTOR_ITERATOR(double) *)v)

/// network stuff
#include <yarp/os/NetInt32.h>

#include <yarp/gsl_compatibility.h>

///////////////////

#include <yarp/os/begin_pack_for_net.h>
class VectorPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
} PACKED_FOR_NET;
#include <yarp/os/end_pack_for_net.h>

bool VectorBase::read(yarp::os::ConnectionReader& connection) {
    // auto-convert text mode interaction
    connection.convertTextMode();
    VectorPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;
    if (header.listLen > 0 && 
        header.listTag == BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE) {
        if ((size_t)getListSize() != (size_t)(header.listLen))
            resize(header.listLen);
        const char *ptr = getMemoryBlock();
        yAssert(ptr != 0);
        int elemSize=getElementSize();
        ok = connection.expectBlock(ptr, elemSize*header.listLen);
        if (!ok) return false;
    } else {
        return false;
    }

    return !connection.isError();
}

bool VectorBase::write(yarp::os::ConnectionWriter& connection) {
    VectorPortContentHeader header;

    //header.totalLen = sizeof(header)+sizeof(double)*this->size();
    header.listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE;
    header.listLen = (int)getListSize();

    connection.appendBlock((char*)&header, sizeof(header));
    const char *ptr = getMemoryBlock();
    int elemSize=getElementSize();
    yAssert(ptr != NULL);

    connection.appendExternalBlock(ptr, elemSize*header.listLen);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}


/**
* Quick implementation, space for improvement.
*/
ConstString Vector::toString(int precision, int width) const
{
    ConstString ret = "";
    size_t c;
    char tmp[350];
    if(width<0){  
        for(c=0;c<length();c++){
            sprintf(tmp, "% .*lf\t", precision, (*this)[c]);
            ret+=tmp;
        }
    }else{
        for(c=0;c<length();c++){
            sprintf(tmp, "% *.*lf ", width, precision, (*this)[c]);
            ret+=tmp;
        }
    }       

    if(length()>=1)
        return ret.substr(0, ret.length()-1);
    return ret;
}

Vector Vector::subVector(unsigned int first, unsigned int last) const
{
    Vector ret;
    if((first<=last)&&((int)last<(int)storage.size()))
    {
        ret.resize(last-first+1);
        for(unsigned int k=first; k<=last; k++)
            ret[k-first]=storage[k];
    }
    return ret;
}

bool Vector::setSubvector(int position, const Vector &v)
{    
    if(position+v.size() > storage.size())
        return false;
    for(size_t i=0;i<v.size();i++)
        storage[position+i] = v(i);
    return true;
}

const Vector &Vector::operator=(const Vector &r)
{
    if(storage.size() == r.storage.size())
    {
        ACE_OS::memcpy(storage.getFirst(), r.storage.getFirst(), sizeof(double)*storage.size());
    }
    else
    {
        storage=r.storage;
        updateGslData();
    }
    return *this;
}

Vector::Vector(size_t s, const double *p)
{
    storage.resize(s);

    ACE_OS::memcpy(storage.getFirst(), p, sizeof(double)*s);

    allocGslData();
    updateGslData();
}

void Vector::zero()
{
    ACE_OS::memset(storage.getFirst(), 0, sizeof(double)*storage.size());
}

const Vector &Vector::operator=(double v)
{
    double *tmp=storage.getFirst();

    for(size_t k=0; k<length(); k++)
        tmp[k]=v;

    return *this;
}

bool Vector::operator==(const yarp::sig::Vector &r) const
{
    //check dimensions first
    size_t c=size();
    if (c!=r.size())
        return false;

    const double *tmp1=data();
    const double *tmp2=r.data();

    while(c--)
    {
        if (*tmp1++!=*tmp2++)
            return false;
    }

    return true;
}

void *Vector::getGslVector()
{
    return gslData;
}

const void *Vector::getGslVector() const
{
    return gslData;
}

void Vector::allocGslData()
{
    gsl_vector *vect=new gsl_vector;
    gsl_block *bl=new gsl_block;
    
    vect->block=bl;

    //these are constant (at least for now)
    vect->owner=1;
    vect->stride=1;

    gslData=vect;
}

void Vector::freeGslData()
{
    gsl_vector *tmp=(gsl_vector *) (gslData);

    if (tmp!=0)
    {
        delete tmp->block;
        delete tmp;
    }

    gslData=0;
}

void Vector::updateGslData()
{
    gsl_vector *tmp=static_cast<gsl_vector *>(gslData);
    tmp->block->data=Vector::data();
    tmp->data=tmp->block->data;
    tmp->block->size=Vector::size();
    tmp->owner=1;
    tmp->stride=1;
    tmp->size=tmp->block->size;
}

bool Vector::read(yarp::os::ConnectionReader& connection) {
    // auto-convert text mode interaction
    connection.convertTextMode();
    VectorPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;
    if (header.listLen > 0 && 
        header.listTag == BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE) {
        if (size() != (size_t)(header.listLen))
            resize(header.listLen);
        
        int k=0;
        for (k=0;k<header.listLen;k++)
            (*this)[k]=connection.expectDouble();
    } else {
        return false;
    }

    return !connection.isError();
}

bool Vector::write(yarp::os::ConnectionWriter& connection) {
    VectorPortContentHeader header;

    header.listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE;
    header.listLen = (int)size();

    connection.appendBlock((char*)&header, sizeof(header));

    int k=0;
    for (k=0;k<header.listLen;k++)
        connection.appendDouble((*this)[k]);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}

