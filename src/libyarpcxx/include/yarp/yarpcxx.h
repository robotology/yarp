// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPCXX_yarpcxx_INC
#define YARPCXX_yarpcxx_INC

#include <yarp.h>
#include <stdlib.h>
#include <stdio.h>

#define YARPCXX_VALID(x) if ((x)==NULL) { fprintf(stderr, "Memory allocation failure, %s:%d\n", __FILE__, __LINE__);  exit(1); }

#endif
