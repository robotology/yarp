/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

// example: yarpdev --device transformServer --ROS::enable_ros_publisher 0 --ROS::enable_ros_subscriber 0

#define _USE_MATH_DEFINES
#include <cmath>

#include "transformServer.h"
#include <sstream>
#include <limits>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cstdlib>

using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


namespace {
YARP_LOG_COMPONENT(TRANSFORMSERVER, "yarp.device.transformServer")
}


/**
  * Transforms storage
  */

bool Transforms_server_storage::delete_transform(int id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (id >= 0 && (size_t)id < m_transforms.size())
    {
        m_transforms.erase(m_transforms.begin() + id);
        return true;
    }
    return false;
}

bool Transforms_server_storage::set_transform(const FrameTransform& t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& m_transform : m_transforms)
    {
       //@@@ this linear search requires optimization!
       if (m_transform.dst_frame_id == t.dst_frame_id && m_transform.src_frame_id == t.src_frame_id)
       {
          //transform already exists, update it
          m_transform=t;
          return true;
       }
    }

    //add a new transform
    m_transforms.push_back(t);
    return true;
}

bool Transforms_server_storage::delete_transform(string t1, string t2)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (t1=="*" && t2=="*")
    {
        m_transforms.clear();
        return true;
    }
    else
    if (t1=="*")
    {
        for (size_t i = 0; i < m_transforms.size(); )
        {
            //source frame is jolly, thus delete all frames with destination == t2
            if (m_transforms[i].dst_frame_id == t2)
            {
                m_transforms.erase(m_transforms.begin() + i);
                i=0; //the erase operation invalidates the iteration, loop restart is required
            }
            else
            {
                i++;
            }
        }
        return true;
    }
    else
    if (t2=="*")
    {
        for (size_t i = 0; i < m_transforms.size(); )
        {
            //destination frame is jolly, thus delete all frames with source == t1
            if (m_transforms[i].src_frame_id == t1)
            {
                m_transforms.erase(m_transforms.begin() + i);
                i=0; //the erase operation invalidates the iteration, loop restart is required
            }
            else
            {
                i++;
            }
        }
        return true;
    }
    else
    {
        for (size_t i = 0; i < m_transforms.size(); i++)
        {
            if ((m_transforms[i].dst_frame_id == t1 && m_transforms[i].src_frame_id == t2) ||
                (m_transforms[i].dst_frame_id == t2 && m_transforms[i].src_frame_id == t1) )
            {
                m_transforms.erase(m_transforms.begin() + i);
                return true;
            }
        }
    }
    return false;
}

void Transforms_server_storage::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_transforms.clear();
}

/**
  * TransformServer
  */

TransformServer::TransformServer() : PeriodicThread(DEFAULT_THREAD_PERIOD)
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

TransformServer::~TransformServer()
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

void TransformServer::list_response(yarp::os::Bottle& out)
{
    std::vector<Transforms_server_storage*> storages;
    std::vector<string>                     storageDescription;
    storages.push_back(m_ros_timed_transform_storage);
    storageDescription.emplace_back("ros timed transforms");

    storages.push_back(m_ros_static_transform_storage);
    storageDescription.emplace_back("ros static transforms");

    storages.push_back(m_yarp_timed_transform_storage);
    storageDescription.emplace_back("yarp timed transforms");

    storages.push_back(m_yarp_static_transform_storage);
    storageDescription.emplace_back("yarp static transforms");

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

        std::string text_to_print = storageDescription[s] + std::string("(") +std::to_string(storages[s]->size())+ std::string("): ");
        out.addString(text_to_print);

        for(size_t i = 0; i < storages[s]->size(); i++)
        {
            out.addString((*storages[s])[i].toString());
        }

    }
}

string TransformServer::get_matrix_as_text(Transforms_server_storage* storage, int i)
{
    if (m_show_transforms_in_diagram==do_not_show)
    {
        return "";
    }
    else if (m_show_transforms_in_diagram==show_quaternion)
    {
        return string(",label=\" ") + (*storage)[i].toString(FrameTransform::display_transform_mode_t::rotation_as_quaternion) + "\"";
    }
    else if (m_show_transforms_in_diagram == show_matrix)
    {
        return string(",label=\" ") + (*storage)[i].toString(FrameTransform::display_transform_mode_t::rotation_as_matrix) + "\"";
    }
    else if (m_show_transforms_in_diagram == show_rpy)
    {
        return string(",label=\" ") + (*storage)[i].toString(FrameTransform::display_transform_mode_t::rotation_as_rpy) + "\"";
    }

    yCError(TRANSFORMSERVER) << "get_matrix_as_text() invalid option";
    return "";
    /*
        //this is a test to use Latek display
        string s = "\\begin{ bmatrix } \
        1 & 2 & 3\\ \
        a & b & c \
        \\end{ bmatrix }";
    */
}

