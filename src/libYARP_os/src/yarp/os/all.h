/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_ALL_H
#define YARP_OS_ALL_H

// everything for the yarp::os namespace
#include <yarp/os/AbstractContactable.h>
#include <yarp/os/BinPortable.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Clock.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Event.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/MessageStack.h>
#include <yarp/os/NetFloat32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetInt64.h>
#include <yarp/os/NetInt8.h>
#include <yarp/os/NetUint16.h>
#include <yarp/os/NetUint32.h>
#include <yarp/os/NetUint64.h>
#include <yarp/os/NetUint8.h>
#include <yarp/os/Network.h>
#include <yarp/os/NetworkClock.h>
#include <yarp/os/Node.h>
#include <yarp/os/Os.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/PortReaderCreator.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/Portable.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Property.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Random.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Terminator.h>
#include <yarp/os/Things.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/UnbufferedContactable.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Wire.h>
#include <yarp/os/WireLink.h>

#ifndef YARP_NO_DEPRECATED
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
     // since YARP 3.0.0
#    include <yarp/os/ConstString.h>
#    include <yarp/os/RateThread.h>
     // since YARP 3.3
#    include <yarp/os/LockGuard.h>
#    include <yarp/os/Mutex.h>
#    include <yarp/os/RecursiveMutex.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif // YARP_NO_DEPRECATED


/**
 * @namespace yarp
 * The main, catch-all namespace for YARP.
 */

/**
 * @namespace yarp::os
 * An interface to the operating system, including Port based communication.
 */

/**
 * @namespace yarp::os::impl
 * The components from which ports and connections are built.
 *
 * These classes are not intended for regular users, but rather
 * for those extending YARP to new situations.
 *
 * Unlike the classes in yarp::os, yarp::sig, and yarp::dev,
 * there are dependencies on the ACE library here.
 */

#endif // YARP_OS_ALL_H
