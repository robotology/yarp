/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <limits>
#include "FrameTransformServer.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <stdlib.h>

using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createFrameTransformServer() {
    return new DriverCreatorOf<yarp::dev::FrameTransformServer>("transformServer",
        "transformServer",
        "yarp::dev::FrameTransformServer");
}


/**
  * Transforms storage
  */

bool Transforms_server_storage::delete_transform(int id)
{
    LockGuard lock(m_mutex);
    if ((size_t)id >= 0 && (size_t)id < m_transforms.size())
    {
        m_transforms.erase(m_transforms.begin() + id);
        return true;
    }
    return false;
}

int Transforms_server_storage::set_transform(FrameTransform t)
{
    LockGuard lock(m_mutex);
    for (size_t i = 0; i < m_transforms.size(); i++)
    {
       //@@@ this linear search requires optimization!
       if (m_transforms[i].dst_frame_id == t.dst_frame_id && m_transforms[i].src_frame_id == t.src_frame_id)
       {   
          //transform already exists, update it
          m_transforms[i]=t;
          return i;
       }
    }

    //add a new transform
    m_transforms.push_back(t);
    return m_transforms.size();
}

bool Transforms_server_storage::delete_transform(string t1, string t2)
{
    LockGuard lock(m_mutex);
    for (size_t i = 0; i < m_transforms.size(); i++)
    {
        if ((m_transforms[i].dst_frame_id == t1 && m_transforms[i].src_frame_id == t2) ||
            (m_transforms[i].dst_frame_id == t2 && m_transforms[i].src_frame_id == t1) )
        {
            m_transforms.erase(m_transforms.begin() + i);
            return true;
        }
    }
    return false;
}

void Transforms_server_storage::clear()
{
    LockGuard lock(m_mutex);
    m_transforms.clear();
}

/**
  * Helper functions
  */

inline TickTime normalizeSecNSec(double yarpTimeStamp)
{
    uint64_t time;
    uint64_t nsec_part;
    uint64_t sec_part;
    TickTime ret;

    time = (uint64_t)(yarpTimeStamp * 1000000000UL);
    nsec_part = (time % 1000000000UL);
    sec_part = (time / 1000000000UL);

    if (sec_part > std::numeric_limits<unsigned int>::max())
    {
        yWarning() << "Timestamp exceeded the 64 bit representation, resetting it to 0";
        sec_part = 0;
    }

    ret.sec = (yarp::os::NetUint32) sec_part;
    ret.nsec = (yarp::os::NetUint32) nsec_part;
    return ret;
}

/**
  * FrameTransformServer
  */

FrameTransformServer::FrameTransformServer() : RateThread(DEFAULT_THREAD_PERIOD)
{
    m_period = DEFAULT_THREAD_PERIOD;
    m_enable_publish_ros_tf = false;
    m_enable_subscribe_ros_tf = false;
    m_yarp_static_transform_storage = 0;
    m_yarp_timed_transform_storage = 0;
    m_ros_static_transform_storage = 0;
    m_ros_timed_transform_storage = 0;
    m_rosNode = 0;
    m_FrameTransformTimeout = 0.200; //ms
}

FrameTransformServer::~FrameTransformServer()
{
    threadRelease();
    if (m_yarp_static_transform_storage)
    {
        delete m_yarp_static_transform_storage;
        m_yarp_static_transform_storage = 0;
    }
    if (m_yarp_timed_transform_storage)
    {
        delete m_yarp_timed_transform_storage;
        m_yarp_timed_transform_storage = 0;
    }
    if (m_ros_timed_transform_storage)
    {
        delete m_ros_timed_transform_storage;
        m_ros_timed_transform_storage = 0;
    }
    if (m_ros_static_transform_storage)
    {
        delete m_ros_static_transform_storage;
        m_ros_static_transform_storage = 0;
    }
}

void FrameTransformServer::list_response(yarp::os::Bottle& out)
{
    std::vector<Transforms_server_storage*> storages;
    std::vector<string>                     storageDescription;
    storages.push_back(m_ros_timed_transform_storage);
    storageDescription.push_back("ros timed transforms");

    storages.push_back(m_ros_static_transform_storage);
    storageDescription.push_back("ros static transforms");

    storages.push_back(m_yarp_timed_transform_storage);
    storageDescription.push_back("yarp timed transforms");

    storages.push_back(m_yarp_static_transform_storage);
    storageDescription.push_back("yarp static transforms");

    for(size_t s = 0; s < storages.size(); s++ )
    {
        if(!storages[s])
        {
            continue;
        }
        yDebug() << storages[s]->size();
        out.addString(storageDescription[s] + ": ");

        for(size_t i = 0; i < storages[s]->size(); i++)
        {
            out.addString((*storages[s])[i].toString());
        }

    }
}

