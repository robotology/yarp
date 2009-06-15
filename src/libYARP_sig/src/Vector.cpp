// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007 Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// $Id: Vector.cpp,v 1.28 2009-06-15 17:47:37 eshuy Exp $

#include <yarp/sig/Vector.h>
#include <yarp/os/impl/IOException.h>
#include <yarp/os/Bottle.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/NetFloat64.h>

#include <ace/config.h>
#include <ace/Vector_T.h>

using namespace yarp::sig;
using namespace yarp::os::impl;
using namespace yarp::sig::impl;

#define RES(v) ((ACE_Vector<T> *)v)
#define RES_ITERATOR(v) ((ACE_Vector_Iterator<double> *)v)

/// network stuff
#include <yarp/os/NetInt32.h>
#include <yarp/os/begin_pack_for_net.h>

#include <yarp/gsl_compatibility.h>

///////////////////

class VectorPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
} PACKED_FOR_NET;

#include <yarp/os/end_pack_for_net.h>

template<class T> 
VectorImpl<T>::VectorImpl()
{
    aceVector=(void *) new ACE_Vector<T>;
}

template<class T>
VectorImpl<T>::VectorImpl(size_t size)
{
    ACE_ASSERT (size>0);
    aceVector=(void *)new ACE_Vector<T>(size);
    RES(aceVector)->resize(size, T(0));
}

template<class T>
VectorImpl<T>::~VectorImpl()
{
    delete RES(aceVector);
}

template<class T>
VectorImpl<T>::VectorImpl(const VectorImpl &l)
{
    aceVector=new ACE_Vector<T>;
    *RES(aceVector)=*RES(l.aceVector);
}

template<class T>
const VectorImpl<T> &VectorImpl<T>::operator=(const VectorImpl &l)
{
    *RES(aceVector)=*RES(l.aceVector);
    return *this;
}

template<class T>
const T &VectorImpl<T>::operator=(const T&l)
{
    int k=0;
    int s=size();

    for(k=0;k<s;k++)
        (*RES(aceVector))[k]=l;

    return l;
}

template<class T>
void VectorImpl<T>::resize(size_t size, const T& def)
{
    ACE_ASSERT (size>0);
    RES(aceVector)->resize(size,def);
}

template<class T>
int VectorImpl<T>::size() const
{
    return RES(aceVector)->size();
}

template<class T>
T &VectorImpl<T>::operator[](int el)
{
    return (*RES(aceVector))[el];
}

template<class T>
const T& VectorImpl<T>::operator[](int el) const
{
    return (*RES(aceVector))[el];
}

template<class T>
void VectorImpl<T>::pop_back()
{
   RES(aceVector)->pop_back();
}

template<class T>
void VectorImpl<T>::push_back(const T&e)
{
   RES(aceVector)->push_back(e);
}

template<class T>
void VectorImpl<T>::clear()
{
    RES(aceVector)->clear();
}


template<class T>
int VectorImpl<T>::set (T const &new_item, size_t slot)
{
    return RES(aceVector)->set(new_item, slot);
}

template<class T>
int VectorImpl<T>::get (T &item, size_t slot) const
{
    return RES(aceVector)->get(item, slot);
}

//////////////////////////////////
// iterator
//
template<class T>
IteratorOf<T>::IteratorOf(const VectorImpl<T> &v)
{
    aceVectorIterator=new ACE_Vector_Iterator<T>((*RES(v.aceVector)));
}

template<class T>
IteratorOf<T>::~IteratorOf()
{
    delete RES_ITERATOR(aceVectorIterator);
}

template<class T>
int IteratorOf<T>::advance()
{
    return RES_ITERATOR(aceVectorIterator)->advance();
}

template<class T>
int IteratorOf<T>::next(T *& n)
{
    return RES_ITERATOR(aceVectorIterator)->next(n);
}

template<class T>
int IteratorOf<T>::done()
{
    return RES_ITERATOR(aceVectorIterator)->done();
}

bool VectorBase::read(yarp::os::ConnectionReader& connection) {
    // auto-convert text mode interaction
    connection.convertTextMode();
    VectorPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;
    if (header.listLen > 0 && 
        header.listTag == BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE) {
        if (getListSize() != (int)(header.listLen))
            resize(header.listLen);
        const char *ptr = getMemoryBlock();
        ACE_ASSERT (ptr != 0);
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
    header.listLen = getListSize();

    connection.appendBlock((char*)&header, sizeof(header));
    const char *ptr = getMemoryBlock();
    int elemSize=getElementSize();
    ACE_ASSERT (ptr != NULL);

    connection.appendExternalBlock(ptr, elemSize*header.listLen);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}

// pasa added this, MSC seems to complain otherwise (admitting it was tested/compiled on gcc already).
namespace yarp {
    namespace sig {
        namespace impl {
            template class VectorImpl<double>;
            template class VectorImpl<int>;
            template class VectorImpl<char>;
            template class VectorImpl<float>;
            template class IteratorOf<double>;
        }
    template class VectorOf<double>;
    }
}


/// vector implementations
#include <yarp/os/impl/String.h>
using namespace yarp::os::impl;
using namespace yarp::os;

/**
* Quick implementation, space for improvement.
*/
ConstString Vector::toString()
{
    String ret = "";
    char tmp[80];
    int c=0;
    for(c=0;c<length()-1;c++)
    {
        sprintf(tmp, "%lf\t", (*this)[c]);
        //ret.append(tmp, strlen(tmp));
        ret+=tmp;
    }

    if (length()>=1) {
        sprintf(tmp, "%lf", (*this)[c]);
        //ret.append(tmp, strlen(tmp));
        ret+=tmp;
    }
    return ConstString(ret.c_str());
}

const Vector &Vector::operator=(const Vector &r)
{
    storage=r.storage;
    updateGslData();
    return *this;
}

Vector::Vector(size_t s, const double *p)
{
    storage.resize(s);

    for(int k=0; k<storage.size(); k++)
        storage[k]=p[k];

    allocGslData();
    updateGslData();
}

void Vector::zero()
{
    for(int k=0; k<storage.size(); k++)
        storage[k]=0;
}

const Vector &Vector::operator=(double v)
{
    double *tmp=storage.getFirst();

    for(int k=0; k<length(); k++)
        tmp[k]=v;

    return *this;
}

bool Vector::operator==(const yarp::sig::Vector &r) const
{
    const double *tmp1=data();
    const double *tmp2=r.data();

    //check dimensions first
    if (size()!=r.size())
        return false;

    int c=size();
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
        if (size() != (int)(header.listLen))
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
    header.listLen = size();

    connection.appendBlock((char*)&header, sizeof(header));

    int k=0;
    for (k=0;k<header.listLen;k++)
        connection.appendDouble((*this)[k]);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}
