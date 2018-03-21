/*
 * Copyright (C) 2013 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

// example: yarpdev --device transformServer --ROS::enable_ros_publisher 0 --ROS::enable_ros_subscriber 0

#include "FrameTransformServer.h"
#include <sstream>
#include <limits>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <cstdlib>

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
    if (id >= 0 && (size_t)id < m_transforms.size())
    {
        m_transforms.erase(m_transforms.begin() + id);
        return true;
    }
    return false;
}

bool Transforms_server_storage::set_transform(FrameTransform t)
{
    LockGuard lock(m_mutex);
    for (size_t i = 0; i < m_transforms.size(); i++)
    {
       //@@@ this linear search requires optimization!
       if (m_transforms[i].dst_frame_id == t.dst_frame_id && m_transforms[i].src_frame_id == t.src_frame_id)
       {
          //transform already exists, update it
          m_transforms[i]=t;
          return true;
       }
    }

    //add a new transform
    m_transforms.push_back(t);
    return true;
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
  * FrameTransformServer
  */

FrameTransformServer::FrameTransformServer() : RateThread(DEFAULT_THREAD_PERIOD)
{
    m_period = DEFAULT_THREAD_PERIOD;
    m_enable_publish_ros_tf = false;
    m_enable_subscribe_ros_tf = false;
    m_yarp_static_transform_storage = nullptr;
    m_yarp_timed_transform_storage = nullptr;
    m_ros_static_transform_storage = nullptr;
    m_ros_timed_transform_storage = nullptr;
    m_rosNode = nullptr;
    m_FrameTransformTimeout = 0.200; //ms
}

