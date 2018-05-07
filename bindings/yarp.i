// Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
// Copyright (C) 2006-2010 RobotCub Consortium
// All rights reserved.
//
// This software may be modified and distributed under the terms of the
// BSD-3-Clause license. See the accompanying LICENSE file for details.

//////////////////////////////////////////////////////////////////////////
//
// This is a configuration file to explain YARP to SWIG
//
// SWIG, for the most part, understands YARP auto-magically.
// There are a few things that need to be explained:
//  + use of multiple inheritance
//  + use of names that clash with special names in Java/Python/Perl/...
//  + use of templates

%module(directors="1") yarp

%define SWIG_PREPROCESSOR_SHOULD_SKIP_THIS %enddef

%import "yarp/conf/api.h"

#if !defined (SWIGMATLAB)
%feature("director") yarp::os::PortReader;
%feature("director") yarp::os::RFModule;
%feature("director") yarp::os::Thread;
#endif

%feature("autodoc", "1");

#if defined (SWIGPYTHON) || defined (SWIGRUBY)
  %include <argcargv.i>
  %apply (int ARGC, char **ARGV) { (int argc, char *argv[]) }
#elif defined (SWIGLUA)
  %include "lua/argcargv.i"
  %apply (int ARGC, char **ARGV) { (int argc, char *argv[]) }
#endif


#if !defined(SWIGCHICKEN) && !defined(SWIGALLEGROCL)
  %include "std_vector.i"
#endif

// Try to make yarp::os::ConstString act like std::string
// Try to translate std::string to native equivalents
%include "std_string.i"
%typemaps_std_string(yarp::os::ConstString, char, SWIG_AsCharPtrAndSize,
             SWIG_FromCharPtrAndSize, %checkcode(STDSTRING));

#if defined(SWIGCSHARP)
    // Get .NET pointers instead of swig generated types (useful when dealing with images)
    %typemap(ctype)  unsigned char * "unsigned char *"
    %typemap(imtype) unsigned char * "System.IntPtr"
    %typemap(cstype) unsigned char * "System.IntPtr"
    %typemap(csin)   unsigned char * "$csinput"
    %typemap(in)     unsigned char * %{ $1 = $input; %}
    %typemap(out)    unsigned char * %{ $result = $1; %}
    %typemap(csout)  unsigned char * { return $imcall; }

    // there's a big CSHARP virtual/override muddle
    // we just bypass the issue for now
    %csmethodmodifiers write "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers find "public new";
    %csmethodmodifiers findGroup "public new";
    %csmethodmodifiers toString "public new";
    %csmethodmodifiers lastRead "public new";
    %csmethodmodifiers isClosed "public new";
    %csmethodmodifiers read "public new";
    %csmethodmodifiers setReplier "public new";
    %csmethodmodifiers onRead "public virtual";
    %csmethodmodifiers getPendingReads "public new";
    %csmethodmodifiers setStrict "public new";
    %csmethodmodifiers useCallback "public new";
    %csmethodmodifiers onCommencement "public virtual";
    %csmethodmodifiers disableCallback "public virtual";
    %csmethodmodifiers acquire "public virtual";
    %csmethodmodifiers release "public virtual";
    %csmethodmodifiers isNull "public virtual";
    %csmethodmodifiers setTargetPeriod "public new";
#endif

// Deal with abstract base class problems, where SWIG guesses
// incorrectly at whether a class can be instantiated or not
%feature("notabstract") Port;
%feature("notabstract") Value;
%feature("notabstract") BufferedPort;
%feature("notabstract") Bottle;
%feature("notabstract") Property;
%feature("notabstract") Stamp;
%feature("notabstract") RpcClient;
%feature("notabstract") RpcServer;
%feature("abstract") Portable;
%feature("abstract") PortReader;
%feature("abstract") PortWriter;
%feature("abstract") Searchable;
%feature("abstract") Contactable;
%feature("abstract") UnbufferedContactable;
%feature("abstract") AbstractContactable;

// Deal with overridden method clashes, simply by ignoring them.
// At some point, these methods should get renamed so they are still
// available.
%ignore *::check(const ConstString& key, Value *& result) const;
%ignore *::check(const ConstString& key, Value *& result, const ConstString& comment) const;
%rename(where_c) *::where();
%rename(seed_c) *::seed(int seed);  // perl clash
%ignore *::setKp(double);
%ignore *::setKi(double);
%ignore *::setKd(double);
%ignore *::setKff(double);
%ignore *::setScale(double);
%ignore *::setOffset(double);
%rename(attach_rpc_server) *::attach(yarp::os::RpcServer&);
%rename(open_str) yarp::dev::PolyDriver::open(const yarp::os::ConstString& txt);

