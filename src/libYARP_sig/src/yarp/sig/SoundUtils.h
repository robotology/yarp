/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDUTILS_H
#define YARP_SIG_SOUNDUTILS_H

#include <yarp/sig/Sound.h>

namespace yarp::sig::utils {

/**
 * @brief Generate a sound containing a sawtooth signal.
 * @param[out] outSound The generated sound.
 * @param[in] duration_s Duration of the tone in seconds.
 * @param[in] channels Number of channels for the sound.
 * @param[in] sampleRate The sample rate of the sound in Hz.
 * @param[in] frequency The frequency of the generated sawtooth signal in Hz
 * @param[in] amplitude The amplitude of the generated sawtooth signal (max ~32767 for 16-bit)
 * @return true on success, false otherwise.
 */
bool YARP_sig_API makeSawTooth(Sound& outSound, double duration_s=1.0, size_t channels=1, size_t sampleRate=16000, double frequency=1, size_t amplitude=30000);

/**
 * @brief Generate a sound containing a sine wave tone.
 * @param[out] outSound The generated sound.
 * @param[in] duration_s Duration of the tone in seconds.
 * @param[in] channels Number of channels for the sound.
 * @param[in] sampleRate The sample rate of the sound in Hz.
 * @param[in] frequency The frequency of the generated tone in Hz (default = 440Hz A4)
 * @param[in] amplitude The amplitude of the generated tone (max ~32767 for 16-bit)
 * @return true on success, false otherwise.
 */
bool YARP_sig_API makeTone(Sound& outSound, double duration_s=1.0, size_t channels=1, size_t sampleRate=16000, double frequency=440, size_t amplitude=30000);

/**
 * @brief Generate a sound containing a square wave signal.
 * @param[out] outSound The generated sound.
 * @param[in] duration_s Duration of the tone in seconds.
 * @param[in] channels Number of channels for the sound.
 * @param[in] sampleRate The sample rate of the sound in Hz.
 * @param[in] frequency The frequency of the generated square wave signal in Hz (default = 1Hz)
 * @param[in] amplitude The amplitude of the generated square wave signal (max ~32767 for 16-bit)
 * @return true on success, false otherwise.
 */
bool YARP_sig_API makeSquareWave(Sound& outSound, double duration_s=1.0, size_t channels=1, size_t sampleRate=16000, double frequency=1, size_t amplitude=30000);

/**
 * @brief Mixes two sounds together into a single sound.
 *
 * This function combines the audio of two sounds with the same number of channels and sample rate.
 * If the sounds have different durations, silence will be added to the shorter one to match the length
 * of the longer sound before mixing.
 *
 * The @c percentage parameter controls the relative loudness of the two sounds in the mix.
 * For example, if @c percentage is 0.7, the resulting sound will consist of 70% of the first sound (@c A)
 * and 30% of the second sound (@c B).
 *
 * The resulting mixed sound is returned as a new Sound object. The original sounds (@c A and @c B)
 * are not modified.
 *
 * @param A The first sound to mix.
 * @param B The second sound to mix.
 * @param percentage The weight of the first sound in the mix (between 0.0 and 1.0).
 *                   The second sound will have a weight of (1 - percentage).
 * @return A new Sound object containing the mixed audio.
 *
 * @throws std::runtime_error If the sounds have different sample rates or channel counts.
 */
Sound YARP_sig_API mix(const Sound& A, const Sound& B, double percentage);

} // namespace yarp::sig::utils


#endif // YARP_SIG_SOUNDUTILS_H
