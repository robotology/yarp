/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2007 Giacomo Spigler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <cuda.h>
#include <cutil.h>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>

class CUDAFunction {
public:
    CUmodule module;
    CUfunction func;

    int offset;

    CUDAFunction(char *name) {
        offset=0;

        int status = cuModuleLoad(&module, name);

        status = cuModuleGetFunction(&func, module, "FragmentProgram" );
    }
};

