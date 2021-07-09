/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2007 Giacomo Spigler
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARPGPUINTERFACES
#define YARPGPUINTERFACES

#include <yarp/dev/api.h>
#include <yarp/sig/Image.h>

/*! \file GPUInterface.h define interfaces for a generic GPU*/

namespace yarp {
    namespace dev {
        class IGPUDevice;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to GPU port devices.
 */
class YARP_dev_API yarp::dev::IGPUDevice {
public:
    virtual ~IGPUDevice() {}

    /**
     * It returns the current workspace's width
     * @return width of current workspace
     */
    virtual int width() = 0;
    /**
     * It returns the current workspace's height
     * @return height of current workspace
     */
    virtual int height() = 0;
    /**
     * It changes the dimensions of the current working space (the texture used for communication with the GPU).
     * @param width the new width
     * @param height the new height
     * @return true if successful
     */
    virtual bool resize(int width, int height) = 0;
    /**
     * It changes the number of bytes per pixel (data matrix element). Matrices are passed as 1D vectors.
     * @param bytespp the new number of bytes per pixel
     */
    virtual void changebpp(int bytespp) = 0;
    /**
     * It changes the size of each pixel (the type each pixel's element is).
     * @param newtype the new pixel's elements type (cfr. enum YarpVocabPixelTypesEnum under image.h)
     */
    virtual void changetype(int newtype) = 0;
    /**
     * Load a program for the GPU.
     * @param name the name of the program to load from the HD
     * @return an object representing the program casted to int (for compatibility purpose)
     */
    virtual int load(char *name) = 0;
    /**
     * Set a program's argument to 'val' (float).
     * @param prg a program
     * @param name the name of the argument to set
     * @param vector the value to set
     */
    virtual void setargument(int prg, char *name, float val) = 0;
    virtual void setargument(int prg, int val) = 0;
    virtual void setargument(int prg, float val) = 0;
    /**
     * Set a program's argument to 'vector' (1D vector for single values).
     * @param prg a program
     * @param name the name of the argument to set
     * @param vector the value/s to set
     * @param len the vector's length
     */
    virtual void setargument(int prg, char *name, float *vector, int len) = 0;
    /**
     * Execute a loaded program on a given matrix (eg: an image).
     * @param prg a previously loaded program
     * @param in the matrix containing the data to be processed
     * @param out the program's results
     */
    virtual void execute(int prg, unsigned char *in, unsigned char *out) = 0;
    /**
     * Execute a loaded program on 2 given matrices (eg: images).
     * @param prg a previously loaded program
     * @param in the first matrix containing the data to be processed
     * @param in2 the second matrix containing the data to be processed
     * @param out the program's results
     */
    virtual void execute(int prg, unsigned char *in, unsigned char *in2, unsigned char *out) = 0;
    /**
     * Execute a loaded program on a given matrix (eg: an image).
     * @param prg a previously loaded program
     * @param in the sig::ImageOf containing the data to be processed
     * @param out the program's results (sig::ImageOf)
     */
    virtual void execute(int prg, yarp::sig::Image *in, yarp::sig::Image *out) =  0;
    /**
     * Execute a loaded program on 2 given matrices (eg: images).
     * @param prg a previously loaded program
     * @param in the first sig::ImageOf containing the data to be processed
     * @param in2 the second sig::ImageOf containing the data to be processed
     * @param out the program's results (sig::ImageOf)
     */
    virtual void execute(int prg, yarp::sig::Image *in, yarp::sig::Image *in2, yarp::sig::Image *out) = 0;
};

#endif
