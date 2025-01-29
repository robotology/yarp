/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_FRAMETRANSFORM_UTILS_H
#define YARP_DEV_FRAMETRANSFORM_UTILS_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/api.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/ReturnValue.h>
#include <mutex>
#include <map>

namespace yarp::dev {

/**
 *  @ingroup dev_impl_other
 *
 * \brief `FrameTransformContainer`: A class that contains a vector of frame transformations and exposes
 *  \ref yarp::dev::IFrameTransformStorageSet and \ref yarp::dev::IFrameTransformStorageGet
 *  interfaces in order to allow external access to it.
 *
 */

class YARP_dev_API FrameTransformContainer :
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::IFrameTransformStorageGet
{
    using ContainerType = std::vector<yarp::math::FrameTransform>;

public:
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ContainerType::difference_type;
        using value_type = ContainerType::value_type;
        using pointer = ContainerType::iterator;
        using reference = value_type&;

        //constructor
        Iterator(ContainerType& data, ContainerType::iterator ptr) :
            m_data(data),
            m_ptr(ptr)
        {
            while (m_ptr != m_data.end() && !m_ptr->isValid()) {
                ++m_ptr;
            }
        }

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        // Prefix increment
        Iterator& operator++()
        {
            do {
                ++m_ptr;
            } while (m_ptr != m_data.end() && !m_ptr->isValid());
            return *this;
        }

        // Postfix increment
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        ContainerType::iterator m_ptr;
        ContainerType& m_data;
    };

public:
    Iterator begin()
    {
        return Iterator(m_transforms, m_transforms.begin());
    }

    Iterator end()
    {
        return Iterator(m_transforms, m_transforms.end());
    }

protected:
    ContainerType m_transforms;
    void invalidateTransform(yarp::math::FrameTransform& trf);

public:
    mutable std::recursive_mutex  m_trf_mutex;

public:
    //non-static transforms older than value (seconds) will be removed by method checkAndRemoveExpired()
    double m_timeout = 0.2;
    bool   m_verbose_debug = false;
    std::string m_name;

public:
    FrameTransformContainer() {}
    ~FrameTransformContainer() {}

    //IFrameTransformStorageSet interface
    yarp::dev::ReturnValue setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    yarp::dev::ReturnValue setTransform(const yarp::math::FrameTransform& transform) override;

    //IFrameTransformStorageGet interface
    yarp::dev::ReturnValue getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

    //IFrameTransformStorageUtils interface
    yarp::dev::ReturnValue deleteTransform(std::string t1, std::string t2) override;
    yarp::dev::ReturnValue clearAll() override;

    bool size(size_t& size) const;

public:
    //other
    bool checkAndRemoveExpired();
    bool checkAndRemoveExpired() const;
};

} // namespace yarp::dev

#endif // YARP_DEV_FRAMETRANSFORM_UTILS_H
