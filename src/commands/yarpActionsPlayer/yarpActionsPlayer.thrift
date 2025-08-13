/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
* yarpActionsPlayer_IDL
*
* Interface.
*/

service yarpActionsPlayer_IDL
{
  /**
  * Start (or resumes, if stopped) the currently selected action.
  * It will stop when the last frame is reached.
  * @return true/false on success/failure
  */
  bool start();

  /**
  * Stops the currently selected (running) action. Can be resumed by start.
  * @return true/false on success/failure
  */
  bool stop();

  /**
  * Rewinds the currently selected action.
  * @return true/false on success/failure
  */
  bool reset();

  /**
  * Similar to play, but it will automatically restart the playback when the last
  * frame is reached.
  * @return true/false on success/failure
  */
  bool forever();

  /**
  * Prints all the frames of the currently selected action.
  * @return true/false on success/failure
  */
  bool print_frames();

  /**
  * Sets the playback speed factor for the currently selected action (default value: 1.0).
  * @return true/false on success/failure
  */
  bool speed_factor(1: double value);

  /**
  * Resamples the currently selected action (in seconds, recommended value 0.010s).
  * @return true/false on success/failure
  */
  bool resample(1: double value);

  /**
  * Choose the current action and wait for further commands.
  * @return true/false on success/failure
  */
  bool choose_action(1:string action_name);

  /**
  * Play an action one single time. The call blocks untils the playback is complete.
  * @return true/false on success/failure
  */
  bool play_action(1:string action_name);

  /**
  * Prints all the loaded actions.
  * @return true/false on success/failure
  */
  bool show_actions();

  /**
  * Sets the period of the sampling thread (for advanced use only, default value: 0.010s).
  * @return true/false on success/failure
  */
  bool set_thread_period(1: double value);

  /**
  * Sets the uration for the initial homing movement (for advanced use only, default value: 2s).
  * @return true/false on success/failure
  */
  bool set_initial_move_time(1: double value);
}
