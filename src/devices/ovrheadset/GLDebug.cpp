/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
