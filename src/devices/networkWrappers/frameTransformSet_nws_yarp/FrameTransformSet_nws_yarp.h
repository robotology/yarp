/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_FRAMETRANSFORMSETNWSYARP_H
#define YARP_DEV_FRAMETRANSFORMSETNWSYARP_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include "FrameTransformStorageSetRPC.h"
#include <mutex>
#include <map>

#include "FrameTransformSet_nws_yarp_ParamsParser.h"

/**
 * @ingroup dev_impl_nws_yarp
 *
 * @brief `frameTransformSet_nws_yarp`: A network wrapper client which publishes the transforms received on the yarp::dev::IFrameTransformStorageSet interface to thrift interface FrameTransformStorageSetRPC interface.
 *
 * \section FrameTransformSet_nws_yarp_device_parameters Parameters
 * This device is paired with its server called FrameTransformSet_nwc_yarp.
 *
 * Parameters required by this device are shown in class: FrameTransformSet_nws_yarpParamsParser
 *
 * \section FrameTransformGet_nwc_yarp_port_example Port names examples
 * Here follow some examples of port names obtained with different parameters configurations
 * -# With
 *      - default-config = true
 *      - nws_thrift_port_prefix = ""\n
 *        The frameTransformSet_nws_yarp thrift port name will be: `/frameTransformServer/frameTransformSet_nws_yarp/thrift`
 * -# With
 *      - default-config = true
 *      - nws_thrift_port_prefix = "/cer"\n
 *        The frameTransformSet_nws_yarp thrift port name will be: `/cer/frameTransformServer/frameTransformSet_nws_yarp/thrift`
 * -# With
 *      - default-config = false
 *      - nws_thrift_port_prefix = "/cer"\n
 *        The frameTransformSet_nws_yarp thrift port name will be: `/cer/frameTransformSet_nws_yarp/thrift`
 *
 * \section FrameTransformSet_nws_yarp_example Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameTransformSet_nws_yarp
 * default-config true
 * nws_thrift_port_prefix /cer
 * \endcode
 */

class FrameTransformSet_nws_yarp :
    public yarp::dev::DeviceDriver,
    public FrameTransformStorageSetRPC,
    public yarp::dev::WrapperSingle,
    public FrameTransformSet_nws_yarp_ParamsParser
{

public:
    FrameTransformSet_nws_yarp()=default;
    ~FrameTransformSet_nws_yarp();

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //wrapper and interfaces
    bool attach(yarp::dev::PolyDriver* device2attach) override;
    bool detach() override;

    //FrameTransformStorageSetRPC functions
    yarp::dev::ReturnValue setTransformsRPC(const std::vector<yarp::math::FrameTransform>& transforms) override;
    yarp::dev::ReturnValue setTransformRPC(const yarp::math::FrameTransform& transform) override;
    yarp::dev::ReturnValue deleteTransformRPC(const std::string& src, const std::string& dst) override;
    yarp::dev::ReturnValue clearAllRPC()override;

private:
    mutable std::mutex                      m_pd_mutex;
    mutable std::mutex                      m_trf_mutex;
    std::string                             m_defaultConfigPrefix{"/frameTransformServer"};
    std::string                             m_deviceName{"frameTransformSet_nws_yarp"};
    yarp::dev::PolyDriver*                  m_pDriver{nullptr};
    std::string                             m_thriftPortName;
    yarp::os::Port                          m_thriftPort;
    yarp::dev::IFrameTransformStorageSet*   m_iSetIf   {nullptr};
    yarp::dev::IFrameTransformStorageUtils* m_iUtilsIf {nullptr};
};

#endif // YARP_DEV_FRAMETRANSFORMSETNWSYARP_H
