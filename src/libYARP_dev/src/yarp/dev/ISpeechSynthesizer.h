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
 * \brief A generic interface for speech transcription.
 */
class YARP_dev_API ISpeechSynthesizer
{
public:
    virtual ~ISpeechSynthesizer();

    /**
     * Sets the language for speech transcription.
     * \param language a string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool setLanguage(const std::string language="auto") = 0;

    /**
     * Gets the current language set for speech transcription.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool getLanguage(std::string& language) = 0;

    /**
     * Sets the language for speech transcription.
     * \param language a string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool setVoice(const std::string voice = "auto") = 0;

    /**
     * Gets the current language set for speech transcription.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool getVoice(std::string& voice) = 0;

    /**
     * Sets the language for speech transcription.
     * \param speed.
     * \return true on success
     */
    virtual bool setSpeed(const double speed=0) = 0;

    /**
     * Gets the current language set for speech transcription.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool getSpeed(double& voice) = 0;

    /**
     * Sets the language for speech transcription.
     * \param language a string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool setPitch(const double pitch) = 0;

    /**
     * Gets the current language set for speech transcription.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    virtual bool getPitch(double& voice) = 0;

    /**
     * Performs the speech synthesis.
     * \param text the text to synthesize
     * \param sound the returned audio stream
     * \return true on success
     */
    virtual bool synthetize(const std::string& text, yarp::sig::Sound& sound) = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_ISPEECHSYNTHESIZER_H
