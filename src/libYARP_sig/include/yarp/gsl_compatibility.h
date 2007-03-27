// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007 Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// $Id: gsl_compatibility.h,v 1.3 2007-03-27 15:06:49 babybot Exp $

/** 
* Defines types for binary compatibily with the GSL. Included by cpp 
* files within yarp.
*/

/// GSL TYPES
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
///
