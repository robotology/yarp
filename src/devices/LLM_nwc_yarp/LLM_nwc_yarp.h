/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/llm/ILLMMsgs.h>
#include <yarp/dev/ILLM.h>

 /**
 * @ingroup dev_impl_network_clients
 *
 * \brief `LLM_nwc_yarp`: The client side of any ILLM capable device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the nwc device.                |       |
 * | remote         |      -         | string  | -              |   -           | Yes          | Full port name of the port opened on the server side, to which the nwc connects to.    |     |
 */
class LLM_nwc_yarp : public yarp::dev::DeviceDriver,
                     public yarp::dev::ILLM
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
    bool ask(const std::string& question, std::string& oAnswer) override;
    bool getConversation(std::vector<std::pair<Author,Content>>& oConversation) override;
    bool deleteConversation() override;
};
