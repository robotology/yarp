// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/**
 * \infile routines for Singular Value Decomposition
 */

#include <yarp/math/SVD.h>
#include <gsl/gsl_linalg.h>

using namespace yarp::sig;

/**
* Perform SVD decomposition on a NxM matrix. Golub Reinsch method.
*/
void yarp::math::SVD(const Matrix &in, 
            Matrix &U,
            Vector &S,
            Matrix &V)
{
    U=in;
    Vector work;
    work.resize(U.cols());
    gsl_linalg_SV_decomp((gsl_matrix *) U.getGslMatrix(),
        (gsl_matrix *) V.getGslMatrix(),
        (gsl_vector *) S.getGslVector(),
        (gsl_vector *) work.getGslVector());
}

/**
* Perform SVD decomposition on a NxM matrix. 
* Modified Golub Reinsch method. fast for M>>N.
*/
void yarp::math::SVDMod(const Matrix &in, 
            Matrix &U,
            Vector &S,
            Matrix &V)
{
    U=in;
    Vector work;
    work.resize(U.cols());
    Matrix X(U.cols(), U.cols());
    gsl_linalg_SV_decomp_mod((gsl_matrix *) U.getGslMatrix(),
        (gsl_matrix *) X.getGslMatrix(),
        (gsl_matrix *) V.getGslMatrix(),
        (gsl_vector *) S.getGslVector(),
        (gsl_vector *) work.getGslVector());
}

/**
* Perform SVD decomposition on a NxM matrix. 
* Jacobi method.
*/
void yarp::math::SVDJacobi(const Matrix &in, 
            Matrix &U,
            Vector &S,
            Matrix &V)
{
    U=in;
    gsl_linalg_SV_decomp_jacobi((gsl_matrix *) U.getGslMatrix(),
        (gsl_matrix *) V.getGslMatrix(),
        (gsl_vector *) S.getGslVector());
}

