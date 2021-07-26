/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDFILTERS_H
#define YARP_SIG_SOUNDFILTERS_H

#include <yarp/sig/Sound.h>

namespace yarp {
    namespace sig{
        namespace soundfilters {
            /**
             * Resample a sound
             * @param snd the sound to resample
             * @param frequency the output frequency
             * @return true on success
             */
            bool YARP_sig_API resample(yarp::sig::Sound& snd, size_t frequency);
        }
    }
}

#endif // YARP_SIG_SOUNDFILTERS_H