bool TransformServer::generate_view()
{
    string dot_string = "digraph G { ";
    for (size_t i = 0; i < m_ros_timed_transform_storage->size(); i++)
    {
        string edge_text = get_matrix_as_text(m_ros_timed_transform_storage, i);
        string trf_text = (*m_ros_timed_transform_storage)[i].src_frame_id + "->" +
                          (*m_ros_timed_transform_storage)[i].dst_frame_id + " " +
                          "[color = black]";
        dot_string += trf_text + '\n';
    }
    for (size_t i = 0; i < m_ros_static_transform_storage->size(); i++)
    {
        string edge_text = get_matrix_as_text(m_ros_static_transform_storage,i);
        string trf_text = (*m_ros_static_transform_storage)[i].src_frame_id + "->" +
                          (*m_ros_static_transform_storage)[i].dst_frame_id + " " +
                          "[color = black, style=dashed "+ edge_text + "]";
        dot_string += trf_text + '\n';
    }
    for (size_t i = 0; i < m_yarp_timed_transform_storage->size(); i++)
    {
        string edge_text = get_matrix_as_text(m_yarp_timed_transform_storage, i);
        string trf_text = (*m_yarp_timed_transform_storage)[i].src_frame_id + "->" +
                          (*m_yarp_timed_transform_storage)[i].dst_frame_id + " " +
                          "[color = blue "+ edge_text + "]";
        dot_string += trf_text + '\n';
    }
    for (size_t i = 0; i < m_yarp_static_transform_storage->size(); i++)
    {
        string edge_text = get_matrix_as_text(m_yarp_static_transform_storage, i);
        string trf_text = (*m_yarp_static_transform_storage)[i].src_frame_id + "->" +
                          (*m_yarp_static_transform_storage)[i].dst_frame_id + " " +
                          "[color = blue, style=dashed " + edge_text + "]";
        dot_string += trf_text + '\n';
    }

    string legend = "\n\
        rankdir=LR\n\
        node[shape=plaintext]\n\
        subgraph cluster_01 {\n\
          label = \"Legend\";\n\
          key[label=<<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\" cellborder=\"0\">\n\
            <tr><td align=\"right\" port=\"i1\">YARP timed transform</td></tr>\n\
            <tr><td align=\"right\" port=\"i2\">YARP static transform</td></tr>\n\
            <tr><td align=\"right\" port=\"i3\">ROS timed transform</td></tr>\n\
            <tr><td align=\"right\" port=\"i4\">ROS static transform</td></tr>\n\
            </table>>]\n\
          key2[label=<<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\" cellborder=\"0\">\n\
            <tr><td port = \"i1\">&nbsp;</td></tr>\n\
            <tr><td port = \"i2\">&nbsp;</td></tr>\n\
            <tr><td port = \"i3\">&nbsp;</td></tr>\n\
            <tr><td port = \"i4\">&nbsp;</td></tr>\n\
            </table>>]\n\
          key:i1:e -> key2:i1:w [color = blue]\n\
          key:i2:e -> key2:i2:w [color = blue, style=dashed]\n\
          key:i3:e -> key2:i3:w [color = black]\n\
          key:i4:e -> key2:i4:w [color = black, style=dashed]\n\
        } }";

    string command_string = "printf '"+dot_string+ legend + "' | dot -Tpdf > frames.pdf";
#if defined (__linux__)
    int ret = std::system("dot -V");
    if (ret != 0)
    {
        yCError(TRANSFORMSERVER) << "dot executable not found. Please install graphviz.";
        return false;
    }

    yCDebug(TRANSFORMSERVER) << "Command string is:" << command_string;
    ret = std::system(command_string.c_str());
    if (ret != 0)
    {
        yCError(TRANSFORMSERVER) << "The following command failed to execute:" << command_string;
        return false;
    }
#else
    yCError(TRANSFORMSERVER) << "Not yet implemented. Available only Linux";
    return false;
#endif
    return true;
}

