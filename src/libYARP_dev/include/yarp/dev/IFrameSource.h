/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IFRAMESOURCE_H
#define YARP_DEV_IFRAMESOURCE_H

#include <unordered_set>
#include <map>
#include <vector>
#include <functional>
#include <yarp/dev/api.h>
#include <yarp/dev/IFrameSet.h> // <---useless include.. but without it IFrameSet doesn't export his symbol. to investigate
#include <yarp/math/Quaternion.h>
#include <yarp/math/FrameTransform.h>
#include <thread>
#include <atomic>

namespace yarp {
namespace dev {
class IFrameSource;
class ImplementIFrameSource;
}
}

class YARP_dev_API yarp::dev::IFrameSource
{
public:
    template <typename T>
    class Result
    {
    public:
#if (defined _WIN32 && _MSC_VER < 1910) || (!defined _WIN32 && __cplusplus < 201402L)
        bool        valid;
#else
        bool        valid{false};
#endif
        int         error;

        T           value;
        //adding a operator bool(){return valid;} is a very bad idea as it can cause confusion between the result of bool functions and their success
    };

    enum errors
    {
        OK = 0,
        FRAME_NOT_FOUND,
        TARGET_NOT_FOUND,
        SOURCE_NOT_FOUND,
        FRAMES_NOT_CONNECTED,
        WRONG_INPUT_FORMAT,
        TIMEOUT,
        CALLBACK_ACTIVE,
        UNDEFINED
    };

    /**
    Disable a specific callback
    * @param id the ID of the callback to be removed. received with setCallback()
    * @return true if succeded false otherwise
    */
    virtual bool unsetCallback(const int& id) = 0;

    /**
    enable a callback system. the data producer will call all the callback at every data update
    * @param cb the callback to be set
    * @return Result<int> the ID of the callback (useful to unset it) wrapped by a result object containing the operation success or failure
    */
    virtual Result<int> setCallback(std::function<bool(IFrameSource*)> cb) = 0;
    
    /**
    Creates a debug string containing the list of all registered frames.
    * @return std::string
    */
    virtual std::string allFramesAsString() = 0;

    /**
    Removes all frames older than lifeMax. frame with timestamp 0.0 are considered static and will not be removed
    * @return true if any frame has been removed, otherwise
    */
    virtual bool clearOlderFrames(const std::chrono::milliseconds& lifeMax) = 0;

    /**
    Removes all static frames.
    * @return true if any frame has been removed, otherwise
    */
    virtual bool clearStaticFrames() = 0;

    /**
    Test if a transform exists.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @return Result<bool>. if the target or the source frame are not present the result will be negative and the value inside should be ignored. if both the frame are correct but not connected 
    *the result will be positive and the value negative. otherwise both positive
    */
    virtual Result<bool> canTransform(const std::string &target_frame, const std::string &source_frame) = 0;

    /**
    Check if a frame exists.
    * @param frame_id the frame to be searched
    * @return true if it exists, false otherwise
    */
    virtual bool frameExists(const std::string &frame_id) = 0;

    /**
    Gets a set containing all the frame_id.
    * @return the set of frame ids
    */
    virtual std::unordered_set<std::string> getAllFrameIds() = 0;

    /**
    Gets a vector containing all the registered frames.
    * @return the vector
    */
    virtual std::vector<yarp::math::FrameTransform> getAllFrames() = 0;

    /**
     Get the parent of a frame.
    * @param frame_id the name of target reference frame
    * @return a valid outcome will contain the parent frame. if the frame_id doesn't exist an invalid outcome should be returned
    */
    virtual Result<std::string> getParent(const std::string& frame_id) = 0;

    /**
     Get the transform between two frames.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param time point in which retrieve the data (if minor than 0 the last data will be used)
    * @return a valid outcome will contain the frameTransform. if a frame doesn't exist or the frame are not connected an invalid outcome should be returned
    */
    virtual Result<yarp::math::FrameTransform> getTransform(const std::string &target_frame_id, const std::string &source_frame_id, double timestamp = -1) = 0;

    /**
    Transform a point into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of frame in which input_point is expressed
    * @param input_point the input point (x y z)
    * @param time point in which retrieve the data (if minor than 0 the last data will be used)
    * @return the point transformed
    */
    virtual Result<yarp::sig::Vector> transformPoint(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, double timestamp = -1) = 0;

    /**
     Transform a Stamped Pose into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of frame in which input_pose is expressed
    * @param input_pose the input quaternion (x y z r p y)
    * @param time point in which retrieve the data (if minor than 0 the last data will be used)
    * @return the pose transformed
    */
    virtual Result<yarp::sig::Vector> transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, double timestamp = -1) = 0;

    /**
     Transform a quaternion into the target frame.
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of frame in which input_quaternion is expressed
    * @param input_quaternion the input quaternion (x y z w)
    * @param time point in which retrieve the data (if minor than 0 the last data will be used)
    * @return the quaternion transformed
    */
    virtual Result<yarp::math::Quaternion> transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::math::Quaternion &input_quaternion, double timestamp = -1) = 0;

    /**
     Block until a transform from source_frame_id to target_frame_id is possible or it times out. returns the frame transform requested
    * @param target_frame_id the name of target reference frame
    * @param source_frame_id the name of source reference frame
    * @param timeout (in seconds) to wait for
    * @return the frameTransform
    */
    virtual Result<yarp::math::FrameTransform> waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) = 0;

};

