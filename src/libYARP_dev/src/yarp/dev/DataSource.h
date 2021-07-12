/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_DATASOURCE_H
#define YARP_DEV_DATASOURCE_H

#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_DATASOURCE_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/DataSource.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

#include <yarp/os/Port.h>
#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>

// #define YARP_INCLUDING_DEPRECATED_HEADER_YARP_OS_RUNNABLE_H_ON_PURPOSE
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/Runnable.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
// #undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_OS_RUNNABLE_H_ON_PURPOSE


#include <yarp/dev/IPreciselyTimed.h>

// These classes are part of the YARP library implementation,
// rather than its user interface
#ifndef DOXYGEN_SHOULD_SKIP_THIS

const int REPORT_TIME=5; //seconds

namespace yarp {
    namespace dev {
        template <class T> class DataSource;
        template <class T> class DataWriter;
        template <class T1, class T2> class DataSource2;
        template <class T1, class T2> class DataWriter2;
    }
}

template <class T>
class YARP_DEPRECATED yarp::dev::DataSource {
public:
    virtual ~DataSource() {}
    virtual bool getDatum(T& datum) = 0;
};

template <class T>
class YARP_DEPRECATED yarp::dev::DataWriter : public yarp::os::Runnable {
private:
    yarp::os::Port& port;
    yarp::os::PortWriterBuffer<T> writer;
    DataSource<T>& dater;
    yarp::os::Stamp stamp;
    IPreciselyTimed *pPrecTime;
    bool canDrop;
    bool addStamp;
    int counter;
    double timePrevious;
    double cumulativeT;
    double minT;
    double maxT;
    double lastSpoke;
public:
    DataWriter(yarp::os::Port& port, DataSource<T>& dater,
               bool canDrop=true,
               bool addStamp=false,
               IPreciselyTimed *pt=NULL) :
        port(port),
        dater(dater),
        pPrecTime(pt),
        canDrop(canDrop),
        addStamp(addStamp),
        counter(0),
        timePrevious(0.0),
        cumulativeT(0.0),
        minT(1e10),
        maxT(0.0),
        lastSpoke(yarp::os::Time::now())
    {
        writer.attach(port);
    }

    void run() override {

        ///////////// execution statistics for logging
        double now=yarp::os::Time::now();
        double deltaT=0.0;

        if (counter==0)
        {
            lastSpoke=now;
            timePrevious=now;
            cumulativeT=0.0;
        }
        else
        {
            deltaT=now-timePrevious;
            cumulativeT+=deltaT;
            if (deltaT>maxT)
               maxT=deltaT;
            if (deltaT<minT)
               minT=deltaT;
            timePrevious=now;
        }

        counter++;

        // print report
        if (now-lastSpoke>REPORT_TIME)
            {
                yInfo("Read [%d] frames in %d[s], average period %.2lf[ms], min %.2lf[ms], max %.2lf[ms]\n",
                                counter,
                                REPORT_TIME,
                                (cumulativeT/counter)*1000,
                                minT*1000, maxT*1000);
                cumulativeT=0;
                counter=0;
                minT=1e10;
                maxT=0.0;
                lastSpoke=now;
            }
        //////////////

        T& datum = writer.get();

        dater.getDatum(datum);
        if (addStamp) {
            if (pPrecTime)
            {
                stamp=pPrecTime->getLastInputStamp();
            }
            else
            {
                stamp.update();
            }
            port.setEnvelope(stamp);
        }
        writer.write(!canDrop);
    }
};



template <class T1, class T2>
class YARP_DEPRECATED yarp::dev::DataSource2 {
public:
    virtual ~DataSource2() {}
    virtual bool getDatum(T1& datum1, T2& datum2) = 0;
};


template <class T1, class T2>
class YARP_DEPRECATED yarp::dev::DataWriter2 : public yarp::os::Runnable {
private:
    yarp::os::Port& port1;
    yarp::os::Port& port2;
    yarp::os::PortWriterBuffer<T1> writer1;
    yarp::os::PortWriterBuffer<T2> writer2;
    DataSource2<T1,T2>& dater;
    bool canDrop;
    bool addStamp;
    yarp::os::Stamp stamp;
public:
    DataWriter2(yarp::os::Port& port1,
                yarp::os::Port& port2,
                DataSource2<T1,T2>& dater,
                bool canDrop=true,
                bool addStamp=false) :
        port1(port1), port2(port2), dater(dater), canDrop(canDrop),
        addStamp(addStamp)
    {
        writer1.attach(port1);
        writer2.attach(port2);
    }

    void run() override {
        T1& datum1 = writer1.get();
        T2& datum2 = writer2.get();
        dater.getDatum(datum1,datum2);
        if (addStamp) {
            stamp.update();
            port1.setEnvelope(stamp);
            port2.setEnvelope(stamp);
        }
        writer1.write(!canDrop);
        writer2.write(!canDrop);
    }
};


#endif // DOXYGEN_SHOULD_SKIP_THIS

YARP_WARNING_POP

#endif // YARP_NO_DEPRECATED

#endif // YARP_DEV_DATASOURCE_H
