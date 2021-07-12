/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPL_FIXEDSIZEBUFFERSMANAGER_H
#define YARP_DEV_IMPL_FIXEDSIZEBUFFERSMANAGER_H

#include <vector>
#include <mutex>


namespace yarp {
namespace dev {
namespace impl {


/**
 * Buffer contains info about a buffer of type T and it is used to exchange
 * information with yarp::dev::impl::FixedSizeBuffersManager.
 */
template <typename T>
class Buffer
{
public:
    uint32_t key;
    T* dataPtr;
    uint32_t numOfElements;

    /**
     * @brief Constructor.
     */
    Buffer();

    /**
     * @brief Destructor.Note that the memory will not deallocated.
     */
    ~Buffer();

    /**
     * @brief Return the data pointer
     */
    T* getData();

    /**
     * @brief Return the number of element of buffer
     */
    uint32_t getSize();

    /**
     * @brief Return the value contained in the buffer at index @c index
     *
     * If @c index is not minor of size of buffer than a std::out_of_range is
     * thrown.
     */
    T getValue(uint32_t index);

    /**
     * Set the value @value in the buffer at index @c index.
     *
     * If @c index is not minor of size of buffer than a std::out_of_range is
     * thrown.
     */
    void setValue(uint32_t index, T value);

    /**
     * @brief Access specified element.
     */
    T& operator[](uint32_t index);
};


/**
 * @brief A manager of fixed size buffers in multi-thread environment.
 *
 * When it is created it allocates @c initialNumOfBuffers buffers of size
 * @c sizeOfBuffers.
 * The user can get a buffer, uses and releases it when he/she doesn't need
 * anymore.
 * If all buffers are busy and a new buffer is required, than the manager
 * creates new one.
 */
template <typename T>
class FixedSizeBuffersManager
{
public:
    /**
     * @brief Constructor.
     *
     * Allocate @c initialNumOfBuffers buffers of size sizeOfBuffers.
     */
    explicit FixedSizeBuffersManager(uint32_t sizeOfBuffers, std::size_t initialNumOfBuffers = 3);

    /**
     * @brief Destructor
     *
     * Delete all allocated buffers.
     */
    ~FixedSizeBuffersManager();

    /**
     * @brief Get a buffer and fill its information in @buffer.
     *
     * @note A buffer is always available unless a std::bad_alloc is thrown,
     * therefore use statement catch to guarantee the required buffer is not
     * null.
     */
    Buffer<T> getBuffer();

    /**
     * @brief Get the number of elements of a buffer.
     */
    std::size_t getBufferSize();

    /**
     * @brief Release a buffer.
     *
     * This function should be used by user when he/she doesn't need it anymore.
     * It releases a buffer in constant time.
     * @param[in] buffer
     */
    void releaseBuffer(Buffer<T>& buffer);

    /**
     * @brief Prints info of buffers.
     *
     * Function for debug purpose
     */
    void printBuffers();

private:
    std::mutex m_mutex;
    std::vector<T*> m_buffers;
    std::vector<bool> m_usedBuffers;
    std::size_t m_numElem;
    uint32_t m_firstFreeBuff; //euristic
};


} // namespace impl
} // namespace dev
} // namespace yarp

#include "FixedSizeBuffersManager-inl.h"

#endif // YARP_DEV_IMPL_FIXEDSIZEBUFFERSMANAGER_H
