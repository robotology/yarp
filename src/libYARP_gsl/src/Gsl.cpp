/*
* Author: Lorenzo Natale.
* Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
* CopyPolicy: Released under the terms of the GPL 2.0 license or later.
*/

/** 
This library provides functions for compatibility with GSL. Including/linking GSL 
forces this library to be GPL.
*/

#include <yarp/gsl_compatibility.h>

#include <yarp/gsl/Gsl.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

using namespace yarp::gsl;
using namespace yarp::sig;

GslMatrix::GslMatrix(const Matrix &v)
{
    gsl_matrix *mat = new gsl_matrix;
    gsl_block *bl = new gsl_block;

    mat->block = bl;

    //this is constant (at least for now)
    mat->owner = 1;

    gslData = mat;

    gsl_matrix *tmp = static_cast<gsl_matrix *>(gslData);
    tmp->block->data = const_cast<double *>(v.data());
    tmp->data = tmp->block->data;
    tmp->block->size = v.rows()*v.cols();
    tmp->owner = 1;
    tmp->tda = v.cols();
    tmp->size1 = v.rows();
    tmp->size2 = v.cols();
}

GslMatrix::~GslMatrix()
{
    gsl_matrix *tmp = (gsl_matrix *)(gslData);

    if (tmp != 0)
    {
        delete tmp->block;
        delete tmp;
    }

    gslData = 0;
}

void *GslMatrix::getGslMatrix()
{
    return gslData;
}


const void *GslMatrix::getGslMatrix() const
{
    return gslData;
}


GslVector::GslVector(const Vector &v)
{
    gsl_vector *vect = new gsl_vector;
    gsl_block *bl = new gsl_block;

    vect->block = bl;

    //these are constant (at least for now)
    vect->owner = 1;
    vect->stride = 1;

    gslData = vect;

    gsl_vector *tmp = static_cast<gsl_vector *>(gslData);
    tmp->block->data = const_cast<double * > (v.data());
    tmp->data = tmp->block->data;
    tmp->block->size = v.size();
    tmp->owner = 1;
    tmp->stride = 1;
    tmp->size = tmp->block->size;
}

GslVector::~GslVector()
{
    gsl_vector *tmp = (gsl_vector *)(gslData);

    if (tmp != 0)
    {
        delete tmp->block;
        delete tmp;
    }

    gslData = 0;
}

void *GslVector::getGslVector()
{
    return gslData;
}


const void *GslVector::getGslVector() const
{
    return gslData;
}





