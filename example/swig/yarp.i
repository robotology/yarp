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

	// you'll need to add an entry for every interface you wish
	// to use
}



