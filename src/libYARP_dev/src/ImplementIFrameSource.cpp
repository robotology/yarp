/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/IFrameSource.h>
#include <algorithm>
#include<thread>
using namespace yarp::dev;
using namespace yarp::math;
using namespace yarp::sig;
using namespace std;

#define FULL_RANGE(c) (c).begin(), (c).end()

FrameTransform operator*(const FrameTransform& l, const FrameTransform& r)
{
    FrameTransform ret;
    ret.dst_frame_id = l.dst_frame_id;
    ret.src_frame_id = r.src_frame_id;
    ret.fromMatrix(l.toMatrix() * r.toMatrix());
    return ret;
}

void invert(FrameTransform& frame)
{
    frame.fromMatrix(yarp::math::SE3inv(frame.toMatrix()));
}

ImplementIFrameSource::~ImplementIFrameSource() = default;

IFrameSource::Result<int> ImplementIFrameSource::setCallback(function<bool(IFrameSource*)> cb)
{
    static int callbackId = 0;
    if (callbackPrepare())
    {
        callbacks[callbackId] = cb;
        callbackId++;
        return {true, 0, callbackId -1};
    }
    else
        return { false, -1, -1 };
}

bool ImplementIFrameSource::unsetCallback(const int& id)
{
    if (id == -1)
        return false;
    callbackDismiss();
    callbacks.erase(id);
    return true;
}

ImplementIFrameSource::ConnectionType ImplementIFrameSource::getConnectionType(const string& target_frame, const string& source_frame, std::string* commonAncestor)
{
    std::vector<std::string> tar2root_vec;
    std::vector<std::string> src2root_vec;
    std::string              child;
    child = source_frame;

    auto par = getParentRaw(child);
    while (par.valid)
    {
        if (par.value == target_frame)
        {
            return DIRECT;
        }

        tar2root_vec.push_back(par.value);
        child = par.value;
        par = getParentRaw(child);
    }
    child    = target_frame;
    par      = getParentRaw(child);
    while (par.valid)
    {
        if (par.value == source_frame)
        {
            return INVERSE;
        }

        src2root_vec.push_back(par.value);
        child = par.value;
        par = getParentRaw(child);
    }

    for (const auto& i : tar2root_vec)
    {
        for (const auto& j : src2root_vec)
        {
            if (i == j)
            {
                if (commonAncestor)
                {
                    *commonAncestor = i;
                }
                return UNDIRECT;
            }
        }
    }

    return DISCONNECTED;
}

std::string ImplementIFrameSource::allFramesAsString()
{
    if(!cacheValid)
        updateFrameContainer(frameContainer);

    std::string ret;
    for (const auto& frame : frameContainer)
    {
        ret += frame.second.toString() + " ";
    }
    return ret;
}

bool ImplementIFrameSource::clearOlderFrames(const std::chrono::milliseconds& lifeMax)
{
    vector<decltype(FrameTransform::frameId)> toErase;
    double t = yarp::os::Time::now() - double(lifeMax.count())/1000.0;
    
    for (const auto& frame : frameContainer)
        if (frame.second.timestamp != 0.0 && frame.second.timestamp < t)
            toErase.emplace_back(frame.second.frameId);

    for (const auto& f : toErase)
        frameContainer.erase(f);

    return toErase.size();
}

bool yarp::dev::ImplementIFrameSource::clearStaticFrames()
{
    vector<decltype(FrameTransform::frameId)> toErase;

    for (const auto& frame : frameContainer)
        if (frame.second.timestamp == 0.0)
            toErase.emplace_back(frame.second.frameId);

    for (const auto& f : toErase)
        frameContainer.erase(f);

    return toErase.size();
}

IFrameSource::Result<bool> ImplementIFrameSource::canTransform(const string &target_frame, const string &source_frame)
{
    if (!cacheValid)
        updateFrameContainer(frameContainer);

    if (!frameExistsRaw(target_frame))
    {
        return{ false, IFrameSource::TARGET_NOT_FOUND, false };
    }
    if (!frameExistsRaw(source_frame))
    {
        return{ false, IFrameSource::SOURCE_NOT_FOUND, false };
    }
    return { true, IFrameSource::OK, getConnectionType(target_frame, source_frame) != DISCONNECTED };
}

bool ImplementIFrameSource::frameExists(const string &frame_id)
{
    if (!cacheValid)
        updateFrameContainer(frameContainer);
    return frameExistsRaw(frame_id);
}

bool ImplementIFrameSource::frameExistsRaw(const string &frame_id)
{
    for (const auto& frame : frameContainer)
    {
        if (frame.second.frameId == frame_id)
            return true;
        if (frame.second.parentFrame == frame_id)
            return true;
    }
    return false;
}

unordered_set<string> ImplementIFrameSource::getAllFrameIds()
{
    if (!cacheValid)
        updateFrameContainer(frameContainer);

    unordered_set<string> ret;
    for (const auto& frame : frameContainer)
    {
        ret.insert(frame.second.frameId);
        ret.insert(frame.second.parentFrame);
    }
    return ret;
}

std::vector<FrameTransform> ImplementIFrameSource::getAllFrames()
{
    if (!cacheValid)
        updateFrameContainer(frameContainer);
    std::vector<FrameTransform> v;
    std::transform(frameContainer.begin(), frameContainer.end(), std::back_inserter(v), [](auto &kv) { return kv.second; });
    return v;
}

IFrameSource::Result<std::string> ImplementIFrameSource::getParent(const std::string& frame_id)
{
    if (!cacheValid)
        updateFrameContainer(frameContainer);

    return getParentRaw(frame_id);
}

