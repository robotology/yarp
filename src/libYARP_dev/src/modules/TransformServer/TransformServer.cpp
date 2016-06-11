/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include "TransformServer.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createTransformServer() {
    return new DriverCreatorOf<yarp::dev::TransformServer>("transformServer",
        "transformServer",
        "yarp::dev::TransformServer");
}


/**
  * Transforms storage
  */

bool Transforms_server_storage::set_transform(Transform_t t)
{
    //@@@ complete here
    return true;
}

bool Transforms_server_storage::delete_transform(string t1, string t2)
{
    //@@@ complete here
    return true;
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
  * TransformServer
  */

TransformServer::TransformServer() : RateThread(DEFAULT_THREAD_PERIOD)
{
    m_period = DEFAULT_THREAD_PERIOD;
    m_enable_publish_ros_tf = false;
    m_enable_subscribe_ros_tf = false;
    m_transform_storage = 0;
    m_rosNode = 0;
}

TransformServer::~TransformServer()
{
    threadRelease();
    if (m_transform_storage)
    {
        delete m_transform_storage;
        m_transform_storage = 0;
    }
}

bool TransformServer::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret = false;
    if (code == VOCAB_ITRANSFORM)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_TRANSFORM_SET)
        {
            Transform_t t;
            t.src_frame_id = in.get(2).asString();
            t.dst_frame_id = in.get(3).asString();
            t.translation.tX = in.get(4).asDouble();
            t.translation.tY = in.get(5).asDouble();
            t.translation.tZ = in.get(6).asDouble();
            t.rotation.rX = in.get(7).asDouble();
            t.rotation.rY = in.get(8).asDouble();
            t.rotation.rZ = in.get(9).asDouble();
            t.rotation.rW = in.get(10).asDouble();
            ret = ret = m_transform_storage->set_transform(t);
        }
        else if (cmd == VOCAB_TRANSFORM_DELETE)
        {
            string frame1 = in.get(2).asString();
            string frame2 = in.get(3).asString();
            ret = m_transform_storage->delete_transform(frame1, frame2);
        }
        else
        {
            yError("Invalid vocab received in TransformServer");
        }
    }
    else
    {
        yError("Invalid vocab received in TransformServer");
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL)
    {
        out.write(*returnToSender);
    }
    else
    {
        yError() << "TransformServer: invalid return to sender";
    }
    return true;
}

bool TransformServer::threadInit()
{
    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yError("TransformServer: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    // open data port
    if (!m_streamingPort.open(m_streamingPortName.c_str()))
    {
        yError("TransformServer: failed to open port %s", m_streamingPortName.c_str());
        return false;
    }

    //open ros publisher (if requested)
    if (m_enable_publish_ros_tf)
    {
        if (m_rosNode == 0)
        {
            m_rosNode = new yarp::os::Node(ROSNODENAME);
        }
        if (!m_rosPublisherPort_tf.topic(ROSTOPICNAME))
        {
            yError() << " Unable to publish data on " << ROSTOPICNAME << " topic, check your yarp-ROS network configuration\n";
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
        if (!m_rosSubscriberPort_tf.topic(ROSTOPICNAME))
        {
            yError() << " Unable to subscribe to " << ROSTOPICNAME << " topic, check your yarp-ROS network configuration\n";
            return false;
        }
    }

    m_transform_storage = new Transforms_server_storage();

    yInfo() << "Transform server started";
    return true;
}



bool TransformServer::open(yarp::os::Searchable &config)
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
        yError() << "Missing ROS group";
        return false;
    }
    Bottle ROS_config = config.findGroup("ROS");
    if (ROS_config.find("enable_ros_publisher").asInt() == 1)
    {
        m_enable_publish_ros_tf = true;
        yInfo() << "Enabled ROS publisher";
    }
    if (ROS_config.find("enable_ros_subscriber").asInt() == 1)
    {
        m_enable_subscribe_ros_tf = true;
        yInfo() << "Enabled ROS subscriber";
    }

    this->start();
    return true;
}

