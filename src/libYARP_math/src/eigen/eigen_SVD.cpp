/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

/**
* \infile routines for Singular Value Decomposition (GSL implementations)
*/

#include <yarp/math/SVD.h>
#include <yarp/math/Math.h>

#include <yarp/eigen/Eigen.h>

#include <Eigen/SVD>

using namespace yarp::sig;
using namespace yarp::eigen;

void yarp::math::SVD(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    return SVDJacobi(in,U,S,V);
}

void yarp::math::SVDMod(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    return SVDJacobi(in,U,S,V);
}

void yarp::math::SVDJacobi(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    Eigen::JacobiSVD< Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > svd(toEigen(in), Eigen::ComputeThinU | Eigen::ComputeThinV);

    toEigen(U) = svd.matrixU();
    toEigen(S) = svd.singularValues();
    toEigen(V) = svd.matrixV();

    return;
}