bool TransformServer::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    string request = in.get(0).asString();

    // parse in, prepare out
    int code = in.get(0).asVocab32();
    bool ret = false;
    if (code == VOCAB_ITRANSFORM)
    {
        int cmd = in.get(1).asVocab32();
        if (cmd == VOCAB_TRANSFORM_SET)
        {
            if (in.size() != 12)
            {
                yCError(TRANSFORMSERVER) << "read(): Protocol error";
                out.clear();
                out.addVocab32(VOCAB_FAILED);
            }
            else
            {
                FrameTransform t;
                t.src_frame_id = in.get(2).asString();
                t.dst_frame_id = in.get(3).asString();
                double duration = in.get(4).asFloat64();
                t.translation.tX = in.get(5).asFloat64();
                t.translation.tY = in.get(6).asFloat64();
                t.translation.tZ = in.get(7).asFloat64();
                t.rotation.w() = in.get(8).asFloat64();
                t.rotation.x() = in.get(9).asFloat64();
                t.rotation.y() = in.get(10).asFloat64();
                t.rotation.z() = in.get(11).asFloat64();
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
                    out.addVocab32(VOCAB_OK);
                }
                else
                {
                    out.clear();
                    out.addVocab32(VOCAB_FAILED);
                    yCError(TRANSFORMSERVER) << "read(): Something strange happened";
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
            out.addVocab32(VOCAB_OK);
        }
        else if (cmd == VOCAB_TRANSFORM_DELETE)
        {
            string frame1 = in.get(2).asString();
            string frame2 = in.get(3).asString();
            bool ret1 = m_yarp_timed_transform_storage->delete_transform(frame1, frame2);
            if (ret1 == true)
            {
                out.clear();
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                bool ret2 = m_yarp_static_transform_storage->delete_transform(frame1, frame2);
                if (ret2 == true)
                {
                    out.clear();
                    out.addVocab32(VOCAB_OK);
                }
            }

        }
        else
        {
            yCError(TRANSFORMSERVER, "Invalid vocab received");
            out.clear();
            out.addVocab32(VOCAB_ERR);
        }
    }
    else if(request == "help")
    {
        out.addVocab32("many");
        out.addString("'list': get all transforms stored");
        out.addString("'delete_all': delete all transforms");
        out.addString("'set_static_transform_rad <src> <dst> <x> <y> <z> <roll> <pitch> <yaw>': create a static transform (angles in radians)");
        out.addString("'set_static_transform_deg <src> <dst> <x> <y> <z> <roll> <pitch> <yaw>': create a static transform (angles in degrees)");
        out.addString("'delete_static_transform <src> <dst>': delete a static transform");
        out.addString("'generate_view <option>': generate a frames.pdf file showing the transform tree diagram.");
        out.addString("     The following values are valid for option (default=none)");
        out.addString("    'show_rpy': show roation as rpy angles");
        out.addString("    'show_quaterion:'show rotation as a quaternion");
        out.addString("    'show_matrix:'show rotationa as a 3x3 rotation matrix");
    }
    else if (request == "set_static_transform_rad" ||
             request == "set_static_transform_deg")
    {
        FrameTransform t;
        t.src_frame_id = in.get(1).asString();
        t.dst_frame_id = in.get(2).asString();
        t.translation.tX = in.get(3).asFloat64();
        t.translation.tY = in.get(4).asFloat64();
        t.translation.tZ = in.get(5).asFloat64();
        if (request == "set_static_transform_rad")
            { t.rotFromRPY(in.get(6).asFloat64(), in.get(7).asFloat64(), in.get(8).asFloat64());}
        else if (request == "set_static_transform_deg")
            { t.rotFromRPY(in.get(6).asFloat64() * 180 / M_PI, in.get(7).asFloat64() * 180 / M_PI, in.get(8).asFloat64() * 180 / M_PI);}
        t.timestamp = yarp::os::Time::now();
        ret = m_yarp_static_transform_storage->set_transform(t);
        if (ret == true)
        {
            yCInfo(TRANSFORMSERVER) << "set_static_transform done";
            out.addString("set_static_transform done");
        }
        else
        {
            yCError(TRANSFORMSERVER) << "read(): something strange happened";
        }
    }
    else if(request == "delete_all")
    {
        m_yarp_timed_transform_storage->clear();
        m_yarp_static_transform_storage->clear();
        m_ros_timed_transform_storage->clear();
        m_ros_static_transform_storage->clear();
        yCInfo(TRANSFORMSERVER) << "delete_all done";
        out.addString("delete_all done");
    }
    else if (request == "list")
    {
        out.addVocab32("many");
        list_response(out);
    }
    else if (request == "generate_view")
    {
        m_show_transforms_in_diagram  = do_not_show;
        if      (in.get(1).asString() == "show_quaternion") m_show_transforms_in_diagram = show_quaternion;
        else if (in.get(1).asString() == "show_matrix") m_show_transforms_in_diagram = show_matrix;
        else if (in.get(1).asString() == "show_rpy") m_show_transforms_in_diagram = show_rpy;
        generate_view();
        out.addString("ok");
    }
    else if (request == "delete_static_transform")
    {
        std::string src = in.get(1).asString();
        std::string dst = in.get(2).asString();
        m_yarp_static_transform_storage->delete_transform(src,dst);
        m_ros_static_transform_storage->delete_transform(src,dst);
        out.addString("delete_static_transform done");
    }
    else
    {
        yCError(TRANSFORMSERVER, "Invalid vocab received");
        out.clear();
        out.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }
    else
    {
        yCError(TRANSFORMSERVER) << "Invalid return to sender";
    }
    return true;
}

