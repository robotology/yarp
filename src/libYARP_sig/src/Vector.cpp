// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
// $Id: Vector.cpp,v 1.3 2006-07-07 09:18:03 eshuy Exp $

#include <yarp/sig/Vector.h>
#include <yarp/IOException.h>
#include <yarp/os/Bottle.h>

/*
preferred network format for a list of doubles.

INT totallen = #bytes whole think
INT listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE
INT listLen = N
double 1
double 2
...
double N
*/

using namespace yarp::sig;

Vector::Vector() : ACE_Array<double>() {}
Vector::Vector(const Vector& x) : ACE_Array<double>((ACE_Array<double>&)x) {}
Vector::~Vector() {}
Vector& Vector::operator=(const Vector& x) { ACE_Array<double>::operator=((ACE_Array<double>&)x); return *this; }
Vector::Vector (size_t size) : ACE_Array<double>(size) {}
Vector::Vector (size_t size, const double& default_value) : ACE_Array<double>(size, default_value) {}


#include <yarp/os/NetInt32.h>
#include <yarp/os/begin_pack_for_net.h>

class YARPVectorPortContentHeader
{
public:
    //yarp::os::NetInt32 totalLen; // not included any more - redundant
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
} PACKED_FOR_NET;

#include <yarp/os/end_pack_for_net.h>

bool Vector::read(yarp::os::ConnectionReader& connection) {
    
    try {
        // auto-convert text mode interaction
        connection.convertTextMode();
        YARPVectorPortContentHeader header;
        connection.expectBlock((char*)&header, sizeof(header));
        if (header.listLen > 0)
        {
            if (this->size() != (unsigned int)(header.listLen))
                this->size(header.listLen);
            double *ptr = &(this->operator[](0));
            ACE_ASSERT (ptr != NULL);
            connection.expectBlock((char *)ptr, sizeof(double)*header.listLen);
        }
        else
            return false;
    } catch (yarp::IOException e) {
        return false;
    }

    return true;
}


bool Vector::write(yarp::os::ConnectionWriter& connection) {
    YARPVectorPortContentHeader header;

    //header.totalLen = sizeof(header)+sizeof(double)*this->size();
    header.listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE;
    header.listLen = this->size();

    connection.appendBlock((char*)&header, sizeof(header));
    double *ptr = &(this->operator[](0));
    ACE_ASSERT (ptr != NULL);

    // Note use of external block.  Implies care needed about ownership.
    connection.appendExternalBlock((char *)ptr, sizeof(double)*header.listLen);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return true;
}

