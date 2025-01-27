/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMETRANSFORM_H
#define YARP_DEV_IFRAMETRANSFORM_H

#include <string>
#include <vector>

#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Quaternion.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IFrameTransform;
}

/**
 * @ingroup dev_iface_transform
 *
 * Transform Interface.
 */
class YARP_dev_API yarp::dev::IFrameTransform
{
public:
    enum
    {
        TRANSFORM_OK            = 0,
        TRANSFORM_GENERAL_ERROR = 1,
        TRANSFORM_TIMEOUT       = 2,
    };
    /**
     * Destructor.
     */
    virtual ~IFrameTransform();

    /**
    Creates a debug string containing the list of all registered frames.
    * @param all_frames the returned string containing the frames
    * @return true/false
    */
    virtual yarp::dev::ReturnValue allFramesAsString(std::string &all_frames) = 0;

    /**
    Test if a transform exists.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return canTransform true if the transformation exists, false otherwise
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     canTransform (const std::string &target_frame, const std::string &source_frame, bool& canTransform)  = 0;

    /**
     Removes all the registered transforms.
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     clear () = 0;

    /**
    Check if a frame exists.
    * @param frame_id the frame to be searched
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return exists true if the transformation exists, false otherwise
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     frameExists (const std::string &frame_id, bool& exists) = 0;

    /**
    Gets a vector containing all the registered frames.
    * @param ids the returned vector containing all frame ids
    * @return true/false
    */
    virtual yarp::dev::ReturnValue    getAllFrameIds (std::vector< std::string > &ids) = 0;

    /**
     Get the parent of a frame.
    * @param frame_id the name of target reference frame
    * @param parent_frame_id the name of parent reference frame
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     getParent (const std::string &frame_id, std::string &parent_frame_id) = 0;

    /**
     Get the transform between two frames.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param transform the transformation matrix from source_frame_id to target_frame_id
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     getTransform (const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) = 0;

    /**
     Register a transform between two frames.
     * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param transform the transformation matrix from source_frame_id to target_frame_id
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     setTransform (const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) = 0;

    /**
    Register a static transform between two frames.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param transform the transformation matrix from source_frame_id to target_frame_id
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     setTransformStatic(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) = 0;

    /**
     Deletes a transform between two frames.
     * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     deleteTransform (const std::string &target_frame_id, const std::string &source_frame_id) = 0;

    /**
    Transform a point into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of frame in which input_point is expressed
    * @param input_point the input point (x y z)
    * @param transformed_point the returned point (x y z)
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     transformPoint (const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point) = 0;

    /**
     Transform a Stamped Pose into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of frame in which input_pose is expressed
    * @param input_pose the input quaternion (x y z r p y)
    * @param transformed_pose the returned (x y z r p y)
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose) = 0;

    /**
     Transform a quaternion into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of frame in which input_quaternion is expressed
    * @param input_quaternion the input quaternion (x y z w)
    * @param transformed_quaternion the returned quaternion (x y z w)
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::math::Quaternion &input_quaternion, yarp::math::Quaternion &transformed_quaternion) = 0;

    /**
     Block until a transform from source_frame_id to target_frame_id is possible or it times out.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param timeout (in seconds) to wait for
    * @param error_msg string filled with error message (if error occurred)
    * @return true/false
    */
    virtual yarp::dev::ReturnValue     waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) = 0;
};

constexpr yarp::conf::vocab32_t VOCAB_ITRANSFORM              = yarp::os::createVocab32('i','t','r','f');
constexpr yarp::conf::vocab32_t VOCAB_TRANSFORM_SET           = yarp::os::createVocab32('t','f','s','t');
constexpr yarp::conf::vocab32_t VOCAB_TRANSFORM_DELETE        = yarp::os::createVocab32('t','f','d','l');
constexpr yarp::conf::vocab32_t VOCAB_TRANSFORM_DELETE_ALL    = yarp::os::createVocab32('t','f','d','a');

#endif // YARP_DEV_IFRAMETRANSFORM_H