bool TransformServer::threadInit()
{
    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yCError(TRANSFORMSERVER, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    // open data port
    if (!m_streamingPort.open(m_streamingPortName))
    {
        yCError(TRANSFORMSERVER, "Failed to open port %s", m_streamingPortName.c_str());
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
            yCError(TRANSFORMSERVER) << "Unable to publish data on " << ROSTOPICNAME_TF << " topic, check your yarp-ROS network configuration";
            return false;
        }
        if (!m_rosPublisherPort_tf_static.topic(ROSTOPICNAME_TF_STATIC))
        {
            yCError(TRANSFORMSERVER) << "Unable to publish data on " << ROSTOPICNAME_TF_STATIC << " topic, check your yarp-ROS network configuration";
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
            yCError(TRANSFORMSERVER) << "Unable to subscribe to " << ROSTOPICNAME_TF << " topic, check your yarp-ROS network configuration";
            return false;
        }
        m_rosSubscriberPort_tf_timed.setStrict();
        if (!m_rosSubscriberPort_tf_static.topic(ROSTOPICNAME_TF_STATIC))
        {
            yCError(TRANSFORMSERVER) << "Unable to subscribe to " << ROSTOPICNAME_TF_STATIC << " topic, check your yarp-ROS network configuration";
            return false;
        }
        m_rosSubscriberPort_tf_static.setStrict();
    }

    m_yarp_static_transform_storage = new Transforms_server_storage();
    m_yarp_timed_transform_storage = new Transforms_server_storage();

    m_ros_static_transform_storage = new Transforms_server_storage();
    m_ros_timed_transform_storage = new Transforms_server_storage();

    yCInfo(TRANSFORMSERVER) << "Transform server started";
    return true;
}