bool FrameTransformServer::read(yarp::os::ConnectionReader& connection)
{
    LockGuard lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    string request = in.get(0).asString();

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret = false;
    if (code == VOCAB_ITRANSFORM)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_TRANSFORM_SET)
        {
            if (in.size() != 12)
            {
                yError() << "FrameTransformServer::read() protocol error";
                out.clear();
                out.addVocab(VOCAB_FAILED);
            }
            else
            {
                FrameTransform t;
                t.src_frame_id = in.get(2).asString();
                t.dst_frame_id = in.get(3).asString();
                double duration = in.get(4).asDouble();
                t.translation.tX = in.get(5).asDouble();
                t.translation.tY = in.get(6).asDouble();
                t.translation.tZ = in.get(7).asDouble();
                t.rotation.rW = in.get(8).asDouble();
                t.rotation.rX = in.get(9).asDouble();
                t.rotation.rY = in.get(10).asDouble();
                t.rotation.rZ = in.get(11).asDouble();
                t.timestamp = yarp::os::Time::now();
                bool static_transform;
                if (duration > 0)
                {
                    static_transform = false;
                }
                else
                {
                    static_transform = true;
                }

                if (static_transform)
                {
                    ret = m_yarp_static_transform_storage->set_transform(t);
                }
                else
                {
                    ret = m_yarp_timed_transform_storage->set_transform(t);
                }
                if (ret == true)
                {
                    out.clear();
                    out.addVocab(VOCAB_OK);
                }
                else
                {
                    out.clear();
                    out.addVocab(VOCAB_FAILED);
                    yError() << "FrameTransformServer::read() something strange happened";
                }
            }
        }
        else if (cmd == VOCAB_TRANSFORM_DELETE_ALL)
        {
            m_yarp_timed_transform_storage->clear();
            m_yarp_static_transform_storage->clear();
            m_ros_timed_transform_storage->clear();
            m_ros_static_transform_storage->clear();
            out.clear();
            out.addVocab(VOCAB_OK);
        }
        else if (cmd == VOCAB_TRANSFORM_DELETE)
        {
            string frame1 = in.get(2).asString();
            string frame2 = in.get(3).asString();
            bool ret1 = m_yarp_timed_transform_storage->delete_transform(frame1, frame2);
            if (ret1 == true)
            {
                out.clear();
                out.addVocab(VOCAB_OK);
            }
            else
            {
                bool ret2 = m_yarp_static_transform_storage->delete_transform(frame1, frame2);
                if (ret2 == true)
                {
                    out.clear();
                    out.addVocab(VOCAB_OK);
                }
            }

        }
        else
        {
            yError("Invalid vocab received in FrameTransformServer");
            out.clear();
            out.addVocab(VOCAB_ERR);
        }
    }
    else if(request == "help")
    {
        out.addString("'list': get all transforms stored");
    }
    else if(request == "list")
    {
        list_response(out);
    }
    else
    {
        yError("Invalid vocab received in FrameTransformServer");
        out.clear();
        out.addVocab(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL)
    {
        out.write(*returnToSender);
    }
    else
    {
        yError() << "FrameTransformServer: invalid return to sender";
    }
    return true;
}

