/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

//-------------------------------------------------

struct return_isRecording {
  1: bool ret = false;
  2: bool isRecording = false;
}

struct yarp_dev_AudioBufferSize {
} (
  yarp.name = "yarp::dev::AudioBufferSize"
  yarp.includefile = "yarp/dev/AudioBufferSize.h"
)

struct yarp_sig_Sound {
} (
  yarp.name = "yarp::sig::Sound"
  yarp.includefile = "yarp/sig/Sound.h"
)

struct return_getSound {
  1: bool ret = false;
  2: yarp_sig_Sound sound;
}

struct return_getRecordingAudioBufferCurrentSize {
  1: bool ret = false;
  2: yarp_dev_AudioBufferSize bufsize;
}

struct return_getRecordingAudioBufferMaxSize {
  1: bool ret = false;
  2: yarp_dev_AudioBufferSize bufsize;
}

typedef i32 ( yarp.type = "size_t" ) size_t

service IAudioGrabberMsgs
{
    bool setHWGain_RPC (1:double gain);
    bool setSWGain_RPC (1:double gain);
    bool resetRecordingAudioBuffer_RPC ();
    bool startRecording_RPC ();
    bool stopRecording_RPC ();
    return_isRecording isRecording_RPC ();
    return_getSound getSound(1: size_t min_number_of_samples, 2: size_t max_number_of_samples, 3: double max_samples_timeout_s);
    return_getRecordingAudioBufferMaxSize getRecordingAudioBufferMaxSize();
    return_getRecordingAudioBufferCurrentSize getRecordingAudioBufferCurrentSize();
}
