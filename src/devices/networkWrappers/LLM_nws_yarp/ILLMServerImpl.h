/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/ILLM.h>
#include <yarp/dev/ILLMMsgs.h>

class ILLMRPCd : public yarp::dev::ILLMMsgs
{
private:
    yarp::dev::ILLM* m_iLlm = nullptr;

    // The conversation is streamed on a yarp port
    yarp::os::BufferedPort<yarp::os::Bottle> m_streaming_port;
    void m_stream_conversation();

public:

    void setInterface(yarp::dev::ILLM* _iLlm, const std::string& streaming_port_name)
    {
        m_iLlm = _iLlm;
        m_streaming_port.open(streaming_port_name);
    }
    void unsetInterface()
    {
        m_iLlm = nullptr;
        m_streaming_port.close();
    }
    // From IGPTMsgs
    yarp::dev::ReturnValue setPrompt(const std::string& prompt) override;
    yarp::dev::return_readPrompt readPrompt() override;
    yarp::dev::return_ask ask(const std::string& question) override;
    yarp::dev::return_getConversation getConversation() override;
    yarp::dev::ReturnValue deleteConversation() override;
    yarp::dev::ReturnValue refreshConversation() override;
};
