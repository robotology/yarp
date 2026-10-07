// SPDX-FileCopyrightText: 2006-2026 Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

//////////////////////////////////////////////////////////////////////////
//
// SWIG wrapping for the YARP_sig library.
//
// This file is not a standalone module: it is meant to be included by
// yarp.i, after YARP_os has been wrapped and before YARP_dev.
// yarp::sig::Vector and yarp::sig::VectorInt are wrapped separately in
// yarp_sig_vector.i (see the comments in that file).

// YARP_sig
%{
#include <yarp/sig/api.h>
%}

%import <yarp/sig/api.h>

// operator() and pixel() const overloads do not get translated well
%ignore yarp::sig::Image::operator()(int,int) const;
%ignore yarp::sig::Image::pixel(int,int) const;
%ignore yarp::sig::Image::getRow(int) const;
%ignore yarp::sig::Image::getReadType() const;

#ifdef SWIGPYTHON
%{
#include <Python.h>

void setExternal(yarp::sig::Image *img, PyObject* mem, int w, int h) {
#if PY_VERSION_HEX >= 0x02070000
        Py_buffer img_buffer;
        int reply;
        reply = PyObject_GetBuffer(mem, &img_buffer, PyBUF_SIMPLE);
        // exit if the buffer could not be read
        if (reply != 0)
        {
            fprintf(stderr, "Could not read Python buffers: error %d\n", reply);
            return;
        }
        img->setExternal((void*)img_buffer.buf, w, h);
        // release the Python buffers
        PyBuffer_Release(&img_buffer);
#else
        fprintf(stderr, "Your python version is not supported\n");
#endif
}

void setExternal2(yarp::sig::Image *img, PyObject* mem, int w, int h) {
        setExternal(img,mem,w,h);
}

%}
#endif

// Define typemaps for Matrix before including it
#ifdef SWIGPYTHON
%include "typemaps.i"
%typemap(in) (int matrix_i_row, int matrix_i_col) {
    if (!PyTuple_Check($input)) {
        PyErr_SetString(PyExc_ValueError, "Error: expecting a tuple (m[1,2] is equivalent to m[(1,2)])");
        return NULL;
    }

    if (PyTuple_Size($input) != 2 ) {
        PyErr_SetString(PyExc_ValueError, "Matrix elements are accessed by using two integers m[i_row,i_col]");
        return NULL;
    }

    $1 = (int)PyLong_AsLong(PyTuple_GetItem($input,0));   /* int i */
    $2 = (int)PyLong_AsLong(PyTuple_GetItem($input,1));   /* int j */
};
#endif

%include <yarp/sig/Image.h>
%include <yarp/sig/ImageFile.h>
%include <yarp/sig/Sound.h>
%include <yarp/sig/SoundFile.h>
%include <yarp/sig/SoundUtils.h>
%include <yarp/sig/Matrix.h>
%include <yarp/sig/Pose6D.h>
%include <yarp/sig/ColorRGB.h>
%include <yarp/sig/CameraDistortionType.h>
%include <yarp/sig/DistortionModelData.h>
%include <yarp/sig/IntrinsicParamsData.h>
%include <yarp/sig/IntrinsicParams.h>

//////////////////////////////////////////////////////////////////////////
// Deal with some templated classes
//////////////////////////////////////////////////////////////////////////
//%template(Vector) yarp::sig::VectorOf<double>;
//%template(VectorInt) yarp::sig::VectorOf<int>;
//%feature("notabstract") Vector;
//%feature("notabstract") VectorInt;

//These definitions are for SWIG
//typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageRgb;
//typedef yarp::sig::ImageOf<yarp::sig::PixelRgba> ImageRgba;
//typedef yarp::sig::ImageOf<yarp::sig::PixelMono> ImageMono;
//typedef yarp::sig::ImageOf<yarp::sig::PixelMono16> ImageMono16;
//typedef yarp::sig::ImageOf<yarp::sig::PixelInt> ImageInt;
//typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> ImageFloat;
//typedef yarp::sig::ImageOf<yarp::sig::PixelRgbFloat> ImageRgbFloat;
typedef yarp::sig::Matrix Matrix;

//These definitions are for C++
%{
typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageRgb;
typedef yarp::sig::ImageOf<yarp::sig::PixelRgba> ImageRgba;
typedef yarp::sig::ImageOf<yarp::sig::PixelMono> ImageMono;
typedef yarp::sig::ImageOf<yarp::sig::PixelMono16> ImageMono16;
typedef yarp::sig::ImageOf<yarp::sig::PixelInt> ImageInt;
typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> ImageFloat;
typedef yarp::sig::ImageOf<yarp::sig::PixelRgbFloat> ImageRgbFloat;
typedef yarp::sig::Sound  Sound;
typedef yarp::sig::Matrix Matrix;
%}

MAKE_COMMS2 (ImageRgb, yarp::sig::ImageOf<yarp::sig::PixelRgb>)
MAKE_COMMS2 (ImageRgba, yarp::sig::ImageOf<yarp::sig::PixelRgba>)
MAKE_COMMS2 (ImageMono, yarp::sig::ImageOf<yarp::sig::PixelMono>)
MAKE_COMMS2 (ImageMono16, yarp::sig::ImageOf<yarp::sig::PixelMono16>)
MAKE_COMMS2 (ImageInt, yarp::sig::ImageOf<yarp::sig::PixelInt>)
MAKE_COMMS2 (ImageFloat, yarp::sig::ImageOf<yarp::sig::PixelFloat>)
MAKE_COMMS2 (ImageRgbFloat, yarp::sig::ImageOf<yarp::sig::PixelRgbFloat>)
MAKE_COMMS  (Matrix, yarp::sig::Matrix)
MAKE_COMMS  (Sound, yarp::sig::Sound)

