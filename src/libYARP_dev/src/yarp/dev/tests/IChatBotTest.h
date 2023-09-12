/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ICHATBOTTEST_H
#define ICHATBOTTEST_H

#include <yarp/dev/IChatBot.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;

namespace yarp::dev::tests {
inline void exec_iChatBot_test_1(yarp::dev::IChatBot* ichatbot)
{
    REQUIRE(ichatbot != nullptr);

    bool b;

    b = ichatbot->setLanguage("eng");
    CHECK(b);

    std::string tempLang;
    b = ichatbot->getLanguage(tempLang);
    CHECK(b);
    CHECK(tempLang == std::string("eng"));

    b = ichatbot->resetBot();
    CHECK(b);

    std::string messageIn{"Hello!"};
    std::string messageOut;
    b = ichatbot->interact(messageIn,messageOut);
    CHECK(b);
}
}

#endif //ICHATBOTTEST_H
