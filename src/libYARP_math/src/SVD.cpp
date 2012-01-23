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
* Factorize the M-by-N matrix 'in' into the singular value decomposition in = U S V^T for M >= N.
* The diagonal elements of the singular value matrix S are stored in the vector S.
* The singular values are non-negative and form a non-increasing sequence from S_1 to S_N. 
* The matrix V contains the elements of V in untransposed form. To form the product U S V^T it 
* is necessary to take the transpose of V. This routine uses the Golub-Reinsch SVD algorithm. 
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
* Perform SVD decomposition on a MxN matrix (for M >= N).
* Modified Golub Reinsch method. Fast for M>>N.
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
* Perform SVD decomposition on a matrix using the Jacobi method. The Jacobi method
* can compute singular values to higher relative accuracy than Golub-Reinsch algorithms.
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
* Perform the moore-penrose pseudo-inverse on a MxN matrix for M >= N.
* @param in input matrix 
* @param tol singular values less than tol are set to zero
* @return pseudo-inverse of the matrix 'in'
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
		if ( Sdiag(c)> tol)
			Spinv(c,c) = 1/Sdiag(c);
	return V*Spinv*U.transposed();
}