#if defined(SWIGCSHARP)
    // there's a big CSHARP virtual/override muddle
    // we just bypass the issue for now
    %csmethodmodifiers write "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers find "public new";
    %csmethodmodifiers findGroup "public new";
    %csmethodmodifiers toString "public new";
    %csmethodmodifiers lastRead "public new";
    %csmethodmodifiers isClosed "public new";
    %csmethodmodifiers read "public new";
    %csmethodmodifiers setReplier "public new";
    %csmethodmodifiers onRead "public new";
    %csmethodmodifiers getPendingReads "public new";
    %csmethodmodifiers setStrict "public new";
    %csmethodmodifiers useCallback "public new";
#endif

// Deal with method name conflicts
#ifndef SWIGJAVA
    %rename(toString_c) *::toString() const;
#endif

// python conflict
#ifdef SWIGPYTHON
    %rename(yield_c) *::yield();
#endif

// java conflict
#ifdef SWIGJAVA
    %rename(wait_c) *::wait();
    %rename(clone_c) *::clone() const;
    %rename(toString_c) *::toString();
#endif

#ifdef SWIGCHICKEN
    // small warning on chicken
    %rename(delay_c) *::delay();
#endif

#ifdef SWIGTCL
    // small warning on chicken
    %rename(configure_c) *::configure();
#endif

//////////////////////////////////////////////////////////////////////////
// Clean up a few unimportant things that give warnings

// abstract methods just confuse SWIG
// We must not do this for allegro
#if !defined(SWIGALLEGROCL)
  #if !defined(SWIGCHICKEN)
    %ignore yarp::os::BufferedPort::open; // let Contactable::open show
    %ignore yarp::os::Port::open; // let Contactable::open show
    %ignore yarp::os::RpcClient::open;
  #endif
#endif
// operator= does not get translated well
%ignore *::operator=;
%ignore yarp::PortReaderBuffer;
%ignore yarp::sig::Image::operator()(int,int) const;
%ignore yarp::sig::Image::pixel(int,int) const;
%ignore yarp::sig::Image::getRow(int) const;
%ignore yarp::sig::Image::getIplImage() const;
%ignore yarp::sig::Image::getReadType();
%ignore yarp::sig::Vector::getType();
%ignore yarp::os::Property::put(const char *,Value *);
%ignore yarp::os::Bottle::add(Value *);
%rename(toString) yarp::os::ConstString::operator const char *() const;
%rename(isEqual) *::operator==;
%rename(notEqual) *::operator!=;
%rename(access) *::operator();
//%ignore yarp::os::PortReader::read;

// Deal with some shadowing in python
#ifdef SWIGPYTHON
%ignore yarp::os::Property::fromCommand(int, char const *[]);
%ignore yarp::os::Property::fromCommand(int, char const *[], bool);
%ignore yarp::os::Property::fromCommand(int, char const *[], bool, bool);
#endif

// Deal with some clash in perl involving the name "seed"
%{
#define _SEARCH_H // strange perl clash
// careful shuffling to deal with perl clash on seed name
#ifdef seed
#define seed_c seed
#undef seed
#endif
#include <yarp/os/Random.h>
#ifdef seed_c
#define seed seed_c
#endif

// Bring in the header files that are important to us
#include <vector>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>


// Sometimes ACE redefines main() - we don't want that
#ifdef main
#undef main
#endif

// Bring in the main important namespace
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;
using namespace yarp::dev;
%}


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


// Now we parse the original header files
// Redefine a few things that SWIG currently chokes on
%define YARP_OS_API
%enddef
%define YARP_sig_API
%enddef
%define YARP_dev_API
%enddef
%define YARP_init_API
%enddef
%define YARP_OS_DEPRECATED_API
%enddef

%define _YARP2_NETINT32_
%enddef
namespace yarp {
  namespace os {
    typedef int NetInt32;
  }
 }

#if defined(SWIGCSHARP)
    %define YARP_OS_VOCAB_H 1
    %enddef
    %define VOCAB(a,b,c,d) 0
    %enddef
    %define VOCAB4(a,b,c,d) VOCAB((a),(b),(c),(d))
    %enddef
    %define VOCAB3(a,b,c) VOCAB((a),(b),(c),(0))
    %enddef
    %define VOCAB2(a,b) VOCAB((a),(b),(0),(0))
    %enddef
    %define VOCAB1(a) VOCAB((a),(0),(0),(0))
    %enddef
#endif

%define YARP_BEGIN_PACK
%enddef
%define YARP_END_PACK
%enddef

#if defined( SWIGALLEGROCL )
  %include "allegro/compat.h"
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

    $1 = (int)PyInt_AsLong(PyTuple_GetItem($input,0));   /* int i */
    $2 = (int)PyInt_AsLong(PyTuple_GetItem($input,1));   /* int j */
};
#endif

