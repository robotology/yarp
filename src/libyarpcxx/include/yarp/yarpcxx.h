/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPCXX_yarpcxx_INC
#define YARPCXX_yarpcxx_INC

#include <yarp.h>
#include <cstdlib>
#include <cstdio>

#define YARPCXX_VALID(x) if ((x)==NULL) { fprintf(stderr, "Memory allocation failure, %s:%d\n", __FILE__, __LINE__);  std::exit(1); }

#endif
