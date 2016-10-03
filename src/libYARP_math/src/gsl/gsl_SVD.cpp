/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

/**
* \infile routines for Singular Value Decomposition (GSL implementations)
*/

#include <yarp/math/SVD.h>
#include <gsl/gsl_linalg.h>
#include <yarp/math/Math.h>

#include <yarp/gsl/Gsl.h>

using namespace yarp::sig;
using namespace yarp::gsl;

void yarp::math::SVD(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    int m=in.rows(), n=in.cols(), k = (m<n)?m:n;
    Vector work(k);
    if((int)S.size()!=k) S.resize(k);
    if(m>=n)
    {
        U = in;
        if(V.rows()!=n || V.cols()!=k) V.resize(n,k);
        gsl_linalg_SV_decomp((gsl_matrix *) GslMatrix(U).getGslMatrix(),
            (gsl_matrix *) GslMatrix(V).getGslMatrix(),
            (gsl_vector *) GslVector(S).getGslVector(),
            (gsl_vector *) GslVector(work).getGslVector());
        return;
    }
    // since in GSL svd is not implemented for fat matrices I have to compute the svd
    // of the transpose of 'in', and swap U and V
    if(U.rows()!=m || U.cols()!=k) U.resize(m,k);
    V = in.transposed();
    gsl_linalg_SV_decomp((gsl_matrix *)GslMatrix(V).getGslMatrix(),
        (gsl_matrix *) GslMatrix(U).getGslMatrix(),
        (gsl_vector *) GslVector(S).getGslVector(),
        (gsl_vector *) GslVector(work).getGslVector());
}

void yarp::math::SVDMod(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    U=in;
    Vector work;
    work.resize(U.cols());
    Matrix X(U.cols(), U.cols());
    gsl_linalg_SV_decomp_mod((gsl_matrix *) GslMatrix(U).getGslMatrix(),
        (gsl_matrix *) GslMatrix(X).getGslMatrix(),
        (gsl_matrix *) GslMatrix(V).getGslMatrix(),
        (gsl_vector *) GslVector(S).getGslVector(),
        (gsl_vector *) GslVector(work).getGslVector());
}

void yarp::math::SVDJacobi(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    U=in;
    gsl_linalg_SV_decomp_jacobi((gsl_matrix *) GslMatrix(U).getGslMatrix(),
        (gsl_matrix *) GslMatrix(V).getGslMatrix(),
        (gsl_vector *) GslVector(S).getGslVector());
}
