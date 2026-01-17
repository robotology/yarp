/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <yarp/sig/SoundUtils.h>
#include <cmath>
#include "yarpAudioPlayer_IDL.h"

using namespace yarp::os;
using namespace yarp::sig;


// ******************** THE MODULE ********************
class PlayerModule: public yarp::os::RFModule, public yarpAudioPlayer_IDL
{
    protected:
    std::string         m_name = "/yarpAudioPlayer";
    yarp::sig::Sound    m_audioFile;
    yarp::os::Port      m_audioPort;
    yarp::os::Port      m_rpcPort;
    size_t              m_split_size = 16000;

    public: //yarpAudioPlayer_IDL methods
    bool play(const std::string& filename) override;
    bool test(double duration) override;

    protected: //internal methods
    bool openFile(const std::string& file_name);
    bool send();

    protected: // RFModule methods
    bool configure(yarp::os::ResourceFinder &rf) override
    {
        bool ret;
        std::string test_string = rf.toString();

        // generic configuration
        if (rf.check("name"))
        { m_name = std::string("/") + rf.find("name").asString().c_str(); }

        if (rf.check("split_size"))
        { m_split_size = rf.find("split_size").asInt32(); }

        // rpc port
        ret = m_rpcPort.open((m_name + "/rpc").c_str());
        if (!ret) { return false;}

        this->yarp().attachAsServer(m_rpcPort);

        // output audio port
        ret = m_audioPort.open(m_name + "/audio:o");
        if (!ret) { return false;}

        yInfo() << "Module started. Send commands to the RPC port" << m_name + "/rpc" << " to play audio files.";

        return true;
    }

    bool close() override
    {
        m_audioPort.close();
        m_rpcPort.close();
        return true;
    }

    virtual double getPeriod() override
    {
        return 1.0;
    }

    bool updateModule() override
    {
        return true;
    }
};

// ******************** Main ********************

int main(int argc, char *argv[])
{
    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("yarpAudioPlayer");
    rf.configure(argc,argv);

    yarp::os::Network yarp;

    if (!yarp.checkNetwork())
    {
        yError() << "yarp.checkNetwork() failed.";
        return -1;
    }

    PlayerModule mod;

    return mod.runModule(rf);
}

// ******************** Methods implementation ********************

bool PlayerModule::play(const std::string& filename)
{
    bool ret;
    ret = openFile(filename);
    if (!ret) { return false; }

    ret = send();
    if (!ret) { return false; }
    return true;
}

bool PlayerModule::test(double duration)
{
    bool ret;
    size_t channels = 1;
    size_t sampleRate = 16000;
    ret = yarp::sig::utils::makeTone(m_audioFile,duration,channels,sampleRate);
    yInfo() << "Generated tone of " << duration << "s, with the following properties: samples:" << m_audioFile.getSamples() << " channels:"<< m_audioFile.getChannels() << " frequency:" << m_audioFile.getFrequency() << " bytes per samples:" << m_audioFile.getBytesPerSample();
    if (!ret) { return false; }

    ret = send();
    if (!ret) { return false; }
    return true;
}

bool PlayerModule::openFile(const std::string& file_name)
{
    yInfo() << "Opening file:" << file_name;
    double st= yarp::os::Time::now();
    //reset completely the sound (frequency, channels, etc)
    m_audioFile = yarp::sig::Sound();
    //load the audio file
    bool ret = yarp::sig::file::read(m_audioFile, file_name.c_str());
    double en= yarp::os::Time::now();

    if (ret == false)
    {
        yError() << "Unable to open file" << file_name.c_str();
        return false;
    }
    yInfo() << "File loaded in: " << en - st << "s";
    yInfo() << "Loaded file has the following properties: samples:" << m_audioFile.getSamples() << " channels:"<< m_audioFile.getChannels() << " frequency:" << m_audioFile.getFrequency() << " bytes per samples:" << m_audioFile.getBytesPerSample();
    return true;
}

bool PlayerModule::send()
{
    bool ret;
    double st= yarp::os::Time::now();
    yInfo() << "Sending sound...";
    if (m_audioPort.getOutputCount() > 0)
    {
        if (m_split_size==0)
        {
            //send the whole file
            ret = m_audioPort.write(m_audioFile);
        }
        else
        {
            //split the file in blocks of fixed size
            size_t totalSamples = m_audioFile.getSamples();
            size_t iterations = totalSamples/m_split_size;
            size_t block_counter =0;
            for (size_t first = 0; first < totalSamples; first += m_split_size)
            {
                Sound block = m_audioFile.subSound(first, m_split_size);
                double st_block = yarp::os::Time::now();
                ret = m_audioPort.write(block);
                double en_block = yarp::os::Time::now();
                yInfo() << "Sent block" << block_counter << "/" << iterations << " with " << block.getSamples() << " samples in" << en_block - st_block << "s";
                block_counter++;
            }
        }
    }
    else
    {
        yWarning() << "No receivers are connected";
    }
    double en= yarp::os::Time::now();
    yInfo() << "Sound sent in: " << en - st << "s";

    return true;
}
