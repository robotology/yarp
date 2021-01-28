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

/*
 * This library provides functions for compatibility with GSL.
 * @warning Including/linking GSL forces this library to be GPL.
 */

#include <yarp/gsl/Gsl.h>
#include <yarp/gsl/impl/gsl_structs.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

using namespace yarp::gsl;
using namespace yarp::sig;

GslMatrix::GslMatrix(const Matrix &v)
{
    auto* mat = new gsl_matrix;
    auto* bl = new gsl_block;

    mat->block = bl;

    //this is constant (at least for now)
    mat->owner = 1;

    gslData = mat;

    auto* tmp = static_cast<gsl_matrix *>(gslData);
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
    auto* tmp = (gsl_matrix *)(gslData);

    if (tmp != nullptr)
    {
        delete tmp->block;
        delete tmp;
    }

    gslData = nullptr;
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
    auto* vect = new gsl_vector;
    auto* bl = new gsl_block;

    vect->block = bl;

    //these are constant (at least for now)
    vect->owner = 1;
    vect->stride = 1;

    gslData = vect;

    auto* tmp = static_cast<gsl_vector *>(gslData);
    tmp->block->data = const_cast<double * > (v.data());
    tmp->data = tmp->block->data;
    tmp->block->size = v.size();
    tmp->owner = 1;
    tmp->stride = 1;
    tmp->size = tmp->block->size;
}

GslVector::~GslVector()
{
    auto* tmp = (gsl_vector *)(gslData);

    if (tmp != nullptr)
    {
        delete tmp->block;
        delete tmp;
    }

    gslData = nullptr;
}

void *GslVector::getGslVector()
{
    return gslData;
}


const void *GslVector::getGslVector() const
{
    return gslData;
}
