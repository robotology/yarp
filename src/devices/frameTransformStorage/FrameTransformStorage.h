/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_FRAMETRANSFORMSTORAGE_H
#define YARP_DEV_FRAMETRANSFORMSTORAGE_H


#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/dev/FrameTransformContainer.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ReturnValue.h>
#include <mutex>
#include <map>
#include <mutex>

#include "FrameTransformStorage_ParamsParser.h"

/**
 *  @ingroup dev_impl_other
 *
 * \brief `frameTransformStorage`: A class that keeps a \ref FrameTransformContainer updated by receiving transformations
 *  through the \ref yarp::dev::IFrameTransformStorageSet interface and periodically querying a set of attached
 *  devices that expose the \ref yarp::dev::IFrameTransformStorageGet interface.
 *  It also exposes the \ref yarp::dev::IFrameTransformStorageGet interface to allow stored transformations retrieval
 *
 * Parameters required by this device are shown in class: FrameTransformStorage_ParamsParser
 *
 */

class FrameTransformStorage :
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::IFrameTransformStorageGet,
    public yarp::dev::IFrameTransformStorageUtils,
    public yarp::os::PeriodicThread,
    public yarp::dev::WrapperSingle,
    public FrameTransformStorage_ParamsParser
{
protected:
    yarp::dev::FrameTransformContainer m_tf_container;

public:
    FrameTransformStorage(double tperiod=0.010) : PeriodicThread (tperiod) {}
    ~FrameTransformStorage() { close(); }

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IFrameTransformStorageSet interface
    yarp::dev::ReturnValue setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    yarp::dev::ReturnValue setTransform(const yarp::math::FrameTransform& transform) override;

    //IFrameTransformStorageGet interface
    yarp::dev::ReturnValue getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

    //IFrameTransformStorageUtils interface
    yarp::dev::ReturnValue deleteTransform(std::string t1, std::string t2) override;
    yarp::dev::ReturnValue size(size_t& size) const override;
    yarp::dev::ReturnValue clearAll() override;
    yarp::dev::ReturnValue getInternalContainer(yarp::dev::FrameTransformContainer*&  container) override;
    yarp::dev::ReturnValue startStorageThread() override;
    yarp::dev::ReturnValue stopStorageThread() override;

    //wrapper and interfaces
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;
private:
    mutable std::mutex  m_pd_mutex;
    yarp::dev::PolyDriver* pDriver = nullptr;
    IFrameTransformStorageGet* iGetIf = nullptr;

    //periodicThread
    void     run() override;
};

#endif // YARP_DEV_FRAMETRANSFORMSTORAGE_H
