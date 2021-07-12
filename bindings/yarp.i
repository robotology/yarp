// SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
// SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
// SPDX-License-Identifier: BSD-3-Clause

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

// YARP_conf
%{
#include <yarp/conf/version.h>
#include <yarp/conf/compiler.h>
#include <yarp/conf/system.h>
#include <yarp/conf/api.h>
#include <yarp/conf/numeric.h>
#include <yarp/conf/string.h>
#include <yarp/conf/environment.h>
#include <yarp/conf/dirs.h>
%}

%include "yarp/conf/version.h"
%import "yarp/conf/compiler.h"
%import "yarp/conf/system.h"
%import "yarp/conf/api.h"
%import "yarp/conf/numeric.h"
%include "yarp/conf/string.h"
%include "yarp/conf/environment.h"
%include "yarp/conf/dirs.h"

// YARP_os
%{
#include <yarp/os/api.h>
%}

%import <yarp/os/api.h>

// YARP_sig
%{
#include <yarp/sig/api.h>
%}

%import <yarp/sig/api.h>

// YARP_dev
%{
#include <yarp/dev/api.h>
%}

%import <yarp/dev/api.h>





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

%include <stdint.i>
%include <std_vector.i>

// Try to translate std::string to native equivalents
%include "std_string.i"

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
%ignore *::check(const std::string& key, Value *& result) const;
%ignore *::check(const std::string& key, Value *& result, const std::string& comment) const;
%rename(where_c) *::where();
%rename(seed_c) *::seed(int seed);  // perl clash
%ignore *::setKp(double);
%ignore *::setKi(double);
%ignore *::setKd(double);
%ignore *::setKff(double);
%ignore *::setScale(double);
%ignore *::setOffset(double);
%rename(attach_rpc_server) *::attach(yarp::os::RpcServer&);
%rename(open_str) yarp::dev::PolyDriver::open(const std::string& txt);

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
    %rename(toString_c) *::toString() const;
#endif

#ifdef SWIGTCL
    %rename(configure_c) *::configure();
#endif

//////////////////////////////////////////////////////////////////////////
// Clean up a few unimportant things that give warnings

// abstract methods just confuse SWIG
%ignore yarp::os::BufferedPort::open; // let Contactable::open show
%ignore yarp::os::Port::open; // let Contactable::open show
%ignore yarp::os::RpcClient::open;

// operator= does not get translated well
%ignore *::operator=;
%ignore yarp::PortReaderBuffer;
%ignore yarp::sig::Image::operator()(int,int) const;
%ignore yarp::sig::Image::pixel(int,int) const;
%ignore yarp::sig::Image::getRow(int) const;
%ignore yarp::sig::Image::getIplImage() const;
%ignore yarp::sig::Image::getReadType() const;
%ignore yarp::sig::VectorOf<double>::getType() const;
%ignore yarp::sig::VectorOf<double>::VectorOf(std::initializer_list<double>);
%ignore yarp::sig::VectorOf<int>::getType() const;
%ignore yarp::sig::VectorOf<int>::VectorOf(std::initializer_list<int>);
%ignore yarp::os::Property::put(const char *,Value *);
%ignore yarp::os::Bottle::add(Value *);
%rename(toString) std::string::operator const char *() const;
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


// Define macros for handling the multiple analog sensors interfaces
%include macrosForMultipleAnalogSensors.i

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

