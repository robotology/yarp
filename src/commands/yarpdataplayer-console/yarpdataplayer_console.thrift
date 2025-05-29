/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
* yarpdataplayer_console_IDL
*
* Interface.
*/

service yarpdataplayer_console_IDL
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
  * Enables the specified part.
  * @param name specifies the name of the loaded data
  * @return true/false on success/failure
  */
  bool enable(1:string part);

  /**
  * Disable the specified part.
  * @param name specifies the name of the loaded data
  * @return true/false on success/failure
  */
  bool disable(1:string part);

  /**
  * Gets the names of all parts loaded.
  * @return list of names of the loaded parts.
  */
  list<string> getAllParts();

  /**
  * Gets the name of the port associated to the specified part
  * @param name specifies the name of the data to modify
  * @return port name
  */
  string getPortName(1:string part);

  /**
  * Sets the name of the port associated to the specified part
  * @param part specifies the name of the data to modify
  * @param new_name specifies the new name to assign to the port
  * @return true/false on success/failure
  */
  bool setPortName(1:string part, 2:string new_name);

  /**
  * Sets the player speed
  * @param speed specifies
  * @return true/false on success/failure
  */
  bool setSpeed(1:double speed);

  /**
  * Gets the player speed
  * @return value of the player speed
  */
  double getSpeed();

  /**
  * Sets repeat mode
  * @param val if true repeat mode is active.
  * @return true/false on success/failure
  */
  bool repeat(1:bool val=false);

  /**
  * Sets strict mode
  * @param val if true strict mode is active.
  * @return true/false on success/failure
  */
  bool setStrict(1:bool val=false);

  /**
  * Steps forward
  * @param steps number of steps to go forward (default=5).
  * @return true/false on success/failure
  */
  bool forward(1:i32 steps=5);

  /**
  * Steps backward
  * @param steps number of steps to go backward (default=5).
  * @return true/false on success/failure
  */
  bool backward(1:i32 steps=5);

  /**
  * Get the progress of the parts
  * @return the progress of the parts
  */
  double getProgress();

  /**
  * Get the status of playing
  * @return the status (playing, paused, stopped)
  */
  string getStatus();

  /**
  * Resume dataset from where it was paused
  * @return true/false on success/failure
  */
  bool resume();

  /**
  * Quit the module.
  * @return true/false on success/failure
  */
  bool quit();
}