IFrameSource::Result<std::string> ImplementIFrameSource::getParentRaw(const std::string& frame_id)
{
    auto it = std::find_if(FULL_RANGE(frameContainer), [&frame_id](const auto& frame) { return frame.second.frameId == frame_id; });
    if (it == frameContainer.end())
    {
        return { false, IFrameSource::FRAME_NOT_FOUND, ""};
    }

    return { true, IFrameSource::OK, it->second.parentFrame };
}

IFrameSource::Result<FrameTransform> ImplementIFrameSource::getChainedTransform(const std::string& target_frame_id, const std::string& source_frame_id)
{
    const auto& tfVec = frameContainer;
    size_t i;

    for (const auto& i : frameContainer)
    {
        if (i.second.frameId == source_frame_id)
        {
            if (i.second.parentFrame == target_frame_id)
            {
                return { true, IFrameSource::OK, i.second };
            }
            else
            {
                auto tf = getChainedTransform(target_frame_id, i.second.parentFrame);
                if (tf.valid)
                {
                    return { true, IFrameSource::OK, tf.value * i.second };
                }
            }
        }
    }

    return { false, IFrameSource::FRAMES_NOT_CONNECTED, FrameTransform() };
}

IFrameSource::Result<FrameTransform> ImplementIFrameSource::getTransform(const std::string &target_frame_id, const std::string &source_frame_id, double timestamp)
{
    if (!cacheValid)
        updateFrameContainer(frameContainer);
    ConnectionType ct;
    std::string    ancestor;
    ct = getConnectionType(target_frame_id, source_frame_id, &ancestor);
    if (ct == DIRECT)
    {
        return getChainedTransform(target_frame_id, source_frame_id);
    }
    else if (ct == UNDIRECT)
    {
        auto root2src = getChainedTransform(ancestor, source_frame_id);
        auto root2tar = getChainedTransform(ancestor, target_frame_id);
        invert(root2src.value);
        return { root2src.valid && root2tar.valid, IFrameSource::UNDEFINED, root2src.value * root2tar.value };
    }
    else if (ct == INVERSE)
    {
        auto ret = getChainedTransform(source_frame_id, target_frame_id);
        invert(ret.value);
        return ret;
    }



    return { false, IFrameSource::FRAMES_NOT_CONNECTED, FrameTransform() };
}

IFrameSource::Result<Vector> ImplementIFrameSource::transformPoint(const string &target_frame_id, const string &source_frame_id, const Vector &input_point, double timestamp)
{
    if (input_point.size() != 3)
    {
        return { false, IFrameSource::WRONG_INPUT_FORMAT, yarp::sig::Vector() };
    }

    auto ret = getTransform(target_frame_id, source_frame_id);
    if (!ret.valid)
    {
        return { ret.valid, ret.error, yarp::sig::Vector() };
    }
    yarp::sig::Vector in = input_point;
    in.push_back(1);
    auto transformed_point = ret.value.toMatrix() * in;
    transformed_point.pop_back();
    return { true, IFrameSource::OK, transformed_point };
}

IFrameSource::Result<Vector> ImplementIFrameSource::transformPose(const string &target_frame_id, const string &source_frame_id, const Vector &input_pose, double timestamp)
{
    auto transformed_pose = yarp::sig::Vector(6);
    if (input_pose.size() != 6)
    {
        return { false, IFrameSource::WRONG_INPUT_FORMAT, yarp::sig::Vector() };;
    }

    auto ret = getTransform(target_frame_id, source_frame_id);
    if (!ret.valid)
    {
        return { ret.valid, ret.error, yarp::sig::Vector() };
    }
    FrameTransform t;
    t.transFromVec(input_pose[0], input_pose[1], input_pose[2]);
    t.rotFromRPY(input_pose[3], input_pose[4], input_pose[5]);
    t.fromMatrix(ret.value.toMatrix() * t.toMatrix());
    transformed_pose[0] = t.translation.tX;
    transformed_pose[1] = t.translation.tY;
    transformed_pose[2] = t.translation.tZ;

    yarp::sig::Vector rot;
    rot = t.getRPYRot();
    transformed_pose[3] = rot[0];
    transformed_pose[4] = rot[1];
    transformed_pose[5] = rot[2];
    return { true, IFrameSource::OK, transformed_pose };
}

IFrameSource::Result<Quaternion> ImplementIFrameSource::transformQuaternion(const string &target_frame_id, const string &source_frame_id, const Quaternion &input_quaternion, double timestamp)
{
    yarp::math::Quaternion transformed_quaternion;
    auto ret = getTransform(target_frame_id, source_frame_id);
    if (!ret.valid)
    {
        return { ret.valid, ret.error, yarp::math::Quaternion() };
    }
    FrameTransform t;
    t.rotation = input_quaternion;
    transformed_quaternion.fromRotationMatrix(ret.value.toMatrix() * t.toMatrix());
    return { true, IFrameSource::OK, transformed_quaternion };
}

IFrameSource::Result<FrameTransform> ImplementIFrameSource::waitForTransform(const string &target_frame_id, const string &source_frame_id, const double &timeout)
{
    //loop until canTransform == true or timeout expires
    double start = yarp::os::SystemClock::nowSystem();
    auto ret = canTransform(target_frame_id, source_frame_id);
    while (!ret.valid)
    {
        if (yarp::os::SystemClock::nowSystem() - start > timeout)
        {
            return { false, IFrameSource::TIMEOUT, FrameTransform() };
        }
        yarp::os::SystemClock::delaySystem(0.001);
        ret = canTransform(target_frame_id, source_frame_id);
    }
    return getTransform(target_frame_id, source_frame_id);
}
