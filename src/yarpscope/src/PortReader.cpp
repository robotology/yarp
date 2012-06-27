/*
 *  This file is part of gPortScope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
static GPortScope::PortReader *s_instance = NULL;
static Glib::Mutex s_mutex;
static const int s_bufSize = 2000;
static const Glib::ustring s_carrier = "mcast";
}



class GPortScope::PortReader::Private
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
        Connection(const Glib::ustring &remotePortName) :
            remotePortName(remotePortName),
            localPortName("/gPortScope" + remotePortName),
            localPort(new yarp::os::BufferedPort<yarp::os::Bottle>()),
            realTime(false),
            initialTime(0.0),
            numberAcquiredData(0)
        {
            usedIndices.clear(); // just to be sure that is empty

            // Open the local port
            localPort->open(localPortName.c_str());
            localPortName = localPort->getName().c_str(); //Get the name of the port if YARP_PORT_PREFIX environment variable is set

            // Connect local port to remote port
            if (!yarp::os::Network::connect(remotePortName.c_str(), localPortName.c_str(), s_carrier.c_str())) {
                warning() << "Connection from port" << localPortName <<  "to port" << remotePortName
                          << "was NOT successfull. Waiting from explicit connection from user.";
            } else {
                debug() << "Listening to port" << remotePortName << "from port" << localPortName;
            }

            yarp::os::Stamp stmp;
            localPort->getEnvelope(stmp);
            if (stmp.isValid()) {
                debug() << "will use real time for port" << remotePortName;
                realTime = true;
                initialTime = stmp.getTime();
            }
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
    };

    Private(PortReader *p) :
        parent(p),
        interval(50),
        acquire(true)
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
    void acquireData(const Glib::ustring &remotePortName, int index);
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



bool GPortScope::PortReader::Private::onTimeout()
{
    debug() << "GPortScope::PortReader::Private::onTimeout called";

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
        int timeout = 0;
        int maxTimeout = 2;

        yarp::os::Bottle *b = NULL;
        while ( !b && timeout < maxTimeout ) {
            b = conn->localPort->read(false);
            if (!b) {
                yarp::os::Time::delay(0.0001);
                timeout++;
            }
        }

        if (timeout == maxTimeout || b == NULL) {
            debug() << "No data received. Using previous values.";
            for (std::vector<Index*>::iterator iit = conn->usedIndices.begin();
                        iit != conn->usedIndices.end(); iit++) {
                Index *ind = *iit;
                ind->X[idx] = ind->X[(idx - 1) % s_bufSize];
                ind->Y[idx] = ind->Y[(idx - 1) % s_bufSize];
                ind->T[idx] = ind->T[(idx - 1) % s_bufSize];
            }
            continue;
        }

        yarp::os::Stamp stmp;
        conn->localPort->getEnvelope(stmp);

        for (std::vector<Index*>::iterator iit = conn->usedIndices.begin();
                    iit != conn->usedIndices.end(); iit++) {
            Index *ind = *iit;

            if (b->size() - 1 < ind->index) {
                warning() << "bottle size =" << b->size() << "requested index =" << ind->index;
                continue;
            }

            ind->X[idx] = idx;
            ind->Y[idx] = b->get(ind->index).asDouble();

            if (conn->realTime && stmp.isValid()) {
                ind->T[idx] = (stmp.getTime() - conn->initialTime);
            } else {
                ind->T[idx] = conn->numberAcquiredData;
            }
        }
        conn->numberAcquiredData++;
    }

    PlotManager::instance().redraw();

    return true;
}

void GPortScope::PortReader::Private::acquireData(const Glib::ustring& remotePortName, int index)
{
    debug() << "PortReader: Acquiring data from port" << remotePortName << "index" << index;
    Connection *curr_connection = NULL;
    for (std::vector<Connection*>::iterator cit = connections.begin();
                cit != connections.end(); cit++) {
        Connection *conn = *cit;
        if (conn->remotePortName.compare(remotePortName) == 0) {
            curr_connection = conn;
            break;
        }
    }
    if (!curr_connection) {
        connections.push_back(new Connection(remotePortName));
        curr_connection = connections.back();
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

void GPortScope::PortReader::Private::clearData()
{
    for (std::vector<Connection*>::iterator cit = connections.begin();
                cit != connections.end(); cit++) {
        Connection *conn = *cit;
        conn->clearData();
    }
}

GPortScope::PortReader::Private::Connection* GPortScope::PortReader::Private::find(const Glib::ustring& remotePortName) const
{
    for (std::vector<Connection*>::const_iterator cit = connections.begin();
                cit != connections.end(); cit++) {
        Connection *conn = *cit;
        if (conn->remotePortName.compare(remotePortName) == 0) {
            return conn;
        }
    }
    return NULL;
}


GPortScope::PortReader::Private::Index* GPortScope::PortReader::Private::find(const Glib::ustring& remotePortName, int index) const
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

float* GPortScope::PortReader::Private::X(const Glib::ustring& remotePortName, int index) const
{
    Index *idx = find(remotePortName, index);
    if (idx) {
        return idx->X;
    }
    return NULL;
}

float* GPortScope::PortReader::Private::Y(const Glib::ustring& remotePortName, int index) const
{
    Index *idx = find(remotePortName, index);
    if (idx) {
        return idx->Y;
    }
    return NULL;
}

float* GPortScope::PortReader::Private::T(const Glib::ustring& remotePortName, int index) const
{
    Index *idx = find(remotePortName, index);
    if (idx) {
        return idx->T;
    }
    return NULL;
}


GPortScope::PortReader::PortReader() :
    mPriv(new Private(this))
{
    toggleAcquire(true);
}

GPortScope::PortReader::~PortReader()
{
    delete mPriv;
}

GPortScope::PortReader& GPortScope::PortReader::instance()
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

void GPortScope::PortReader::toggleAcquire(bool acquire)
{
    if (acquire && !mPriv->timer_connection.connected()) {
        mPriv->timer_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(*mPriv, &GPortScope::PortReader::Private::onTimeout), mPriv->interval);
    } else if (!acquire && mPriv->timer_connection.connected()) {
        mPriv->timer_connection.disconnect();
    }

    mPriv->acquire = acquire;
}

void GPortScope::PortReader::toggleAcquire()
{
    toggleAcquire(!mPriv->acquire);
}

bool GPortScope::PortReader::isAcquiring() const
{
    return mPriv->acquire;
}

void GPortScope::PortReader::setInterval(int interval)
{
    if (mPriv->timer_connection.connected()) {
        mPriv->timer_connection.disconnect();
    }

    mPriv->interval = interval;
    if (mPriv->acquire) {
        mPriv->timer_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(*mPriv, &GPortScope::PortReader::Private::onTimeout), mPriv->interval);
    }
}

int GPortScope::PortReader::interval() const
{
    return mPriv->interval;
}

void GPortScope::PortReader::clearData()
{
    mPriv->clearData();
}


void GPortScope::PortReader::acquireData(const Glib::ustring& remotePortName, int index)
{
    mPriv->acquireData(remotePortName, index);
}

float* GPortScope::PortReader::X(const Glib::ustring& remotePortName, int index) const
{
    return mPriv->X(remotePortName, index);
}
float* GPortScope::PortReader::Y(const Glib::ustring& remotePortName, int index) const
{
    return mPriv->Y(remotePortName, index);
}
float* GPortScope::PortReader::T(const Glib::ustring& remotePortName, int index) const
{
    return mPriv->T(remotePortName, index);
}

int GPortScope::PortReader::bufSize() const
{
    return s_bufSize;
}
