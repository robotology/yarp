/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_LLM_MESSAGE_H
#define YARP_DEV_LLM_MESSAGE_H

#include <yarp/dev/LLM_MessageData.h>

namespace yarp::dev {

class YARP_dev_API LLM_Message : public yarp::dev::LLM_MessageData
{
public:
    LLM_Message();
    LLM_Message(const std::string& type,
                const std::string& content,
                const std::vector<std::string>& parameters,
                const std::vector<std::string>& arguments);

    virtual ~LLM_Message();
};

}

#endif // YARP_DEV_LLM_MESSAGE_H
