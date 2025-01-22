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
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_set_language {
  1: yReturnValue ret;
}

struct return_set_voice {
  1: yReturnValue ret;
}

struct return_set_speed {
  1: yReturnValue ret;
}

struct return_set_pitch {
  1: yReturnValue ret;
}

struct return_get_language {
  1: yReturnValue ret;
  2: string language;
}

struct return_get_voice {
  1: yReturnValue ret;
  2: string voice;
}

struct return_get_speed {
  1: yReturnValue ret;
  2: double speed;
}

struct return_get_pitch {
  1: yReturnValue ret;
  2: double pitch;
}

struct return_synthesize {
  1: yReturnValue ret;
  2: yarp_sig_Sound sound;
}

service ISpeechSynthesizerMsgs
{
    return_set_language set_language (1:string language);
    return_get_language get_language ();
    return_set_voice    set_voice    (1:string language);
    return_get_voice    get_voice    ();
    return_set_speed    set_speed    (1:double speed);
    return_get_speed    get_speed    ();
    return_set_pitch    set_pitch    (1:double pitch);
    return_get_pitch    get_pitch    ();
    return_synthesize   synthesize   (1:string text);
}