bool TransformServer::parseStartingTf(yarp::os::Searchable &config)
{

    if (config.check("USER_TF"))
    {
        Bottle all_transforms_group = config.findGroup("USER_TF").tail();
        yCDebug(TRANSFORMSERVER) << all_transforms_group.toString();

        for (size_t i = 0; i < all_transforms_group.size(); i++)
        {
            FrameTransform t;

            Bottle*  b = all_transforms_group.get(i).asList();
            if(!b)
            {
                yCError(TRANSFORMSERVER) << "No entries in USER_TF group";
                return false;
            }

            if(b->size() == 18)
            {
                bool   r(true);
                Matrix m(4, 4);

                for(int i = 0; i < 16; i++)
                {
                    if(!b->get(i).isFloat64())
                    {
                        yCError(TRANSFORMSERVER) << "transformServer: element " << i << " is not a double.";
                        r = false;
                    }
                    else
                    {
                        m.data()[i] = b->get(i).asFloat64();
                    }
                }

                if(!b->get(16).isString() || !b->get(17).isString())
                {
                    r = false;
                }

                if(!r)
                {
                    yCError(TRANSFORMSERVER) << "transformServer: param not correct.. for the 4x4 matrix mode" <<
                                "you must provide 18 parameter. the matrix, the source frame(string) and the destination frame(string)";
                    return false;
                }

                t.fromMatrix(m);
                t.src_frame_id = b->get(16).asString();
                t.dst_frame_id = b->get(17).asString();
            }
            else if( b->size() == 8       &&
                     b->get(0).isFloat64() &&
                     b->get(1).isFloat64() &&
                     b->get(2).isFloat64() &&
                     b->get(3).isFloat64() &&
                     b->get(4).isFloat64() &&
                     b->get(5).isFloat64() &&
                     b->get(6).isString() &&
                     b->get(7).isString())
            {
                t.translation.set(b->get(0).asFloat64(), b->get(1).asFloat64(), b->get(2).asFloat64());
                t.rotFromRPY(b->get(3).asFloat64(), b->get(4).asFloat64(), b->get(5).asFloat64());
                t.src_frame_id = b->get(6).asString();
                t.dst_frame_id = b->get(7).asString();
            }
            else
            {
                yCError(TRANSFORMSERVER) << "transformServer: param not correct.. a tf requires 8 param in the format:" <<
                            "x(dbl) y(dbl) z(dbl) r(dbl) p(dbl) y(dbl) src(str) dst(str)";
                return false;
            }

            if(m_yarp_static_transform_storage->set_transform(t))
            {
                yCInfo(TRANSFORMSERVER) << "Transform from" << t.src_frame_id << "to" << t.dst_frame_id << "successfully set";
            }
            else
            {
                yCInfo(TRANSFORMSERVER) << "Unbale to set transform from " << t.src_frame_id << "to" << t.dst_frame_id;
            }
        }
        return true;
    }
    else
    {
        yCInfo(TRANSFORMSERVER) << "No starting tf found";
    }
    return true;
}

bool TransformServer::open(yarp::os::Searchable &config)
{
    yCWarning(TRANSFORMSERVER) << "The 'transformServer' device is deprecated in favour of 'frameTransformServer'.";
    yCWarning(TRANSFORMSERVER) << "The old device is no longer supported, and it will be deprecated in YARP 3.6 and removed in YARP 4.";
    yCWarning(TRANSFORMSERVER) << "Please update your scripts.";

    Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        m_period = 0.01;
    }
    else
    {
        m_period = config.find("period").asInt32() / 1000.0;
        yCInfo(TRANSFORMSERVER) << "Thread period set to:" << m_period;
    }

    if (config.check("transforms_lifetime"))
    {
        m_FrameTransformTimeout = config.find("transforms_lifetime").asFloat64();
        yCInfo(TRANSFORMSERVER) << "transforms_lifetime set to:" << m_FrameTransformTimeout;
    }

    std::string name;
    if (!config.check("name"))
    {
        name = "transformServer";
    }
    else
    {
        name = config.find("name").asString();
    }
    m_streamingPortName =  "/"+ name + "/transforms:o";
    m_rpcPortName = "/" + name + "/rpc";

    //ROS configuration
    if (!config.check("ROS"))
    {
        yCError(TRANSFORMSERVER) << "Missing ROS group";
        return false;
    }
    Bottle ROS_config = config.findGroup("ROS");
    if (ROS_config.check("enable_ros_publisher") == false)
    {
        yCError(TRANSFORMSERVER) << "Missing 'enable_ros_publisher' in ROS group";
        return false;
    }
    if (ROS_config.find("enable_ros_publisher").asInt32() == 1 || ROS_config.find("enable_ros_publisher").asString() == "true")
    {
        m_enable_publish_ros_tf = true;
        yCInfo(TRANSFORMSERVER) << "Enabled ROS publisher";
    }
    if (ROS_config.check("enable_ros_subscriber") == false)
    {
        yCError(TRANSFORMSERVER) << "Missing 'enable_ros_subscriber' in ROS group";
        return false;
    }
    if (ROS_config.find("enable_ros_subscriber").asInt32() == 1 || ROS_config.find("enable_ros_subscriber").asString() == "true")
    {
        m_enable_subscribe_ros_tf = true;
        yCInfo(TRANSFORMSERVER) << "Enabled ROS subscriber";
    }

    this->start();

    yarp::os::Time::delay(0.5);
    parseStartingTf(config);

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

