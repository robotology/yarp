//////////////////////////////////////////////////////////////////////////
// 
// This is a configuration file to explain YARP to SWIG
//
// SWIG, for the most part, understands YARP auto-magically.
// There are a few things that need to be explained:
//  + use of multiple inheritance
//  + use of names that clash with special names in Java/Python/Perl/...
//  + use of templates


%module yarpswig

// Translate std::string to whatever the native string type is
%include "std_string.i"
// Translate std::vector to whatever the native vector type is
%include "std_vector.i"


// Deal with Java method name conflicts
// We rename a few methods as follows:
//   toString -> toString_c
//   wait -> wait_c
//   clone -> clone_c
%rename(toString_c) *::toString() const;
%rename(wait_c) *::wait();
%rename(clone_c) *::clone() const;

// Deal with abstract base class problems, where SWIG guesses
// incorrectly at whether a class can be instantiated or not
%feature("notabstract") Port;
%feature("notabstract") BufferedPort;
%feature("notabstract") Bottle;
%feature("notabstract") Property;
%feature("notabstract") Stamp;
%feature("abstract") Portable;
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

//////////////////////////////////////////////////////////////////////////
// Clean up a few unimportant things that give warnings

// abstract methods just confuse SWIG
%ignore yarp::os::BufferedPort::open; // let Contactable::open show
%ignore yarp::os::Port::open; // let Contactable::open show
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
%ignore yarp::os::PortReader::read;

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
using namespace yarp::dev;
%}


// Now we parse the original header files
// Redefine a few things that SWIG currently chokes on
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
%include <yarp/os/ConstString.h>
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
%include <yarp/os/Port.h>
%include <yarp/os/Bottle.h>
%include <yarp/os/PortReaderBuffer.h>
%include <yarp/os/PortReaderCreator.h>
%include <yarp/os/PortWriterBuffer.h>
%include <yarp/os/Property.h>
%include <yarp/os/Random.h>
%include <yarp/os/Searchable.h>
%include <yarp/os/Semaphore.h>
%include <yarp/os/Thread.h>
%include <yarp/os/Time.h>
%include <yarp/sig/Image.h>
%include <yarp/sig/Sound.h>
%include <yarp/dev/PolyDriver.h>
%include <yarp/dev/FrameGrabberInterfaces.h>
%include <yarp/dev/AudioVisualInterfaces.h>
%include <yarp/dev/ControlBoardInterfaces.h>
%include <yarp/dev/ControlBoardPid.h>

namespace std {
   %template(DVector) vector<double>;
   %template(PidVector) vector<yarp::dev::Pid>;
};

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

%define MAKE_COMMS(name)
%feature("notabstract") yarp::os::BufferedPort<name>;
%feature("notabstract") BufferedPort ## name;
%template(TypedReader ## name) yarp::os::TypedReader<name>;
%template(TypedReaderCallback ## name) yarp::os::TypedReaderCallback<name>;
%template(BufferedPort ## name) yarp::os::BufferedPort<name>;
%feature("notabstract") yarp::os::BufferedPort<name>;
%feature("notabstract") BufferedPort ## name;
%enddef

MAKE_COMMS(Property)
MAKE_COMMS(Bottle)


// Now we do ImageRgb - it is a little trickey
//%template(ImageRgb) yarp::sig::ImageOf<yarp::sig::PixelRgb>;

%{
typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageRgb;
typedef yarp::os::TypedReader<ImageRgb> TypedReaderImageRgb;
typedef yarp::os::TypedReaderCallback<ImageRgb> TypedReaderCallbackImageRgb;
typedef yarp::os::BufferedPort<ImageRgb> BufferedPortImageRgb;
%}

%feature("notabstract") ImageRgb;
%feature("notabstract") yarp::os::BufferedPort<ImageRgb>;
%feature("notabstract") BufferedPortImageRgb;

%template(ImageRgb) yarp::sig::ImageOf<yarp::sig::PixelRgb>;
%template(TypedReaderImageRgb) yarp::os::TypedReader<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;
%template(TypedReaderCallbackImageRgb) yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;
%template(BufferedPortImageRgb) yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;



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
 	  	 
