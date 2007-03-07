// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

// $Id: Matrix.cpp,v 1.1 2007-03-07 15:28:24 natta Exp $

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/IOException.h>
#include <yarp/os/Bottle.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/NetFloat64.h>

#include <ace/config.h>
#include <ace/Vector_T.h>

using namespace yarp::sig;
using namespace yarp;

#define RES(v) ((ACE_Vector<T> *)v)
#define RES_ITERATOR(v) ((ACE_Vector_Iterator<double> *)v)

#include <yarp/os/begin_pack_for_net.h>

class MatrixPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
	yarp::os::NetInt32 rows;
	yarp::os::NetInt32 cols;
} PACKED_FOR_NET;
#include <yarp/os/end_pack_for_net.h>

/// network stuff
#include <yarp/os/NetInt32.h>
#include <yarp/os/begin_pack_for_net.h>

bool MatrixBase::read(yarp::os::ConnectionReader& connection) {
    try {
        // auto-convert text mode interaction
        connection.convertTextMode();
        MatrixPortContentHeader header;
        connection.expectBlock((char*)&header, sizeof(header));
        int r=rows();
		int c=cols();
		if (header.listLen > 0)
            {
				if ( (r*c) != (int)(header.listLen))
				{
					resize(header.rows, header.cols);
            	}
                const char *ptr = getMemoryBlock();
                ACE_ASSERT (ptr != 0);
                connection.expectBlock(ptr, sizeof(double)*header.listLen);
            }
        else
            return false;
    } catch (yarp::IOException e) {
        return false;
    }

    return true;
}


bool MatrixBase::write(yarp::os::ConnectionWriter& connection) {
	MatrixPortContentHeader header;

    //header.totalLen = sizeof(header)+sizeof(double)*this->size();
    header.listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE;
	header.rows=rows();
	header.cols=cols();
    header.listLen = header.rows*header.cols;

    connection.appendBlock((char*)&header, sizeof(header));
    const char *ptr = getMemoryBlock();
    ACE_ASSERT (ptr != NULL);

    connection.appendExternalBlock(ptr, sizeof(double)*header.listLen);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return true;
}

