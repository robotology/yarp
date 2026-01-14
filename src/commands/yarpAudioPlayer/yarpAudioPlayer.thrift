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
}
