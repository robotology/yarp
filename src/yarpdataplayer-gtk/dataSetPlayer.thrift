# Copyright: (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Vadim Tikhanoff
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#
# dataSetPlayer.thrift

/**
* dataSetPlayer_IDL
*
* Interface. 
*/

service dataSetPlayer_IDL
{
  /**
  * Steps the player once. The player will be stepped 
  * until all parts have sent data
  * @return true/false on success/failure
  */
  bool step();
  
  /**
  * Sets the frame number to the user desired frame.
  * @param name specifies the name of the loaded data
  * @param frameNum specifies the frame number the user
  *  would like to skip to
  * @return true/false on success/failure
  */
  bool setFrame(1:string name, 2:i32 frameNum);
  
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
  * Quit the module.
  * @return true/false on success/failure
  */
  bool quit();  
}