// Add getPixel and setPixel methods to access float values
%extend yarp::sig::ImageOf<yarp::sig::PixelFloat> {
   float getPixel(int x, int y) {
       return self->pixel(x,y);
       }

   void setPixel(int x, int y, float v) {
       self->pixel(x,y) = v;
       }
}

// Add getPixel and setPixel methods to access int values
%extend yarp::sig::ImageOf<yarp::sig::PixelInt> {
    int getPixel(int x, int y) {
        return self->pixel(x,y);
    }

    void setPixel(int x, int y, int v) {
        self->pixel(x,y) = v;
    }
}

%extend yarp::sig::Sound{
    std::vector<short int> sound2VecNonInterleaved()
    {
        int samples=self->getSamples();
        int channels=self->getChannels();
        std::vector<short int> vec;
        vec.reserve(samples*channels);
        for (size_t c = 0; c < channels; c++)
        {
            for (size_t t = 0; t < samples; t++)
            {
                vec.push_back(self->get(t, c));
            }
        }
        return vec;
    }

    void vecNonInterleaved2Sound(std::vector<short int> vec,int samples,int channels)
    {
        for (size_t c = 0; c < channels; c++)
        {
            for (size_t t = 0; t <samples; t++)
            {
                self->set(vec[t+samples*c],t, c);
            }
        }
        return;
    }

    std::vector<short int> sound2VecInterleaved()
    {
        int samples=self->getSamples();
        int channels=self->getChannels();

        std::vector<short int> vec;
        vec.reserve(samples*channels);
        for (size_t t = 0; t < samples; t++)
        {
            for (size_t c = 0; c < channels; c++)
            {
                vec.push_back(self->get(t, c));
            }
        }
        return vec;
    }

    void vecInterleaved2Sound(std::vector<short int> vec,int samples,int channels)
    {
        for (size_t t = 0; t < channels; t++)
        {
            for (size_t c = 0; c <samples; c++)
            {
                self->set(vec[c+t*channels],t, c);
            }
        }
        return;
    }
}

// Add getPixel and setPixel methods to access float values
// %extend yarp::sig::ImageOf<yarp::sig::PixelRgbFloat> {
//    float getPixel(int x, int y) {
//        return self->pixel(x,y);
//        }
//
//    void setPixel(int x, int y, float v) {
//        self->pixel(x,y) = v;
//        }
// }

%extend yarp::sig::Matrix {

    double get(int i, int j)
    {
        return self->operator ()(i, j);
    }

    void set(int i, int j, double v)
    {
        self->operator ()(i,j) = v;
    }


#ifdef SWIGPYTHON
    void __setitem__(int matrix_i_row, int matrix_i_col, double value) {
        self->operator ()(matrix_i_row,matrix_i_col) = value;
    }

    double __getitem__(int matrix_i_row, int matrix_i_col) {
        return self->operator ()(matrix_i_row,matrix_i_col);
    }
#endif
}

#ifdef SWIGPYTHON

// Contributed by Arnaud Degroote for MORSE
// Conversion of Python buffer type object into a pointer
%extend yarp::sig::Image {
    void setExternal(PyObject* mem, int w, int h) {
      ::setExternal(self,mem,w,h);
    }
    void setExternal2(PyObject* mem, int w, int h) {
      ::setExternal2(self,mem,w,h);
    }
}

%extend yarp::sig::Image {
  std::string tostring() const {
    return std::string((const char *)self->getRawImage(),
               (size_t)self->getRawImageSize());
  }

  // no copy, make sure to keep string alive
  void fromstring(const std::string& str, int w, int h) {
    self->setExternal((char *)str.c_str(),w,h);
  }
}

#endif

#ifdef SWIGJAVA

/*

Contributed by Leo Pape

Motivation: I found that the Java interface (with SWIG) to YARP is
very slow for image transfer. This is because SWIG only allows for
direct access to primitives, not arrays. The current solution is to
treat an image as a collection of pixels, where each pixel is a Java
object. Transferring a simple 320x240-pixel image from YARP through
the Java Native Interface (JNI) to Java is very slow, and can take up
to 1 second.

*/

%include "carrays.i"
%array_class(unsigned char, charArray);

/**
 * EXAMPLE JAVA METHOD:
 *
 * Converts color YARP image into a vector.
 * Returns a [H*W*P] vector which contains the 'justaposition' of the
 * three color planes of the image. This array can be copied into a
 * Matlab matrix:
 * From OUT you can create a Matlab image [HxWxP] by typing:
 * IMG = reshape(uint8(OUT), [H W P]);
 */

/*
public static short[] getRawImg(Image img) {
  int pixelsize = img.getPixelSize();
  int width = img.width();
  int height = img.height();
  int imgsize = img.getRawImageSize();
  short [] vec1ds = new short [imgsize];

  charArray car = charArray.frompointer(img.getRawImage());

  // in MATLAB, USE: reshape(OUT, [height width pixelsize]);
  for(int r=0; r<height; r++)
    for(int c=0; c<width; c++)
      for(int p=0; p<pixelsize; p++)
    vec1ds[(c * height) + r + (p * width * height)] = (short) car.getitem((r * width * pixelsize) + (c * pixelsize) + p);
  return vec1ds;
}
*/

#endif
