/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_TRANSFORM
#define YARP_TRANSFORM

#include <yarp/os/Vocab.h>
#include <yarp/sig/Matrix.h>
#include <vector>

namespace yarp {
    namespace dev {
        class ITransform;
      }
}

/**
 * @ingroup dev_iface_transform
 *
 * Transform Interface.
 */
class YARP_dev_API yarp::dev::ITransform
{
public:
    enum
    {
        TRANSFORM_OK = 0,
        TRANSFORM_GENERAL_ERROR = 1,
        TRANSFORM_TIMEOUT = 2,
    };
    /**
     * Destructor.
     */
    virtual ~ITransform() {}

    /**
    Creates a debug string containing the list of all registered frames.
    * @param all_frames the returned string containing the frames
    * @return true/false
    */
    virtual bool allFramesAsString(std::string &all_frames) = 0;

    /**
    Test if a transform exists. 
    * @return true/false
    */
    virtual bool     canTransform (const std::string &target_frame, const std::string &source_frame, std::string *error_msg=NULL)  = 0;

    /**
     Removes all the registered transforms. 
    * @return true/false
    */
    virtual bool     clear () = 0;

    /**
    Check if a frame exists. 
    * @param frame_id the frame to be searched
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return true/false
    */
    virtual bool     frameExists (const std::string &frame_id) = 0;

    /**
    Gets a vector containing all the registered frames. 
    * @param ids the returned vector containing all frame ids
    * @return true/false
    */
    virtual bool    getAllFrameIds (std::vector< std::string > &ids) = 0;

    /**
     Get the parent of a frame. 
    * @param frame_id the name of target reference frame
    * @param parent_frame_id the name of parent reference frame
    * @return true/false
    */
    virtual bool     getParent (const std::string &frame_id, std::string &parent_frame_id) = 0;
    
    /**
     Get the transform between two frames.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param transform the transformation matrix from source_frame_id to target_frame_id
    * @return true/false
    */
    virtual bool     getTransform (const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) = 0;

    /**
     Register a transform between two frames.
     * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param transform the transformation matrix from source_frame_id to target_frame_id    
    * @return true/false
    */
    virtual bool     setTransform (const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) = 0;

    /**
     Deletes a transform between two frames.
     * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return true/false
    */
    virtual bool     deleteTransform (const std::string &target_frame_id, const std::string &source_frame_id) = 0;

    /**
    Transform a point into the target frame.  
    * @param target_frame_id the name of target reference frame
    * @param input_point the input point (x y z)    
    * @param transformed_point the returned point (x y z)
    * @return true/false
    */
    virtual bool     transformPoint (const std::string &target_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point) = 0; 

    /**
     Transform a Stamped Pose into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param input_pose the input quaternion (x y z r p y)
    * @param transformed_pose the returned (x y z r p y)
    * @return true/false
    */
    virtual bool     transformPose (const std::string &target_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose) = 0;
    
    /**
     Transform a quaternion into the target frame.  
    * @param target_frame_id the name of target reference frame
    * @param input_quaternion the input quaternion (x y z w)
    * @param transformed_quaternion the returned quaternion (x y z w)
    * @return true/false
    */
    virtual bool     transformQuaternion (const std::string &target_frame_id, const yarp::sig::Vector &input_quaternion, yarp::sig::Vector &transformed_quaternion) = 0;

    /**
     Block until a transform from source_frame_id to target_frame_id is possible or it times out.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param timeout (in seconds) to wait for
    * @param error_msg string filled with error message (if error occurred)
    * @return true/false
    */
    virtual bool     waitForTransform (const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) = 0;
};

class Transform_t
{
public:
    std::string src_frame_id;
    std::string dst_frame_id;

    struct Translation_t
    {
        double tX;
        double tY;
        double tZ;
    } translation;

    struct Rotation_t
    {
        double rX;
        double rY;
        double rZ;
        double rW;
    } rotation;

    std::string toString()
    {
        char buff [1024];
        sprintf("%s -> %s \n tran: %f %f %f \n rot: %f %f %f %f \n\n", src_frame_id.c_str(), src_frame_id.c_str(), translation.tX, translation.tY, translation.tZ, rotation.rX, rotation.rY, rotation.rZ, rotation.rW);
        return std::string(buff);
    }
};

#define VOCAB_ITRANSFORM          VOCAB4('i','t','r','f')
#define VOCAB_TRANSFORM_SET       VOCAB4('t','f','s','t')
#define VOCAB_TRANSFORM_DELETE    VOCAB4('t','f','d','l')

#endif