bool FrameTransformServer::threadInit()
{
    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yError("FrameTransformServer: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    // open data port
    if (!m_streamingPort.open(m_streamingPortName.c_str()))
    {
        yError("FrameTransformServer: failed to open port %s", m_streamingPortName.c_str());
        return false;
    }

    //open ros publisher (if requested)
    if (m_enable_publish_ros_tf)
    {
        if (m_rosNode == 0)
        {
            m_rosNode = new yarp::os::Node(ROSNODENAME);
        }
        if (!m_rosSubscriberPort_tf_timed.topic(ROSTOPICNAME_TF))
        {
            yError() << "FrameTransformServer: unable to publish data on " << ROSTOPICNAME_TF << " topic, check your yarp-ROS network configuration";
            return false;
        }
        if (!m_rosSubscriberPort_tf_static.topic(ROSTOPICNAME_TF_STATIC))
        {
            yError() << "FrameTransformServer: unable to publish data on " << ROSTOPICNAME_TF_STATIC << " topic, check your yarp-ROS network configuration";
            return false;
        }
    }

    //open ros subscriber(if requested)
    if (m_enable_subscribe_ros_tf)
    {
        if (m_rosNode == 0)
        {
            m_rosNode = new yarp::os::Node(ROSNODENAME);
        }
        if (!m_rosSubscriberPort_tf_timed.topic(ROSTOPICNAME_TF))
        {
            yError() << "FrameTransformServer: unable to subscribe to " << ROSTOPICNAME_TF << " topic, check your yarp-ROS network configuration";
            return false;
        }
        if (!m_rosSubscriberPort_tf_static.topic(ROSTOPICNAME_TF_STATIC))
        {
            yError() << "FrameTransformServer: unable to subscribe to " << ROSTOPICNAME_TF_STATIC << " topic, check your yarp-ROS network configuration";
            return false;
        }
    }

    m_yarp_static_transform_storage = new Transforms_server_storage();
    m_yarp_timed_transform_storage = new Transforms_server_storage();

    m_ros_static_transform_storage = new Transforms_server_storage();
    m_ros_timed_transform_storage = new Transforms_server_storage();

    yInfo() << "Transform server started";
    return true;
}



bool FrameTransformServer::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString().c_str());

    if (!config.check("period"))
    {
        m_period = 10;
    }
    else
    {
        m_period = config.find("period").asInt();
        yInfo() << "FrameTransformServer: thread period set to:" << m_period;
    }

    if (config.check("transforms_lifetime"))
    {
        m_FrameTransformTimeout = config.find("transforms_lifetime").asDouble();
        yInfo() << "FrameTransformServer: transforms_lifetime set to:" << m_FrameTransformTimeout;
    }

    std::string name;
    if (!config.check("name"))
    {
        name = "transformServer";
    }
    else
    {
        name = config.find("name").asString().c_str();
    }
    m_streamingPortName =  "/"+ name + "/transforms:o";
    m_rpcPortName = "/" + name + "/rpc";

    //ROS configuration
    if (!config.check("ROS"))
    {
        yError() << "FrameTransformServer: Missing ROS group";
        return false;
    }
    Bottle ROS_config = config.findGroup("ROS");
    if (ROS_config.check("enable_ros_publisher") == false)
    {
        yError() << "FrameTransformServer: Missing 'enable_ros_publisher' in ROS group";
        return false;
    }
    if (ROS_config.find("enable_ros_publisher").asInt() == 1 || ROS_config.find("enable_ros_publisher").asString() == "true")
    {
        m_enable_publish_ros_tf = true;
        yInfo() << "FrameTransformServer: Enabled ROS publisher";
    }
    if (ROS_config.check("enable_ros_subscriber") == false)
    {
        yError() << "FrameTransformServer: Missing 'enable_ros_subscriber' in ROS group";
        return false;
    }
    if (ROS_config.find("enable_ros_subscriber").asInt() == 1 || ROS_config.find("enable_ros_subscriber").asString() == "true")
    {
        m_enable_subscribe_ros_tf = true;
        yInfo() << "FrameTransformServer: Enabled ROS subscriber";
    }

    this->start();
    return true;
}

void FrameTransformServer::threadRelease()
{
    m_streamingPort.interrupt();
    m_streamingPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();
    if (m_enable_publish_ros_tf)
    {
        m_rosPublisherPort_tf_timed.interrupt();
        m_rosPublisherPort_tf_timed.close();
    }
    if (m_enable_subscribe_ros_tf)
    {
        m_rosSubscriberPort_tf_timed.interrupt();
        m_rosSubscriberPort_tf_timed.close();
    }
    if (m_enable_publish_ros_tf)
    {
        m_rosPublisherPort_tf_static.interrupt();
        m_rosPublisherPort_tf_static.close();
    }
    if (m_enable_subscribe_ros_tf)
    {
        m_rosSubscriberPort_tf_static.interrupt();
        m_rosSubscriberPort_tf_static.close();
    }
    if (m_rosNode)
    {
        m_rosNode->interrupt();
        delete  m_rosNode;
        m_rosNode = 0;
    }
}

