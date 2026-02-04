/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDUTILS_H
#define YARP_SIG_SOUNDUTILS_H

#include <yarp/sig/Sound.h>

namespace yarp::sig::utils {

/**
 * @brief Generate a sound containing a sine wave tone.
 * @param[out] outSound The generated sound.
 * @param[in] duration_s Duration of the tone in seconds.
 * @param[in] channels Number of channels for the sound.
 * @param[in] sampleRate The sample rate of the sound in Hz.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API makeTone(Sound& outSound, double duration_s=1.0, size_t channels=1, size_t sampleRate=16000);

} // namespace yarp::sig::utils


#endif // YARP_SIG_SOUNDUTILS_H