void TransformServer::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (true)
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
        yCDebug(TRANSFORMSERVER) << "yarp size" << tfVecSize_yarp << "ros_size" << tfVecSize_ros;
#endif
        yarp::os::Bottle& b = m_streamingPort.prepare();
        b.clear();

        for (size_t i = 0; i < tfVecSize_static_yarp; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_yarp_static_transform_storage)[i].src_frame_id);
            transform.addString((*m_yarp_static_transform_storage)[i].dst_frame_id);
            transform.addFloat64((*m_yarp_static_transform_storage)[i].timestamp);

            transform.addFloat64((*m_yarp_static_transform_storage)[i].translation.tX);
            transform.addFloat64((*m_yarp_static_transform_storage)[i].translation.tY);
            transform.addFloat64((*m_yarp_static_transform_storage)[i].translation.tZ);

            transform.addFloat64((*m_yarp_static_transform_storage)[i].rotation.w());
            transform.addFloat64((*m_yarp_static_transform_storage)[i].rotation.x());
            transform.addFloat64((*m_yarp_static_transform_storage)[i].rotation.y());
            transform.addFloat64((*m_yarp_static_transform_storage)[i].rotation.z());
        }
        for (size_t i = 0; i < tfVecSize_timed_yarp; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_yarp_timed_transform_storage)[i].src_frame_id);
            transform.addString((*m_yarp_timed_transform_storage)[i].dst_frame_id);
            transform.addFloat64((*m_yarp_timed_transform_storage)[i].timestamp);

            transform.addFloat64((*m_yarp_timed_transform_storage)[i].translation.tX);
            transform.addFloat64((*m_yarp_timed_transform_storage)[i].translation.tY);
            transform.addFloat64((*m_yarp_timed_transform_storage)[i].translation.tZ);

            transform.addFloat64((*m_yarp_timed_transform_storage)[i].rotation.w());
            transform.addFloat64((*m_yarp_timed_transform_storage)[i].rotation.x());
            transform.addFloat64((*m_yarp_timed_transform_storage)[i].rotation.y());
            transform.addFloat64((*m_yarp_timed_transform_storage)[i].rotation.z());
        }
        for (size_t i = 0; i < tfVecSize_timed_ros; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_ros_timed_transform_storage)[i].src_frame_id);
            transform.addString((*m_ros_timed_transform_storage)[i].dst_frame_id);
            transform.addFloat64((*m_ros_timed_transform_storage)[i].timestamp);

            transform.addFloat64((*m_ros_timed_transform_storage)[i].translation.tX);
            transform.addFloat64((*m_ros_timed_transform_storage)[i].translation.tY);
            transform.addFloat64((*m_ros_timed_transform_storage)[i].translation.tZ);

            transform.addFloat64((*m_ros_timed_transform_storage)[i].rotation.w());
            transform.addFloat64((*m_ros_timed_transform_storage)[i].rotation.x());
            transform.addFloat64((*m_ros_timed_transform_storage)[i].rotation.y());
            transform.addFloat64((*m_ros_timed_transform_storage)[i].rotation.z());
        }
        for (size_t i = 0; i < tfVecSize_static_ros; i++)
        {
            yarp::os::Bottle& transform = b.addList();
            transform.addString((*m_ros_static_transform_storage)[i].src_frame_id);
            transform.addString((*m_ros_static_transform_storage)[i].dst_frame_id);
            transform.addFloat64((*m_ros_static_transform_storage)[i].timestamp);

            transform.addFloat64((*m_ros_static_transform_storage)[i].translation.tX);
            transform.addFloat64((*m_ros_static_transform_storage)[i].translation.tY);
            transform.addFloat64((*m_ros_static_transform_storage)[i].translation.tZ);

            transform.addFloat64((*m_ros_static_transform_storage)[i].rotation.w());
            transform.addFloat64((*m_ros_static_transform_storage)[i].rotation.x());
            transform.addFloat64((*m_ros_static_transform_storage)[i].rotation.y());
            transform.addFloat64((*m_ros_static_transform_storage)[i].rotation.z());
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
        yCError(TRANSFORMSERVER, "Returned error");
    }
}

bool TransformServer::close()
{
    yCTrace(TRANSFORMSERVER, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    return true;
}
