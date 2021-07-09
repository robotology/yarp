/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
