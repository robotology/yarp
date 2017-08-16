/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_TYPEDREADERCALLBACK_INL_H
#define YARP_OS_TYPEDREADERCALLBACK_INL_H


#ifndef DOXYGEN_SHOULD_SKIP_THIS
YARP_OS_API void typedReaderMissingCallback();
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

template <typename T>
yarp::os::TypedReaderCallback<T>::~TypedReaderCallback()
{
}

template <typename T>
void yarp::os::TypedReaderCallback<T>::onRead(T& datum)
{
    YARP_UNUSED(datum);
    typedReaderMissingCallback();
}

template <typename T>
void yarp::os::TypedReaderCallback<T>::onRead(T& datum, const yarp::os::TypedReader<T>& reader)
{
    YARP_UNUSED(reader);
    onRead(datum);
}

#ifdef _MSC_VER
template class YARP_OS_API yarp::os::TypedReaderCallback<yarp::os::Bottle>;
#endif

#endif // YARP_OS_TYPEDREADERCALLBACK_INL_H
