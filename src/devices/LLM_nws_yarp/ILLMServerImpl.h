/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ILLMMsgs.h"
#include <yarp/dev/ILLM.h>

class ILLMRPCd : public yarp::dev::ILLMMsgs
{
private:
    yarp::dev::ILLM *m_iLlm = nullptr;

public:
    void setInterface(yarp::dev::ILLM *_iLlm) { m_iLlm = _iLlm; }
    // From IGPTMsgs
    bool setPrompt(const std::string &prompt) override;
    yarp::dev::return_readPrompt readPrompt() override;
    yarp::dev::return_ask ask(const std::string &question) override;
    yarp::dev::return_getConversation getConversation() override;
    bool deleteConversation() override;
};