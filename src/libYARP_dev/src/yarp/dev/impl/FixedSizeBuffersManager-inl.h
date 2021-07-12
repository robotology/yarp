/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <stdexcept>


template <typename T>
yarp::dev::impl::Buffer<T>::Buffer() :
        key(0), dataPtr(nullptr), numOfElements(0)
{
}


template <typename T>
yarp::dev::impl::Buffer<T>::~Buffer()
{
}


template <typename T>
T* yarp::dev::impl::Buffer<T>::getData()
{
    return dataPtr;
}


template <typename T>
uint32_t yarp::dev::impl::Buffer<T>::getSize()
{
    return numOfElements;
}


template <typename T>
T yarp::dev::impl::Buffer<T>::getValue(uint32_t index)
{
    if (index < numOfElements) {
        return dataPtr[index];
    } else {
        throw std::out_of_range("yarp::dev::impl::Buffer::getValue(index): index is out of range");
    }
}


template <typename T>
void yarp::dev::impl::Buffer<T>::setValue(uint32_t index, T value)
{
    if (index < numOfElements) {
        dataPtr[index] = value;
    } else {
        throw std::out_of_range("yarp::dev::impl::Buffer::setValue(index, value): index is out of range");
    }
}


template <typename T>
T& yarp::dev::impl::Buffer<T>::operator[](uint32_t index)
{
    return dataPtr[index];
}


template <typename T>
yarp::dev::impl::FixedSizeBuffersManager<T>::FixedSizeBuffersManager(uint32_t zizeOfBuffers, std::size_t initialNumOfBuffers)
{
    m_numElem = zizeOfBuffers;
    m_buffers.resize(0);

    for (size_t i = 0; i < initialNumOfBuffers; i++) {
        T* buff = new T[m_numElem];
        m_buffers.push_back(buff);
    }
    m_usedBuffers.resize(initialNumOfBuffers, false);
    m_firstFreeBuff = 0;
}


template <typename T>
yarp::dev::impl::Buffer<T> yarp::dev::impl::FixedSizeBuffersManager<T>::getBuffer()
{
    m_mutex.lock();
    //get first free buffer
    Buffer<T> buffer;
    uint32_t i;
    T* dataPtr;
    bool needNewBuff = true;
    if (false == m_usedBuffers[m_firstFreeBuff]) {
        //you are lucky
        i = m_firstFreeBuff;
        needNewBuff = false;
    } else {
        for (std::size_t p = 0; p < m_buffers.size(); p++) {
            if (false == m_usedBuffers[p]) {
                i = p;
                needNewBuff = false;
                break;
            }
        }
    }

    //if all buffers are used, I create new one and return it
    if (needNewBuff)
    {
        dataPtr = new T[m_numElem];
        if (nullptr == dataPtr) {
            //I should not never be here because, if no more memory is available, I should be in "catch" branch
            yError() << "FixedSizeBuffersManager::getBuffer() no more memory!!";
        }
        m_buffers.push_back(dataPtr);
        m_usedBuffers.push_back(true);
        // yError() << "I need to create a new buffer. Now size is " << m_buffers.size() << "pointer is " << dataPtr;
        i = m_buffers.size() - 1;
    }
    else
    {
        //use the first free buffer
        dataPtr = m_buffers[i];
        m_usedBuffers[i] = true;
    }
    buffer.key = i;
    buffer.dataPtr = dataPtr;
    buffer.numOfElements = m_numElem;
    //yInfo() << "getBuffer: key=" << buffer.key << " ptr=" << buffer.dataPtr;
    m_mutex.unlock();
    return buffer;
}


// template <typename T>
// void yarp::dev::impl::FixedSizeBuffersManager<T>::releaseBuffer(T* datapointer)
// {
//     m_mutex.lock();
//     std::size_t i;
//     for(i=0; i< m_buffers.size(); i++) {
//         if(m_buffers[i] == datapointer) {
//             m_usedBuffers[i] = false;
//             break;
//         }
//     }
//     if(i>=m_buffers.size()) {
//         yError() << "FixedSizeBuffersManager::releaseBuffer(T* datapointer) error in deallocation!!";
//     }
//     m_mutex.unlock();
// }


template <typename T>
void yarp::dev::impl::FixedSizeBuffersManager<T>::releaseBuffer(yarp::dev::impl::Buffer<T>& buffer)
{
    m_mutex.lock();

    if (buffer.key >= m_buffers.size()) {
        yError() << "FixedSizeBuffersManager::releaseBuffer((Buffer<T> &buffer) error in deallocation!!";
    }

    m_usedBuffers[buffer.key] = false;
    m_firstFreeBuff = buffer.key;
    //yInfo() << "ReleaseBuffer: key=" << buffer.key << " ptr=" << buffer.dataPtr;
    m_mutex.unlock();
}


template <typename T>
void yarp::dev::impl::FixedSizeBuffersManager<T>::printBuffers()
{
    m_mutex.lock();
    for (std::size_t i = 0; i < m_buffers.size(); i++) {
        yDebug() << "buff[" << i << "]: addr = " << m_buffers[i] << "; it is used?" << m_usedBuffers[i];
    }

    m_mutex.unlock();
}


template <typename T>
yarp::dev::impl::FixedSizeBuffersManager<T>::~FixedSizeBuffersManager()
{
    for (size_t i = 0; i < m_buffers.size(); i++) {
        delete[] m_buffers[i];
    }
}


template <typename T>
std::size_t yarp::dev::impl::FixedSizeBuffersManager<T>::getBufferSize()
{
    return m_numElem;
}
