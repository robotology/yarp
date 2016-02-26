/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

