/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_SERVICE_YARPDATAPLAYER_CONSOLE_IDL_H
#define YARP_THRIFT_GENERATOR_SERVICE_YARPDATAPLAYER_CONSOLE_IDL_H

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/os/ApplicationNetworkProtocolVersion.h>

/**
 * yarpdataplayer_console_IDL
 * Interface.
 */
class yarpdataplayer_console_IDL :
        public yarp::os::Wire
{
public:
    //ProtocolVersion
    virtual yarp::os::ApplicationNetworkProtocolVersion getLocalProtocolVersion();
    virtual yarp::os::ApplicationNetworkProtocolVersion getRemoteProtocolVersion();
    virtual bool checkProtocolVersion();

    // Constructor
    yarpdataplayer_console_IDL();

    //Service methods
    /**
     * Steps the player once. The player will be stepped
     * until all parts have sent data
     * @return true/false on success/failure
     */
    virtual bool step();

    /**
     * Sets the frame number to the user desired frame.
     * @param frameNum specifies the frame number the user
     *  would like to skip to
     * @return true/false on success/failure
     */
    virtual bool setFrame(const std::int32_t frameNum);

    /**
     * Gets the frame number the user is requesting
     * @param name specifies the name of the data to modify
     *  would like to skip to
     * @return i32 returns the current frame index
     */
    virtual std::int32_t getFrame(const std::string& name);

    /**
     * Loads a dataset from a path
     * @return true/false on success/failure
     */
    virtual bool load(const std::string& path);

    /**
     * Plays the dataSets
     * @return true/false on success/failure
     */
    virtual bool play();

    /**
     * Pauses the dataSets
     * @return true/false on success/failure
     */
    virtual bool pause();

    /**
     * Stops the dataSets
     * @return true/false on success/failure
     */
    virtual bool stop();

    /**
     * Enables the specified part.
     * @param name specifies the name of the loaded data
     * @return true/false on success/failure
     */
    virtual bool enable(const std::string& part);

    /**
     * Disable the specified part.
     * @param name specifies the name of the loaded data
     * @return true/false on success/failure
     */
    virtual bool disable(const std::string& part);

    /**
     * Gets the names of all parts loaded.
     * @return list of names of the loaded parts.
     */
    virtual std::vector<std::string> getAllParts();

    /**
     * Gets the name of the port associated to the specified part
     * @param name specifies the name of the data to modify
     * @return port name
     */
    virtual std::string getPortName(const std::string& part);

    /**
     * Sets the name of the port associated to the specified part
     * @param part specifies the name of the data to modify
     * @param new_name specifies the new name to assign to the port
     * @return true/false on success/failure
     */
    virtual bool setPortName(const std::string& part, const std::string& new_name);

    /**
     * Sets the player speed
     * @param speed specifies
     * @return true/false on success/failure
     */
    virtual bool setSpeed(const double speed);

    /**
     * Gets the player speed
     * @return value of the player speed
     */
    virtual double getSpeed();

    /**
     * Sets repeat mode
     * @param val if true repeat mode is active.
     * @return true/false on success/failure
     */
    virtual bool repeat(const bool val = false);

    /**
     * Sets strict mode
     * @param val if true strict mode is active.
     * @return true/false on success/failure
     */
    virtual bool setStrict(const bool val = false);

    /**
     * Steps forward
     * @param steps number of steps to go forward (default=5).
     * @return true/false on success/failure
     */
    virtual bool forward(const std::int32_t steps = 5);

    /**
     * Steps backward
     * @param steps number of steps to go backward (default=5).
     * @return true/false on success/failure
     */
    virtual bool backward(const std::int32_t steps = 5);

    /**
     * Get the progress of the parts
     * @return the progress of the parts
     */
    virtual double getProgress();

    /**
     * Get the status of playing
     * @return the status (playing, paused, stopped)
     */
    virtual std::string getStatus();

    /**
     * Resume dataset from where it was paused
     * @return true/false on success/failure
     */
    virtual bool resume();

    /**
     * Quit the module.
     * @return true/false on success/failure
     */
    virtual bool quit();

    // help method
    virtual std::vector<std::string> help(const std::string& functionName = "--all");

    // read from ConnectionReader
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_THRIFT_GENERATOR_SERVICE_YARPDATAPLAYER_CONSOLE_IDL_H
