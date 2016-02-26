/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>

class NVIDIAGPU : public yarp::dev::IGPUDevice, public yarp::dev::DeviceDriver {
private:
    int w, h;
    int oglformat;
    int ogltype;
    unsigned int tex, tex2, oTex;
public:
    NVIDIAGPU() {
        h = w = 0;
    }

    bool open(int w, int h, int bytespp, int elemtype);

    virtual bool open(yarp::os::Searchable& config) { 
        // -extract width and height configuration, if present
        // otherwise use 256x256
        // -also extract the number of bytes per pixel (matrix' element) (eg: for image processing is 4 for RGBA, 3 for RGB, 1 for B/W). RGBA is the default value
        int desiredWidth = config.check("w",yarp::os::Value(256)).asInt();
        int desiredHeight = config.check("h",yarp::os::Value(256)).asInt();
        int desiredBytes = config.check("bpp",yarp::os::Value(4)).asInt();
        int desiredType =  config.check("type",yarp::os::Value(VOCAB_PIXEL_RGB)).asInt();
        return open(desiredWidth, desiredHeight, desiredBytes, desiredType);
    }

    virtual bool close() { 
        return true; // easy
    }

    virtual int width() {
      return w;
    }

    virtual int height() {
      return h;
    }

    virtual void changebpp(int bytespp) {
        if(bytespp==1) {
            oglformat = GL_LUMINANCE;
        } else if(bytespp==3) {
            oglformat = GL_RGB;
        } else if(bytespp==4) {
            oglformat = GL_RGBA;
        }
    }

    virtual void changetype(int elemtype) {
        if(elemtype==VOCAB_PIXEL_MONO || elemtype==VOCAB_PIXEL_RGB ||  elemtype==VOCAB_PIXEL_HSV || elemtype==VOCAB_PIXEL_BGR || elemtype==VOCAB_PIXEL_RGBA || elemtype==VOCAB_PIXEL_BGRA) {
          ogltype = GL_UNSIGNED_BYTE;
        } else if(elemtype==VOCAB_PIXEL_INT || elemtype==VOCAB_PIXEL_RGB_INT) {
          ogltype = GL_INT;
        } else if(elemtype==VOCAB_PIXEL_MONO_SIGNED || elemtype==VOCAB_PIXEL_RGB_SIGNED) {
          ogltype = GL_BYTE;
        } else if(elemtype==VOCAB_PIXEL_MONO_FLOAT || elemtype==VOCAB_PIXEL_RGB_FLOAT || elemtype==VOCAB_PIXEL_HSV_FLOAT) {
          ogltype = GL_FLOAT;
        }
    }

    bool resize(int width, int height);

    int load(char *name);

    void setargument(int prg, char *name, float val);
    void setargument(int prg, char *name, float *vector, int len);

    virtual void setargument(int prg, int val) { };
    virtual void setargument(int prg, float val) { };


    void execute(int prg, unsigned char *in, unsigned char *out);
    void execute(int prg, unsigned char *in, unsigned char *in2, unsigned char *out);

    void execute(int prg, yarp::sig::Image *in, yarp::sig::Image *out);
    void execute(int prg, yarp::sig::Image *in, yarp::sig::Image *in2, yarp::sig::Image *out);
};

