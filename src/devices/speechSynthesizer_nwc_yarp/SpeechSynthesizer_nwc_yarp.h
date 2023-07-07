/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SPEECHSYNTHESIZER_NWC_YARP_H
#define YARP_DEV_SPEECHSYNTHESIZER_NWC_YARP_H

#include <cstdio>
#include <cstdlib>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/dev/WrapperSingle.h>
#include "ISpeechSynthesizerMsgs.h"


/**
 *  @ingroup dev_impl_nwc_yarp
 *
 * \section speechSynthesizer_nwc_yarp
 *
 * \brief `speechSynthesizer_nwc_yarp`: A network wrapper client that connects to a speechSynthesizer_nws_yarp to perform speech synthesis.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter | Type    | Units          | Default Value | Required     | Description                                                                                           | Notes |
 * |:--------------:|:------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------------------------------------------:|:-----:|
 * | local          | -            | string  | -              | -             | Yes          | Full port name opened by the device.                                                                  |       |
 * | server         | -            | string  | -              | -             | Yes          | Full port name of the port remotely opened by the server, to which this client connects to.           |       |
 * | carrier        | -            | string  | -              | tcp           | No           | The carrier used for the connection with the server.                                                   |       |
 */

class SpeechSynthesizer_nwc_yarp :
    public yarp::dev::DeviceDriver,
    public yarp::dev::ISpeechSynthesizer
{
    // Methods
private:
    bool closeMain();

public:
    SpeechSynthesizer_nwc_yarp() = default;
    SpeechSynthesizer_nwc_yarp(const SpeechSynthesizer_nwc_yarp&) = delete;
    SpeechSynthesizer_nwc_yarp(SpeechSynthesizer_nwc_yarp&&) = delete;
    SpeechSynthesizer_nwc_yarp& operator=(const SpeechSynthesizer_nwc_yarp&) = delete;
    SpeechSynthesizer_nwc_yarp& operator=(SpeechSynthesizer_nwc_yarp&&) = delete;
    ~SpeechSynthesizer_nwc_yarp() override;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /**
     * Sets the language for speech synthesis.
     * \param language a string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    bool setLanguage(const std::string& language="auto") override;

    /**
     * Gets the current language set for speech synthesis.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    bool getLanguage(std::string& language) override;

    /**
     * Sets the voice set for speech synthesis.
     * \param voice_name the name of of the voice (device dependent).
     * \return true on success
     */
    bool setVoice(const std::string& voice_name = "auto") override;

    /**
     * Gets the current voice set for speech synthesis.
     * \param voice_name the currently used voice (device dependent).
     * \return true on success
     */
    bool getVoice(std::string& voice_name) override;

    /**
     * Sets the voice speed for speech synthesis.
     * \param speed the voice speed.
     * \return true on success
     */
    bool setSpeed(const double speed=0) override;

    /**
     * Gets the current voice speed.
     * \param speed the current voice speed.
     * \return true on success
     */
    bool getSpeed(double& speed) override;

    /**
     * Sets the pitch for speech synthesis.
     * \param pitch the voice pitch.
     * \return true on success
     */
    bool setPitch(const double pitch) override;

    /**
     * Gets the current pitch set for speech synthesis.
     * \param pitch the current voice pitch.
     * \return true on success
     */
    bool getPitch(double& pitch) override;

    /**
     * Performs the speech synthesis.
     * \param text the text to synthesize
     * \param sound the synthesized audio stream
     * \return true on success
     */
    bool synthesize(const std::string& text, yarp::sig::Sound& sound) override;

    // Parameters
private:
    ISpeechSynthesizerMsgs m_thriftClient;

protected:
    yarp::os::Port                m_thriftClientPort;
    std::string                   m_local_name;
    std::string                   m_thrift_server_name;
    std::string                   m_carrier_name{"tcp"};
};


#endif // YARP_DEV_SPEECHSYNTHESIZER_NWC_YARP_H
