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
#include "LLM_nws_yarp_ParamsParser.h"

/**
* @ingroup dev_impl_nws_yarp
*
* \section LLM_nws_yarp
*
* \brief `LLM_nws_yarp`: A server which can be wrap multiple algorithms and devices to provide LLM services.
*
* Parameters required by this device are shown in class: LLM_nws_yarp_ParamsParser
*
*/
class LLM_nws_yarp : public yarp::dev::DeviceDriver,
                     public yarp::dev::WrapperSingle,
                     public yarp::os::PortReader,
                     public LLM_nws_yarp_ParamsParser
{
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