%include <yarp/os/NetInt8.h>
%include <yarp/os/NetInt16.h>
%include <yarp/os/NetInt32.h>
%include <yarp/os/NetInt64.h>
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
%include <yarp/os/PeriodicThread.h>
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
%include <yarp/os/Wire.h>
%include <yarp/os/WireLink.h>

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
%include <yarp/os/RateThread.h>
#endif

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
%include <yarp/dev/IFrameGrabberImage.h>
%include <yarp/dev/IFrameGrabberControls.h>
%include <yarp/dev/IFrameGrabberControlsDC1394.h>
%include <yarp/dev/IFrameWriterImage.h>
%include <yarp/dev/AudioVisualInterfaces.h>
%include <yarp/dev/ControlBoardInterfaces.h>
%include <yarp/dev/IAxisInfo.h>
%include <yarp/dev/IAmplifierControl.h>
%include <yarp/dev/IControlDebug.h>
%include <yarp/dev/IControlLimits.h>
%include <yarp/dev/ControlBoardPid.h>
%include <yarp/dev/CartesianControl.h>
%include <yarp/dev/GazeControl.h>
%include <yarp/dev/IPositionControl.h>
%include <yarp/dev/IEncoders.h>
%include <yarp/dev/CalibratorInterfaces.h>
%include <yarp/dev/ControlBoardPid.h>
%include <yarp/dev/IControlMode.h>
%include <yarp/dev/IInteractionMode.h>
%include <yarp/dev/IEncodersTimed.h>
%include <yarp/dev/IMotor.h>
%include <yarp/dev/IMotorEncoders.h>
%include <yarp/dev/ITorqueControl.h>
%include <yarp/dev/IImpedanceControl.h>
%include <yarp/dev/IVelocityControl.h>
%include <yarp/dev/IPWMControl.h>
%include <yarp/dev/ICurrentControl.h>
%include <yarp/dev/IAnalogSensor.h>
%include <yarp/dev/IRemoteVariables.h>
%include <yarp/dev/IPidControl.h>
%include <yarp/dev/IPositionDirect.h>
%include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
%include <yarp/dev/FrameGrabberControl2.h>
%include <yarp/dev/IControlMode2.h>
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0
%include <yarp/dev/IFrameGrabber.h>
%include <yarp/dev/IFrameGrabberRgb.h>
#endif YARP_NO_DEPRECATED // Since YARP 3.5.0


%template(DVector) std::vector<double>;
%template(BVector) std::vector<bool>;
%template(SVector) std::vector<std::string>;
%template(IVector) std::vector<int>;
%template(ShortVector) std::vector<short int>;

#ifdef SWIGMATLAB
  // Extend IVector for handling conversion of vectors from and to Matlab
  %include "matlab/vectors_fromTo_matlab.i"
#endif

#if defined(SWIGCSHARP)
  SWIG_STD_VECTOR_SPECIALIZE_MINIMUM(Pid,yarp::dev::Pid)
#endif
%template(PidVector) std::vector<yarp::dev::Pid>;

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

%inline
%{
typedef yarp::sig::VectorOf<double> Vector;
typedef yarp::sig::VectorOf<int> VectorInt;
%}

%{
typedef yarp::os::TypedReader<Vector> TypedReaderVector;
typedef yarp::os::TypedReaderCallback<Vector> TypedReaderCallbackVector;
typedef yarp::os::BufferedPort<Vector> BufferedPortVector;
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

%template(Vector) yarp::sig::VectorOf<double>;
#if SWIG_VERSION < 0x030012
%rename(VectorIterator) yarp::sig::VectorOf<double>::iterator;
%rename(VectorConstIterator) yarp::sig::VectorOf<double>::const_iterator;
#endif
%template(TypedReaderVector) yarp::os::TypedReader<yarp::sig::VectorOf<double> >;
%template(TypedReaderCallbackVector) yarp::os::TypedReaderCallback<yarp::sig::VectorOf<double> >;
%template(BufferedPortVector) yarp::os::BufferedPort<yarp::sig::VectorOf<double> >;

%template(VectorInt) yarp::sig::VectorOf<int>;
%template(TypedReaderVectorInt) yarp::os::TypedReader<yarp::sig::VectorOf<int> >;
%template(TypedReaderCallbackVectorInt) yarp::os::TypedReaderCallback<yarp::sig::VectorOf<int> >;
%template(BufferedPortVectorInt) yarp::os::BufferedPort<yarp::sig::VectorOf<int> >;

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

%define CAST_POLYDRIVER_TO_INTERFACE(interface)
    yarp::dev:: ## interface *view ## interface ## () {
        yarp::dev:: ## interface *result;
        self->view(result);
        return result;
    }
%enddef

