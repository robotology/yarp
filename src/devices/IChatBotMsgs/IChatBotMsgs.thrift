/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct return_interact{
    1: bool result;
    2: string messageOut;
}

struct return_getLanguage{
    1: bool result;
    2: string language;
}

service IChatBotMsgs {
    return_interact interactRPC(1: string messageIn);
    bool setLanguageRPC(1: string language);
    return_getLanguage getLanguageRPC();
    bool resetBotRPC();
}
