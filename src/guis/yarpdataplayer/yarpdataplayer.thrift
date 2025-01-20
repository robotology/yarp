/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
* yarpdataplayer_IDL
*
* Interface.
*/

service yarpdataplayer_IDL
{
  /**
  * Steps the player once. The player will be stepped
  * until all parts have sent data
  * @return true/false on success/failure
  */
  bool step();

  /**
  * Sets the frame number to the user desired frame.
  * @param frameNum specifies the frame number the user
  *  would like to skip to
  * @return true/false on success/failure
  */
  bool setFrame(1:i32 frameNum);

  /**
  * Gets the frame number the user is requesting
  * @param name specifies the name of the data to modify
  *  would like to skip to
  * @return i32 returns the current frame index
  */
  i32 getFrame(1:string name);

  /**
  * Loads a dataset from a path
  * @return true/false on success/failure
  */
  bool load(1:string path);


  /**
   * Get slider percentage
   * @return i32 percentage
   */
  i32 getSliderPercentage();

  /**
  * Get the status of playing
  * @return the status (playing, paused, stopped)
  */
  string getStatus();

  /**
  * Plays the dataSets
  * @return true/false on success/failure
  */
  bool play();

  /**
  * Pauses the dataSets
  * @return true/false on success/failure
  */
  bool pause();

  /**
  * Stops the dataSets
  * @return true/false on success/failure
  */
  bool stop();

  /**
  * Quit the module.
  * @return true/false on success/failure
  */
  bool quit();
}
