/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ILLM_H
#define YARP_DEV_ILLM_H

#include <yarp/dev/api.h>

#include <string>
#include <vector>

typedef std::string Author;
typedef std::string Content;

namespace yarp::dev {

class YARP_dev_API ILLM
{
public:

    virtual ~ILLM()
    {
    }

    virtual bool setPrompt(const std::string& prompt) = 0;

    virtual bool readPrompt(std::string& oPrompt) = 0;

    virtual bool ask(const std::string& question, std::string& answer) = 0;

    virtual bool getConversation(std::vector<std::pair<Author, Content>>& conversation) = 0;

    virtual bool deleteConversation() = 0;
};

}

#endif
