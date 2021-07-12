/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_TYPEDREADERTHREAD_H
#define YARP_OS_TYPEDREADERTHREAD_H

#include <yarp/os/Thread.h>

namespace yarp {
namespace os {

template <typename T>
class TypedReader;

template <typename T>
class TypedReaderCallback;

template <typename T>
class TypedReaderThread : public Thread
{
public:
    TypedReader<T>* reader;
    TypedReaderCallback<T>* callback;

    TypedReaderThread();

    TypedReaderThread(TypedReader<T>& reader,
                      TypedReaderCallback<T>& callback);

    void run() override;

    void onStop() override;
};

} // namespace os
} // namespace yarp

#include <yarp/os/TypedReaderThread-inl.h>

#endif // YARP_OS_TYPEDREADERTHREAD_H
