/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_TYPEDREADERCALLBACK_INL_H
#define YARP_OS_TYPEDREADERCALLBACK_INL_H


#ifndef DOXYGEN_SHOULD_SKIP_THIS
YARP_os_API void typedReaderMissingCallback();
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
template class YARP_os_API yarp::os::TypedReaderCallback<yarp::os::Bottle>;
#endif

#endif // YARP_OS_TYPEDREADERCALLBACK_INL_H
