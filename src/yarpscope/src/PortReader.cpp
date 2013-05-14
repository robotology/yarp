/*
 * Copyright (C) 2006  RobotCub Consortium
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Francesco Nori, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "PortReader.h"
#include "Debug.h"
#include "PlotManager.h"

#include <glibmm/objectbase.h>
#include <glibmm/main.h>
#include <glibmm/thread.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Stamp.h>


namespace {
static YarpScope::PortReader *s_instance = NULL;
static Glib::StaticMutex s_mutex;
static const int s_bufSize = 2000;
}



class YarpScope::PortReader::Private
{
public:

    struct Index
    {
        Index(int index) :
            index(index),
            X(new float[s_bufSize]),
            Y(new float[s_bufSize]),
            T(new float[s_bufSize])
        {
            clearData();
        }

        ~Index()
        {
            delete [] X;
            delete [] Y;
            delete [] T;
        }

        void clearData()
        {
            for (int i = 0; i < s_bufSize; i++) {
                X[i] = 0;
                Y[i] = 0;
                T[i] = 0;
            }
        }

        int index;
        float *X; // X values (sequencial number)
        float *Y; // Y values (read value)
        float *T; // Time values (if realtime) or sequencial number
    };

    struct Connection
    {
        Connection(const Glib::ustring &remotePortName, const Glib::ustring &localPortName) :
            remotePortName(remotePortName),
            localPortName(localPortName),
            localPort(new yarp::os::BufferedPort<yarp::os::Bottle>()),
            realTime(false),
            initialTime(0.0),
            numberAcquiredData(0)
        {
            usedIndices.clear(); // just to be sure that is empty

            // Open the local port
            if (localPortName.empty()) {
                localPort->open();
            } else {
                localPort->open(localPortName.c_str());
            }

            realTime = true;
            initialTime = yarp::os::Time::now();
        }

        ~Connection()
        {
            for (std::vector<Index*>::iterator iit = usedIndices.begin();
                        iit != usedIndices.end(); iit++) {
                Index *idx = *iit;
                if (idx) {
                    delete idx;
                    idx = NULL;
                }
            }
            usedIndices.clear();

            yarp::os::Network::disconnect(remotePortName.c_str(), localPort->getName().c_str(), style);
            delete localPort;
        }

        void doConnect(const yarp::os::ContactStyle &style) {

            //Get the name of the port after the port is open (and therefore the real name assigned)
            const Glib::ustring &realLocalPortName = localPort->getName().c_str();

            // Connect local port to remote port
            if (!yarp::os::Network::connect(remotePortName.c_str(), realLocalPortName.c_str(), style)) {
                warning() << "Connection from port" << realLocalPortName <<  "to port" << remotePortName
                          << "was NOT successfull. Waiting from explicit connection from user.";
            } else {
                debug() << "Listening to port" << remotePortName << "from port" << realLocalPortName;
                // Connection was successfull. Save the ContactStyle in order to reuse it for disconnecting;
                this->style = style;
            }

            yarp::os::Stamp stmp;
            localPort->getEnvelope(stmp);
            if (stmp.isValid()) {
                debug() << "will use real time for port" << remotePortName;
                realTime = true;
                initialTime = stmp.getTime();
            } else {
                debug() << "will NOT use real time for port" << remotePortName;
                realTime = false;
            }
        }

        void clearData()
        {
            for (std::vector<Index*>::iterator iit = usedIndices.begin();
                        iit != usedIndices.end(); iit++) {
                Index *idx = *iit;
                idx->clearData();
            }

            PlotManager::instance().redraw(false);
        }

        Glib::ustring remotePortName;
        Glib::ustring localPortName;
        std::vector<Index*> usedIndices;
        yarp::os::BufferedPort<yarp::os::Bottle> *localPort;
        bool realTime;
        double initialTime;
        long long int numberAcquiredData;
        yarp::os::ContactStyle style;
    };

    Private(PortReader *p) :
        parent(p),
        interval(50),
        acquire(true)
    {
    }

    ~Private()
    {
        for (std::vector<Connection*>::iterator cit = connections.begin();
                    cit != connections.end(); cit++) {
            Connection *conn = *cit;
            if (conn) {
                delete conn;
                conn = NULL;
            }
            connections.clear();
        }
    }

    bool onTimeout();
    void acquireData(const Glib::ustring &remotePortName,
                     int index,
                     const Glib::ustring &localPortName,
                     const yarp::os::ContactStyle *style);
    void clearData();

    Connection* find(const Glib::ustring& remotePortName) const;
    Index* find(const Glib::ustring& remotePortName, int index) const;

    float* X(const Glib::ustring &remotePortName, int index) const;
    float* Y(const Glib::ustring &remotePortName, int index) const;
    float* T(const Glib::ustring &remotePortName, int index) const;


    PortReader * const parent;

    int interval;
    bool acquire;

    sigc::connection timer_connection;

    std::vector<Connection*> connections;
};



bool YarpScope::PortReader::Private::onTimeout()
{
    if (!acquire) {
        return true;
    }

    for (std::vector<Connection*>::iterator cit = connections.begin();
                cit != connections.end(); cit++) {
        Connection *conn = *cit;

        if (conn->usedIndices.empty()) {
            // Do not read data from this port if we don't need it
            continue;
        }

        int idx = conn->numberAcquiredData % s_bufSize;
        yarp::os::Bottle *b = conn->localPort->read(false);
        if (!b) {
            debug() << "No data received. Using previous values.";
            for (std::vector<Index*>::iterator iit = conn->usedIndices.begin();
                        iit != conn->usedIndices.end(); iit++) {
                Index *ind = *iit;
                ind->X[idx] = ind->X[idx == 0 ? s_bufSize - 1 : idx - 1];
                ind->Y[idx] = ind->Y[idx == 0 ? s_bufSize - 1 : idx - 1];
                ind->T[idx] = ind->T[idx == 0 ? s_bufSize - 1 : idx - 1];
            }
        } else {
            yarp::os::Stamp stmp;
            conn->localPort->getEnvelope(stmp);

            for (std::vector<Index*>::iterator iit = conn->usedIndices.begin();
                        iit != conn->usedIndices.end(); iit++) {
                Index *ind = *iit;

                if (b->size() - 1 < ind->index) {
                    warning() << "bottle size =" << b->size() << "requested index =" << ind->index;
                    continue;
                }

                ind->X[idx] = (float)idx;
                ind->Y[idx] = (float)(b->get(ind->index).asDouble());

                if (conn->realTime && stmp.isValid()) {
                    ind->T[idx] = (float)(stmp.getTime() - conn->initialTime);
                } else {
                    ind->T[idx] = (float)conn->numberAcquiredData;
                }
            }
        }

        conn->numberAcquiredData++;
    }

    PlotManager::instance().redraw();

    return true;
}

void YarpScope::PortReader::Private::acquireData(const Glib::ustring &remotePortName,
                                                 int index,
                                                 const Glib::ustring &localPortName,
                                                 const yarp::os::ContactStyle *style)
{
    debug() << "PortReader: Acquiring data from port" << remotePortName << "index" << index << "to port" << localPortName;
    Connection *curr_connection = NULL;
    for (std::vector<Connection*>::iterator cit = connections.begin();
                cit != connections.end(); cit++) {
        Connection *conn = *cit;
        if (conn->remotePortName.compare(remotePortName) == 0) {
            if (!conn->localPortName.compare(localPortName)) {
                warning() << "Trying to connect the same remote port to 2 different local ports. Only the first will be used.";
            }
            curr_connection = conn;
            break;
        }
    }
    if (!curr_connection) {
        connections.push_back(new Connection(remotePortName, localPortName));
        curr_connection = connections.back();

        if (style) {
            // If style is not null the ports are connected, otherwise the user will
            // have to do it manually
            curr_connection->doConnect(*style);
        }

    }


    bool found = false;
    for (std::vector<Index*>::iterator iit = curr_connection->usedIndices.begin();
                iit != curr_connection->usedIndices.end(); iit++) {
        Index *idx = *iit;
        if (idx->index == index) {
            found = true;
            break;
        }
    }
    if (!found) {
        curr_connection->usedIndices.push_back(new Index(index));
    }

}

void YarpScope::PortReader::Private::clearData()
{
    for (std::vector<Connection*>::iterator cit = connections.begin();
                cit != connections.end(); cit++) {
        Connection *conn = *cit;
        conn->clearData();
    }
}

YarpScope::PortReader::Private::Connection* YarpScope::PortReader::Private::find(const Glib::ustring& remotePortName) const
{
    if (remotePortName.empty() && connections.size() == 1) {
        return connections.at(0);
    }
    for (std::vector<Connection*>::const_iterator cit = connections.begin();
                cit != connections.end(); cit++) {
        Connection *conn = *cit;
        if (conn->remotePortName.compare(remotePortName) == 0) {
            return conn;
        }
    }
    return NULL;
}


YarpScope::PortReader::Private::Index* YarpScope::PortReader::Private::find(const Glib::ustring& remotePortName, int index) const
{
    Connection *conn = find(remotePortName);
    if (conn) {
        for (std::vector<Index*>::iterator iit = conn->usedIndices.begin();
                    iit != conn->usedIndices.end(); iit++) {
            Index *idx = *iit;
            if (idx->index == index) {
                return idx;
            }
        }
    }
    return NULL;
}

float* YarpScope::PortReader::Private::X(const Glib::ustring& remotePortName, int index) const
{
    Index *idx = find(remotePortName, index);
    if (idx) {
        return idx->X;
    }
    return NULL;
}

float* YarpScope::PortReader::Private::Y(const Glib::ustring& remotePortName, int index) const
{
    Index *idx = find(remotePortName, index);
    if (idx) {
        return idx->Y;
    }
    return NULL;
}

float* YarpScope::PortReader::Private::T(const Glib::ustring& remotePortName, int index) const
{
    Index *idx = find(remotePortName, index);
    if (idx) {
        return idx->T;
    }
    return NULL;
}


YarpScope::PortReader::PortReader() :
    mPriv(new Private(this))
{
    toggleAcquire(true);
}

YarpScope::PortReader::~PortReader()
{
    delete mPriv;
}

YarpScope::PortReader& YarpScope::PortReader::instance()
{
    // I don't know if a static Glib::Mutex is thread safe but it shouldn't be
    // used on concurrent threads during the creation, so it shouldn't be a
    // problem.
    Glib::Mutex::Lock lock(s_mutex);
    if (!s_instance) {
        s_instance = new PortReader();
    }

    return *s_instance;
}

void YarpScope::PortReader::toggleAcquire(bool acquire)
{
    if (acquire && !mPriv->timer_connection.connected()) {
        mPriv->timer_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(*mPriv, &YarpScope::PortReader::Private::onTimeout), mPriv->interval);
    } else if (!acquire && mPriv->timer_connection.connected()) {
        mPriv->timer_connection.disconnect();
    }

    mPriv->acquire = acquire;
}

void YarpScope::PortReader::toggleAcquire()
{
    toggleAcquire(!mPriv->acquire);
}

bool YarpScope::PortReader::isAcquiring() const
{
    return mPriv->acquire;
}

void YarpScope::PortReader::setInterval(int interval)
{
    if (mPriv->timer_connection.connected()) {
        mPriv->timer_connection.disconnect();
    }

    mPriv->interval = interval;
    if (mPriv->acquire) {
        mPriv->timer_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(*mPriv, &YarpScope::PortReader::Private::onTimeout), mPriv->interval);
    }
}

int YarpScope::PortReader::interval() const
{
    return mPriv->interval;
}

void YarpScope::PortReader::clearData()
{
    mPriv->clearData();
}

void YarpScope::PortReader::acquireData(const Glib::ustring &remotePortName,
                                        int index,
                                        const Glib::ustring &localPortName,
                                        const Glib::ustring &carrier,
                                        bool persistent)
{
    // Setup ConnectionStyle
    yarp::os::ContactStyle  style;
    style.persistent = persistent;
    style.carrier = carrier.c_str();
    mPriv->acquireData(remotePortName, index, localPortName, &style);
}

float* YarpScope::PortReader::X(const Glib::ustring& remotePortName, int index) const
{
    return mPriv->X(remotePortName, index);
}
float* YarpScope::PortReader::Y(const Glib::ustring& remotePortName, int index) const
{
    return mPriv->Y(remotePortName, index);
}
float* YarpScope::PortReader::T(const Glib::ustring& remotePortName, int index) const
{
    return mPriv->T(remotePortName, index);
}

int YarpScope::PortReader::bufSize() const
{
    return s_bufSize;
}
