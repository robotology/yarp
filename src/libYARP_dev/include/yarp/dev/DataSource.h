// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef YARP2_DATASOURCE
#define YARP2_DATASOURCE

#include <yarp/os/Port.h>
#include <yarp/os/Runnable.h>
#include <yarp/os/PortWriterBuffer.h>

namespace yarp {
    namespace dev {
        template <class T> class DataSource;
        template <class T> class DataWriter;
        template <class T1, class T2> class DataSource2;
        template <class T1, class T2> class DataWriter2;
    }
}


template <class T>
class yarp::dev::DataSource {
public:
    virtual bool getDatum(T& datum) = 0;
};


template <class T>
class yarp::dev::DataWriter : public yarp::os::Runnable {
private:
    yarp::os::Port& port;
    yarp::os::PortWriterBuffer<T> writer;
    DataSource<T>& dater;
public:
    DataWriter(yarp::os::Port& port, DataSource<T>& dater) : 
        port(port), dater(dater)
    {
        writer.attach(port);
    }

    virtual void run() {
        T& datum = writer.get();
        dater.getDatum(datum);
        writer.write();
    }
};



template <class T1, class T2>
class yarp::dev::DataSource2 {
public:
    virtual bool getDatum(T1& datum, T2& datum) = 0;
};


template <class T1, class T2>
class yarp::dev::DataWriter2 : public yarp::os::Runnable {
private:
    yarp::os::Port& port1;
    yarp::os::Port& port2;
    yarp::os::PortWriterBuffer<T1> writer1;
    yarp::os::PortWriterBuffer<T2> writer2;
    DataSource2<T1,T2>& dater;
public:
    DataWriter2(yarp::os::Port& port1, 
                yarp::os::Port& port2,
                DataSource2<T1,T2>& dater) : 
        port1(port1), port2(port2), dater(dater)
    {
        writer1.attach(port1);
        writer2.attach(port2);
    }

    virtual void run() {
        T1& datum1 = writer1.get();
        T2& datum2 = writer2.get();
        dater.getDatum(datum1,datum2);
        writer1.write();
        writer2.write();
    }
};


#endif

