/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct Message {
    string sender;
    string content;
}

struct return_readPrompt{
    1: bool ret = false;
    2: string prompt;
}

struct return_ask{
    1: bool ret = false;
    2: string answer;
}

struct return_getConversation{
    1: bool ret = false;
    2: list<Message> conversation;
}

service ILLMMsgs {
    bool setPrompt(1: string prompt);
    return_readPrompt readPrompt();
    return_ask ask(1: string question);
    return_getConversation getConversation();
    bool deleteConversation();
}
