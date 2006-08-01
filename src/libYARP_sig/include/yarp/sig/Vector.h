// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
// $Id: Vector.h,v 1.3 2006-08-01 23:20:30 gmetta Exp $

#ifndef _YARP2_VECTOR_
#define _YARP2_VECTOR_

#include <ace/config.h>
#include <ace/Containers_T.h>
#include <yarp/os/Portable.h>

/**
 * \file Vector.h contains the definition of a Vector type with corresponding 
 * Portable character.
 */

namespace yarp {
    namespace sig {
        class Vector;
        // template class VectorOf<class T> ...;
    }
}

/**
 * \ingroup sig_class
 *
 * A simple Vector class derived from ACE and with Portable functionality.
 * Things you can do on Vector:
 * - use [] to access single elements without range checking
 * - use get() to access single elements with range checking
 * - use size() to get the current size of the Vector
 * - use max_size() to get the maximum size of the Vector (history of allocated memory)
 * - use operator= to copy Vectors
 * This is derived from ACE_Array & ACE_Array_Base but it doesn't need to be the case if this
 * shows to be unefficient.
 */
class yarp::sig::Vector : public ACE_Array<double>, public yarp::os::Portable {
public:
    /**
     * Default constructor.
     */
    Vector();

    /**
     * Copy contructor.
     */
    Vector(const Vector& x);

    /**
     * Destructor.
     */
    virtual ~Vector();

    /**
     * Copies a Vector object into the current one.
     * @param x is a reference to an object of type Vector.
     * @return a reference to the current object.
     */
    Vector& operator=(const Vector& x);

    /**
     * Create a Vector object of a given size and fill the content.
     * @param size is the number of elements of the vector.
     */
    Vector (size_t size);

    /**
     * Initialize a Vector of the given size placing the default_value in each index.
     * @param size is the size of the vector.
     * @param default_value is the initial value of the vector.
     */
    Vector (size_t size, const double& default_value);

    /**
     * Read vector from a connection.
     * return true iff a vector was read correctly
     */
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
     * Write vector to a connection.
     * return true iff a vector was written correctly
     */
    virtual bool write(yarp::os::ConnectionWriter& connection);
};


#endif
