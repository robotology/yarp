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





#if !defined (SWIGMATLAB)
%feature("director") yarp::os::PortReader;
%feature("director") yarp::os::RFModule;
%feature("director") yarp::os::Thread;
#endif

%feature("autodoc", "1");

#if defined (SWIGPYTHON)
  %include <argcargv.i>
  %apply (int ARGC, char **ARGV) { (int argc, char *argv[]) }
#elif defined (SWIGLUA)
  %include "lua/argcargv.i"
  %apply (int ARGC, char **ARGV) { (int argc, char *argv[]) }
#endif

%include <stdint.i>
%include <std_vector.i>

// Try to translate std::string and std::pair to native equivalents
%include "std_string.i"
%include "std_pair.i"

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
%rename(attach_rpc_server) *::attach(yarp::os::RpcServer&);

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
%ignore yarp::sig::Image::getReadType() const;
// yarp::sig::VectorOf<T> is an alias template (not supported by SWIG) which selects
// one of the concrete classes yarp::sig::VectorOfDouble, yarp::sig::VectorOfInt, etc.
// Only the double and int versions are wrapped, with the names Vector and VectorInt.
// Their methods come from the CRTP base class yarp::sig::VectorBase, which is
// instantiated with %template before including Vector.h (see below).
%ignore vector_selector;
%ignore yarp::sig::VectorOfFloat;
%ignore yarp::sig::VectorOfString;
%ignore yarp::sig::VectorOfSizet;
%rename(Vector) yarp::sig::VectorOfDouble;
%rename(VectorInt) yarp::sig::VectorOfInt;
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


// Sometimes ACE redefines main() - we don't want that
#ifdef main
#undef main
#endif

// Bring in the main important namespace
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;
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

    $1 = (int)PyLong_AsLong(PyTuple_GetItem($input,0));   /* int i */
    $2 = (int)PyLong_AsLong(PyTuple_GetItem($input,1));   /* int j */
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
%include <yarp/os/Vocab32.h>
%include <yarp/os/Vocab64.h>
%include <yarp/os/BinPortable.h>
%include <yarp/os/BufferedPort.h>
%include <yarp/os/Contact.h>
%include <yarp/os/Contactable.h>
%include <yarp/os/UnbufferedContactable.h>
%include <yarp/os/Port.h>
%include <yarp/os/AbstractContactable.h>
%include <yarp/os/Contact.h>
%include <yarp/os/Network.h>
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
%include <yarp/os/LogComponent.h>
%include <yarp/os/Log.h>
%include <yarp/os/LogStream.h>
%include <yarp/os/Wire.h>
%include <yarp/os/WireLink.h>
%include <yarp/os/Type.h>

//--------------------------------------------------------------------
%define MAKE_COMMS(name, fullname)

%feature("notabstract") name;

%feature("notabstract") yarp::os::BufferedPort<fullname>;
%feature("notabstract") BufferedPort ## name;

#if !defined (SWIGMATLAB)
%feature("director") yarp::os::TypedReader<fullname>;
%feature("director") yarp::os::TypedReaderCallback<fullname>;
%feature("director") yarp::os::BufferedPort<fullname>;
#endif

