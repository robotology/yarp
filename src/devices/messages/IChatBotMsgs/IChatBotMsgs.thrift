/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_interact{
    1: yReturnValue result;
    2: string messageOut;
}

struct return_getLanguage{
    1: yReturnValue result;
    2: string language;
}

struct return_getStatus{
    1: yReturnValue result;
    2: string status;
}

service IChatBotMsgs {
    return_interact interactRPC(1: string messageIn);
    yReturnValue setLanguageRPC(1: string language);
    return_getLanguage getLanguageRPC();
    return_getStatus getStatusRPC();
    yReturnValue resetBotRPC();
}
