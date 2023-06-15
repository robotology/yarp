/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ISPEECHTRANSCRIPTION_H
#define YARP_DEV_ISPEECHTRANSCRIPTION_H

#include <yarp/dev/api.h>
#include <yarp/sig/Sound.h>

namespace yarp::dev {

/**
 * \ingroup dev_iface_other
 *
 * \brief A generic interface for speech transcription.
 */
class YARP_dev_API ISpeechTranscription
{
public:
    virtual ~ISpeechTranscription();

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
     * Performs the speech transcription.
     * \param sound the audio data to transcribe
     * \param transcription the returned transcription (it may be empty)
     * \param score the returned score/confidence value in the range (0-1.0). It may be not implemented.
     * \return true on success
     */
    virtual bool transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score) = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_ISPEECHTRANSCRIPTION_H