%template(TypedReader ## name) yarp::os::TypedReader<fullname>;
%template(name ## Callback) yarp::os::TypedReaderCallback<fullname>;
%template(BufferedPort ## name) yarp::os::BufferedPort<fullname>;
%enddef
//--------------------------------------------------------------------

//--------------------------------------------------------------------
%define MAKE_COMMS2(name, templatename)

%template(name) templatename;
%feature("notabstract") name;

%feature("notabstract") yarp::os::BufferedPort<templatename>;
%feature("notabstract") BufferedPort ## name;

#if !defined (SWIGMATLAB)
%feature("director") yarp::os::TypedReaderCallback<templatename>;
#endif

%template(TypedReader ## name) yarp::os::TypedReader<templatename>;
%template(name ## Callback) yarp::os::TypedReaderCallback<templatename>;
%template(BufferedPort ## name) yarp::os::BufferedPort<templatename>;
%enddef
//--------------------------------------------------------------------

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
// YARP_dev
%include "yarp_dev.i"


%template(DVector) std::vector<double>;
%template(BVector) std::vector<bool>;
%template(SVector) std::vector<std::string>;
%template(IVector) std::vector<int>;
%template(ShortVector) std::vector<short int>;
%template() std::pair<std::string, std::string>;
%template(SPairVector) std::vector<std::pair<std::string, std::string>>;

#ifdef SWIGMATLAB
  // Extend IVector for handling conversion of vectors from and to Matlab
  %include "matlab/vectors_fromTo_matlab.i"
#endif

// Vector.h must be included after the std::vector<double> and std::vector<int> templates,
// so that the constructors Vector(const std::vector<double>&) and VectorInt(const std::vector<int>&)
// (defined below with %extend) can accept native lists.
// VectorBase is a CRTP base class: it must not be instantiated directly
%ignore yarp::sig::VectorBase::VectorBase;
%include <yarp/sig/VectorBase.h>
%template(VectorBaseDouble) yarp::sig::VectorBase<yarp::sig::VectorOfDouble, double>;
%template(VectorBaseInt) yarp::sig::VectorBase<yarp::sig::VectorOfInt, int>;
%include <yarp/sig/Vector.h>

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
typedef yarp::sig::VectorOfDouble Vector;
typedef yarp::sig::VectorOfInt VectorInt;
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
typedef yarp::sig::VectorOf<double> Vector;
typedef yarp::sig::VectorOf<int> VectorInt;
typedef yarp::sig::Sound  Sound;
typedef yarp::sig::Matrix Matrix;
%}

#if SWIG_VERSION < 0x030012
%rename(VectorIterator) yarp::sig::VectorOfDouble::iterator;
%rename(VectorConstIterator) yarp::sig::VectorOfDouble::const_iterator;
#endif

MAKE_COMMS  (Property, Property)
MAKE_COMMS  (Bottle, yarp::os::Bottle)
MAKE_COMMS2 (ImageRgb, yarp::sig::ImageOf<yarp::sig::PixelRgb>)
MAKE_COMMS2 (ImageRgba, yarp::sig::ImageOf<yarp::sig::PixelRgba>)
MAKE_COMMS2 (ImageMono, yarp::sig::ImageOf<yarp::sig::PixelMono>)
MAKE_COMMS2 (ImageMono16, yarp::sig::ImageOf<yarp::sig::PixelMono16>)
MAKE_COMMS2 (ImageInt, yarp::sig::ImageOf<yarp::sig::PixelInt>)
MAKE_COMMS2 (ImageFloat, yarp::sig::ImageOf<yarp::sig::PixelFloat>)
MAKE_COMMS2 (ImageRgbFloat, yarp::sig::ImageOf<yarp::sig::PixelRgbFloat>)
MAKE_COMMS  (Vector, yarp::sig::VectorOfDouble)
MAKE_COMMS  (VectorInt, yarp::sig::VectorOfInt)
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

%extend yarp::sig::VectorOfDouble {

    // SWIG does not handle the constructors inherited from yarp::sig::VectorBase
    // (using VectorBase::VectorBase), so they are re-declared here.
    // All the other methods are inherited from VectorBase.

    VectorOfDouble()
    {
        return new yarp::sig::VectorOfDouble();
    }

    VectorOfDouble(size_t size)
    {
        return new yarp::sig::VectorOfDouble(size);
    }

    VectorOfDouble(size_t size, double def)
    {
        return new yarp::sig::VectorOfDouble(size, def);
    }

    VectorOfDouble(const yarp::sig::VectorOfDouble& other)
    {
        return new yarp::sig::VectorOfDouble(other);
    }

    // This in not a real constructor actually, it is converted by swig to a function returning a pointer.
    // See: http://www.swig.org/Doc3.0/CPlusPlus11.html#CPlusPlus11_initializer_lists
    VectorOfDouble(const std::vector<double>& values)
    {
        yarp::sig::VectorOfDouble* newVec = new yarp::sig::VectorOfDouble();
        newVec->reserve(values.size());
        for (const auto& element : values) {
            newVec->push_back(element);
        }
        return newVec;
    }

    // toString() comes from VectorOf*Data and operator== is a friend function of VectorBase:
    // neither is visible to SWIG.
    std::string toString()
    {
        return self->toString();
    }

    bool isEqual(const yarp::sig::VectorOfDouble& other) const
    {
        return *self == other;
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

    size_t __len__() {
        return self->size();
    }
#endif
}

%extend yarp::sig::VectorOfInt {

    // SWIG does not handle the constructors inherited from yarp::sig::VectorBase
    // (using VectorBase::VectorBase), so they are re-declared here.
    // All the other methods are inherited from VectorBase.

    VectorOfInt()
    {
        return new yarp::sig::VectorOfInt();
    }

    VectorOfInt(size_t size)
    {
        return new yarp::sig::VectorOfInt(size);
    }

    VectorOfInt(size_t size, int def)
    {
        return new yarp::sig::VectorOfInt(size, def);
    }

    VectorOfInt(const yarp::sig::VectorOfInt& other)
    {
        return new yarp::sig::VectorOfInt(other);
    }

    // This in not a real constructor actually, it is converted by swig to a function returning a pointer.
    // See: http://www.swig.org/Doc3.0/CPlusPlus11.html#CPlusPlus11_initializer_lists
    VectorOfInt(const std::vector<int>& values)
    {
        yarp::sig::VectorOfInt* newVec = new yarp::sig::VectorOfInt();
        newVec->reserve(values.size());
        for (const auto& element : values) {
            newVec->push_back(element);
        }
        return newVec;
    }

    // toString() comes from VectorOf*Data and operator== is a friend function of VectorBase:
    // neither is visible to SWIG.
    std::string toString()
    {
        return self->toString();
    }

    bool isEqual(const yarp::sig::VectorOfInt& other) const
    {
        return *self == other;
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

    size_t __len__() {
        return self->size();
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

    yarp::sig::VectorOfDouble* asVector() {
        return self->cast_as<yarp::sig::VectorOfDouble>();
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
// Just in Python (and in yarp bindings itself, not in downstream bindings
// that include yarp.i) add some code to automatically call
// add_dll_directory as necessary
// See https://github.com/robotology/robotology-superbuild/issues/1268
// for more details
#if defined(SWIGPYTHON) && defined(SWIG_GENERATING_YARP_BINDINGS)
%include <swig_python_windows_preable.i>
#endif
