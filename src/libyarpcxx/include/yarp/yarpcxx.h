/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPCXX_yarpcxx_INC
#define YARPCXX_yarpcxx_INC

#include <yarp.h>
#include <stdlib.h>
#include <stdio.h>

#define YARPCXX_VALID(x) if ((x)==NULL) { fprintf(stderr, "Memory allocation failure, %s:%d\n", __FILE__, __LINE__);  exit(1); }

#endif
