/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/conf/system.h>

#include "UnixSockTwoWayStream.h"

//#include <yarp/os/impl/Logger.h>
#include <yarp/os/Time.h>
#include <yarp/os/NetType.h>
#include <yarp/os/LogStream.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */

#include <cerrno>
#include <cstring>

using namespace yarp::os;
using namespace std;


UnixSockTwoWayStream::UnixSockTwoWayStream(const std::string& _socketPath) :
        socketPath(_socketPath)
{
}

bool UnixSockTwoWayStream::open(bool sender)
{
    struct sockaddr_un addr;
    if ((fd = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("UnixSockTwoWayStream error:");
        return false;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    if (socketPath.empty())
    {
      *addr.sun_path = '\0';
      strncpy(addr.sun_path+1, socketPath.c_str()+1, sizeof(addr.sun_path)-2);
    }
    else
    {
      strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path)-1);
      if (!sender)
      {
          ::unlink(socketPath.c_str());
      }
    }

    if (sender)
    {
        int attempts = 0;
        //try connection 5 times, waiting that the receiver bind the socket
        while (attempts < 5)
        {
            int result = ::connect(fd, (struct sockaddr*)&addr, sizeof(addr));
            if (result == 0)
            {
                break;
            }
            yarp::os::Time::delay(0.01);
            attempts++;
        }

        if (attempts >= 5)
        {
            perror("UnixSockTwoWayStream connect error, I tried 5 times...");
            return false;
        }
    }
    else
    {
        if (::bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        {
            perror("UnixSockTwoWayStream bind error");
            return false;
        }

        // the socket will listen only 1 client
        if (::listen(fd, 2) == -1)
        {
            perror("UnixSockTwoWayStream listen error");
            return false;
        }
        struct sockaddr_un remote;
        uint lenRemote = sizeof(remote);

        if ((cl = ::accept(fd, (struct sockaddr *)&remote, &lenRemote)) == -1)
        {
            perror("UnixSockTwoWayStream accept error");
            return false;
        }
    }


    return true;
}

UnixSockTwoWayStream::~UnixSockTwoWayStream()
{
    closeMain();
}

InputStream& UnixSockTwoWayStream::getInputStream()
{
    return *this;
}

OutputStream& UnixSockTwoWayStream::getOutputStream()
{
    return *this;
}

const Contact& UnixSockTwoWayStream::getLocalAddress() const
{
    return localAddress;
}

const Contact& UnixSockTwoWayStream::getRemoteAddress() const
{
    return remoteAddress;
}

void UnixSockTwoWayStream::setLocalAddress(Contact &_localAddress)
{
    localAddress = _localAddress;
}

void UnixSockTwoWayStream::setRemoteAddress(Contact &_remoteAddress)
{
    remoteAddress = _remoteAddress;
}

void UnixSockTwoWayStream::interrupt()
{
    bool act = false;
    mutex.lock();
    if ((!closed) && (!interrupting) && happy) {
        act = true;
        interrupting = true;
        closed = true;
    }
    mutex.unlock();
    if (act) {
        if(reader)
        {
            int ct = 3;
            while (happy && ct>0) {
                ct--;
                UnixSockTwoWayStream tmp(socketPath);
                tmp.open(true);
                ManagedBytes empty(10);
                for (size_t i=0; i<empty.length(); i++) {
                    empty.get()[i] = 0;
                }

                tmp.fd = cl; // this allows the fake stream to write on the socket waiting something to read.
                tmp.write(empty.bytes());
                tmp.flush();
                tmp.close();
                if (happy) {
                    yarp::os::SystemClock::delaySystem(0.25);
                }
            }
            yDebug("dgram interrupt done");
        }
        mutex.lock();
        interrupting = false;
        mutex.unlock();
    } else {
        // wait for interruption to be done
        if (interrupting) {
            while (interrupting) {
                yDebug("waiting for dgram interrupt to be finished...");
                yarp::os::SystemClock::delaySystem(0.1);
            }
        }
    }
}

void UnixSockTwoWayStream::close()
{
    closeMain();

}

void UnixSockTwoWayStream::closeMain()
{
    if (fd > 0) //check socket id
    {
        interrupt();
        mutex.lock();
        closed = true;
        mutex.unlock();
        while (interrupting) {
            happy = false;
            yarp::os::SystemClock::delaySystem(0.1);
        }
        mutex.lock();
        // If the connect descriptor is valid close socket
        // and free the memory dedicated.
        //socket closure
        if (reader)
        {
            ::shutdown(cl, SHUT_RDWR);
            ::close(cl);
            ::unlink(socketPath.c_str());
            cl = -1;
        }
        else
        {
            ::shutdown(fd, SHUT_RDWR);
            ::close(fd);
            fd = -1;

        }
        happy = false;
        mutex.unlock();
    }
    happy = false;

}

yarp::conf::ssize_t UnixSockTwoWayStream::read(Bytes& b)
{
    reader = true;

    int result;
    result = ::read(cl, b.get(), b.length());
    if (closed || result == 0) {
        happy = false;
        return -1;
    }
    if (result<0)
    {
        perror("unixSock::read():Packet payload");
        return -1;
    }
    return result;
}

void UnixSockTwoWayStream::write(const Bytes& b)
{
    if (reader) {
        return;
    }

    if (fd < 0)
    {
        close();
        return;
    }

    int writtenMem = ::write(fd, b.get(), b.length());
    if (writtenMem < 0)
    {
        perror("unixSock::write:Packet payload");
        if(errno != ETIMEDOUT)
        {
            close();
        }
        return;
    }
}

void UnixSockTwoWayStream::flush()
{
}

bool UnixSockTwoWayStream::isOk() const
{
    return happy;
}

void UnixSockTwoWayStream::reset()
{
}

void UnixSockTwoWayStream::beginPacket()
{
}

void UnixSockTwoWayStream::endPacket()
{
}

Bytes UnixSockTwoWayStream::getMonitor()
{
    return monitor.bytes();
}

void UnixSockTwoWayStream::setMonitor(const Bytes &data)
{
    monitor = yarp::os::ManagedBytes(data, false);
    monitor.copy();
}

void UnixSockTwoWayStream::removeMonitor()
{
    monitor.clear();
}
