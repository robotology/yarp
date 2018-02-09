/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
        while (!isStopping()&&!reader->isClosed()) {
            if (reader->read()) {
                callback->onRead(*(reader->lastRead()),
                                    *reader);
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

