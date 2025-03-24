/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev.llm

struct LLM_Message {
} (
  yarp.name = "yarp::dev::LLM_Message"
  yarp.includefile="yarp/dev/ILLM.h"
)

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_readPrompt{
    1: yReturnValue ret;
    2: string prompt;
}

struct return_ask{
    1: yReturnValue ret;
    2: LLM_Message answer;
}

struct return_getConversation{
    1: yReturnValue ret;
    2: list<LLM_Message> conversation;
}

service ILLMMsgs {
    yReturnValue setPrompt(1: string prompt);
    return_readPrompt readPrompt();
    return_ask ask(1: string question);
    return_getConversation getConversation();
    yReturnValue deleteConversation();
    yReturnValue refreshConversation();
}
