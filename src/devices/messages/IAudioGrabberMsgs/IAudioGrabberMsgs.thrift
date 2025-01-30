/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

//-------------------------------------------------

struct yarp_sig_AudioBufferSize {
} (
  yarp.name = "yarp::sig::AudioBufferSize"
  yarp.includefile = "yarp/sig/AudioBufferSize.h"
)

struct yarp_sig_Sound {
} (
  yarp.name = "yarp::sig::Sound"
  yarp.includefile = "yarp/sig/Sound.h"
)

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_isRecording {
  1: yReturnValue ret;
  2: bool isRecording = false;
}

struct return_getSound {
  1: yReturnValue ret;
  2: yarp_sig_Sound sound;
}

struct return_getRecordingAudioBufferCurrentSize {
  1: yReturnValue ret;
  2: yarp_sig_AudioBufferSize bufsize;
}

struct return_getRecordingAudioBufferMaxSize {
  1: yReturnValue ret;
  2: yarp_sig_AudioBufferSize bufsize;
}

typedef i32 ( yarp.type = "size_t" ) size_t

service IAudioGrabberMsgs
{
    yReturnValue                              setHWGain_RPC (1:double gain);
    yReturnValue                              setSWGain_RPC (1:double gain);
    yReturnValue                              resetRecordingAudioBuffer_RPC ();
    yReturnValue                              startRecording_RPC ();
    yReturnValue                              stopRecording_RPC ();
    return_isRecording                        isRecording_RPC ();
    return_getSound                           getSound_RPC (1: size_t min_number_of_samples, 2: size_t max_number_of_samples, 3: double max_samples_timeout_s);
    return_getRecordingAudioBufferMaxSize     getRecordingAudioBufferMaxSize_RPC ();
    return_getRecordingAudioBufferCurrentSize getRecordingAudioBufferCurrentSize_RPC ();
}