%include <yarp/os/ConstString.h>
%include <yarp/os/PortReport.h>
%include <yarp/os/Contact.h>
%include <yarp/os/ConnectionReader.h>
%include <yarp/os/ConnectionWriter.h>
%include <yarp/os/PortReader.h>
%include <yarp/os/PortWriter.h>
%include <yarp/os/Portable.h>
%include <yarp/os/Searchable.h>
%include <yarp/os/Value.h>
%include <yarp/os/Vocab.h>
%include <yarp/os/BinPortable.h>
%include <yarp/os/BufferedPort.h>
%include <yarp/os/Contact.h>
%include <yarp/os/Contactable.h>
%include <yarp/os/UnbufferedContactable.h>
%include <yarp/os/Port.h>
%include <yarp/os/AbstractContactable.h>
%include <yarp/os/Contact.h>
%include <yarp/os/Network.h>
%include <yarp/os/PortablePair.h>
%include <yarp/os/PortReaderCreator.h>
%include <yarp/os/Property.h>
%include <yarp/os/Bottle.h>
%include <yarp/os/TypedReader.h>
%include <yarp/os/TypedReaderCallback.h>
%include <yarp/os/TypedReaderThread.h>
%include <yarp/os/PortReaderBuffer.h>
%include <yarp/os/PortWriterBuffer.h>
%include <yarp/os/Random.h>
%include <yarp/os/Searchable.h>
%include <yarp/os/Semaphore.h>
%include <yarp/os/Thread.h>
%include <yarp/os/RateThread.h>
%include <yarp/os/Time.h>
%include <yarp/os/RFModule.h>
%include <yarp/os/Stamp.h>
%include <yarp/os/NameStore.h>
%include <yarp/os/Searchable.h>
%include <yarp/os/ContactStyle.h>
%include <yarp/os/ResourceFinder.h>
%include <yarp/os/RpcServer.h>
%include <yarp/os/RpcClient.h>
%include <yarp/os/DummyConnector.h>
%include <yarp/os/Things.h>
%include <yarp/os/QosStyle.h>
%include <yarp/os/Log.h>
%include <yarp/os/LogStream.h>

%define MAKE_COMMS(name)
%feature("notabstract") yarp::os::BufferedPort<name>;
%feature("notabstract") BufferedPort ## name;

#if !defined (SWIGMATLAB)
%feature("director") yarp::os::TypedReaderCallback<name>;
%feature("director") yarp::os::TypedReaderCallback<yarp::os::name>;
#endif

%template(TypedReader ## name) yarp::os::TypedReader<name>;
%template(name ## Callback) yarp::os::TypedReaderCallback<name>;
%template(BufferedPort ## name) yarp::os::BufferedPort<name>;
%feature("notabstract") yarp::os::BufferedPort<name>;
%feature("notabstract") BufferedPort ## name;
%enddef

MAKE_COMMS(Property)
MAKE_COMMS(Bottle)

%include <yarp/sig/Image.h>
%include <yarp/sig/ImageFile.h>
%include <yarp/sig/Sound.h>
%include <yarp/sig/Matrix.h>
%include <yarp/sig/Vector.h>
%include <yarp/os/IConfig.h>
%include <yarp/dev/DeviceDriver.h>
%include <yarp/dev/PolyDriver.h>
%include <yarp/dev/Drivers.h>
%include <yarp/dev/FrameGrabberInterfaces.h>
%include <yarp/dev/AudioVisualInterfaces.h>
%include <yarp/dev/ControlBoardInterfaces.h>
%include <yarp/dev/ControlBoardPid.h>
%include <yarp/dev/CartesianControl.h>
%include <yarp/dev/GazeControl.h>
%include <yarp/dev/IPositionControl.h>
%include <yarp/dev/IEncoders.h>
%include <yarp/dev/CalibratorInterfaces.h>
%include <yarp/dev/ControlBoardPid.h>
%include <yarp/dev/IControlMode.h>
%include <yarp/dev/IControlMode2.h>
%include <yarp/dev/IEncoders.h>
%include <yarp/dev/IMotorEncoders.h>
%include <yarp/dev/ITorqueControl.h>
%include <yarp/dev/IImpedanceControl.h>
%include <yarp/dev/IVelocityControl.h>
%include <yarp/dev/IPWMControl.h>
%include <yarp/dev/ICurrentControl.h>
%include <yarp/dev/IAnalogSensor.h>
%include <yarp/dev/IRemoteVariables.h>
%include <yarp/dev/FrameGrabberControl2.h>
%include <yarp/dev/IPidControl.h>
%include <yarp/dev/IPositionDirect.h>

#if !defined(SWIGCHICKEN) && !defined(SWIGALLEGROCL)
  %template(DVector) std::vector<double>;
  %template(BVector) std::vector<bool>;
  %template(SVector) std::vector<std::string>;
#ifdef SWIGMATLAB
  // Extend IVector for handling conversion of vectors from and to Matlab
  %include "matlab/IVector_fromTo_matlab.i"
#endif
  %template(IVector) std::vector<int>;

  #if defined(SWIGCSHARP)
      SWIG_STD_VECTOR_SPECIALIZE_MINIMUM(Pid,yarp::dev::Pid)
  #endif
  %template(PidVector) std::vector<yarp::dev::Pid>;
#endif

//////////////////////////////////////////////////////////////////////////
// Match Java toString behaviour

%extend yarp::os::Bottle {
    std::string toString() {
        return self->toString().c_str();
    }
}

%extend yarp::os::Value {
    std::string toString() {
        return self->toString().c_str();
    }
}

%extend yarp::os::Property {
    std::string toString() {
        return self->toString().c_str();
    }
}



//////////////////////////////////////////////////////////////////////////
// Deal with some templated classes
//
// We have to shuffle things around a little bit
//   ImageRgb = ImageOf<PixelRgb>
//   BufferedPortImageRgb = BufferedPort<ImageOf<PixelRgb> >
//   BufferedPortBottle = BufferedPort<Bottle>
//   BufferedPortProperty = BufferedPort<Property>
//   BufferedPortImageRgbFloat = BufferedPort<ImageOf<PixelRgbFloat> >

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageRgb;
typedef yarp::os::TypedReader<ImageRgb> TypedReaderImageRgb;
typedef yarp::os::TypedReaderCallback<ImageRgb> TypedReaderCallbackImageRgb;
typedef yarp::os::BufferedPort<ImageRgb> BufferedPortImageRgb;
%}

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelRgba> ImageRgba;
typedef yarp::os::TypedReader<ImageRgba> TypedReaderImageRgba;
typedef yarp::os::TypedReaderCallback<ImageRgba> TypedReaderCallbackImageRgba;
typedef yarp::os::BufferedPort<ImageRgba> BufferedPortImageRgba;
%}

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelMono> ImageMono;
typedef yarp::os::TypedReader<ImageMono> TypedReaderImageMono;
typedef yarp::os::TypedReaderCallback<ImageMono> TypedReaderCallbackImageMono;
typedef yarp::os::BufferedPort<ImageMono> BufferedPortImageMono;
%}

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelMono16> ImageMono16;
typedef yarp::os::TypedReader<ImageMono16> TypedReaderImageMono16;
typedef yarp::os::TypedReaderCallback<ImageMono16> TypedReaderCallbackImageMono16;
typedef yarp::os::BufferedPort<ImageMono16> BufferedPortImageMono16;
%}

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelInt> ImageInt;
typedef yarp::os::TypedReader<ImageInt> TypedReaderImageInt;
typedef yarp::os::TypedReaderCallback<ImageInt> TypedReaderCallbackImageInt;
typedef yarp::os::BufferedPort<ImageInt> BufferedPortImageInt;
%}

