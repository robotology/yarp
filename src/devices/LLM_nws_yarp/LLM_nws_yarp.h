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

class LLM_nws_yarp : public yarp::dev::DeviceDriver,
                     public yarp::dev::WrapperSingle,
                     public yarp::os::PortReader
{

protected:
    ILLMRPCd    m_RPC;
    yarp::os::RpcServer m_RpcPort;
    yarp::dev::ILLM *m_iLlm;

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