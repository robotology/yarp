/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
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
