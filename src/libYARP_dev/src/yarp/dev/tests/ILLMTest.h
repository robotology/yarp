/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ILLMTEST_H
#define ILLMTEST_H

#include <yarp/dev/ILLM.h>
#include <yarp/dev/LLM_Message.h>

#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;

namespace yarp::dev::tests {
inline void exec_iLLM_test_1(yarp::dev::ILLM* illm)
{
    REQUIRE(illm != nullptr);

    bool b;
    std::string input_prompt1 = "A prompt";
    std::string input_prompt2 = "A new prompt";

    //set a prompt
    b = illm->setPrompt(input_prompt1);
    CHECK(b);

    std::string prompt;
    b = illm->readPrompt(prompt);
    CHECK(b);
    CHECK(prompt == input_prompt1);

    //a new prompt can be assigned only after deleting conversation
    b = illm->setPrompt("A new prompt, this will fail, until a reset is done");
    CHECK(!b);

    b = illm->deleteConversation();
    CHECK(b);

    //set a new prompt
    b = illm->setPrompt(input_prompt2);
    CHECK(b);

    b = illm->readPrompt(prompt);
    CHECK(b);
    CHECK(prompt == input_prompt2);

    //check answers
    yarp::dev::LLM_Message answer;
    b = illm->ask("A question", answer);
    CHECK(b);
    CHECK(answer.type == "assistant");
    CHECK(answer.content != "");
    CHECK(answer.parameters.empty()); // No parameters in a non-function call
    CHECK(answer.arguments.empty());  // No arguments in a non-function call

    // The fake device returns a function_call if the question is "function"
    b = illm->ask("function", answer);
    CHECK(b);
    CHECK(answer.type == "function");
    CHECK(answer.content != "");
    CHECK(!answer.parameters.empty());
    CHECK(!answer.arguments.empty());


    std::vector<yarp::dev::LLM_Message> conversation;
    b = illm->getConversation(conversation);
    CHECK(b);

    b = illm->refreshConversation();
    CHECK(b);

    b = illm->readPrompt(prompt);
    CHECK(b);

    b = illm->deleteConversation();
    CHECK(b);
}
}

#endif
