/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKESPEECHSYNTHESIZER_H
#define FAKESPEECHSYNTHESIZER_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>

using namespace yarp::os;

/**
 * @ingroup dev_impl_other
 *
 * \brief `FakeSpeechTranscription`: A fake implementation of a speech transcriber plugin.
 */
class FakeSpeechSynthesizer :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISpeechSynthesizer
{
private:
    bool m_verbose = true;
    std::string m_language = "auto";
    std::string m_voice = "auto";
    double m_pitch = 0;
    double m_speed = 0;

public:
    FakeSpeechSynthesizer();
    virtual ~FakeSpeechSynthesizer();
    FakeSpeechSynthesizer(const FakeSpeechSynthesizer&) = delete;
    FakeSpeechSynthesizer(FakeSpeechSynthesizer&&) = delete;
    FakeSpeechSynthesizer& operator=(const FakeSpeechSynthesizer&) = delete;
    FakeSpeechSynthesizer& operator=(FakeSpeechSynthesizer&&) = delete;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    virtual bool setLanguage(const std::string language) override;
    virtual bool getLanguage(std::string& language) override;
    virtual bool setVoice(const std::string voice) override;
    virtual bool getVoice(std::string& voice) override;
    virtual bool setSpeed(const double speed) override;
    virtual bool getSpeed(double& voice) override;
    virtual bool setPitch(const double pitch) override;
    virtual bool getPitch(double& voice) override;
    virtual bool synthetize(const std::string& text, yarp::sig::Sound& sound) override;
};

#endif
