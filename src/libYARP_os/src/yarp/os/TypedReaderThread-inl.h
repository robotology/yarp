/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

template <class T>
yarp::os::TypedReaderThread<T>::TypedReaderThread() :
        reader(nullptr),
        callback(nullptr)
{
}

template <class T>
yarp::os::TypedReaderThread<T>::TypedReaderThread(TypedReader<T>& reader,
                                                  TypedReaderCallback<T>& callback)
{
    this->reader = &reader;
    this->callback = &callback;
    start(); // automatically starts running
}

template <class T>
void yarp::os::TypedReaderThread<T>::run()
{
    if (reader != nullptr && callback != nullptr) {
        while (!isStopping() && !reader->isClosed()) {
            if (reader->read()) {
                callback->onRead(*(reader->lastRead()), *reader);
            }
        }
    }
}

template <class T>
void yarp::os::TypedReaderThread<T>::onStop()
{
    if (reader != nullptr) {
        reader->interrupt();
    }
}