/**
 the ImplementIFrameSource class provides a default implementation of all the method of IFrameSource interface wich can be reused by the derived classes, 
 requiring them to only implement some protected method and filling up a container with their frames.
*/
class YARP_dev_API yarp::dev::ImplementIFrameSource : public yarp::dev::IFrameSource
{
private:
    std::map<int, std::function<bool(IFrameSource*)>> callbacks;
protected:
    
    class FrameEditor
    {
        std::map<std::string, yarp::math::FrameTransform> storage;
        yarp::math::FrameTransform nullframe;

    public:
#if (defined _WIN32 && _MSC_VER < 1910) || (!defined _WIN32 && __cplusplus < 201402L)
        const std::map<std::string, yarp::math::FrameTransform>::const_iterator begin() const { return storage.begin(); }
        const std::map<std::string, yarp::math::FrameTransform>::const_iterator end() const { return storage.end(); }
        const size_t erase(const std::string& k) { return storage.erase(k); }
#else
        const auto begin() const { return storage.begin(); }
        const auto end() const { return storage.end(); }
        const auto erase(const std::string& k) { return storage.erase(k); }
#endif
        bool insertUpdate(const yarp::math::FrameTransform& frame)
        {
            storage[frame.frameId] = frame;
            return true;
        }

        Result<yarp::math::FrameTransform> get(const std::string& frameid)
        {
            if (storage.find(frameid) == storage.end())
            {
                return { false, 0, nullframe };
            }
            return { true, 0, storage[frameid] };
        }
    };

    /**
     this should be set to true and false in the derived class to to flag the frames present in the container as obsolete and request a new update.
     tipically the implementer will set it to false when before getting/receiving new transforms and set it to true at the end of updateFrameContainer()
    */
    std::atomic_bool cacheValid;
    
    /**
     the method to be called from the derived classes to call the callbacks.
    */
    bool callAllCallbacks()
    {
        if (!callbacks.size())
            return false;

        updateFrameContainer(frameContainer);
        for (auto callback : callbacks)
            if (callback.second)
                callback.second((IFrameSource*)this);
        return true;
    }
    
    /**
     the method that will be called from this class in order to get the frames from the derived classes.
     this is called if cacheValid is false (and so the frames in the container should be updated).
     THREAD CONCURRENCY WARNING: can be called by both the user thread and the implementer thread (if any) when calling callAllCallbacks().
    */
    virtual void updateFrameContainer(FrameEditor& frameContainer) = 0;
    
    /**
     the method for the subclasses to allow or forbid the callback system
    */
    virtual bool callbackPrepare() = 0;
    
    /**
     the method for signaling the subclasses the dismount of a callback
    */
    virtual bool callbackDismiss() = 0;

private:
    enum ConnectionType
    {
        DIRECT,
        INVERSE,
        UNDIRECT,
        DISCONNECTED
    };
    
    //is pimpl pattern suitable for this case? we have protected member that should be accessed from the private methods.
    FrameEditor         frameContainer;
    ConnectionType      getConnectionType(const std::string& target_frame, const std::string& source_frame, std::string* commonAncestor = nullptr);
    bool                frameExistsRaw(const std::string &frame_id);
    Result<std::string> getParentRaw(const std::string& frame_id);

    Result<yarp::math::FrameTransform> getChainedTransform(const std::string& target_frame_id, const std::string& source_frame_id);

public:
    virtual ~ImplementIFrameSource();
    virtual bool unsetCallback(const int& id) override;
    virtual Result<int> setCallback(std::function<bool(IFrameSource*)> cb) override;
    virtual std::string allFramesAsString() override;
    virtual bool clearOlderFrames(const std::chrono::milliseconds& lifeMax) override;
    virtual bool clearStaticFrames() override;
    virtual Result<bool> canTransform(const std::string &target_frame, const std::string &source_frame) override;
    virtual bool frameExists(const std::string &frame_id) override;
    virtual std::unordered_set<std::string> getAllFrameIds() override;
    virtual std::vector<yarp::math::FrameTransform> getAllFrames() override;
    virtual Result<std::string> getParent(const std::string& frame_id) override;
    virtual Result<yarp::math::FrameTransform> getTransform(const std::string &target_frame_id, const std::string &source_frame_id, double timestamp = -1) override;
    virtual Result<yarp::sig::Vector> transformPoint(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, double timestamp = -1) override;
    virtual Result<yarp::sig::Vector> transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, double timestamp = -1) override;
    virtual Result<yarp::math::Quaternion> transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::math::Quaternion &input_quaternion, double timestamp = -1) override;
    virtual Result<yarp::math::FrameTransform> waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) override;
};

#endif //YARP_DEV_IFRAMESOURCE_H