%extend yarp::dev::PolyDriver {

    CAST_POLYDRIVER_TO_INTERFACE(IFrameGrabberImage)
    CAST_POLYDRIVER_TO_INTERFACE(IPositionControl)
    CAST_POLYDRIVER_TO_INTERFACE(IVelocityControl)
    CAST_POLYDRIVER_TO_INTERFACE(IEncoders)
    CAST_POLYDRIVER_TO_INTERFACE(IEncodersTimed)
    CAST_POLYDRIVER_TO_INTERFACE(IMotor)
    CAST_POLYDRIVER_TO_INTERFACE(IMotorEncoders)
    CAST_POLYDRIVER_TO_INTERFACE(IPidControl)
    CAST_POLYDRIVER_TO_INTERFACE(IAmplifierControl)
    CAST_POLYDRIVER_TO_INTERFACE(IControlLimits)
    CAST_POLYDRIVER_TO_INTERFACE(ICartesianControl)
    CAST_POLYDRIVER_TO_INTERFACE(IGazeControl)
    CAST_POLYDRIVER_TO_INTERFACE(IImpedanceControl)
    CAST_POLYDRIVER_TO_INTERFACE(ITorqueControl)
    CAST_POLYDRIVER_TO_INTERFACE(IControlMode)

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    yarp::dev::IControlMode *viewIControlMode2() {
        yarp::dev::IControlMode *result;
        self->view(result);
        return result;
    }
#endif

    CAST_POLYDRIVER_TO_INTERFACE(IInteractionMode)
    CAST_POLYDRIVER_TO_INTERFACE(IPWMControl)
    CAST_POLYDRIVER_TO_INTERFACE(ICurrentControl)
    CAST_POLYDRIVER_TO_INTERFACE(IAnalogSensor)

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    yarp::dev::IFrameGrabberControls *viewIFrameGrabberControls2() {
        yarp::dev::IFrameGrabberControls *result;
        self->view(result);
        return result;
    }
#endif

    CAST_POLYDRIVER_TO_INTERFACE(IFrameGrabberControls)
    CAST_POLYDRIVER_TO_INTERFACE(IPositionDirect)
    CAST_POLYDRIVER_TO_INTERFACE(IRemoteVariables)
    CAST_POLYDRIVER_TO_INTERFACE(IAxisInfo)

// These views are currently disabled in SWIG + java generator since they are
// useless without the EXTENDED_ANALOG_SENSOR_INTERFACE part.
// See also https://github.com/robotology/yarp/issues/1770
#if !defined(SWIGJAVA) && !defined(SWIGCSHARP)
    CAST_POLYDRIVER_TO_INTERFACE(IThreeAxisGyroscopes)
    CAST_POLYDRIVER_TO_INTERFACE(IThreeAxisLinearAccelerometers)
    CAST_POLYDRIVER_TO_INTERFACE(IThreeAxisMagnetometers)
    CAST_POLYDRIVER_TO_INTERFACE(IOrientationSensors)
    CAST_POLYDRIVER_TO_INTERFACE(ITemperatureSensors)
    CAST_POLYDRIVER_TO_INTERFACE(ISixAxisForceTorqueSensors)
    CAST_POLYDRIVER_TO_INTERFACE(IContactLoadCellArrays)
    CAST_POLYDRIVER_TO_INTERFACE(IEncoderArrays)
    CAST_POLYDRIVER_TO_INTERFACE(ISkinPatches)
#endif

    // you'll need to add an entry for every interface you wish
    // to use
}


//////////////////////////////////////////////////////////////////////////
// Deal with ControlBoardInterfaces pointer arguments that don't translate

