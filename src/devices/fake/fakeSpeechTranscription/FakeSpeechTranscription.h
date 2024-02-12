/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKESPEECHTRANSCRIPTION_H
#define FAKESPEECHTRANSCRIPTION_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechTranscription.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>

#include "FakeSpeechTranscription_ParamsParser.h"

using namespace yarp::os;

/**
 * @ingroup dev_impl_fake dev_impl_other
 *
 * \brief `FakeSpeechTranscription`: A fake implementation of a speech transcriber plugin.
 *
 * Parameters required by this device are shown in class: FakeSpeechTranscription_ParamsParser
 */
class FakeSpeechTranscription :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISpeechTranscription,
        public FakeSpeechTranscription_ParamsParser
{
private:
    bool m_verbose = true;
    std::string m_language="auto";

public:
    FakeSpeechTranscription();
    virtual ~FakeSpeechTranscription();
    FakeSpeechTranscription(const FakeSpeechTranscription&) = delete;
    FakeSpeechTranscription(FakeSpeechTranscription&&) = delete;
    FakeSpeechTranscription& operator=(const FakeSpeechTranscription&) = delete;
    FakeSpeechTranscription& operator=(FakeSpeechTranscription&&) = delete;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    virtual bool setLanguage(const std::string& language) override;
    virtual bool getLanguage(std::string& language) override;
    virtual bool transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score) override;
};

#endif
