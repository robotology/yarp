/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_AUDIORECORDER_NWC_YARP_H
#define YARP_DEV_AUDIORECORDER_NWC_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IAudioGrabberSound.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include "IAudioGrabberMsgs.h"
#include <mutex>

/*
#define DEFAULT_THREAD_PERIOD 20 //ms
const int BATTERY_TIMEOUT=100; //ms


class BatteryInputPortProcessor : public yarp::os::BufferedPort<yarp::os::Bottle>
{
    yarp::os::Bottle lastBottle;
    std::mutex mutex;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    int state;
    int count;

public:

    inline void resetStat();

    BatteryInputPortProcessor();

    using yarp::os::BufferedPort<yarp::os::Bottle>::onRead;
    void onRead(yarp::os::Bottle &v) override;

    inline int getLast(yarp::os::Bottle &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    double getVoltage();
    double getCurrent();
    double getCharge();
    double getTemperature();
    int getStatus();

};
*/

/**
* @ingroup dev_impl_network_clients
*
* \brief `audioRecoder_nwc_yarp`: The client side of any IAudioGrabberSound capable device.
*
*  Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
* | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the batteryClient device.                |       |
* | remote         |      -         | string  | -              |   -           | Yes          | Full port name of the port opened on the server side, to which the batteryClient connects to.    |     |
* | carrier        |     -          | string  | -              | tcp           | No           | The carier used for the connection with the server.               |       |
*/
class AudioRecorder_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IAudioGrabberSound
{
protected:
   // BatteryInputPortProcessor m_inputPort;
    yarp::os::Port      m_rpcPort;
    IAudioGrabberMsgs   m_audiograb_RPC;
    std::mutex          m_mutex;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IAudioGrabberSound */
    virtual bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    virtual bool startRecording() override;
    virtual bool stopRecording() override;
    virtual bool isRecording(bool& recording_enabled) override;
    virtual bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetRecordingAudioBuffer() override;
    virtual bool setSWGain(double gain) override;
    virtual bool setHWGain(double gain) override;

};

#endif // YARP_DEV_AUDIORECORDER_NWC_YARP_H
