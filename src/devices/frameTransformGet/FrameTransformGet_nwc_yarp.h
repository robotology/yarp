/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_FRAMETRANSFORMGET_NWC_YARP_H
#define YARP_DEV_FRAMETRANSFORMGET_NWC_YARP_H

#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameTransformStorage.h>


#include <FrameTransformStorageGetRPC.h>

/**
 * @ingroup dev_impl_nwc_yarp
 *
 * @brief `frameTransformGet_nwc_yarp`: A network wrapper client which converts the input retrieved from a FrameTransformStorageGetRPC thrift interface to an IFrameTransformStorageGet interface.
 *
 * \section FrameTransformGet_nwc_yarp_device_parameters Parameters
 * This device is paired with its server called FrameTransformGet_nws_yarp.
 * It is attached to a device with an IFrameTransformStorageGet interface.
 * When it a method of the interface is called, the request is forwarded on a RPC port (which uses the thrift FrameTransformStorageGetRPC interface) and it returns all the transforms received
 * from the port to the caller.
 * For further information see \subpage FrameTransform.

 *   Parameters required by this device are:
 * | Parameter name               | SubParameter | Type    | Units  | Default Value  | Required  | Description                                                                                                                                                                        |
 * |:----------------------------:|:-------------|:-------:|:------:|:--------------:|:------- -:|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
 * | default-client               | -            | bool    | -      | true           | No        | tells whether or not the nwc is instantiated by the frameTransformClient device. If true, "/frameTransformClient" will appended to the port name prefix                            |
 * | default-server               | -            | bool    | -      | true           | No        | tells whether or not the nws is instantiated by the frameTransformServer device. If true, "/frameTransformServer/frameTransformGet_nws_yarp" will appended to the port name prefix |
 * | nwc_thrift_port_prefix       | -            | string  | -      | ""             | No        | a prefix for the nwc thrift rpc port name                                                                                                                                          |
 * | nws_thrift_port_prefix       | -            | string  | -      | ""             | No        | a prefix for the nws thrift rpc port name                                                                                                                                          |
 * | input_streaming_port_prefix  | -            | string  | -      | -              | No        | a prefix for the input streaming port name (if the port is enabled)                                                                                                                |
 * | output_streaming_port_prefix | -            | string  | -      | -              | No        | a prefix for the output streaming port name (if the port is enabled)                                                                                                               |
 * | streaming_enabled            | -            | bool    | -      | false          | No        | if enabled, tfs are received from the streaming port instead of using RPCs                                                                                                         |
 *
 * \section FrameTransformGet_nwc_yarp_port_example Port names examples
 * Here follow some examples of port names obtained with different parameters configurations
 * -# With
 *      - default-client = true
 *      - nwc_thrift_port_prefix = ""\n
 *        The frameTransformGet_nwc_yarp thrift port name will be: `/frameTransformClient/frameTransformGet_nwc_yarp/thrift`
 * -# With
 *      - default-client = true
 *      - nwc_thrift_port_prefix = "/cer"\n
 *        The frameTransformGet_nwc_yarp thrift port name will be: `/cer/frameTransformClient/frameTransformGet_nwc_yarp/thrift`
 * -# With
 *      - default-client = false
 *      - nwc_thrift_port_prefix = "/cer"\n
 *        The frameTransformGet_nwc_yarp thrift port name will be: `/cer/frameTransformGet_nwc_yarp/thrift`
 * -# With
 *      - default-server = true
 *      - nws_thrift_port_prefix = ""\n
 *        The name of the thrift port the frameTransformGet_nwc_yarp thrift port will connect to will be: `/frameTransformServer/frameTransformGet_nws_yarp/thrift`
 * -# With
 *      - default-server = true
 *      - nws_thrift_port_prefix = "/cer"\n
 *        The name of the thrift port the frameTransformGet_nwc_yarp thrift port will connect to will be: `/cer/frameTransformServer/frameTransformGet_nws_yarp/thrift`
 * -# With
 *      - default-server = false
 *      - nws_thrift_port_prefix = "/cer"\n
 *        The name of the thrift port the frameTransformGet_nwc_yarp thrift port will connect to will be: `/cer/thrift`
 * -# With
 *      - default-client = true
 *      - input_streaming_port_prefix = ""\n
 *        The name of the frameTransformGet_nwc_yarp input port for the tf stream will be: `/frameTransformClient/frameTransformGet_nwc_yarp/tf:i`
 * -# With
 *      - default-client = true
 *      - input_streaming_port_prefix = "/cer"\n
 *        The name of the frameTransformGet_nwc_yarp input port for the tf stream will be: `/cer/frameTransformClient/frameTransformGet_nwc_yarp/tf:i`
 * -# With
 *      - default-client = false
 *      - input_streaming_port_prefix = "/cer"\n
 *        The name of the frameTransformGet_nwc_yarp input port for the tf stream will be: `/cer/frameTransformGet_nwc_yarp/tf:i`
 * -# With
 *      - default-server = true
 *      - output_streaming_port_prefix = ""\n
 *        The name of the output port for the tf stream the nwc will connect to will be: `/frameTransformServer/frameTransformGet_nws_yarp/tf:o`
 * -# With
 *      - default-server = true
 *      - output_streaming_port_prefix = "/cer"\n
 *        The name of the output port for the tf stream the nwc will connect to will be: `/cer/frameTransformServer/frameTransformGet_nws_yarp/tf:o`
 * -# With
 *      - default-server = false
 *      - output_streaming_port_prefix = "/cer"\n
 *        The name of the output port for the tf stream the nwc will connect to will be: `/cer/tf:o`
 *
 * \section FrameTransformGet_nwc_yarp_device_configuration Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameTransformGet_nwc_yarp
 * nwc_thrift_port_prefix /local
 * nws_thrift_port_prefix /cer
 * \endcode
 */
