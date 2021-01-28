/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
