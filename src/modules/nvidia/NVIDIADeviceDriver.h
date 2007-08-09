// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 */

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>

class NVIDIAGPU : public yarp::dev::IGPUDevice, public yarp::dev::DeviceDriver {
private:
    int w, h, bpp;
    int oglformat;
    unsigned int tex, oTex;
public:
    NVIDIAGPU() {
        h = w = 0;
    }

    bool open(int w, int h, int bytespp);

    virtual bool open(yarp::os::Searchable& config) { 
        // -extract width and height configuration, if present
        // otherwise use 256x256
        // -also extract the number of bytes per pixel (matrix' element) (eg: for image processing is 4 for RGBA, 3 for RGB, 1 for B/W). RGBA is the default value
        int desiredWidth = config.check("w",yarp::os::Value(256)).asInt();
        int desiredHeight = config.check("h",yarp::os::Value(256)).asInt();
        int desiredBytes = config.check("bpp",yarp::os::Value(4)).asInt();
        return open(desiredWidth,desiredHeight, desiredBytes);
    }

    virtual bool close() { 
        return true; // easy
    }

    virtual void changebpp(int bytespp) {
      this->bpp=bytespp;
    }

    bool resize(int width, int height);

    int load(char *name);

    void setargument(int prg, char *name, float *vector, int len);

    void execute(int prg, unsigned char *in, unsigned char *out);
};

