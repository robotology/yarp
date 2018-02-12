/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_TYPEDREADERTHREAD_H
#define YARP_OS_TYPEDREADERTHREAD_H

#include <yarp/os/Thread.h>

// Defined in this file:
namespace yarp { namespace os { template <typename T> class TypedReaderThread; }}

namespace yarp { namespace os { template <typename T> class TypedReader; }}
namespace yarp { namespace os { template <typename T> class TypedReaderCallback; }}

namespace yarp {
namespace os {

template <typename T>
class TypedReaderThread : public Thread {
public:
    TypedReader<T> *reader;
    TypedReaderCallback<T> *callback;

    TypedReaderThread();

    TypedReaderThread(TypedReader<T>& reader,
                      TypedReaderCallback<T>& callback);

    virtual void run() override;

    virtual void onStop() override;
};

} // namespace os
} // namespace yarp

#include <yarp/os/TypedReaderThread-inl.h>

#endif // YARP_OS_TYPEDREADERTHREAD_H
