/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yarp_sig_Sound {
} (
  yarp.name = "yarp::sig::Sound"
  yarp.includefile = "yarp/sig/Sound.h"
)

struct yReturnValue {
} (
  yarp.name = "yarp::dev::yarp_ret_value"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_set_language {
  1: yReturnValue ret;
}

struct return_get_language {
  1: yReturnValue ret;
  2: string language;
}

struct return_transcribe {
  1: yReturnValue ret;
  2: string transcription;
  3: double score;
}

service ISpeechTranscriptionMsgs
{
    return_set_language set_language (1:string language);
    return_get_language get_language ();
    return_transcribe   transcribe   (1:yarp_sig_Sound sound);
}
