// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

/**
 * \infile routines for Singular Value Decomposition
 */

#include <yarp/math/SVD.h>
#include <gsl/gsl_linalg.h>
#include <yarp/math/Math.h>

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

/**
* Perform the moore-penrose pseudo-inverse on 
* a NxM matrix. 
* 
*/
Matrix yarp::math::pinv(const Matrix &in, double tol)
{
	int m = in.rows();
	int n = in.cols();
	Matrix U(m,n);
	Vector Sdiag(n);
	Matrix V(n,n);

	yarp::math::SVD(in, U, Sdiag, V);

	Matrix Spinv = zeros(n,n);
	for (int c=0;c<n; c++)
	{
		for(int r=0;r<n;r++)
		{
			if ( r==c && Sdiag(c)> tol)
				Spinv(r,c) = 1/Sdiag(c);
			else
				Spinv(r,c) = 0;
		}
	}
	return V*Spinv*U.transposed();
}
