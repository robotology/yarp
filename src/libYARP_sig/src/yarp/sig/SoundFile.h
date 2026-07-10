/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDFILE_H
#define YARP_SIG_SOUNDFILE_H

#include <yarp/sig/Sound.h>

namespace yarp::sig::file {
/**
 * Read a sound from a generic audio file.
 * @param data sound to read
 * @param filename name of file (supported extensions: .wav, .mp3)
 * @return true on success
 */
bool YARP_sig_API read(Sound& data, const char* filename);

/**
 *Read a sound from a byte array.
 * @param data sound to read (output)
 * @param bytestream the byte array (input)
 * @param streamsize size of the byte array (input)
 * @param format format of the audio data (e.g., "wav", "mp3")
 * @return true on success
 */
bool YARP_sig_API read_bytestream(Sound& data, const char* bytestream, size_t streamsize, std::string format);

/**
 *Writes a sound to a byte array.
 * @param data sound to write (intput)
 * @param bytestream the returned byte array (output)
 * @param streamsize the returned size of the byte array (output)
 * @param format format of the audio data (e.g., "wav", "mp3")
 * @return true on success
 */
bool YARP_sig_API write_bytestream(const Sound& data, char* bytestream, size_t& streamsize, std::string format);


/**
 * Write a sound to file. The format is specified by the extension.
 * @param data sound to write
 * @param filename name of file (supported extensions: .wav, .mp3)
 * @return true on success
 */
bool YARP_sig_API write(const Sound& data, const char* filename);

} // namespace yarp::sig::file

#endif // YARP_SIG_SOUNDFILE_H