%{
typedef yarp::os::TypedReader<Sound> TypedReaderSound;
typedef yarp::os::TypedReaderCallback<Sound> TypedReaderCallbackSound;
typedef yarp::os::BufferedPort<Sound> BufferedPortSound;
%}

%{
typedef yarp::os::TypedReader<yarp::sig::Vector> TypedReaderVector;
typedef yarp::os::TypedReaderCallback<yarp::sig::Vector> TypedReaderCallbackVector;
typedef yarp::os::BufferedPort<yarp::sig::Vector> BufferedPortVector;
%}

%feature("notabstract") ImageRgb;
%feature("notabstract") yarp::os::BufferedPort<ImageRgb>;
%feature("notabstract") BufferedPortImageRgb;

%feature("notabstract") ImageRgba;
%feature("notabstract") yarp::os::BufferedPort<ImageRgba>;
%feature("notabstract") BufferedPortImageRgba;

%feature("notabstract") ImageMono;
%feature("notabstract") yarp::os::BufferedPort<ImageMono>;
%feature("notabstract") BufferedPortImageMono;

%feature("notabstract") ImageMono16;
%feature("notabstract") yarp::os::BufferedPort<ImageMono16>;
%feature("notabstract") BufferedPortImageMono16;

%feature("notabstract") ImageInt;
%feature("notabstract") yarp::os::BufferedPort<ImageInt>;
%feature("notabstract") BufferedPortImageInt;

%feature("notabstract") Sound;
%feature("notabstract") yarp::os::BufferedPort<Sound>;
%feature("notabstract") BufferedPortSound;

%feature("notabstract") Vector;
%feature("notabstract") yarp::os::BufferedPort<Vector>;
%feature("notabstract") BufferedPortVector;

%template(ImageRgb) yarp::sig::ImageOf<yarp::sig::PixelRgb>;
%template(TypedReaderImageRgb) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;
%template(TypedReaderCallbackImageRgb) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;
%template(BufferedPortImageRgb) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;

%template(ImageRgba) yarp::sig::ImageOf<yarp::sig::PixelRgba>;
%template(TypedReaderImageRgba) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelRgba> >;
%template(TypedReaderCallbackImageRgba) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelRgba> >;
%template(BufferedPortImageRgba) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgba> >;

%template(ImageMono) yarp::sig::ImageOf<yarp::sig::PixelMono>;
%template(TypedReaderImageMono) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelMono> >;
%template(TypedReaderCallbackImageMono) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelMono> >;
%template(BufferedPortImageMono) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono> >;

%template(ImageMono16) yarp::sig::ImageOf<yarp::sig::PixelMono16>;
%template(TypedReaderImageMono16) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelMono16> >;
%template(TypedReaderCallbackImageMono16) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelMono16> >;
%template(BufferedPortImageMono16) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono16> >;

