/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <transformClient.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file TransformClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;


inline void Transforms_client_storage::resetStat()
{

}

void Transforms_client_storage::onRead(yarp::os::Bottle &b)
{
    m_now = Time::now();
    LockGuard guard(m_mutex);

    if (m_count>0)
    {
        double tmpDT = m_now - m_prev;
        m_deltaT += tmpDT;
        if (tmpDT>m_deltaTMax)
            m_deltaTMax = tmpDT;
        if (tmpDT<m_deltaTMin)
            m_deltaTMin = tmpDT;

        //compare network time
        /*if (tmpDT*1000<TRANSFORM_TIMEOUT)
        {
            state = b.get(5).asInt();
        }
        else
        {
            state = TRANSFORM_TIMEOUT;
        }*/
    }

    m_prev = m_now;
    m_count++;

    m_lastBottle = b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialization (first received data)
    if (m_lastStamp.isValid() == false)
    {
        m_lastStamp = newStamp;
    }

    //now compare timestamps
   // if ((1000 * (newStamp.getTime() - m_lastStamp.getTime()))<TRANSFORM_TIMEOUT_MS)
    if (1)
    {
        m_state = ITransform::TRANSFORM_OK;

        m_transforms.clear();
        for (int i = 0; i < b.size(); i++)
        {
            Bottle* bt = b.get(i).asList();
            if (bt != 0)
            {
                Transform_t t;
                t.src_frame_id = bt->get(0).asString();
                t.dst_frame_id = bt->get(1).asString();
                t.translation.tX = bt->get(2).asDouble();
                t.translation.tY = bt->get(3).asDouble();
                t.translation.tZ = bt->get(4).asDouble();
                t.rotation.rX = bt->get(5).asDouble();
                t.rotation.rY = bt->get(6).asDouble();
                t.rotation.rZ = bt->get(7).asDouble();
                t.rotation.rW = bt->get(8).asDouble();
                m_transforms.push_back(t);
            }
        }
    }
    else
    {
        m_state = ITransform::TRANSFORM_TIMEOUT;
    }
    m_lastStamp = newStamp;
}

inline int Transforms_client_storage::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    LockGuard guard(m_mutex);

    int ret = m_state;
    if (ret != ITransform::TRANSFORM_GENERAL_ERROR)
    {
        data = m_lastBottle;
        stmp = m_lastStamp;
    }

    return ret;
}

inline int Transforms_client_storage::getIterations()
{
    LockGuard guard(m_mutex);
    int ret = m_count;
    return ret;
}

// time is in ms
void Transforms_client_storage::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    LockGuard guard(m_mutex);
    ite=m_count;
    min=m_deltaTMin*1000;
    max=m_deltaTMax*1000;
    if (m_count<1)
    {
        av=0;
    }
    else
    {
        av=m_deltaT/m_count;
    }
    av=av*1000;
}

Transforms_client_storage::Transforms_client_storage(std::string local_streaming_name)
{
    m_count = 0;
    m_deltaT = 0;
    m_deltaTMax = 0;
    m_deltaTMin = 1e22;
    m_now = Time::now();
    m_prev = m_now;

    if (!this->open(local_streaming_name.c_str()))
    {
        yError("TransformClient::open() error could not open port %s, check network", local_streaming_name.c_str());
    }
    this->useCallback();
}

Transforms_client_storage::~Transforms_client_storage()
{
    this->interrupt();
    this->close();
}

