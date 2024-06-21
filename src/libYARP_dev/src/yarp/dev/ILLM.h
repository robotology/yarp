/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ILLM_H
#define YARP_DEV_ILLM_H

#include <yarp/dev/api.h>
#include <yarp/dev/LLM_Message.h>

#include <string>
#include <vector>

namespace yarp::dev {
class ILLM;
} // namespace yarp

/**
 * @ingroup dev_iface_other
 *
 * ILLM interface. Provides methods to interact with a LLM.
 */
class YARP_dev_API yarp::dev::ILLM
{
public:
    /**
     * Destructor.
     */
    virtual ~ILLM();

    /**
    * Performs a question
    * @param prompt provides a prompt to the LLM
    * @return true/false
    */
    virtual bool setPrompt(const std::string& prompt) = 0;

    /**
    * Retrieves the provided prompt
    * @param prompt the stored prompt
    * @return true/false
    */
    virtual bool readPrompt(std::string& oPrompt) = 0;

    /**
    * Performs a question
    * @param question the text of the question provided by the user
    * @param answer the returned answer
    * @return true/false
    */
    virtual bool ask(const std::string& question, yarp::dev::LLM_Message& answer) = 0;

    /**
    * Retrieves the whole conversation
    * @param conversation the conversation
    * @return true/false
    */
    virtual bool getConversation(std::vector<yarp::dev::LLM_Message>& conversation) = 0;

    /**
    * Delete the conversation and clear the system context from any internally stored context.
    * @return true/false
    */
    virtual bool deleteConversation() = 0;

    /** 
    *  Refresh the conversation
    * @return true/false
    */
    virtual bool refreshConversation() = 0;
};

#endif
