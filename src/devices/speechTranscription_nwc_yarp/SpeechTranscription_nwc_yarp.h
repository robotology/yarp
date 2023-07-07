/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SPEECHTRANSCRIPTION_NWC_YARP_H
#define YARP_DEV_SPEECHTRANSCRIPTION_NWC_YARP_H

#include <cstdio>
#include <cstdlib>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISpeechTranscription.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/dev/WrapperSingle.h>
#include "ISpeechTranscriptionMsgs.h"


/**
 *  @ingroup dev_impl_nwc_yarp
 *
 * \section speechTranscription_nwc_yarp
 *
 * \brief `speechTranscription_nwc_yarp`: A network wrapper client that connects to a speechTranscription_nws_yarp to perform speech transcription.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter | Type    | Units          | Default Value | Required     | Description                                                                                           | Notes |
 * |:--------------:|:------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------------------------------------------:|:-----:|
 * | local          | -            | string  | -              | -             | Yes          | Full port name opened by the device.                                                                  |       |
 * | server         | -            | string  | -              | -             | Yes          | Full port name of the port remotely opened by the server, to which this client connects to.           |       |
 * | carrier        | -            | string  | -              | tcp           | No           | The carrier used for the connection with the server.                                                   |       |
 */

class SpeechTranscription_nwc_yarp :
    public yarp::dev::DeviceDriver,
    public yarp::dev::ISpeechTranscription
{
    // Methods
private:
    bool closeMain();

public:
    SpeechTranscription_nwc_yarp() = default;
    SpeechTranscription_nwc_yarp(const SpeechTranscription_nwc_yarp&) = delete;
    SpeechTranscription_nwc_yarp(SpeechTranscription_nwc_yarp&&) = delete;
    SpeechTranscription_nwc_yarp& operator=(const SpeechTranscription_nwc_yarp&) = delete;
    SpeechTranscription_nwc_yarp& operator=(SpeechTranscription_nwc_yarp&&) = delete;
    virtual ~SpeechTranscription_nwc_yarp() override;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //yarp::dev::ISpeechTranscription
    /**
     * Sets the language for speech transcription.
     * \param language a string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    bool setLanguage(const std::string& language="auto") override;

    /**
     * Gets the current language set for speech transcription.
     * \param language the returned string (code) representing the speech language (e.g. ita, eng...). Default value is "auto".
     * \return true on success
     */
    bool getLanguage(std::string& language) override;

    /**
     * Performs the speech transcription.
     * \param sound the audio data to transcribe
     * \param transcription the returned transcription (it may be empty)
     * \param score the returned score/confidence value in the range (0-1.0). It may be not implemented.
     * \return true on success
     */
    bool transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score) override;

    // Parameters
private:
    ISpeechTranscriptionMsgs m_thriftClient;

protected:
    yarp::os::Port                m_thriftClientPort;
    std::string                   m_local_name;
    std::string                   m_thrift_server_name;
    std::string                   m_carrier_name{"tcp"};
};


#endif // YARP_DEV_SPEECHTRANSCRIPTION_NWC_YARP_H
