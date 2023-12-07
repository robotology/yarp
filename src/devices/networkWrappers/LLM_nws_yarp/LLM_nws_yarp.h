/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/RpcServer.h>
#include <yarp/dev/ILLM.h>

#include "ILLMServerImpl.h"

/**
* @ingroup dev_impl_nws_yarp
*
* \section LLM_nws_yarp
*
* \brief `LLM_nws_yarp`: A server which can be wrap multiple algorithms and devices to provide LLM services.
*
*  Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units  | Default Value            | Required     | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:------:|:------------------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
* | name           |      -         | string  | -      | -                        | Yes          | Full name of the rpc port opened by device, e.g. /LLM_nws/rpc     |       |
*/
class LLM_nws_yarp : public yarp::dev::DeviceDriver,
                     public yarp::dev::WrapperSingle,
                     public yarp::os::PortReader
{

private:
    std::string m_streaming_port_name = "/llm/conv:o";

protected:
    ILLMRPCd    m_RPC;
    yarp::os::RpcServer m_RpcPort;
    yarp::dev::ILLM *m_iLlm = nullptr;

public:
    // From DeviceDriver
    virtual bool open(yarp::os::Searchable &prop) override;
    virtual bool close() override;

    // From WrapperSingle
    virtual bool attach(yarp::dev::PolyDriver *drv) override;
    virtual bool detach() override;

    // From PortReader
    bool read(yarp::os::ConnectionReader& connection) override;
};