FrameTransformServer::~FrameTransformServer()
{
    threadRelease();
    if (m_yarp_static_transform_storage)
    {
        delete m_yarp_static_transform_storage;
        m_yarp_static_transform_storage = nullptr;
    }
    if (m_yarp_timed_transform_storage)
    {
        delete m_yarp_timed_transform_storage;
        m_yarp_timed_transform_storage = nullptr;
    }
    if (m_ros_timed_transform_storage)
    {
        delete m_ros_timed_transform_storage;
        m_ros_timed_transform_storage = nullptr;
    }
    if (m_ros_static_transform_storage)
    {
        delete m_ros_static_transform_storage;
        m_ros_static_transform_storage = nullptr;
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

    if (storages[0]->size() == 0 &&
        storages[1]->size() == 0 &&
        storages[2]->size() == 0 &&
        storages[3]->size() == 0)
    {
        out.addString("no transforms found");
        return;
    }

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
                t.rotation.w() = in.get(8).asDouble();
                t.rotation.x() = in.get(9).asDouble();
                t.rotation.y() = in.get(10).asDouble();
                t.rotation.z() = in.get(11).asDouble();
                t.timestamp = yarp::os::Time::now();

                if (duration > 0)
                {
                    ret = m_yarp_timed_transform_storage->set_transform(t);
                }
                else
                {
                    ret = m_yarp_static_transform_storage->set_transform(t);
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
        out.addVocab(Vocab::encode("many"));
        out.addString("'list': get all transforms stored");
        out.addString("'delete_all': delete all transforms");
        out.addString("'set_static_transform <src> <dst> <x> <y> <z> <roll> <pitch> <yaw>': create a static transform");
    }
    else if (request == "set_static_transform")
    {
        FrameTransform t;
        t.src_frame_id = in.get(1).asString();
        t.dst_frame_id = in.get(2).asString();
        t.translation.tX = in.get(3).asDouble();
        t.translation.tY = in.get(4).asDouble();
        t.translation.tZ = in.get(5).asDouble();
        t.rotFromRPY(in.get(6).asDouble(), in.get(7).asDouble(), in.get(8).asDouble());
        t.timestamp = yarp::os::Time::now();
        ret = m_yarp_static_transform_storage->set_transform(t);
        if (ret == true)
        {
            yInfo() << "set_static_transform done";
            out.addString("set_static_transform done");
        }
        else
        {
            yError() << "FrameTransformServer::read() something strange happened";
        }
    }
    else if(request == "delete_all")
    {
        m_yarp_timed_transform_storage->clear();
        m_yarp_static_transform_storage->clear();
        m_ros_timed_transform_storage->clear();
        m_ros_static_transform_storage->clear();
        yInfo() << "delete_all done";
        out.addString("delete_all done");
    }
    else if (request == "list")
    {
        out.addVocab(Vocab::encode("many"));
        list_response(out);
    }
    else
    {
        yError("Invalid vocab received in FrameTransformServer");
        out.clear();
        out.addVocab(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
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
        if (m_rosNode == nullptr)
        {
            m_rosNode = new yarp::os::Node(ROSNODENAME);
        }
        if (!m_rosPublisherPort_tf_timed.topic(ROSTOPICNAME_TF))
        {
            yError() << "FrameTransformServer: unable to publish data on " << ROSTOPICNAME_TF << " topic, check your yarp-ROS network configuration";
            return false;
        }
        if (!m_rosPublisherPort_tf_static.topic(ROSTOPICNAME_TF_STATIC))
        {
            yError() << "FrameTransformServer: unable to publish data on " << ROSTOPICNAME_TF_STATIC << " topic, check your yarp-ROS network configuration";
            return false;
        }
    }

    //open ros subscriber(if requested)
    if (m_enable_subscribe_ros_tf)
    {
        if (m_rosNode == nullptr)
        {
            m_rosNode = new yarp::os::Node(ROSNODENAME);
        }
        if (!m_rosSubscriberPort_tf_timed.topic(ROSTOPICNAME_TF))
        {
            yError() << "FrameTransformServer: unable to subscribe to " << ROSTOPICNAME_TF << " topic, check your yarp-ROS network configuration";
            return false;
        }
        m_rosSubscriberPort_tf_timed.setStrict();
        if (!m_rosSubscriberPort_tf_static.topic(ROSTOPICNAME_TF_STATIC))
        {
            yError() << "FrameTransformServer: unable to subscribe to " << ROSTOPICNAME_TF_STATIC << " topic, check your yarp-ROS network configuration";
            return false;
        }
        m_rosSubscriberPort_tf_static.setStrict();
    }

    m_yarp_static_transform_storage = new Transforms_server_storage();
    m_yarp_timed_transform_storage = new Transforms_server_storage();

    m_ros_static_transform_storage = new Transforms_server_storage();
    m_ros_timed_transform_storage = new Transforms_server_storage();

    yInfo() << "Transform server started";
    return true;
}

bool FrameTransformServer::parseStartingTf(yarp::os::Searchable &config)
{

    if (config.check("USER_TF"))
    {
        Bottle group = config.findGroup("USER_TF").tail();

        for (int i = 0; i < group.size(); i++)
        {
            string         tfName;
            FrameTransform t;
            Bottle         b;
            Bottle*        list = group.get(i).asList();
            if(!list)
            {
                yError() << "no entries in USER_TF group";
                return false;
            }

            tfName   = list->get(0).asString();
            b        = group.findGroup(tfName).tail();
            string s = b.toString();

            if(b.size() == 18)
            {
                bool   r(true);
                Matrix m(4, 4);

                for(int i = 0; i < 16; i++)
                {
                    if(!b.get(i).isDouble())
                    {
                        yError() << "transformServer: param " << tfName << ", element " << i << " is not a double.";
                        r = false;
                    }
                    else
                    {
                        m.data()[i] = b.get(i).asDouble();
                    }
                }

                if(!b.get(16).isString() || !b.get(17).isString())
                {
                    r = false;
                }

                if(!r)
                {
                    yError() << "transformServer: param" << tfName << "not correct.. for the 4x4 matrix mode" <<
                                "you must provide 18 parameter. the matrix, the source frame(string) and the destination frame(string)";
                    return false;
                }

                t.fromMatrix(m);
                t.src_frame_id = b.get(16).asString();
                t.dst_frame_id = b.get(17).asString();
            }
            else if( b.size() == 8       &&
                     b.get(0).isDouble() &&
                     b.get(1).isDouble() &&
                     b.get(2).isDouble() &&
                     b.get(3).isDouble() &&
                     b.get(4).isDouble() &&
                     b.get(5).isDouble() &&
                     b.get(6).isString() &&
                     b.get(7).isString())
            {
                t.translation.set(b.get(0).asDouble(), b.get(1).asDouble(), b.get(2).asDouble());
                t.rotFromRPY(b.get(3).asDouble(), b.get(4).asDouble(), b.get(5).asDouble());
                t.src_frame_id = b.get(6).asString();
                t.dst_frame_id = b.get(7).asString();
            }
            else
            {
                yError() << "transformServer: param" << tfName << "not correct.. a tf requires 8 param in the format:" <<
                            "x(dbl) y(dbl) z(dbl) r(dbl) p(dbl) y(dbl) src(str) dst(str)";
                return false;
            }

            if(m_yarp_static_transform_storage->set_transform(t))
            {
                yInfo() << tfName << "from" << t.src_frame_id << "to" << t.dst_frame_id << "succesfully set";
            }
        }
        return true;
    }
    else
    {
        yInfo() << "transformServer: no starting tf found";
    }
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

    yarp::os::Time::delay(0.5);
    parseStartingTf(config);

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
        m_rosNode = nullptr;
    }
}

void FrameTransformServer::run()
{
    LockGuard lock(m_mutex);
    if (1)
    {
        double current_time = yarp::os::Time::now();

        //timeout check for yarp timed transforms.
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

        //timeout check for ROS timed transforms.
        do
        {
            repeat_check = false;
            size_t tfVecSize_timed_ros = m_ros_timed_transform_storage->size();
            for (size_t i = 0; i < tfVecSize_timed_ros; i++)
            {
                if (current_time - (*m_ros_timed_transform_storage)[i].timestamp > m_FrameTransformTimeout)
                {
                    m_ros_timed_transform_storage->delete_transform(i);
                    repeat_check = true;
                    break;
                }
            }
        } while (repeat_check);

        //ros subscriber
        if (m_enable_subscribe_ros_tf)
        {
            yarp::rosmsg::tf2_msgs::TFMessage* rosInData_timed = nullptr;
            do
            {
                rosInData_timed = m_rosSubscriberPort_tf_timed.read(false);
                if (rosInData_timed != nullptr)
                {
                    std::vector <yarp::rosmsg::geometry_msgs::TransformStamped> tfs = rosInData_timed->transforms;
                    size_t tfs_size = tfs.size();
                    for (size_t i = 0; i < tfs_size; i++)
                    {
                        FrameTransform t;
                        t.translation.tX = tfs[i].transform.translation.x;
                        t.translation.tY = tfs[i].transform.translation.y;
                        t.translation.tZ = tfs[i].transform.translation.z;
                        t.rotation.x() = tfs[i].transform.rotation.x;
                        t.rotation.y() = tfs[i].transform.rotation.y;
                        t.rotation.z() = tfs[i].transform.rotation.z;
                        t.rotation.w() = tfs[i].transform.rotation.w;
                        t.src_frame_id = tfs[i].header.frame_id;
                        t.dst_frame_id = tfs[i].child_frame_id;
                        //@@@ should we use yarp or ROS timestamps? 
                        t.timestamp = yarp::os::Time::now();
                        //t.timestamp = tfs[i].header.stamp.sec; //@@@this needs some revising
                        (*m_ros_timed_transform_storage).set_transform(t);
                    }
                }
            } while (rosInData_timed != nullptr);

            yarp::rosmsg::tf2_msgs::TFMessage* rosInData_static = nullptr;
            do
            {
                rosInData_static = m_rosSubscriberPort_tf_static.read(false);
                if (rosInData_static != nullptr)
                {
                    std::vector <yarp::rosmsg::geometry_msgs::TransformStamped> tfs = rosInData_static->transforms;
                    size_t tfs_size = tfs.size();
                    for (size_t i = 0; i < tfs_size; i++)
                    {
                        FrameTransform t;
                        t.translation.tX = tfs[i].transform.translation.x;
                        t.translation.tY = tfs[i].transform.translation.y;
                        t.translation.tZ = tfs[i].transform.translation.z;
                        t.rotation.x() = tfs[i].transform.rotation.x;
                        t.rotation.y() = tfs[i].transform.rotation.y;
                        t.rotation.z() = tfs[i].transform.rotation.z;
                        t.rotation.w() = tfs[i].transform.rotation.w;
                        t.src_frame_id = tfs[i].header.frame_id;
                        t.dst_frame_id = tfs[i].child_frame_id;
                        //@@@ should we use yarp or ROS timestamps?
                        t.timestamp = yarp::os::Time::now();
                        //t.timestamp = tfs[i].header.stamp; //@@@ is this ok?
                        (*m_ros_static_transform_storage).set_transform(t);
                    }
                }
            } while (rosInData_static != nullptr);
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

            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.w());
            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.x());
            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.y());
            transform.addDouble((*m_yarp_static_transform_storage)[i].rotation.z());
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

            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.w());
            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.x());
            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.y());
            transform.addDouble((*m_yarp_timed_transform_storage)[i].rotation.z());
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

            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.w());
            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.x());
            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.y());
            transform.addDouble((*m_ros_timed_transform_storage)[i].rotation.z());
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

            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.w());
            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.x());
            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.y());
            transform.addDouble((*m_ros_static_transform_storage)[i].rotation.z());
        }

        m_streamingPort.setEnvelope(m_lastStateStamp);
        m_streamingPort.write();

        //ros publisher
        if (m_enable_publish_ros_tf)
        {
            static int                        rosMsgCounter = 0;
            yarp::rosmsg::tf2_msgs::TFMessage& rosOutData_timed = m_rosPublisherPort_tf_timed.prepare();
            yarp::rosmsg::geometry_msgs::TransformStamped transform_timed;
            rosOutData_timed.transforms.clear();
            for (size_t i = 0; i < tfVecSize_timed_yarp; i++)
            {
                transform_timed.child_frame_id = (*m_yarp_timed_transform_storage)[i].dst_frame_id;
                transform_timed.header.frame_id = (*m_yarp_timed_transform_storage)[i].src_frame_id;
                transform_timed.header.seq = rosMsgCounter;
                transform_timed.header.stamp = (*m_yarp_timed_transform_storage)[i].timestamp;
                transform_timed.transform.rotation.x = (*m_yarp_timed_transform_storage)[i].rotation.x();
                transform_timed.transform.rotation.y = (*m_yarp_timed_transform_storage)[i].rotation.y();
                transform_timed.transform.rotation.z = (*m_yarp_timed_transform_storage)[i].rotation.z();
                transform_timed.transform.rotation.w = (*m_yarp_timed_transform_storage)[i].rotation.w();
                transform_timed.transform.translation.x = (*m_yarp_timed_transform_storage)[i].translation.tX;
                transform_timed.transform.translation.y = (*m_yarp_timed_transform_storage)[i].translation.tY;
                transform_timed.transform.translation.z = (*m_yarp_timed_transform_storage)[i].translation.tZ;

                rosOutData_timed.transforms.push_back(transform_timed);
            }
            m_rosPublisherPort_tf_timed.write();

            yarp::rosmsg::tf2_msgs::TFMessage& rosOutData_static = m_rosPublisherPort_tf_static.prepare();
            yarp::rosmsg::geometry_msgs::TransformStamped transform_static;
            rosOutData_static.transforms.clear();
            for (size_t i = 0; i < tfVecSize_static_yarp; i++)
            {
                transform_static.child_frame_id = (*m_yarp_static_transform_storage)[i].dst_frame_id;
                transform_static.header.frame_id = (*m_yarp_static_transform_storage)[i].src_frame_id;
                transform_static.header.seq = rosMsgCounter;
                transform_static.header.stamp = yarp::os::Time::now(); //@@@check timestamp of static transform?
                transform_static.transform.rotation.x = (*m_yarp_static_transform_storage)[i].rotation.x();
                transform_static.transform.rotation.y = (*m_yarp_static_transform_storage)[i].rotation.y();
                transform_static.transform.rotation.z = (*m_yarp_static_transform_storage)[i].rotation.z();
                transform_static.transform.rotation.w = (*m_yarp_static_transform_storage)[i].rotation.w();
                transform_static.transform.translation.x = (*m_yarp_static_transform_storage)[i].translation.tX;
                transform_static.transform.translation.y = (*m_yarp_static_transform_storage)[i].translation.tY;
                transform_static.transform.translation.z = (*m_yarp_static_transform_storage)[i].translation.tZ;

                rosOutData_static.transforms.push_back(transform_static);
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