bool yarp::dev::TransformClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name  = config.find("local").asString().c_str();
    m_remote_name = config.find("remote").asString().c_str();

    if (m_local_name == "")
    {
        yError("TransformClient::open() error you have to provide valid local name");
        return false;
    }
    if (m_remote_name == "")
    {
        yError("TransformClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt();
    }
    else
    {
        m_period = 10;
        yWarning("TransformClient: using default period of %d ms" , m_period);
    }

    ConstString local_rpc = m_local_name;
    local_rpc += "/rpc";
    ConstString remote_rpc = m_remote_name;
    remote_rpc += "/rpc";
    ConstString remote_streaming_name = m_remote_name;
    remote_streaming_name += "/transforms:o";
    ConstString local_streaming_name = m_local_name;
    local_streaming_name += "/transforms:i";

    if (!m_rpcPort.open(local_rpc.c_str()))
    {
        yError("TransformClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    m_transform_storage = new Transforms_client_storage(local_streaming_name);
    bool ok=Network::connect(remote_streaming_name.c_str(), local_streaming_name.c_str(), "udp");
    if (!ok)
    {
        yError("TransformClient::open() error could not connect to %s", remote_streaming_name.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("TransformClient::open() error could not connect to %s", remote_rpc.c_str());
        return false;
    }

    return true;
}

bool yarp::dev::TransformClient::close()
{
    m_rpcPort.close();
    if (m_transform_storage != 0)
    {
        delete m_transform_storage;
        m_transform_storage = 0;
    }
    return true;
}

bool yarp::dev::TransformClient::allFramesAsString(std::string &all_frames)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        all_frames += (*m_transform_storage)[i].toString() + " ";
    }
    return true;
}

bool yarp::dev::TransformClient::canLinearTransform(const std::string &target_frame, const std::string &source_frame, std::string *error_msg) const
{
    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t i;
    for (i = 0; i < tfVec.size(); i++)
    {
        if (tfVec[i].dst_frame_id == target_frame)
        {
            if (tfVec[i].src_frame_id == source_frame)
            {
                return true;
            }
            else
            {
                return canLinearTransform(tfVec[i].src_frame_id, source_frame, error_msg);
            }
        }
    }
    return false;
}

bool yarp::dev::TransformClient::canTransform(const std::string &target_frame, const std::string &source_frame, std::string *error_msg)
{
    //Transforms_client_storage& tfVec = *m_transform_storage;
    //size_t i;
    //for (i = 0; i < tfVec.size(); i++)
    //{
    //    if (tfVec[i].dst_frame_id == target_frame)
    //    {
    //        if (tfVec[i].src_frame_id == source_frame)
    //        {
    //            return true;
    //        }
    //        else
    //        {
    //            return canTransform(tfVec[i].src_frame_id, source_frame, error_msg);
    //            /*if (canTransform(tfVec[i].src_frame_id, source_frame, error_msg))
    //            {
    //                return true;
    //            }*/
    //        }
    //    }
    //}

    if (canLinearTransform(target_frame, source_frame) || canLinearTransform(source_frame, target_frame))
    {
        return true;
    }

    if (error_msg)
    {
        *error_msg = "tf connection not found";
    }
    return false;
}

bool yarp::dev::TransformClient::clear()
{
    m_transform_storage->clear();
    return true;
}

bool yarp::dev::TransformClient::frameExists(const std::string &frame_id)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        if (((*m_transform_storage)[i].src_frame_id) == frame_id) { return true; }
        if (((*m_transform_storage)[i].dst_frame_id) == frame_id) { return true; }
    }
    return false;
}

bool yarp::dev::TransformClient::getAllFrameIds(std::vector< std::string > &ids)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        bool found = false;
        for (size_t j = 0; j < ids.size(); j++)
        {
            if (((*m_transform_storage)[i].src_frame_id) == ids[j]) { found = true; break; }
        }
        if (found == false) ids.push_back((*m_transform_storage)[i].src_frame_id);
    }

    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        bool found = false;
        for (size_t j = 0; j < ids.size(); j++)
        {
            if (((*m_transform_storage)[i].dst_frame_id) == ids[j]) { found = true; break; }
        }
        if (found == false) ids.push_back((*m_transform_storage)[i].dst_frame_id);
    }

    return true;
}

bool yarp::dev::TransformClient::getParent(const std::string &frame_id, std::string &parent_frame_id)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        std::string par((*m_transform_storage)[i].dst_frame_id);
        if (((*m_transform_storage)[i].dst_frame_id == frame_id))
        {
            
            parent_frame_id = (*m_transform_storage)[i].src_frame_id;
            return true;
        }
    }
    return false;
}

bool yarp::dev::TransformClient::getLinearTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform)
{
    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t i;
    for (i = 0; i < tfVec.size(); i++)
    {
        if (tfVec[i].dst_frame_id == target_frame_id)
        {
            if (tfVec[i].src_frame_id == source_frame_id)
            {
                transform = tfVec[i].toMatrix();
                return true;
            }
            else
            {
                yarp::sig::Matrix m;
                if (getTransform(tfVec[i].src_frame_id, source_frame_id, m))
                {
                    //to uncomment -- build yarp math
                    transform = m * tfVec[i].toMatrix();
                    return true;
                }
            }
        }
    }
    return false;
}

