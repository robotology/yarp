/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#ifndef TRANSFORM_CLIENT_H
#define TRANSFORM_CLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ITransform.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class TransformClient;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms
const int TRANSFORM_TIMEOUT_MS=100; //ms

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class TransformInputPortProcessor : public yarp::os::BufferedPort<yarp::os::Bottle>
{
    yarp::os::Bottle lastBottle;
    yarp::os::Semaphore mutex;
    yarp::os::Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    int state;
    int count;

public:

    inline void resetStat();

    TransformInputPortProcessor();

    using yarp::os::BufferedPort<yarp::os::Bottle>::onRead;
    virtual void onRead(yarp::os::Bottle &v);

    inline int getLast(yarp::os::Bottle &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    int getStatus();

};
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
* @ingroup dev_impl_wrapper
*
* The client side of any IBattery capable device.
* Still single thread! concurrent access is unsafe.
*/
class yarp::dev::TransformClient: public DeviceDriver,
                                  public ITransform
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    TransformInputPortProcessor inputPort;
    yarp::os::Port rpcPort;
    yarp::os::ConstString local;
    yarp::os::ConstString remote;
    int _rate;
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DevideDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();


    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp();

    /**
    A way to see what frames have been cached Useful for debugging.
    */
     std::string allFramesAsString() ;

    /**
    Test if a transform exists.
    * @return true/false
    */
     bool     canTransform(const std::string &target_frame, const std::string &source_frame, std::string *error_msg = NULL) ;

    /**
    Removes all the registered transforms.
    * @return true/false
    */
     bool     clear() ;

    /**
    Check if a frame exists.
    * @param frame_id the frame to be searched
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return true/false
    */
     bool     frameExists(const std::string &frame_id) ;

    /**
    Gets a vector containing all the registered frames.
    * @param ids the returned vector containing all frame ids
    * @return true/false
    */
     bool     getAllFrameIds(std::vector< std::string > &ids) ;

    /**
    Get the parent of a frame.
    * @param frame_id the name of target reference frame
    * @param parent_frame_id the name of parent reference frame
    * @return true/false
    */
     bool     getParent(const std::string &frame_id, std::string &parent_frame_id) ;

    /**
    Get the transform between two frames.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param transform the transformation matrix from source_frame_id to target_frame_id
    * @return true/false
    */
     bool     getTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) ;

    /**
    Register a transform between two frames.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param transform the transformation matrix from source_frame_id to target_frame_id
    * @return true/false
    */
     bool     setTransform(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) ;

    /**
    Deletes a transform between two frames.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return true/false
    */
     bool     deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)     ;

    /**
    Transform a point into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param input_point the input point (x y z)
    * @param transformed_point the returned point (x y z)
    * @return true/false
    */
     bool     transformPoint(const std::string &target_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point) ;

    /**
    Transform a Stamped Pose into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param input_pose the input quaternion (x y z r p y)
    * @param transformed_pose the returned (x y z r p y)
    * @return true/false
    */
     bool     transformPose(const std::string &target_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose) ;

    /**
    Transform a quaternion into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param input_quaternion the input quaternion (x y z w)
    * @param transformed_quaternion the returned quaternion (x y z w)
    * @return true/false
    */
     bool     transformQuaternion(const std::string &target_frame_id, const yarp::sig::Vector &input_quaternion, yarp::sig::Vector &transformed_quaternion) ;

    /**
    Block until a transform from source_frame_id to target_frame_id is possible or it times out.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param timeout (in seconds) to wait for
    * @param error_msg string filled with error message (if error occurred)
    * @return true/false
    */
     bool     waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) ;
};

#endif // TRANSFORM_CLIENT_H
