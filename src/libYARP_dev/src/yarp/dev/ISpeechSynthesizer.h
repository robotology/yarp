/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ISPEECHSYNTHESIZER_H
#define YARP_DEV_ISPEECHSYNTHESIZER_H

#include <yarp/dev/api.h>
#include <yarp/sig/Sound.h>

namespace yarp::dev {

/**
 * \ingroup dev_iface_other
 *
 * \brief A generic interface for speech synthesis.
 */
class YARP_dev_API ISpeechSynthesizer
{
public:
    virtual ~ISpeechSynthesizer();

    /**
     * Sets the language for speech synthesis.
     * \param language a string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool setLanguage(const std::string& language="auto") = 0;

    /**
     * Gets the current language set for speech synthesis.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool getLanguage(std::string& language) = 0;

    /**
     * Sets the voice set for speech synthesis.
     * \param voice_name the name of of the voice (device dependent).
     * \return true on success
     */
    virtual bool setVoice(const std::string& voice_name = "auto") = 0;

    /**
     * Gets the current voice set for speech synthesis.
     * \param voice_name the currently used voice (device dependent).
     * \return true on success
     */
    virtual bool getVoice(std::string& voice_name) = 0;

    /**
     * Sets the voice speed for speech synthesis.
     * \param speed the voice speed.
     * \return true on success
     */
    virtual bool setSpeed(const double speed=0) = 0;

    /**
     * Gets the current voice speed.
     * \param speed the current voice speed.
     * \return true on success
     */
    virtual bool getSpeed(double& speed) = 0;

    /**
     * Sets the pitch for speech synthesis.
     * \param pitch the voice pitch.
     * \return true on success
     */
    virtual bool setPitch(const double pitch) = 0;

    /**
     * Gets the current pitch set for speech synthesis.
     * \param pitch the current voice pitch.
     * \return true on success
     */
    virtual bool getPitch(double& voice) = 0;

    /**
     * Performs the speech synthesis.
     * \param text the text to synthesize
     * \param sound the synthesized audio stream
     * \return true on success
     */
    virtual bool synthesize(const std::string& text, yarp::sig::Sound& sound) = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_ISPEECHSYNTHESIZER_H
