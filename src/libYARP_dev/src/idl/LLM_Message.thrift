/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct LLM_Message
{
    /** type of message (can be prompt, assistant, function, ...) */
    1: string type;
    /** content of the message */
    2: string content;
    /** possible parameters */
    3: list<string> parameters = [];
    /** possible arguments of the parameters */
    4: list<string> arguments = [];
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