%template(ImageInt) yarp::sig::ImageOf<yarp::sig::PixelInt>;
%template(TypedReaderImageInt) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelInt> >;
%template(TypedReaderCallbackImageInt) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelInt> >;
%template(BufferedPortImageInt) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> >;

#if !defined (SWIGMATLAB)
%feature("director") yarp::os::TypedReaderCallback<Sound>;
%feature("director") yarp::os::TypedReaderCallback<yarp::sig::Sound>;
#endif
%template(TypedReaderSound) yarp::os::TypedReader<yarp::sig::Sound >;
%template(TypedReaderCallbackSound) yarp::os::TypedReaderCallback<yarp::sig::Sound>;
%template(BufferedPortSound) yarp::os::BufferedPort<yarp::sig::Sound >;

%template(TypedReaderVector) yarp::os::TypedReader<yarp::sig::Vector >;
%template(TypedReaderCallbackVector) yarp::os::TypedReaderCallback<yarp::sig::Vector>;
%template(BufferedPortVector) yarp::os::BufferedPort<yarp::sig::Vector >;

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

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> ImageFloat;
typedef yarp::os::TypedReader<ImageFloat> TypedReaderImageFloat;
typedef yarp::os::TypedReaderCallback<ImageFloat> TypedReaderCallbackImageFloat;
typedef yarp::os::BufferedPort<ImageFloat> BufferedPortImageFloat;
%}

%feature("notabstract") ImageFloat;
%feature("notabstract") yarp::os::BufferedPort<ImageFloat>;
%feature("notabstract") BufferedPortImageFloat;

%template(ImageFloat) yarp::sig::ImageOf<yarp::sig::PixelFloat>;
%template(TypedReaderImageFloat) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelFloat> >;
%template(TypedReaderCallbackImageFloat) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelFloat> >;
%template(BufferedPortImageFloat) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelFloat> >;

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

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelRgbFloat> ImageRgbFloat;
typedef yarp::os::TypedReader<ImageRgbFloat> TypedReaderImageRgbFloat;
typedef yarp::os::TypedReaderCallback<ImageRgbFloat> TypedReaderCallbackImageRgbFloat;
typedef yarp::os::BufferedPort<ImageRgbFloat> BufferedPortImageRgbFloat;
%}

%feature("notabstract") ImageRgbFloat;
%feature("notabstract") yarp::os::BufferedPort<ImageRgbFloat>;
%feature("notabstract") BufferedPortImageRgbFloat;

%template(ImageRgbFloat) yarp::sig::ImageOf<yarp::sig::PixelRgbFloat>;
%template(TypedReaderImageRgbFloat) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelRgbFloat> >;
%template(TypedReaderCallbackImageRgbFloat) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelRgbFloat> >;
%template(BufferedPortImageRgbFloat) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgbFloat> >;

//////////////////////////////////////////////////////////////////////////
// Deal with poor translation of interface inheritance in current SWIG

%extend yarp::os::Port {
    bool write(Bottle& data) {
        return self->write(*((PortWriter*)(&data)));
    }

    bool write(Property& data) {
        return self->write(*((PortWriter*)(&data)));
    }

    bool write(yarp::sig::ImageOf<yarp::sig::PixelRgb>& data) {
        return self->write(*((PortWriter*)(&data)));
    }

    bool write(yarp::sig::ImageOf<yarp::sig::PixelFloat>& data) {
        return self->write(*((PortWriter*)(&data)));
    }

    bool write(Bottle& data1, Bottle& data2) {
        return self->write(*((PortWriter*)(&data1)), *((PortReader*)(&data2)));
    }

    bool write(Bottle& data1, yarp::sig::ImageOf<yarp::sig::PixelFloat>& data2){
        return self->write(*((PortWriter*)(&data1)), *((PortReader*)(&data2)));
    }

    bool reply(Bottle& data) {
        return self->reply(*((PortWriter*)(&data)));
    }
}

%extend yarp::os::RpcClient {
    bool write(Bottle& data1, Bottle& data2) {
        return self->write(*((PortWriter*)(&data1)), *((PortReader*)(&data2)));
    }
}

%extend yarp::os::Contactable {
  bool setEnvelope(Portable& data) {
    return self->setEnvelope(*((PortWriter*)(&data)));
  }
}

//////////////////////////////////////////////////////////////////////////
// Deal with PolyDriver idiom that doesn't translate too well

