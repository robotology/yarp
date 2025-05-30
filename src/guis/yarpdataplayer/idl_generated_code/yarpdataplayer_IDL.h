/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_SERVICE_YARPDATAPLAYER_IDL_H
#define YARP_THRIFT_GENERATOR_SERVICE_YARPDATAPLAYER_IDL_H

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/os/ApplicationNetworkProtocolVersion.h>

/**
 * yarpdataplayer_IDL
 * Interface.
 */
class yarpdataplayer_IDL :
        public yarp::os::Wire
{
public:
    //ProtocolVersion
    virtual yarp::os::ApplicationNetworkProtocolVersion getLocalProtocolVersion();
    virtual yarp::os::ApplicationNetworkProtocolVersion getRemoteProtocolVersion();
    virtual bool checkProtocolVersion();

    // Constructor
    yarpdataplayer_IDL();

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
     * Get slider percentage
     * @return i32 percentage
     */
    virtual std::int32_t getSliderPercentage();

    /**
     * Get the status of playing
     * @return the status (playing, paused, stopped)
     */
    virtual std::string getStatus();

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
     * Quit the module.
     * @return true/false on success/failure
     */
    virtual bool quit();

    // help method
    virtual std::vector<std::string> help(const std::string& functionName = "--all");

    // read from ConnectionReader
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_THRIFT_GENERATOR_SERVICE_YARPDATAPLAYER_IDL_H
