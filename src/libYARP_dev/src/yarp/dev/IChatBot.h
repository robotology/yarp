/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICHATBOT_H
#define YARP_DEV_ICHATBOT_H

#include <yarp/dev/api.h>

#include <string>
#include <vector>


namespace yarp::dev {
class IChatBot;
} // namespace yarp

/**
 * @ingroup dev_iface_other
 *
 * IChatBot interface. Provides methods to interact with chatbots.
 */
class YARP_dev_API yarp::dev::IChatBot
{
public:
    /**
     * Destructor
     */
    virtual ~IChatBot();

    /**
    * Sends a message to the chatbot
    * @param messageIn the text of the input message
    * @param messageOut the output message
    * @return true/false
    */
    virtual bool interact(const std::string& messageIn, std::string& messageOut) = 0;

    /**
     * Sets the chat bot language.
     * \param language a string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool setLanguage(const std::string& language="auto") = 0;

    /**
     * Gets the current chatbot language.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool getLanguage(std::string& language) = 0;

    /**
     * Resets the chatbot
     * \return true on success
    */
    virtual bool resetBot() = 0;
};


#endif //YARP_DEV_ICHATBOT_H
