/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
* yarpAudioPlayer_IDL
*
* Interface.
*/

service yarpAudioPlayer_IDL
{
  /**
  * Play the given audio file
  * @return true/false on success/failure
  */
  bool play(1: string filename);

  /**
  * Generate and send a test sound
  * @param type can be "sine" , "sawtooth"  , "squarewave"
  * @param duration the duration of the sound in seconds
  * @param frequency the signal in hertz
  * @param amplitude the amplitude of the signal (16 bit)
  * @param channels number of channels
  * @param sampling_rate signal sampling rate (samples/s)
  * @return true/false on success/failure
  */
  bool test(1: string type="sine", 2: double duration=1, 3: double frequency=440, 4: i16 amplitude=30000, 5: i16 sampling_rate=16000, 6: i16 channels=1);
}
