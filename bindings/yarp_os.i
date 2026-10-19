// SPDX-FileCopyrightText: 2006-2026 Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

//////////////////////////////////////////////////////////////////////////
//
// SWIG wrapping for the YARP_os library.
//
// This file is not a standalone module: it is meant to be included by
// yarp.i, before YARP_sig and YARP_dev.

// YARP_os
%{
#include <yarp/os/api.h>
%}

%import <yarp/os/api.h>

#if !defined (SWIGMATLAB)
%feature("director") yarp::os::PortReader;
%feature("director") yarp::os::RFModule;
%feature("director") yarp::os::Thread;
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

// abstract methods just confuse SWIG
%ignore yarp::os::BufferedPort::open; // let Contactable::open show
%ignore yarp::os::Port::open; // let Contactable::open show
%ignore yarp::os::RpcClient::open;

// These methods do not get translated well
%ignore yarp::PortReaderBuffer;
%ignore yarp::os::Property::put(const char *,Value *);
%ignore yarp::os::Bottle::add(Value *);

// Deal with some shadowing in python
#ifdef SWIGPYTHON
%ignore yarp::os::Property::fromCommand(int, char const *[]);
%ignore yarp::os::Property::fromCommand(int, char const *[], bool);
%ignore yarp::os::Property::fromCommand(int, char const *[], bool, bool);
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

MAKE_COMMS  (Bottle, yarp::os::Bottle)

//////////////////////////////////////////////////////////////////////////
// Deal with poor translation of interface inheritance in current SWIG

%extend yarp::os::Port {
    bool write(Bottle& data) {
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

#ifdef SWIGJAVA


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
