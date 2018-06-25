/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_FIXEDSIZEBUFFERSMANAGER_H
#define YARP_OS_FIXEDSIZEBUFFERSMANAGER_H

#include <yarp/os/Mutex.h>
#include <vector>
#include <stdexcept>


// Defined in this file:
namespace yarp { namespace os { template <typename T> class FixedSizeBuffersManager; }}
namespace yarp { namespace os { template <typename T> class Buffer; }}


namespace yarp {
namespace os {


/**
* Buffer contains info about a buffer of type T and it is used to exchange information
* with yarp::os::FixedSizeBuffersManager.
*/

template <typename T>
class Buffer
{
friend class FixedSizeBuffersManager<T>;

private:
    uint32_t key;
    T* dataPtr;
    uint32_t numOfElements;
public:
    /**
     * Costructor.
     */
    Buffer();

    /**
     * Destructor.Note that the memory will not deallocated.
     */
    ~Buffer();

    /**
     * Return the data pointer
     *
     */
    T* getData();

    /**
     * Return the number of element of buffer
     *
     */
    uint32_t getSize();

    /**
     * Return the value contained in the buffer at index @index
     * If @index is not minor of size of buffer than a std::out_of_range is thrown.
     *
     */
    T getValue(uint32_t index) throw (std::out_of_range);

    /**
     * Set the value @value in the buffer at index @index.
     * If @index is not minor of size of buffer than a std::out_of_range is thrown.
     *
     */
    void setValue(uint32_t index, T value) throw (std::out_of_range);

    T& operator[](uint32_t index);
};





/**
 * Fixedsizebuffermanager, like name says, is a manager of fixed size buffers in multi-thread environment.
 * When it is created it allocates @initialNumOfBuffers buffers of size @zizeOfBuffers.
 * The user can get a buffer, uses and releases it when he/she doesn't need anymore.
 * If all buffers are busy and a new buffer is required, than the manager creates new one.
 *
 */
template <typename T>
class FixedSizeBuffersManager
{
public:

    /**
     * Costructor.Allocate @initialNumOfBuffers buffer of size zizeOfBuffers
     */
    explicit FixedSizeBuffersManager(uint32_t zizeOfBuffers, std::size_t initialNumOfBuffers=3);

    /**
     * Destructor: delete all allocated buffer
     */
    ~FixedSizeBuffersManager();

    /**
     * Get a buffer and fill its information in @buffer.
     * Note that a buffer is always availble unless a bad_allocation is thrown, therofore use statement catch to guarantee the required buffer is not null.
     */
     Buffer<T> getBuffer(void);

    /**
     * Get the number of elements of a buffer.
     */
    std::size_t getBufferSize(void);


    /**
     * Release a buffer. This function should be used by user when he/she doesn't need it anymore.
     * This function releases a buffer in constant time.
     *  @param[in] buffer
     */
    void releaseBuffer(Buffer<T> &buffer);


    /**
     * prints info of buffers.
     * Function for debug purpose
     */
    void printBuffers(void);


private:
    yarp::os::Mutex m_mutex;
    std::vector<T*> m_buffers;
    std::vector<bool> m_usedBuffers;
    std::size_t m_numElem;
    uint32_t m_firstFreeBuff; //euristic

};


} // namespace os
} // namespace yarp

#include "FixedSizeBuffersManager-inl.h"

#endif // YARP_OS_FIXEDSIZEBUFFERSMANAGER_H