class FrameTransformGet_nwc_yarp:
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageGet
{
    class DataReader : public yarp::os::BufferedPort<return_getAllTransforms>
    {
        std::mutex m_mutex;
        return_getAllTransforms m_Transforms;

        public:
        DataReader() = default;

        using               yarp::os::BufferedPort<return_getAllTransforms>::onRead;
        void        onRead(return_getAllTransforms& v) override;
        bool                getData(return_getAllTransforms& data);
    };

public:
    FrameTransformGet_nwc_yarp() = default;
    FrameTransformGet_nwc_yarp(const FrameTransformGet_nwc_yarp&) = delete;
    FrameTransformGet_nwc_yarp(FrameTransformGet_nwc_yarp&&) = delete;
    FrameTransformGet_nwc_yarp& operator=(const FrameTransformGet_nwc_yarp&) = delete;
    FrameTransformGet_nwc_yarp& operator=(FrameTransformGet_nwc_yarp&&) = delete;
    ~FrameTransformGet_nwc_yarp() override = default;

    // yarp::dev::DeviceDriver
    bool  open(yarp::os::Searchable &params) override;
    bool  close() override;

    // yarp::dev::IFrameTransformStorageGet
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

private:
    int            m_verbose{4};
    std::string    m_deviceName{"frameTransformGet_nwc_yarp"};
    std::string    m_defaultConfigPrefix{"/frameTransformClient"};
    std::string    m_defaultServerPrefix{"/frameTransformServer/frameTransformGet_nws_yarp"};

    //streaming port
    bool           m_streaming_port_enabled = false;
    std::string    m_streaming_input_port_name;
    std::string    m_streaming_output_port_name;
    DataReader*    m_dataReader =nullptr;

    // for the RPC with the NWS
    yarp::os::Port                      m_thrift_rpcPort;
    std::string                         m_thrift_rpcPort_Name;
    std::string                         m_thrift_server_rpcPort_Name;
    mutable FrameTransformStorageGetRPC m_frameTransformStorageGetRPC;

};


#endif // YARP_DEV_FRAMETRANSFORMGET_NWC_YARP_H
