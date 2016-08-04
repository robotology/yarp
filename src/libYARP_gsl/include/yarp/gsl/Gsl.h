/*
* Author: Lorenzo Natale.
* Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
* CopyPolicy: Released under the terms of the GPL 2.0 license or later.
*/

#ifndef YARP_GSL_GSL_HEADER_H
#define YARP_GSL_GSL_HEADER_H

/**
This library provides functions for compatibility with GSL. Including/linking GSL
forces this library to be GPL.
*/

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/gsl/api.h>

namespace yarp {
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

#endif //h

