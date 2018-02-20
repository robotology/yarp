/*
 * Copyright (C) 2015-2017 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#include "GLDebug.h"

#include <yarp/os/LogStream.h>
#include <GL/glew.h>

void yarp::dev::checkGlError(const char* file, int line, const char* func)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        yarp::os::LogStream ls = yarp::os::Log(file, line, func).error();
        switch (error) {
        case GL_INVALID_ENUM:
            ls << "OpenGL Error GL_INVALID_ENUM: GLenum argument out of range";
            break;
        case GL_INVALID_VALUE:
            ls << "OpenGL Error GL_INVALID_VALUE: Numeric argument out of range";
            break;
        case GL_INVALID_OPERATION:
            ls << "OpenGL Error GL_INVALID_OPERATION: Operation illegal in current state";
            break;
        case GL_STACK_OVERFLOW:
            ls << "OpenGL Error GL_STACK_OVERFLOW: Command would cause a stack overflow";
            break;
        case GL_OUT_OF_MEMORY:
            ls << "OpenGL Error GL_OUT_OF_MEMORY: Not enough memory left to execute command";
            break;
        default:
            ls << "OpenGL Error " << error;
            break;
        }
    }
    if (!(error == 0)) { yarp::os::Log(file, line, func).fatal("OpenGL Error at %s:%d (%s)", file, line, func); }
}
