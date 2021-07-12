/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef YARP_GSL_IMPL_GSL_COMPATIBILITY_H
#define YARP_GSL_IMPL_GSL_COMPATIBILITY_H

/*
 * This file defines types for binary compatibility with the GSL.
 */

#include <cstddef>


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
