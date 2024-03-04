/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/llm/ILLMMsgs.h>
#include <yarp/dev/ILLM.h>
#include <yarp/dev/LLM_Message.h>
#include "LLM_nwc_yarp_ParamsParser.h"

 /**
 * @ingroup dev_impl_nwc_yarp
 *
 * \brief `LLM_nwc_yarp`: The client side of any ILLM capable device.
 *
 * Parameters required by this device are shown in class: LLM_nwc_yarp_ParamsParser
 */
class LLM_nwc_yarp : public yarp::dev::DeviceDriver,
                     public yarp::dev::ILLM,
                     public LLM_nwc_yarp_ParamsParser
{
protected:
    // thrift interface
    yarp::dev::llm::ILLMMsgs m_LLM_RPC;

    // rpc port
    yarp::os::Port m_rpc_port_to_LLM_server;

public:

    //From DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //From ILLM
    bool setPrompt(const std::string& prompt) override;
    bool readPrompt(std::string& oPrompt) override;
    bool ask(const std::string& question, yarp::dev::LLM_Message& oAnswer) override;
    bool getConversation(std::vector<yarp::dev::LLM_Message>& oConversation) override;
    bool deleteConversation() override;
};
