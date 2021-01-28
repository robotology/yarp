/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef YARP_GSL_GSL_HEADER_H
#define YARP_GSL_GSL_HEADER_H

/*
 * This library provides functions for compatibility with GSL.
 * @warning Including/linking GSL forces this library to be GPL.
 */

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/gsl/api.h>

namespace yarp {
    /**
     * A library for interoperability with the GSL library.
     *
     * @warning Including/linking GSL forces this library to be GPL.
     */
    namespace gsl {
        class GslMatrix;
        class GslVector;
    }
}

/**
* Classes for compatibility with the GSL library.
* Legacy code like:
* Matrix m;
* gsl_function(m.getGslMatrix());
*
* Should be replaced with:
* Matrix m;
* gsl_function(GslMatrix(m).getGslMatrix());
*/
class YARP_gsl_API yarp::gsl::GslMatrix
{
public:
    /**
    * Allocate from yarp Matrix.
    */
    GslMatrix(const yarp::sig::Matrix &);
    ~GslMatrix();

    /**
    * Return GSL compatile pointer.
    */
    void *getGslMatrix();

    /**
    * Return GSL compatile, const pointer.
    */
    const void *getGslMatrix() const;

private:
    void *gslData;
};

/**
* Classes for compatibility with the GSL library.
* Legacy code like:
* Vector v;
* gsl_function(v.getGslVector());
*
* Should be replaced with:
* Vector v;
* gsl_function(GslVector(v).getGslVetor());
*/
class YARP_gsl_API yarp::gsl::GslVector
{
public:
    GslVector(const yarp::sig::Vector &);
    ~GslVector();
    void *getGslVector();
    const void *getGslVector() const;

private:
    void *gslData;
};

#endif // YARP_GSL_GSL_HEADER_H