%extend yarp::dev::PolyDriver {
    yarp::dev::IFrameGrabberImage *viewFrameGrabberImage() {
        yarp::dev::IFrameGrabberImage *result;
        self->view(result);
        return result;
    }

    yarp::dev::IPositionControl *viewIPositionControl() {
        yarp::dev::IPositionControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::IVelocityControl *viewIVelocityControl() {
        yarp::dev::IVelocityControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::IEncoders *viewIEncoders() {
        yarp::dev::IEncoders *result;
        self->view(result);
        return result;
    }

    yarp::dev::IMotorEncoders *viewIMotorEncoders() {
        yarp::dev::IMotorEncoders *result;
        self->view(result);
        return result;
    }

    yarp::dev::IPidControl *viewIPidControl() {
        yarp::dev::IPidControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::IAmplifierControl *viewIAmplifierControl() {
        yarp::dev::IAmplifierControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::IControlLimits *viewIControlLimits() {
        yarp::dev::IControlLimits *result;
        self->view(result);
        return result;
    }

    yarp::dev::ICartesianControl *viewICartesianControl() {
        yarp::dev::ICartesianControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::IGazeControl *viewIGazeControl() {
      yarp::dev::IGazeControl *result;
      self->view(result);
      return result;
    }

    yarp::dev::IImpedanceControl *viewIImpedanceControl() {
        yarp::dev::IImpedanceControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::ITorqueControl *viewITorqueControl() {
        yarp::dev::ITorqueControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::IControlMode *viewIControlMode() {
        yarp::dev::IControlMode *result;
        self->view(result);
        return result;
    }

    yarp::dev::IControlMode2 *viewIControlMode2() {
        yarp::dev::IControlMode2 *result;
        self->view(result);
        return result;
    }

    yarp::dev::IPWMControl *viewIPWMControl() {
            yarp::dev::IPWMControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::ICurrentControl *viewICurrentControl() {
            yarp::dev::ICurrentControl *result;
        self->view(result);
        return result;
    }

    yarp::dev::IAnalogSensor *viewIAnalogSensor() {
        yarp::dev::IAnalogSensor *result;
        self->view(result);
        return result;
    }

    yarp::dev::IFrameGrabberControls2 *viewIFrameGrabberControls2() {
        yarp::dev::IFrameGrabberControls2 *result;
        self->view(result);
        return result;
    }

    yarp::dev::IPositionDirect *viewIPositionDirect() {
        yarp::dev::IPositionDirect *result;
        self->view(result);
        return result;
    }

    yarp::dev::IRemoteVariables *viewIRemoteVariables() {
        yarp::dev::IRemoteVariables *result;
        self->view(result);
        return result;
    }

    yarp::dev::IAxisInfo *viewIAxisInfo() {
        yarp::dev::IAxisInfo *result;
        self->view(result);
        return result;
    }

    // you'll need to add an entry for every interface you wish
    // to use
}


//////////////////////////////////////////////////////////////////////////
// Deal with ControlBoardInterfaces pointer arguments that don't translate

%extend yarp::dev::IPositionControl {
    int getAxes() {
        int buffer;
        bool ok = self->getAxes(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool positionMove(std::vector<double>& data) {
        return self->positionMove(&data[0]);
    }

    bool relativeMove(std::vector<double>& data) {
        return self->relativeMove(&data[0]);
    }

    bool setRefSpeeds(std::vector<double>& data) {
        return self->setRefSpeeds(&data[0]);
    }

    bool getRefSpeed(int j, std::vector<double>& data) {
        return self->getRefSpeed(j, &data[0]);
    }

    bool getRefSpeeds(std::vector<double>& data) {
        return self->getRefSpeeds(&data[0]);
    }

    bool getRefAcceleration(int j, std::vector<double>& data) {
        return self->getRefAcceleration(j, &data[0]);
    }

    bool getRefAccelerations(std::vector<double>& data) {
        return self->getRefAccelerations(&data[0]);
    }


    bool checkMotionDone() {
        bool result;
        bool ok = self->checkMotionDone(&result);
        if(!ok) { return 1; } //In case of error tell the motion has been completed
        return result;
    }

    bool checkMotionDone(std::vector<bool>& flag) {
      // complication: vector<bool> is packed in C++
      // and isn't a regular container.
      std::vector<char> data(flag.size());
      bool result = self->checkMotionDone((bool*)(&data[0]));
      for (size_t i=0; i<data.size(); i++) {
        flag[i] = data[i]!=0;
      }
      return result;
    }

    bool checkMotionDone(int i, std::vector<bool>& flag) {
      std::vector<char> data(flag.size());
      bool result = self->checkMotionDone(i,(bool*)(&data[0]));
      for (size_t i=0; i<data.size(); i++) {
        flag[i] = data[i]!=0;
      }
      return result;
    }

    bool isMotionDone(int i) {
        bool buffer;
        self->checkMotionDone(i,&buffer);
        return buffer;
    }

    bool isMotionDone() {
        int buffer;
        self->getAxes(&buffer);
        bool data = true;
        for (int i=0; i<buffer; i++) {
            bool buffer2;
            self->checkMotionDone(i,&buffer2);
            data = data && buffer2;
            }
        return data;
    }

}

%extend yarp::dev::IVelocityControl {
    int getAxes() {
        int buffer;
        bool ok = self->getAxes(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool velocityMove(std::vector<double>& data) {
        return self->velocityMove(&data[0]);
    }

    bool setRefAccelerations(std::vector<double>& data) {
        return self->setRefAccelerations(&data[0]);
    }

    bool getRefAcceleration(int j, std::vector<double>& data) {
        return self->getRefAcceleration(j, &data[0]);
    }

    bool getRefAccelerations(std::vector<double>& data) {
        return self->getRefAccelerations(&data[0]);
    }
}

%extend yarp::dev::IEncoders {
    int getAxes() {
        int buffer;
        bool ok = self->getAxes(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool setEncoders(std::vector<double>& data) {
        return self->setEncoders(&data[0]);
    }

    double getEncoder(int j) {
        double data;
        bool ok = self->getEncoder(j, &data);
        if (!ok) return 0;
        return data;
    }

    bool getEncoders(std::vector<double>& data) {
        return self->getEncoders(&data[0]);
    }

    double getEncoderSpeed(int j) {
        double data;
        bool ok = self->getEncoderSpeed(j, &data);
        if (!ok) return 0;
        return data;
    }

    bool getEncoderSpeeds(std::vector<double>& data) {
        return self->getEncoderSpeeds(&data[0]);
    }

    double getEncoderAcceleration(int j) {
        double data;
        bool ok = self->getEncoderAcceleration(j, &data);
        if (!ok) return 0;
        return data;
    }

    bool getEncoderAccelerations(std::vector<double>& data) {
        return self->getEncoderAccelerations(&data[0]);
    }
}

%extend yarp::dev::IMotorEncoders {
    int getNumberOfMotorEncoders() {
        int nbEncs;
        bool ok = self->getNumberOfMotorEncoders(&nbEncs);
        if (!ok) return 0;
        return nbEncs;
    }

    double getMotorEncoder(int j) {
        double enc;
        bool ok = self->getMotorEncoder(j, &enc);
        if (!ok) return 0;
        return enc;
    }

    bool getMotorEncoders(std::vector<double>& encs) {
        return self->getMotorEncoders(&encs[0]);
    }

    bool getMotorEncoderTimed(int j, std::vector<double>& enc, std::vector<double>& time) {
        return self->getMotorEncoderTimed(j, &enc[0], &time[0]);
    }

    bool getMotorEncodersTimed(std::vector<double>& encs, std::vector<double>& times) {
        return self->getMotorEncodersTimed(&encs[0], &times[0]);
    }

    double getMotorEncoderSpeed(int j) {
        double speed;
        bool ok = self->getMotorEncoderSpeed(j, &speed);
        if (!ok) return 0;
        return speed;
    }

    bool getMotorEncoderSpeeds(std::vector<double>& speeds) {
        return self->getMotorEncoderSpeeds(&speeds[0]);
    }
}


%extend yarp::dev::IAmplifierControl {
    bool getCurrents(std::vector<double>& data) {
        return self->getCurrents(&data[0]);
    }

    bool getCurrent(int j, std::vector<double>& data) {
        return self->getCurrent(j, &data[0]);
    }
}

%extend yarp::dev::IControlLimits {
    bool getLimits(int axis, std::vector<double>& min, std::vector<double>& max) {
        return self->getLimits(axis, &min[0], &max[0]);
    }
}

%extend yarp::dev::IControlMode {
    int getControlMode(int j) {
        int buffer;
        bool ok = self->getControlMode(j, &buffer);
        if (!ok) return -1;
        return buffer;
    }

    bool getControlModes(std::vector<int>& data) {
        return self->getControlModes(&data[0]);
    }
}

%extend yarp::dev::IControlMode2 {
    bool getControlModes(int n_joint, std::vector<int>& joints, std::vector<int>& data) {
        return self->getControlModes(n_joint, &joints[0], &data[0]);
    }

    bool setControlModes(std::vector<int>& data) {
        return self->setControlModes(&data[0]);
    }

    bool setControlModes(int n_joint, std::vector<int>& joints, std::vector<int>& data) {
        return self->setControlModes(n_joint, &joints[0], &data[0]);
    }
}

%extend yarp::dev::IPositionDirect {
    int getAxes() {
        int buffer;
        bool ok = self->getAxes(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool setPositions(std::vector<double>& data) {
        return self->setPositions(&data[0]);
    }
}

%extend yarp::dev::IAxisInfo {
    std::string getAxisName(int axis) {
        yarp::os::ConstString name;
        bool ok = self->getAxisName(axis, name);
        if (!ok) return "unknown";
        return name;
    }
}

%extend yarp::sig::Vector {

    double get(int j)
    {
        return self->operator [](j);
    }

    void set(int j, double v)
    {
        self->operator [](j) = v;
    }


#ifdef SWIGPYTHON
    void __setitem__(int key, double value) {
        self->operator[](key) = value;
    }

    double __getitem__(int key) {
        return self->operator[](key);
    }

    double __len__() {
        return self->length();
    }
#endif
}

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

%extend yarp::dev::ICartesianControl {
    bool checkMotionDone(std::vector<bool>& flag) {
      std::vector<char> data(flag.size());
      bool result = self->checkMotionDone((bool*)(&data[0]));
      for (size_t i=0; i<data.size(); i++) {
        flag[i] = data[i]!=0;
      }
      return result;
    }

    bool checkMotionDone() {
        bool flag;
        if(self->checkMotionDone(&flag)) {
            return flag;
        } else {
            return false;
        }
    }

    bool isMotionDone() {
        bool data = true;
        self->checkMotionDone(&data);
        return data;
    }

    int storeContext() {
        // bad id to return if the real
        // storeContext returns false
        int badContextId = -1000;
        int ret = badContextId;
        // call the real storeContext
        bool ok = self->storeContext(&ret);
        // if not ok, return the badContextId
        if( !ok ) {
            ret = badContextId;
        }

        return ret;
    }
}

%extend yarp::dev::IGazeControl {

    bool getTrackingMode() {
        bool flag;

        if(self->getTrackingMode(&flag)) {
            return flag;
        } else {
            return false; //Not sure what is best to assume here...
        }
    }

    double getNeckTrajTime() {
              double result;

              if(self->getNeckTrajTime(&result)) {
            return result;
        } else {
            return -1.0; //On error return -1.0
          }
    }

    double getEyesTrajTime() {
        double result;

        if(self->getEyesTrajTime(&result)) {
            return result;
        } else {
            return -1.0; //On error return -1.0
        }
    }

    bool checkMotionDone() {
          bool flag;
        if(self->checkMotionDone(&flag)) {
            return flag;
        } else {
            return false;
        }
    }
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


// From Leo Pape

%extend yarp::os::NetworkBase {
    static bool write(const char* port_name, Bottle& cmd, Bottle& reply) {
        return yarp::os::NetworkBase::write(port_name, *((PortWriter*)(&cmd)), *((PortReader*)(&reply)));
    }

    static bool write(const Contact& contact, Bottle& cmd, Bottle& reply, const ContactStyle& style) {
        return yarp::os::NetworkBase::write(contact, *((PortWriter*)(&cmd)), *((PortReader*)(&reply)), style);
    }

    static bool write(const Contact& contact, Bottle& cmd, Bottle& reply, bool admin, bool quiet, double timeout) {
        return yarp::os::NetworkBase::write(contact, *((PortWriter*)(&cmd)), *((PortReader*)(&reply)), admin, quiet, timeout);
    }
}


#endif


/*
 * Extending yarp::os::Things.h
 */
%extend yarp::os::Things  {
public:

    yarp::os::Value* asValue() {
        return self->cast_as<yarp::os::Value>();
    }

    yarp::os::Bottle* asBottle() {
        return self->cast_as<yarp::os::Bottle>();
    }

    yarp::os::Property* asProperty() {
        return self->cast_as<yarp::os::Property>();
    }

    yarp::sig::Vector* asVector() {
        return self->cast_as<yarp::sig::Vector>();
    }

    yarp::sig::Matrix* asMatrix() {
        return self->cast_as<yarp::sig::Matrix>();
    }

    yarp::sig::Image* asImage() {
        return self->cast_as<yarp::sig::Image>();
    }

    yarp::sig::ImageOf<yarp::sig::PixelRgb>* asImageOfPixelRgb() {
        return self->cast_as<yarp::sig::ImageOf<yarp::sig::PixelRgb> >();
    }

    yarp::sig::ImageOf<yarp::sig::PixelBgr>* asImageOfPixelBgr() {
        return self->cast_as<yarp::sig::ImageOf<yarp::sig::PixelBgr> >();
    }

    yarp::sig::ImageOf<yarp::sig::PixelMono>* asImageOfPixelMono() {
        return self->cast_as<yarp::sig::ImageOf<yarp::sig::PixelMono> >();
    }
}


//////////////////////////////////////////////////////////////////////////
// Deal with IFrameGrabberControls2 pointer arguments that don't translate

%extend yarp::dev::IFrameGrabberControls2 {
  CameraDescriptor getCameraDescription() {
      CameraDescriptor result;
      self->getCameraDescription(&result);
      return result;
  }

  bool hasFeature(int feature) {
      bool result;
      self->hasFeature(feature, &result);
      return result;
  }

  double getFeature(int feature) {
      double result;
      self->getFeature(feature, &result);
      return result;
  }

  bool hasOnOff(int feature) {
      bool result;
      self->hasOnOff(feature, &result);
      return result;
  }

  bool getActive(int feature) {
      bool result;
      self->getActive(feature, &result);
      return result;
  }

  bool hasAuto(int feature) {
      bool result;
      self->hasAuto(feature, &result);
      return result;
  }

  bool hasManual(int feature) {
      bool result;
      self->hasManual(feature, &result);
      return result;
  }

  bool hasOnePush(int feature) {
      bool result;
      self->hasOnePush(feature, &result);
      return result;
  }

  FeatureMode getMode(int feature) {
      FeatureMode result;
      self->getMode(feature, &result);
      return result;
  }
}