bool yarp::dev::TransformClient::getTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform)
{
    

    if (canLinearTransform(target_frame_id, source_frame_id))
    {
        return getLinearTransform(target_frame_id, source_frame_id, transform);
    }
    else if (canLinearTransform(source_frame_id, target_frame_id))
    {
        yarp::sig::Matrix m(4, 4);
        getLinearTransform(source_frame_id, target_frame_id, m);
        transform = yarp::math::SE3inv(m);
        return true;
    }

    yError() << "frame not connected! call canTransform() first nextime, instead of wasting my time please..";
    return false;
}

bool yarp::dev::TransformClient::setTransform(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform)
{
    if (canTransform(target_frame_id, source_frame_id))
    {
        yError() << "such transform already exist (directly or by chaining transforms)!";
        return false;
    }

    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    Transform_t      tf;
    
    if (!tf.fromMatrix(transform))
    {
        yError() << "wrong matrix format, it has to be 4 by 4";
        return false;
    }

    b.addVocab(VOCAB_ITRANSFORM);
    b.addVocab(VOCAB_TRANSFORM_SET);
    b.addString(source_frame_id);
    b.addString(target_frame_id);
    b.addDouble(tf.translation.tX);
    b.addDouble(tf.translation.tY);
    b.addDouble(tf.translation.tZ);
    b.addDouble(tf.rotation.rX);
    b.addDouble(tf.rotation.rY);
    b.addDouble(tf.rotation.rZ);
    b.addDouble(tf.rotation.rW);
    bool ret = m_rpcPort.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "recived error from server on creating frame between " + source_frame_id + " and " + target_frame_id;
            return false;
        }
    }
    else
    {
        yError() << "setFrame() -> error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::TransformClient::deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    b.addVocab(VOCAB_ITRANSFORM);
    b.addVocab(VOCAB_TRANSFORM_DELETE);
    b.addString(target_frame_id);
    b.addString(source_frame_id);
    bool ret = m_rpcPort.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab()!=VOCAB_OK)
        {
            yError() << "recived error from server on deleting frame between "+source_frame_id+" and "+target_frame_id;
            return false;
        }
    }
    else
    {
        yError() << "deleteFrame()->error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::TransformClient::transformPoint(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point)
{
    if (input_point.size() != 3)
    {
        yError() << "sorry.. only 3 dimensional vector allowed my dear..";
        return false;
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yError() << "no transform found between source and target";
        return false;
    }
    yarp::sig::Vector in = input_point;
    in.push_back(1);
    transformed_point = m * in;
    transformed_point.pop_back();
    return true;
}

bool yarp::dev::TransformClient::transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose)
{
    if (input_pose.size() != 6)
    {
        yError() << "sorry.. only 6 dimensional vector (3 axes + roll pith and yaw) allowed, dear friend of mine..";
        return false;
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yError() << "no transform found between source and target";
        return false;
    }
    Transform_t t;
    t.transFromVec(input_pose[0], input_pose[1], input_pose[2]);
    t.rotFromRPY(input_pose[3], input_pose[4], input_pose[5]);
    t.fromMatrix(m * t.toMatrix());
    transformed_pose[0] = t.translation.tX;
    transformed_pose[1] = t.translation.tY;
    transformed_pose[2] = t.translation.tZ;
    
    yarp::sig::Vector rot;
    rot = t.getRPYRot();
    transformed_pose[3] = rot[0];
    transformed_pose[4] = rot[1];
    transformed_pose[5] = rot[2];
    return true;
}

bool yarp::dev::TransformClient::transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_quaternion, yarp::sig::Vector &transformed_quaternion)
{
    if (input_quaternion.size() != 4)
    {
        yError() << "we're very sorry.. only quaternion allowed man..";
        return false;
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yError() << "no transform found between source and target";
        return false;
    }
    Transform_t t;
    t.rotation.fromQuaternion(input_quaternion);
    transformed_quaternion = yarp::math::dcm2quat(m * t.toMatrix());
    return true;
}

bool yarp::dev::TransformClient::waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout)
{
    //loop fintanto che ccantTRransform e' true o ppure scade timeout
    double start = yarp::os::Time::now();
    while (canTransform(target_frame_id, source_frame_id))
    {
        if (yarp::os::Time::now() - start > timeout)
        {
            yError() << "timeout reached";
            return false;
        }
    }
    return true;
}

/*Stamp yarp::dev::TransformClient::getLastInputStamp()
{
    return m_lastTs;
}*/

yarp::dev::DriverCreator *createTransformClient()
{
    return new DriverCreatorOf<TransformClient>
               (
                   "transformClient",
                   "",
                   "transformClient"
               );
}
