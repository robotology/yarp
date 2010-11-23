// Copyright: (C) 2010 RobotCub Consortium
// Author: Paul Fitzpatrick, Stephane Lallee, Arnaud Degroote
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

//////////////////////////////////////////////////////////////////////////
// 
// This is a configuration file to explain YARP to SWIG
//
// SWIG, for the most part, understands YARP auto-magically.
// There are a few things that need to be explained:
//  + use of multiple inheritance
//  + use of names that clash with special names in Java/Python/Perl/...
//  + use of templates


%module(directors="1") yarpswig
%feature("director") yarp::os::PortReader;

// Try to translate std::string and std::vector to native equivalents
%include "std_string.i"
#if !defined(SWIGCHICKEN) && !defined(SWIGALLEGROCL)
  %include "std_vector.i"
#endif

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
    %csmethodmodifiers onRead "public virtual";
    %csmethodmodifiers getPendingReads "public new";
    %csmethodmodifiers setStrict "public new";
    %csmethodmodifiers useCallback "public new";
    %csmethodmodifiers onCommencement "public virtual";
    %csmethodmodifiers disableCallback "public virtual";
    %csmethodmodifiers acquire "public virtual";
    %csmethodmodifiers release "public virtual";
    %csmethodmodifiers isNull "public virtual";
#endif

// Deal with abstract base class problems, where SWIG guesses
// incorrectly at whether a class can be instantiated or not
%feature("notabstract") Port;
%feature("notabstract") BufferedPort;
%feature("notabstract") Bottle;
%feature("notabstract") Property;
%feature("notabstract") Stamp;
%feature("abstract") Portable;
%feature("abstract") PortReader;
%feature("abstract") PortWriter;
%feature("abstract") Searchable;
%feature("abstract") Contactable;

// Deal with overridden method clashes, simply by ignoring them.
// At some point, these methods should get renamed so they are still
// available.
%ignore *::check(const char *key, Value *& result);
%ignore *::check(const char *key, Value *& result, const char *comment);
%rename(where_c) *::where();
%rename(seed_c) *::seed(int seed);  // perl clash
%ignore *::setPid(int j, const Pid &pid);
%ignore *::getPid(int j, Pid *pid);
%ignore *::setKp(double);
%ignore *::setKi(double);
%ignore *::setKd(double);
%ignore *::setScale(double);
%ignore *::setOffset(double);
%rename(open_str) yarp::dev::PolyDriver::open(const char *txt);

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
%rename(toString_c) *::toString() const;

// python conflict
#ifdef SWIGPYTHON
	%rename(yield_c) *::yield();
#endif

// java conflict
#ifdef SWIGJAVA
	%rename(wait_c) *::wait();
	%rename(clone_c) *::clone() const;
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
  #endif
#endif
// operator= does not get translated well
%ignore *::operator=;
%ignore yarp::PortReaderBuffer;
%ignore yarp::sig::Image::operator()(int,int) const;
%ignore yarp::sig::Image::pixel(int,int) const;
%ignore yarp::sig::Image::getRow(int) const;
%ignore yarp::sig::Image::getIplImage() const;
%ignore yarp::os::Property::put(const char *,Value *);
%ignore yarp::os::Bottle::add(Value *);
%rename(toString) yarp::os::ConstString::operator const char *() const;
%rename(isEqual) *::operator==;
%rename(notEqual) *::operator!=;
%rename(access) *::operator();
//%ignore yarp::os::PortReader::read;

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
#if PY_VERSION_HEX >= 0x03000000
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
	  fprintf(stderr, "Sorry, setExternal(PyObject *mem,...) requires Python3\n");
	  return;
#endif
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
%define _YARP2_NETINT32_
%enddef
typedef int yarp::os::NetInt32;
%define _YARP2_VOCAB_ 1
%enddef
%define PACKED_FOR_NET 
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
#if defined( SWIGALLEGROCL )
  %include "compat.h"
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
%include <yarp/os/Contact.h>
%include <yarp/os/Network.h>
%include <yarp/os/PortablePair.h>
%include <yarp/os/PortReaderCreator.h>
%include <yarp/os/Property.h>
%include <yarp/os/Port.h>
%include <yarp/os/Bottle.h>
%include <yarp/os/PortReaderBuffer.h>
%include <yarp/os/PortWriterBuffer.h>
%include <yarp/os/Random.h>
%include <yarp/os/Searchable.h>
%include <yarp/os/Semaphore.h>
%include <yarp/os/Thread.h>
%include <yarp/os/Time.h>

