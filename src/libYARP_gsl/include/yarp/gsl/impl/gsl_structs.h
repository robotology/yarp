/*
* Copyright (C) 2007 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the GPLv2 or later
*/

/**
* Defines types for binary compatibily with the GSL.
*/

#ifndef YARP_GSL_IMPL_GSL_COMPATIBILITY_H
#define YARP_GSL_IMPL_GSL_COMPATIBILITY_H

#include <stddef.h>


#ifndef gsl_block
typedef struct
{
  size_t size;
  double *data;
} gsl_block;
#endif

#ifndef gsl_vector
struct gsl_vector
{
  size_t size;
  size_t stride;
  double *data;
  gsl_block *block;
  int owner;
};
#endif

#ifndef gsl_matrix
struct gsl_matrix
{
  size_t size1;
  size_t size2;
  size_t tda;
  double * data;
  gsl_block * block;
  int owner;
};
#endif

#endif // YARP_GSL_IMPL_GSL_COMPATIBILITY_H
