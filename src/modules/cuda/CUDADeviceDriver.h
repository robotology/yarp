/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <GL/glew.h>
#include <GL/gl.h>

//#include <cuda.h>
//#include <cutil.h>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>

class CUDAGPU : public yarp::dev::IGPUDevice, public yarp::dev::DeviceDriver {
private:
    int w, h;
    int oglformat;
    int ogltype;
    unsigned int tex, tex2, oTex;
    unsigned int size;
public:
    CUDAGPU() {
        h = w = 0;
    }

    ~CUDAGPU() {
        this->close();
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

    bool close();

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

    //This just loads a module. The main function's name is "FragmentProgram"
    //TODO: load(name, const) where optionalarg specifies a value: ie, if it is set to 0, it just loads the module. if it is set to 1, it accepts another optional argument, specifying the name of the function to load. Faster and more efficient way.
    int load(char *name);

    void setargument(int prg, int val);
    void setargument(int prg, float val);

    //THE LAST FRAGMENTPROGRAMS' ARGUMENTS MUST *ALWAYS* BE 'width', 'height', 'in' AND 'out' arrays
    void execute(int prg, unsigned char *in, unsigned char *out);
    void execute(int prg, yarp::sig::Image *in, yarp::sig::Image *out);

    //THE LAST FRAGMENTPROGRAMS' ARGUMENTS MUST *ALWAYS* BE 'width', 'height', 'in', 'in2' AND 'out' arrays
    void execute(int prg, unsigned char *in, unsigned char *in2, unsigned char *out);
    void execute(int prg, yarp::sig::Image *in, yarp::sig::Image *in2, yarp::sig::Image *out);


    //IGNORE THESE ONES
    virtual void setargument(int prg, char *name, float val) { }
    virtual void setargument(int prg, char *name, float *vector, int len) { };
};