void FrameTransformServer::run()
{
    LockGuard lock(m_mutex);
    if (1)
    {
        double current_time = yarp::os::Time::now();

        //timeout check for yarp timed transforms. Obviously not need need for ros transforms, since ROS aleady mangages timeouts
        bool repeat_check;
        do
        {
            repeat_check = false;
            size_t tfVecSize_timed_yarp = m_yarp_timed_transform_storage->size();
            for (size_t i = 0; i < tfVecSize_timed_yarp; i++)
            {
                if (current_time - (*m_yarp_timed_transform_storage)[i].timestamp > m_FrameTransformTimeout)
                {
                    m_yarp_timed_transform_storage->delete_transform(i);
                    repeat_check = true;
                    break;
                }
            }
        }
        while (repeat_check);

        //ros subscriber
        if (m_enable_subscribe_ros_tf)
        {
            tf_tfMessage*   rosInData_timed = m_rosSubscriberPort_tf_timed.read(false);
            tf_tfMessage*   rosInData_static = m_rosSubscriberPort_tf_static.read(false);

            if (rosInData_timed != 0)
            {
                std::vector <geometry_msgs_TransformStamped> tfs = rosInData_timed->transforms;
                size_t tfs_size = tfs.size();
                for (size_t i = 0; i < tfs_size; i++)
                {
                    FrameTransform t;
                    t.translation.tX = tfs[i].transform.translation.x;
                    t.translation.tY = tfs[i].transform.translation.y;
                    t.translation.tZ = tfs[i].transform.translation.z;
                    t.rotation.rX = tfs[i].transform.rotation.x;
                    t.rotation.rY = tfs[i].transform.rotation.y;
                    t.rotation.rZ = tfs[i].transform.rotation.z;
                    t.rotation.rW = tfs[i].transform.rotation.w;
                    t.src_frame_id = tfs[i].header.frame_id;
                    t.dst_frame_id = tfs[i].child_frame_id;
                    //@@@ add timestamp
                    //@@@ set_transform should take care of updating (and not adding!) an already existing transform.
                    //@@ This requires a fast hash table, for now, we just clearing all the storage.
                    (*m_ros_timed_transform_storage).set_transform(t);
                }
            }

            if (rosInData_static != 0)
            {
                std::vector <geometry_msgs_TransformStamped> tfs = rosInData_static->transforms;
                size_t tfs_size = tfs.size();
                for (size_t i = 0; i < tfs_size; i++)
                {
                    FrameTransform t;
                    t.translation.tX = tfs[i].transform.translation.x;
                    t.translation.tY = tfs[i].transform.translation.y;
                    t.translation.tZ = tfs[i].transform.translation.z;
                    t.rotation.rX = tfs[i].transform.rotation.x;
                    t.rotation.rY = tfs[i].transform.rotation.y;
                    t.rotation.rZ = tfs[i].transform.rotation.z;
                    t.rotation.rW = tfs[i].transform.rotation.w;
                    t.src_frame_id = tfs[i].header.frame_id;
                    t.dst_frame_id = tfs[i].child_frame_id;
                    //@@@ add timestamp
                    //@@@ set_transform should take care of updating (and not adding!) an already existing transform.
                    //@@ This requires a fast hash table, for now, we just clearing all the storage.
                    (*m_ros_static_transform_storage).set_transform(t);
                }
            }
        }

        //yarp streaming port
        m_lastStateStamp.update();
        size_t    tfVecSize_static_yarp = m_yarp_static_transform_storage->size();
        size_t    tfVecSize_timed_yarp = m_yarp_timed_transform_storage->size();
        size_t    tfVecSize_static_ros  = m_ros_static_transform_storage->size();
        size_t    tfVecSize_timed_ros = m_ros_timed_transform_storage->size();
#if 0
        yDebug() << "yarp size" << tfVecSize_yarp << "ros_size" << tfVecSize_ros;
#endif 
        yarp::os::Bottle& b = m_streamingPort.prepare();
        b.clear();

        for (size_t i = 0; i < tfVecSize_static_yarp; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_yarp_static_transform_storage)[i].src_frame_id);
            transform.addString((*m_yarp_static_transform_storage)[i].dst_frame_id);
            transform.addDouble((*m_yarp_static_transform_storage)[i].timestamp);

            transform.addDouble((*m_yarp_static_transform_storage)[i].translation.tX);
            transform.addDouble((*m_yarp_static_transform_storage)[i].translation.tY);
            transform.addDouble((*m_yarp_static_transform_storage)[i].translation.tZ);

            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.rW);
            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.rX);
            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.rY);
            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.rZ);
        }
        for (size_t i = 0; i < tfVecSize_timed_yarp; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_yarp_timed_transform_storage)[i].src_frame_id);
            transform.addString((*m_yarp_timed_transform_storage)[i].dst_frame_id);
            transform.addDouble((*m_yarp_timed_transform_storage)[i].timestamp);

            transform.addDouble((*m_yarp_timed_transform_storage)[i].translation.tX);
            transform.addDouble((*m_yarp_timed_transform_storage)[i].translation.tY);
            transform.addDouble((*m_yarp_timed_transform_storage)[i].translation.tZ);

            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.rW);
            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.rX);
            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.rY);
            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.rZ);
        }
        for (size_t i = 0; i < tfVecSize_timed_ros; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_ros_timed_transform_storage)[i].src_frame_id);
            transform.addString((*m_ros_timed_transform_storage)[i].dst_frame_id);
            transform.addDouble((*m_ros_timed_transform_storage)[i].timestamp);

            transform.addDouble((*m_ros_timed_transform_storage)[i].translation.tX);
            transform.addDouble((*m_ros_timed_transform_storage)[i].translation.tY);
            transform.addDouble((*m_ros_timed_transform_storage)[i].translation.tZ);

            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.rW);
            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.rX);
            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.rY);
            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.rZ);
        }
        for (size_t i = 0; i < tfVecSize_static_ros; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_ros_static_transform_storage)[i].src_frame_id);
            transform.addString((*m_ros_static_transform_storage)[i].dst_frame_id);
            transform.addDouble((*m_ros_static_transform_storage)[i].timestamp);

            transform.addDouble((*m_ros_static_transform_storage)[i].translation.tX);
            transform.addDouble((*m_ros_static_transform_storage)[i].translation.tY);
            transform.addDouble((*m_ros_static_transform_storage)[i].translation.tZ);

            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.rW);
            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.rX);
            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.rY);
            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.rZ);
        }

        m_streamingPort.setEnvelope(m_lastStateStamp);
        m_streamingPort.write();

        //ros publisher
        if (m_enable_publish_ros_tf)
        {
            static int                        rosMsgCounter = 0;
            tf_tfMessage&                     rosOutData_timed = m_rosPublisherPort_tf_timed.prepare();
            geometry_msgs_TransformStamped    FrameTransformimed;

            for (size_t i = 0; i < tfVecSize_timed_yarp; i++)
            {
                FrameTransformimed.child_frame_id = (*m_yarp_timed_transform_storage)[i].dst_frame_id;
                FrameTransformimed.header.frame_id = (*m_yarp_timed_transform_storage)[i].src_frame_id;
                FrameTransformimed.header.seq = rosMsgCounter;
                FrameTransformimed.header.stamp = normalizeSecNSec((*m_yarp_timed_transform_storage)[i].timestamp);
                FrameTransformimed.transform.rotation.x = (*m_yarp_timed_transform_storage)[i].rotation.rX;
                FrameTransformimed.transform.rotation.y = (*m_yarp_timed_transform_storage)[i].rotation.rY;
                FrameTransformimed.transform.rotation.z = (*m_yarp_timed_transform_storage)[i].rotation.rZ;
                FrameTransformimed.transform.rotation.w = (*m_yarp_timed_transform_storage)[i].rotation.rW;
                FrameTransformimed.transform.translation.x = (*m_yarp_timed_transform_storage)[i].translation.tX;
                FrameTransformimed.transform.translation.y = (*m_yarp_timed_transform_storage)[i].translation.tY;
                FrameTransformimed.transform.translation.z = (*m_yarp_timed_transform_storage)[i].translation.tZ;

                rosOutData_timed.transforms[i] = FrameTransformimed;
            }
            m_rosPublisherPort_tf_timed.write();

            tf_tfMessage&                     rosOutData_static = m_rosPublisherPort_tf_static.prepare();
            geometry_msgs_TransformStamped    transform_static;
            for (size_t i = 0; i < tfVecSize_static_yarp; i++)
            {
                transform_static.child_frame_id = (*m_yarp_static_transform_storage)[i].dst_frame_id;
                transform_static.header.frame_id = (*m_yarp_static_transform_storage)[i].src_frame_id;
                transform_static.header.seq = rosMsgCounter;
                transform_static.header.stamp = normalizeSecNSec(yarp::os::Time::now()); //@@@check timestamp of static transform?
                transform_static.transform.rotation.x = (*m_yarp_static_transform_storage)[i].rotation.rX;
                transform_static.transform.rotation.y = (*m_yarp_static_transform_storage)[i].rotation.rY;
                transform_static.transform.rotation.z = (*m_yarp_static_transform_storage)[i].rotation.rZ;
                transform_static.transform.rotation.w = (*m_yarp_static_transform_storage)[i].rotation.rW;
                transform_static.transform.translation.x = (*m_yarp_static_transform_storage)[i].translation.tX;
                transform_static.transform.translation.y = (*m_yarp_static_transform_storage)[i].translation.tY;
                transform_static.transform.translation.z = (*m_yarp_static_transform_storage)[i].translation.tZ;

                rosOutData_static.transforms[i] = transform_static;
            }
            m_rosPublisherPort_tf_static.write();

            rosMsgCounter++;
        }

    }
    else
    {
        yError("FrameTransformServer returned error");
    }
}

bool FrameTransformServer::close()
{
    yTrace("FrameTransformServer::Close");
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    return true;
}
