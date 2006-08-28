%module yarpswig

%include "std_string.i"

// Deal with Java method name conflicts
%rename(toString_c) *::toString() const;
%rename(wait_c) *::wait();
%rename(clone_c) *::clone() const;

//%rename(open_contact) *::open(const Contact &);
//%rename(open_contact) *::open(const Contact &,bool); 

// Deal with abstract base class problems
%feature("notabstract") Port;
%feature("notabstract") Bottle;
//%feature("abstract") Vocab;

// Deal with overridden method clashes
%ignore *::check(const char *key, Value *& result);
%ignore *::where();
%ignore *::seed(int seed);  // perl clash
//%ignore *::open(Contact const &);
//%ignore *::open(Contact const &, bool);


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

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

//deal with the usual ACE residual define...
#ifdef main
#undef main
#endif

using namespace yarp::os;
using namespace yarp::sig;
%}


// Parse the original header files
// Redefine a few things that SWIG currently chokes on
%define _YARP2_NETINT32_
%enddef
// removed by nat: on win was making swig clash while compiling the java interface
//%define NetInt32 int 
//%enddef
%define _YARP2_VOCAB_ 1
%enddef
%define PACKED_FOR_NET 
%enddef
//%define VOCAB(a,b,c,d) (((d)*256*65536)+((c)*65536)+((b)*256)+(a))
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


//////////////////////////////////////////////////////////////////////////
// Match Java toString behaviour

%extend yarp::os::Bottle {
	std::string toString() {
		return self->toString().c_str();
        }
}


//////////////////////////////////////////////////////////////////////////
// Deal with some templated classes

%feature("notabstract") yarp::os::BufferedPort;

%define MAKE_COMMS(name)
%feature("notabstract") yarp::os::BufferedPort<name>;
%feature("notabstract") BufferedPort ## name;
%template(TypedReader ## name) yarp::os::TypedReader<name>;
%template(TypedReaderCallback ## name) yarp::os::TypedReaderCallback<name>;
%template(BufferedPort ## name) yarp::os::BufferedPort<name>;
%feature("notabstract") yarp::os::BufferedPort<name>;
%feature("notabstract") BufferedPort ## name;
%enddef

%template(ImageRgb) yarp::sig::ImageOf<yarp::sig::PixelRgb>;
%feature("notabstract") ImageRgb;


MAKE_COMMS(Property)
MAKE_COMMS(Bottle)


//MAKE_COMMS(ImageRgb) // actually this turns out to be a bit trickier...
%{
//typedef yarp::sig::ImageOf<yarp::sig::PixelMono> ImageMono;
typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageRgb;
typedef yarp::os::TypedReader<ImageRgb> TypedReaderImageRgb;
typedef yarp::os::TypedReaderCallback<ImageRgb> TypedReaderCallbackImageRgb;
typedef yarp::os::BufferedPort<ImageRgb> BufferedPortImageRgb;
%}
%feature("notabstract") yarp::os::BufferedPort<ImageRgb>;
%feature("notabstract") BufferedPortImageRgb;
typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageRgb;
typedef yarp::os::TypedReader<ImageRgb> TypedReaderImageRgb;
typedef yarp::os::TypedReaderCallback<ImageRgb> TypedReaderCallbackImageRgb;
typedef yarp::os::BufferedPort<ImageRgb> BufferedPortImageRgb;
%template(TypedReaderImageRgb) yarp::os::TypedReader<ImageRgb>;
%template(TypedReaderCallback) yarp::os::TypedReaderCallback<ImageRgb>;
%template(BufferedPortImageRgb) yarp::os::BufferedPort<ImageRgb>;
%feature("notabstract") yarp::os::BufferedPort<ImageRgb>;
%feature("notabstract") BufferedPortImageRgb;


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

