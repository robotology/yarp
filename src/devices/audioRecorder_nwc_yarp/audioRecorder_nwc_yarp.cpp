/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "audioRecorder_nwc_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(AUDIORECORDER_NWC, "yarp.device.audioRecorder_nwc_yarp")
} // namespace

/*
inline void BatteryInputPortProcessor::resetStat()
{
    mutex.lock();
    count=0;
    deltaT=0;
    deltaTMax=0;
    deltaTMin=1e22;
    now=Time::now();
    prev=now;
    mutex.unlock();
}

BatteryInputPortProcessor::BatteryInputPortProcessor()
{
    state=IBattery::BATTERY_GENERAL_ERROR;
    resetStat();
}

void BatteryInputPortProcessor::onRead(yarp::os::Bottle &b)
{
    now=Time::now();
    mutex.lock();

    if (count>0)
    {
        double tmpDT=now-prev;
        deltaT+=tmpDT;
        if (tmpDT > deltaTMax) {
            deltaTMax = tmpDT;
        }
        if (tmpDT < deltaTMin) {
            deltaTMin = tmpDT;
        }

        //compare network time
        if (tmpDT*1000<BATTERY_TIMEOUT)
        {
            state = b.get(5).asInt32();
        }
        else
        {
            state = IBattery::BATTERY_TIMEOUT;
        }
    }

    prev=now;
    count++;

    lastBottle=b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialization (first received data)
    if (lastStamp.isValid()==false)
    {
        lastStamp = newStamp;
    }

    //now compare timestamps
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<BATTERY_TIMEOUT)
    {
        state = b.get(5).asInt32();
    }
    else
    {
        state = IBattery::BATTERY_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.unlock();
}

inline int BatteryInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.lock();
    int ret=state;
    if (ret != IBattery::BATTERY_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.unlock();

    return ret;
}

double BatteryInputPortProcessor::getVoltage()
{
    mutex.lock();
    double voltage = lastBottle.get(0).asFloat64();
    mutex.unlock();
    return voltage;
}

double BatteryInputPortProcessor::getCurrent()
{
    mutex.lock();
    double current = lastBottle.get(1).asFloat64();
    mutex.unlock();
    return current;
}

double BatteryInputPortProcessor::getCharge()
{
    mutex.lock();
    double charge = lastBottle.get(2).asFloat64();
    mutex.unlock();
    return charge;
}

int    BatteryInputPortProcessor::getStatus()
{
    mutex.lock();
    int status = lastBottle.get(4).asInt32();
    mutex.unlock();
    return status;
}

double BatteryInputPortProcessor::getTemperature()
{
    mutex.lock();
    double temperature = lastBottle.get(3).asFloat64();
    mutex.unlock();
    return temperature;
}

inline int BatteryInputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void BatteryInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    mutex.lock();
    ite=count;
    min=deltaTMin*1000;
    max=deltaTMax*1000;
    if (count<1)
    {
        av=0;
    }
    else
    {
        av=deltaT/count;
    }
    av=av*1000;
    mutex.unlock();
}
*/

bool AudioRecorder_nwc_yarp::open(yarp::os::Searchable &config)
{
    yCDebug(AUDIORECORDER_NWC) << config.toString();
    std::string local_name = config.find("local").asString();
    std::string remote_name = config.find("remote").asString();
    std::string carrier = config.check("carrier", yarp::os::Value("tcp"), "the carrier used for the connection with the server").asString();

    if (local_name =="")
    {
        yCError(AUDIORECORDER_NWC, "open(): Invalid local name. --local parameter missing.");
        return false;
    }
    if (remote_name =="")
    {
        yCError(AUDIORECORDER_NWC, "open(): Invalid remote name. --remote parameter missing.");
        return false;
    }

    std::string local_stream = local_name;
    local_stream += "/data:i";
    std::string local_rpc = local_name;
    local_rpc += "/rpc";
    std::string remote_stream = remote_name;
    remote_stream += "/data:o";
    std::string remote_rpc = remote_name;
    remote_rpc += "/rpc";

    /*
    if (!m_inputPort.open(local_stream))
    {
        yCError(BATTERYCLIENT, "open(): Could not open port %s, check network", local_stream.c_str());
        return false;
    }
    m_inputPort.useCallback();
    */

    if (!m_rpcPort.open(local_rpc))
    {
        yCError(AUDIORECORDER_NWC, "open(): Could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok;
    /*ok=Network::connect(remote_stream.c_str(), local_stream.c_str(), carrier);
    if (!ok)
    {
        yCError(BATTERYCLIENT, "open(): Could not connect %s -> %s", remote_stream.c_str(), local_stream.c_str());
        return false;
    }*/

    ok=Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
       yCError(AUDIORECORDER_NWC, "open() Could not connect %s -> %s", remote_rpc.c_str(), local_rpc.c_str());
       return false;
    }

    if (!m_audiograb_RPC.yarp().attachAsClient(m_rpcPort))
    {
        yCError(AUDIORECORDER_NWC, "Error! Cannot attach the port as a client");
        return false;
    }

    return true;
}

bool AudioRecorder_nwc_yarp::close()
{
    m_rpcPort.close();
    /*m_inputPort.close();*/
    return true;
}

bool AudioRecorder_nwc_yarp::setSWGain(double gain)
{
    bool b = m_audiograb_RPC.setSWGain_RPC(gain);
    return b;
}

bool AudioRecorder_nwc_yarp::setHWGain(double gain)
{
    bool b = m_audiograb_RPC.setHWGain_RPC(gain);
    return b;
}

bool AudioRecorder_nwc_yarp::startRecording()
{
    bool b = m_audiograb_RPC.startRecording_RPC();
    return b;
}

bool AudioRecorder_nwc_yarp::stopRecording()
{
    bool b = m_audiograb_RPC.stopRecording_RPC();
    return b;
}

bool AudioRecorder_nwc_yarp::resetRecordingAudioBuffer()
{
    bool b = m_audiograb_RPC.resetRecordingAudioBuffer_RPC();
    return b;
}

bool   AudioRecorder_nwc_yarp::isRecording(bool& recording_enabled)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.isRecording_RPC();
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: isRecording()");
        return false;
    }
    recording_enabled = ret.isRecording;
    return true;
}

bool   AudioRecorder_nwc_yarp::getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.getRecordingAudioBufferMaxSize();
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: getRecordingAudioBufferMaxSize()");
        return false;
    }
    size = ret.bufsize;
    return true;
}

bool   AudioRecorder_nwc_yarp::getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.getRecordingAudioBufferCurrentSize();
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: getRecordingAudioBufferCurrentSize()");
        return false;
    }
    size = ret.bufsize;
    return true;
}

bool AudioRecorder_nwc_yarp::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.getSound(min_number_of_samples,max_number_of_samples, max_samples_timeout_s);
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: getRecordingAudioBufferCurrentSize()");
        return false;
    }
    sound = ret.sound;
    return true;
}