void TransformServer::threadRelease()
{
    m_streamingPort.interrupt();
    m_streamingPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();
    if (m_enable_publish_ros_tf)
    {
        m_rosPublisherPort_tf.interrupt();
        m_rosPublisherPort_tf.close();
    }
    if (m_enable_subscribe_ros_tf)
    {
        m_rosSubscriberPort_tf.interrupt();
        m_rosSubscriberPort_tf.close();
    }
    if (m_rosNode)
    {
        delete  m_rosNode;
        m_rosNode = 0;
    }
}

void TransformServer::run()
{
    if (1)
    {
        //ros subscriber
        if (m_enable_subscribe_ros_tf)
        {
            tf_tfMessage*   rosInData = m_rosSubscriberPort_tf.read(false);
            if (rosInData != 0)
            {
                std::vector <geometry_msgs_TransformStamped> tfs = rosInData->transforms;
                size_t tfs_size = tfs.size();
                for (size_t i = 0; i < tfs_size; tfs_size)
                {
                    Transform_t t;
                    t.translation.tX = tfs[i].transform.translation.x;
                    t.translation.tY = tfs[i].transform.translation.y;
                    t.translation.tZ = tfs[i].transform.translation.z;
                    t.rotation.rX = tfs[i].transform.rotation.x;
                    t.rotation.rY = tfs[i].transform.rotation.y;
                    t.rotation.rZ = tfs[i].transform.rotation.z;
                    t.rotation.rW = tfs[i].transform.rotation.w;
                    t.src_frame_id = tfs[i].header.frame_id;
                    t.dst_frame_id = tfs[i].child_frame_id;
                    (*m_transform_storage).set_transform(t);
                }
            }
        }

        //yarp streaming port
        m_lastStateStamp.update();
        size_t    tfVecSize = m_transform_storage->size();
        yarp::os::Bottle& b = m_streamingPort.prepare();
        b.clear();

        for (size_t i = 0; i < tfVecSize; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_transform_storage)[i].src_frame_id);
            transform.addString((*m_transform_storage)[i].dst_frame_id);

            transform.addDouble((*m_transform_storage)[i].translation.tX);
            transform.addDouble((*m_transform_storage)[i].translation.tY);
            transform.addDouble((*m_transform_storage)[i].translation.tZ);

            transform.addDouble((*m_transform_storage)[i].rotation.rX);
            transform.addDouble((*m_transform_storage)[i].rotation.rY);
            transform.addDouble((*m_transform_storage)[i].rotation.rZ);
            transform.addDouble((*m_transform_storage)[i].rotation.rW);
        }
        
        m_streamingPort.setEnvelope(m_lastStateStamp);
        m_streamingPort.write();

        //ros publisher
        if (m_enable_publish_ros_tf)
        {
            static int                        rosMsgCounter = 0;
            tf_tfMessage&                     rosOutData = m_rosPublisherPort_tf.prepare();
            geometry_msgs_TransformStamped    transform;

            for (size_t i = 0; i < tfVecSize; i++)
            {
                transform.child_frame_id = (*m_transform_storage)[i].dst_frame_id;
                transform.header.frame_id = (*m_transform_storage)[i].src_frame_id;
                transform.header.seq = rosMsgCounter;
                transform.header.stamp = normalizeSecNSec(yarp::os::Time::now());
                transform.transform.rotation.x = (*m_transform_storage)[i].rotation.rX;
                transform.transform.rotation.y = (*m_transform_storage)[i].rotation.rY;
                transform.transform.rotation.z = (*m_transform_storage)[i].rotation.rZ;
                transform.transform.rotation.w = (*m_transform_storage)[i].rotation.rW;
                transform.transform.translation.x = (*m_transform_storage)[i].translation.tX;
                transform.transform.translation.y = (*m_transform_storage)[i].translation.tY;
                transform.transform.translation.z = (*m_transform_storage)[i].translation.tZ;

                rosOutData.transforms[i] = transform;
            }
            m_rosPublisherPort_tf.write();
            rosMsgCounter++;
        }

    }
    else
    {
        yError("TransformServer returned error");
    }
}

bool TransformServer::close()
{
    yTrace("TransformServer::Close");
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    RateThread::stop();
    return true;
}