%define MAKE_COMMS(name)
%feature("notabstract") yarp::os::BufferedPort<name>;
%feature("notabstract") BufferedPort ## name;
%feature("director") yarp::os::TypedReaderCallback<name>;
%feature("director") yarp::os::TypedReaderCallback<yarp::os::name>;
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
%include <yarp/os/IConfig.h>
%include <yarp/dev/DeviceDriver.h>
%include <yarp/dev/PolyDriver.h>
%include <yarp/dev/FrameGrabberInterfaces.h>
%include <yarp/dev/AudioVisualInterfaces.h>
%include <yarp/dev/ControlBoardInterfaces.h>
%include <yarp/dev/ControlBoardPid.h>

#if !defined(SWIGCHICKEN) && !defined(SWIGALLEGROCL)
  %template(DVector) std::vector<double>;
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

%feature("notabstract") ImageRgb;
%feature("notabstract") yarp::os::BufferedPort<ImageRgb>;
%feature("notabstract") BufferedPortImageRgb;

%feature("notabstract") ImageRgba;
%feature("notabstract") yarp::os::BufferedPort<ImageRgba>;
%feature("notabstract") BufferedPortImageRgba;

%feature("notabstract") ImageMono;
%feature("notabstract") yarp::os::BufferedPort<ImageMono>;
%feature("notabstract") BufferedPortImageMono;

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

// Add getPixel and setPixel methods to access float values
%extend yarp::sig::ImageOf<yarp::sig::PixelFloat> {
   float getPixel(int x, int y) {
       return self->pixel(x,y);
       }

   void setPixel(int x, int y, float v) {
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

%extend yarp::dev::IPidControl {
	bool setReferences(std::vector<double>& data) {
		return self->setReferences(&data[0]);
	}
	
	bool getReference(int j, std::vector<double>& data) {
		return self->getReference(j, &data[0]);
	}

	bool getReferences(std::vector<double>& data) {
		return self->getReferences(&data[0]);
	}
	
	bool setErrorLimits(std::vector<double>& data) {
		return self->setErrorLimits(&data[0]);
	}
	
	bool getErrorLimit(int j, std::vector<double>& data) {
		return self->getErrorLimit(j, &data[0]);
	}
	
	bool getErrorLimits(std::vector<double>& data) {
		return self->getErrorLimits(&data[0]);
	}
	
	bool getError(int j, std::vector<double>& data) {
		return self->getError(j, &data[0]);
	}
	
	bool getErrors(std::vector<double>& data) {
		return self->getErrors(&data[0]);
	}
	
	bool getOutput(int j, std::vector<double>& data) {
		return self->getOutput(j, &data[0]);
	}
	
	bool getOutputs(std::vector<double>& data) {
		return self->getOutputs(&data[0]);
	}
	
	bool setPid(int j, yarp::dev::Pid pid) {
		return self->setPid(j,pid);
	}

	bool setPids(std::vector<yarp::dev::Pid> pids) {
		return self->setPids(&pids[0]);
	}

	bool getPid(int j, std::vector<yarp::dev::Pid> pid) {
		return self->getPid(j,&pid[0]);
	}

	bool getPids(std::vector<yarp::dev::Pid> pids) {
		return self->getPids(&pids[0]);
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
 	  	 
%extend yarp::sig::Image {
	void setExternal(long int mem, int w, int h) {
		self->setExternal((void*)mem, w, h);
        }
}

%extend yarp::sig::Image {
    void setExternal(long long mem, int w, int h) {
        self->setExternal((void*)mem, w, h);
        }
}

#ifdef SWIGPYTHON

// Contributed by Arnaud Degroote for MORSE
// Conversion of Python buffer type object into a pointer
%extend yarp::sig::Image {
    void setExternal(PyObject* mem, int w, int h) {
      ::setExternal(self,mem,w,h);
    }
}

#endif
