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

    b = illm->setPrompt("A prompt");
    CHECK(b);

    std::string prompt;
    b = illm->readPrompt(prompt);
    CHECK(b);

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
}
}

#endif