%extend yarp::dev::IImpedanceControl {
    int getAxes() {
        int buffer;
        bool ok = self->getAxes(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getImpedance(int j, std::vector<double>& stiffness, std::vector<double>& damping) {
        return self->getImpedance(j, &stiffness[0], &damping[0]);
    }

    bool getImpedanceOffset(int j, std::vector<double>& data) {
        return self->getImpedanceOffset(j, &data[0]);
    }

    bool getCurrentImpedanceLimit(int j, std::vector<double>& min_stiff, std::vector<double>& max_stiff, std::vector<double>& min_damp, std::vector<double>& max_damp) {
        return self->getCurrentImpedanceLimit(j, &min_stiff[0], &max_stiff[0], &min_damp[0], &max_damp[0]);
    }
}

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

    bool positionMove(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->positionMove(n_joint, &joints[0], &data[0]);
    }

    bool relativeMove(std::vector<double>& data) {
        return self->relativeMove(&data[0]);
    }

    bool relativeMove(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->relativeMove(n_joint, &joints[0], &data[0]);
    }

    bool setRefSpeeds(std::vector<double>& data) {
        return self->setRefSpeeds(&data[0]);
    }

    bool setRefSpeeds(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setRefSpeeds(n_joint, &joints[0], &data[0]);
    }

    bool getRefSpeed(int j, std::vector<double>& data) {
        return self->getRefSpeed(j, &data[0]);
    }

    bool getRefSpeeds(std::vector<double>& data) {
        return self->getRefSpeeds(&data[0]);
    }

    bool getRefSpeeds(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getRefSpeeds(n_joint, &joints[0], &data[0]);
    }

    bool setRefAccelerations(std::vector<double>& data) {
        return self->setRefAccelerations(&data[0]);
    }

    bool setRefAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setRefAccelerations(n_joint, &joints[0], &data[0]);
    }

    bool getRefAcceleration(int j, std::vector<double>& data) {
        return self->getRefAcceleration(j, &data[0]);
    }

    bool getRefAccelerations(std::vector<double>& data) {
        return self->getRefAccelerations(&data[0]);
    }

    bool getRefAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getRefAccelerations(n_joint, &joints[0], &data[0]);
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

    bool checkMotionDone(int n_joint, std::vector<int>& joints, std::vector<bool>& flag) {
        std::vector<char> data(n_joint);
        bool result = self->checkMotionDone(n_joint, &joints[0], (bool*)(&data[0]));
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

    bool stop(int n_joint, std::vector<int>& joints) {
        return self->stop(n_joint, &joints[0]);
    }

    bool getTargetPosition(int j, std::vector<double>& data) {
        return self->getTargetPosition(j, &data[0]);
    }

    bool getTargetPositions(std::vector<double>& data) {
        return self->getTargetPositions(&data[0]);
    }

    bool getTargetPositions(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getTargetPositions(n_joint, &joints[0], &data[0]);
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

    bool velocityMove(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->velocityMove(n_joint, &joints[0], &data[0]);
    }

    bool getRefVelocity(int j, std::vector<double>& data) {
        return self->getRefVelocity(j, &data[0]);
    }

    bool getRefVelocities(std::vector<double>& data) {
        return self->getRefVelocities(&data[0]);
    }

    bool getRefVelocities(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getRefVelocities(n_joint, &joints[0], &data[0]);
    }

    bool setRefAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setRefAccelerations(n_joint, &joints[0], &data[0]);
    }

    bool getRefAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getRefAccelerations(n_joint, &joints[0], &data[0]);
    }

    bool stop(int n_joint, std::vector<int>& joints) {
        return self->stop(n_joint, &joints[0]);
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

%extend yarp::dev::IEncodersTimed {
    bool getEncodersTimed(std::vector<double>& data, std::vector<double>& time) {
        return self->getEncodersTimed(&data[0], &time[0]);
    }

    bool getEncoderTimed(int j, std::vector<double>& data, std::vector<double>& time) {
        return self->getEncoderTimed(j, &data[0], &time[0]);
    }
}

%extend yarp::dev::IMotorEncoders {
    int getNumberOfMotorEncoders() {
        int nbEncs;
        bool ok = self->getNumberOfMotorEncoders(&nbEncs);
        if (!ok) return 0;
        return nbEncs;
    }

    bool getMotorEncoderCountsPerRevolution(int j, std::vector<double>& data) {
        return self->getMotorEncoderCountsPerRevolution(j, &data[0]);
    }

    bool setMotorEncoders(std::vector<double>& encs) {
        return self->setMotorEncoders(&encs[0]);
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

    bool getMotorEncoderAcceleration(int j, std::vector<double>& acc) {
        return self->getMotorEncoderAcceleration(j, &acc[0]);
    }

    bool getMotorEncoderAccelerations(std::vector<double>& accs) {
        return self->getMotorEncoderAccelerations(&accs[0]);
    }
}

%extend yarp::dev::IAmplifierControl {
    bool getAmpStatus(std::vector<int>& data) {
        return self->getAmpStatus(&data[0]);
    }

    bool getAmpStatus(int j, std::vector<int>& data) {
        return self->getAmpStatus(j, &data[0]);
    }

    bool getCurrents(std::vector<double>& data) {
        return self->getCurrents(&data[0]);
    }

    bool getCurrent(int j, std::vector<double>& data) {
        return self->getCurrent(j, &data[0]);
    }

    bool getMaxCurrent(int j, std::vector<double>& data) {
        return self->getMaxCurrent(j, &data[0]);
    }

    bool getNominalCurrent(int j, std::vector<double>& data) {
        return self->getNominalCurrent(j, &data[0]);
    }

    bool getPeakCurrent(int j, std::vector<double>& data) {
        return self->getPeakCurrent(j, &data[0]);
    }

    bool getPWM(int j, std::vector<double>& data) {
        return self->getPWM(j, &data[0]);
    }

    bool getPWMLimit(int j, std::vector<double>& data) {
        return self->getPWMLimit(j, &data[0]);
    }

    bool getPowerSupplyVoltage(int j, std::vector<double>& data) {
        return self->getPowerSupplyVoltage(j, &data[0]);
    }
}

%extend yarp::dev::IControlLimits {
    bool getLimits(int axis, std::vector<double>& min, std::vector<double>& max) {
        return self->getLimits(axis, &min[0], &max[0]);
    }

    bool getVelLimits(int axis, std::vector<double>& min, std::vector<double>& max) {
        return self->getVelLimits(axis, &min[0], &max[0]);
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

%extend yarp::dev::IInteractionMode {
    yarp::dev::InteractionModeEnum getInteractionMode(int axis) {
       yarp::dev::InteractionModeEnum mode = VOCAB_IM_UNKNOWN;
       self->getInteractionMode(axis, &mode);
       return mode;
    }

    bool getInteractionModes(int n_joint, std::vector<int>& joints, std::vector<int>& data) {
        return self->getInteractionModes(n_joint, &joints[0], (yarp::dev::InteractionModeEnum*)&data[0]);
    }

    bool getInteractionModes(std::vector<int>& data) {
        return self->getInteractionModes((yarp::dev::InteractionModeEnum*)&data[0]);
    }

    bool setInteractionModes(int n_joint, std::vector<int>& joints, std::vector<int>& data) {
        return self->setInteractionModes(n_joint, &joints[0], (yarp::dev::InteractionModeEnum*)&data[0]);
    }

    bool setInteractionModes(std::vector<int>& data) {
        return self->setInteractionModes((yarp::dev::InteractionModeEnum*)&data[0]);
    }
}

%extend yarp::dev::IPositionDirect {
    int getAxes() {
        int buffer;
        bool ok = self->getAxes(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool setPositions(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setPositions(n_joint, &joints[0], &data[0]);
    }

    bool setPositions(std::vector<double>& data) {
        return self->setPositions(&data[0]);
    }

    bool getRefPosition(int j, std::vector<double>& data) {
        return self->getRefPosition(j, &data[0]);
    }

    bool getRefPositions(std::vector<double>& data) {
        return self->getRefPositions(&data[0]);
    }

    bool getRefPositions(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getRefPositions(n_joint, &joints[0], &data[0]);
    }
}

%extend yarp::dev::IAxisInfo {
    std::string getAxisName(int axis) {
        std::string name;
        bool ok = self->getAxisName(axis, name);
        if (!ok) return "unknown";
        return name;
    }

    yarp::dev::JointTypeEnum getJointType(int axis) {
        yarp::dev::JointTypeEnum type;
        bool ok = self->getJointType(axis, type);
        if (!ok) return VOCAB_JOINTTYPE_UNKNOWN;
        return type;
    }
}

%extend yarp::dev::ICurrentControl {
    int getNumberOfMotors() {
        int buffer;
        bool ok = self->getNumberOfMotors(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getCurrent(int j, std::vector<double>& data) {
        return self->getCurrent(j, &data[0]);
    }

    bool getCurrents(std::vector<double>& data) {
        return self->getCurrents(&data[0]);
    }

    bool getCurrentRange(int j, std::vector<double>& min, std::vector<double>& max) {
        return self->getCurrentRange(j, &min[0], &max[0]);
    }

    bool getCurrentRanges(std::vector<double>& mins, std::vector<double>& maxs) {
        return self->getCurrentRanges(&mins[0], &maxs[0]);
    }

    bool setRefCurrents(std::vector<double>& data) {
        return self->setRefCurrents(&data[0]);
    }

    bool setRefCurrents(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setRefCurrents(n_joint, &joints[0], &data[0]);
    }

    bool getRefCurrents(std::vector<double>& data) {
        return self->getRefCurrents(&data[0]);
    }

    bool getRefCurrent(int j, std::vector<double>& data) {
        return self->getRefCurrent(j, &data[0]);
    }
}

%extend yarp::dev::IMotor {
    int getNumberOfMotors() {
        int buffer;
        bool ok = self->getNumberOfMotors(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getTemperature(int j, std::vector<double>& data) {
        return self->getTemperature(j, &data[0]);
    }

    bool getTemperatures(std::vector<double>& data) {
        return self->getTemperatures(&data[0]);
    }

    bool getTemperatureLimit(int j, std::vector<double>& data) {
        return self->getTemperatureLimit(j, &data[0]);
    }

    bool getGearboxRatio(int j, std::vector<double>& data) {
        return self->getGearboxRatio(j, &data[0]);
    }
}

%extend yarp::dev::IPWMControl {
    int getNumberOfMotors() {
        int buffer;
        bool ok = self->getNumberOfMotors(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool setRefDutyCycles(std::vector<double>& data) {
        return self->setRefDutyCycles(&data[0]);
    }

    bool getRefDutyCycle(int j, std::vector<double>& data) {
        return self->getRefDutyCycle(j, &data[0]);
    }

    bool getRefDutyCycles(std::vector<double>& data) {
        return self->getRefDutyCycles(&data[0]);
    }

    bool getDutyCycle(int j, std::vector<double>& data) {
        return self->getDutyCycle(j, &data[0]);
    }

    bool getDutyCycles(std::vector<double>& data) {
        return self->getDutyCycles(&data[0]);
    }
}

%extend yarp::dev::ITorqueControl {
    int getAxes() {
        int buffer;
        bool ok = self->getAxes(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getRefTorques(std::vector<double>& data) {
        return self->getRefTorques(&data[0]);
    }

    bool getRefTorque(int j, std::vector<double>& data) {
        return self->getRefTorque(j, &data[0]);
    }

    bool setRefTorques(std::vector<double>& data) {
        return self->setRefTorques(&data[0]);
    }

    bool setRefTorques(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setRefTorques(n_joint, &joints[0], &data[0]);
    }

    bool getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters& params) {
        return self->getMotorTorqueParams(j, &params);
    }

    bool getTorque(int j, std::vector<double>& data) {
        return self->getTorque(j, &data[0]);
    }

    bool getTorques(std::vector<double>& data) {
        return self->getTorques(&data[0]);
    }

    bool getTorqueRange(int j, std::vector<double>& min, std::vector<double>& max) {
        return self->getTorqueRange(j, &min[0], &max[0]);
    }

    bool getTorqueRanges(std::vector<double>& mins, std::vector<double>& maxs) {
        return self->getTorqueRanges(&mins[0], &maxs[0]);
    }
}

%extend yarp::dev::IPidControl {
    bool setPid(int pidtype, int j, const yarp::dev::Pid& pid) {
        return self->setPid((yarp::dev::PidControlTypeEnum)pidtype, j, pid);
    }

    bool setPids(int pidtype, std::vector<yarp::dev::Pid>& pids) {
        return self->setPids((yarp::dev::PidControlTypeEnum)pidtype, &pids[0]);
    }

    bool setPidReference(int pidtype, int j, double ref) {
        return self->setPidReference((yarp::dev::PidControlTypeEnum)pidtype, j, ref);
    }

    bool setPidReferences(int pidtype, std::vector<double>& data) {
        return self->setPidReferences((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool setPidErrorLimit(int pidtype, int j, double limit) {
        return self->setPidErrorLimit((yarp::dev::PidControlTypeEnum)pidtype, j, limit);
    }

    bool setPidErrorLimits(int pidtype, std::vector<double>& data) {
        return self->setPidErrorLimits((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidError(int pidtype, int j, std::vector<double>& data) {
        return self->getPidError((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidErrors(int pidtype, std::vector<double>& data) {
        return self->getPidErrors((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidOutput(int pidtype, int j, std::vector<double>& data) {
        return self->getPidOutput((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidOutputs(int pidtype, std::vector<double>& data) {
        return self->getPidOutputs((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPid(int pidtype, int j, std::vector<yarp::dev::Pid>& data) {
        return self->getPid((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPids(int pidtype, std::vector<yarp::dev::Pid>& data) {
        return self->getPids((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidReference(int pidtype, int j, std::vector<double>& data) {
        return self->getPidReference((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidReferences(int pidtype, std::vector<double>& data) {
        return self->getPidReferences((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidErrorLimit(int pidtype, int j, std::vector<double>& data) {
        return self->getPidErrorLimit((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidErrorLimits(int pidtype, std::vector<double>& data) {
        return self->getPidErrorLimits((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool resetPid(int pidtype, int j) {
        return self->resetPid((yarp::dev::PidControlTypeEnum)pidtype, j);
    }

    bool disablePid(int pidtype, int j) {
        return self->disablePid((yarp::dev::PidControlTypeEnum)pidtype, j);
    }

    bool enablePid(int pidtype, int j) {
        return self->enablePid((yarp::dev::PidControlTypeEnum)pidtype, j);
    }

    bool setPidOffset(int pidtype, int j, double offset) {
        return self->setPidOffset((yarp::dev::PidControlTypeEnum)pidtype, j, offset);
    }

    bool isPidEnabled(int pidtype, int j, std::vector<bool>& flag) {
        std::vector<char> data(flag.size());
        bool result = self->isPidEnabled((yarp::dev::PidControlTypeEnum)pidtype, j, (bool*)(&data[0]));
        for (size_t i = 0; i < data.size(); i++) flag[i] = data[i] != 0;
        return result;
    }
}

// This is part is currently broken in SWIG + java generator since SWIG 3.0.3
// (last swig version tested: 3.0.12)
// See also https://github.com/robotology/yarp/issues/1770
#if !defined(SWIGJAVA) && !defined(SWIGCSHARP)
    %extend yarp::dev::IThreeAxisGyroscopes {EXTENDED_ANALOG_SENSOR_INTERFACE(ThreeAxisGyroscope)}
    %extend yarp::dev::IThreeAxisLinearAccelerometers {EXTENDED_ANALOG_SENSOR_INTERFACE(ThreeAxisLinearAccelerometer)}
    %extend yarp::dev::IThreeAxisMagnetometers {EXTENDED_ANALOG_SENSOR_INTERFACE(ThreeAxisMagnetometer)}
    %extend yarp::dev::IOrientationSensors {EXTENDED_ANALOG_SENSOR_INTERFACE(OrientationSensor)}
    %extend yarp::dev::ITemperatureSensors {EXTENDED_ANALOG_SENSOR_INTERFACE(TemperatureSensor)}
    %extend yarp::dev::ISixAxisForceTorqueSensors {EXTENDED_ANALOG_SENSOR_INTERFACE(SixAxisForceTorqueSensor)}
    %extend yarp::dev::IContactLoadCellArrays {EXTENDED_ANALOG_SENSOR_INTERFACE(ContactLoadCellArray)}
    %extend yarp::dev::IEncoderArrays {EXTENDED_ANALOG_SENSOR_INTERFACE(EncoderArray)}
    %extend yarp::dev::ISkinPatches {EXTENDED_ANALOG_SENSOR_INTERFACE(SkinPatch)}
#endif

%extend yarp::sig::VectorOf<double> {

    // This in not a real constructor actually, it is converted by swig to a function returning a pointer.
    // See: http://www.swig.org/Doc3.0/CPlusPlus11.html#CPlusPlus11_initializer_lists
    VectorOf<double>(const std::vector<double>& values)
    {
        VectorOf<double>* newVec = new VectorOf<double>(0);
        newVec->reserve(values.size());
        for (const auto& element : values) {
            newVec->push_back(element);
        }
        return newVec;
    }

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

%extend yarp::sig::VectorOf<int> {

    // This in not a real constructor actually, it is converted by swig to a function returning a pointer.
    // See: http://www.swig.org/Doc3.0/CPlusPlus11.html#CPlusPlus11_initializer_lists
    VectorOf<int>(const std::vector<int>& values)
    {
        VectorOf<int>* newVec = new VectorOf<int>(0);
        newVec->reserve(values.size());
        for (const auto& element : values) {
            newVec->push_back(element);
        }
        return newVec;
    }

    int get(int j)
    {
        return self->operator [](j);
    }

    void set(int j, int v)
    {
        self->operator [](j) = v;
    }



#ifdef SWIGPYTHON
    void __setitem__(int key, int value) {
        self->operator[](key) = value;
    }

    int __getitem__(int key) {
        return self->operator[](key);
    }

    int __len__() {
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

    yarp::sig::VectorOf<double>* asVector() {
        return self->cast_as<yarp::sig::VectorOf<double>>();
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
// Deal with IFrameGrabberControls pointer arguments that don't translate
%extend yarp::dev::IFrameGrabberControls {
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
