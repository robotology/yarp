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

#include <yarp/os/begin_pack_for_net.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetInt32.h>

#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/gsl_compatibility.h>


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
        YARP_ASSERT (ptr != 0);
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
    YARP_ASSERT (ptr != NULL);

    connection.appendExternalBlock(ptr, elemSize*header.listLen);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}

/***************************************************************************************************************************/
/******************************************************** VECTOR ***********************************************************/
/***************************************************************************************************************************/

Vector::Vector(size_t s, const double *p)
{
    storage.assign(s, p);
    allocGslData();
    updateGslData();
}

const Vector &Vector::operator=(const Vector &r)
{
    if(size() == r.size())
        memcpy(data(), r.data(), sizeof(double)*size());
    else
    {
        // DelPrete 04/10/2013:
        // If r is a VectorN then it is wrong to copy the content of storage
        // because the data are inside storageStatic! Solution: use data() rather than storage.
        //storage=r.storage;
        storage.assign(r.size(), r.data());
        updateGslData();
    }
    return *this;
}

void Vector::zero(){ ACE_OS::memset(data(), 0, sizeof(double)*size()); }

yarp::os::ConstString Vector::toString(int precision, int width) const
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


/*********************************************************** SERIALIZATION METHODS *******************************************/


/*
* Read vector from a connection.
* return true iff a vector was read correctly
*/
bool Vector::read(yarp::os::ConnectionReader& connection)
{
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

/**
* Write vector to a connection.
* return true iff a vector was written correctly
*/
bool Vector::write(yarp::os::ConnectionWriter& connection)
{
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


/****************************************************** PRIVATE METHODS ***********************************************/


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
    gsl_vector *tmp = static_cast<gsl_vector *>(gslData);
    tmp->block->data = data();
    tmp->data = tmp->block->data;
    tmp->block->size = size();
    tmp->owner = 1;
    tmp->stride = 1;
    tmp->size = tmp->block->size;
}



/***************************************************************************************************************************/
/******************************************************** VECTORN **********************************************************/
/***************************************************************************************************************************/
