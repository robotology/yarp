/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IODOMETRY2DTEST_H
#define IODOMETRY2DTEST_H

#include <yarp/dev/ILLM.h>

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

    std::string answer;
    b = illm->ask("A question", answer);
    CHECK(b);

    std::vector<std::pair<std::string, std::string>> conversation;
    b = illm->getConversation(conversation);
    CHECK(b);
}
}

#endif
