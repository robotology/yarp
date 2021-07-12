/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FrameTransformClient.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/math/Math.h>

#include <cmrc/cmrc.hpp>
#include <mutex>
CMRC_DECLARE(frameTransformRC);

 /*! \file FrameTransformClient.cpp */

//example: yarpdev --device frameTransformClient --local /transformClient --remote /transformServer

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORMCLIENT, "yarp.device.FrameTransformClient")
}
#define LOG_THROTTLE_PERIOD 1.0

//------------------------------------------------------------------------------------------------------------------------------
bool FrameTransformClient::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> lock (m_rpc_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    string request = in.get(0).asString();
    if (request == "help")
    {
        out.addVocab32("many");
        out.addString("'get_transform <src> <dst>: print the transform from <src> to <dst>");
        out.addString("'list_frames: print all the available reference frames");
        out.addString("'list_ports: print all the opened ports for transform broadcasting");
        out.addString("'publish_transform <src> <dst> <portname> [format]: opens a port to publish transform from src to dst");
        out.addString("'unpublish_transform <portname>: closes a previously opened port to publish a transform");
        out.addString("'unpublish_all: closes a all previously opened ports to publish a transform");
    }
    else if (request == "list_frames")
    {
        std::vector<string> v;
        this->getAllFrameIds(v);
        out.addVocab32("many");
        out.addString("List of available reference frames:");
        int count = 0;
        for (auto& vec : v)
        {
            count++;
            string str = std::to_string(count) + "- " + vec;
            out.addString(str.c_str());
        }
    }
    else if (request == "get_transform")
    {
        string src = in.get(1).asString();
        string dst = in.get(2).asString();
        out.addVocab32("many");
        yarp::sig::Matrix m;
        this->getTransform(src, dst, m);
        out.addString("Transform from " + src + " to " + dst + " is: ");
        out.addString(m.toString());
    }
    else if (request == "list_ports")
    {
        out.addVocab32("many");
        if (m_array_of_ports.size()==0)
        {
            out.addString("No ports are currently active");
        }
        for (auto& m_array_of_port : m_array_of_ports)
        {
            if (m_array_of_port)
            {
                string  s = m_array_of_port->port.getName() + " "+ m_array_of_port->transform_src +  " -> " + m_array_of_port->transform_dst;
                out.addString(s);
            }
        }
    }
    else if (request == "publish_transform")
    {
        out.addVocab32("many");
        string src  = in.get(1).asString();
        string dst  = in.get(2).asString();
        string port_name = in.get(3).asString();
        string format_s = "matrix";
        broadcast_port_t::format_t eformat = broadcast_port_t::format_t::matrix;
        if   (in.size() == 5) {format_s = in.get(4).asString();}

        if (format_s == "matrix") {eformat = broadcast_port_t::format_t::matrix;}
        else {yCError(FRAMETRANSFORMCLIENT) << "Invalid format" << format_s << "using format `matrix`. Only `matrix` is currently supported."; }

        if (port_name[0] == '/')  port_name.erase(port_name.begin());
        std::string full_port_name = m_local_name + "/" + port_name;

        //print a warning if the frames do not exists yet
        if (this->frameExists(src) == false)
        {
            out.addString("Requested src frame " + src + " does not exists.");
            yCWarning(FRAMETRANSFORMCLIENT, "Requested src frame %s does not exists.", src.c_str());
        }
        if (this->frameExists(dst) == false)
        {
            out.addString("Requested dst frame " + dst + " does not exists.");
            yCWarning(FRAMETRANSFORMCLIENT, "Requested fst frame %s does not exists.", dst.c_str());
        }

        //check if the port is already active in yarp
        bool port_already_exists = yarp::os::Network::exists(full_port_name);
        if (port_already_exists) { yCError(FRAMETRANSFORMCLIENT, "Requested port already exists!"); }

        //check if the port is already registered by the system
        bool bcast_already_exists = false;
        for (auto& m_array_of_port : m_array_of_ports)
        {
            if (m_array_of_port && m_array_of_port->port.getName() == full_port_name)
            {
                bcast_already_exists |= true;
                break;
            }
        }

        //create the broadcaster
        if (bcast_already_exists == false && port_already_exists == false)
        {
            auto* b = new broadcast_port_t;
            b->transform_src = src;
            b->transform_dst = dst;
            b->format = eformat;
            bool pret = b->port.open(full_port_name);
            if (pret)
            {
                out.addString("Operation complete. Port " + full_port_name + " opened.");
                m_array_of_ports.push_back(b);
                if (m_array_of_ports.size()==1) this->start();
            }
            else
            {
                delete b;
                out.addString("Operation failed. Unable to open port " + full_port_name + ".");
            }
        }
        else
        {
            out.addString("unable to perform operation");
        }
    }
    else if (request == "unpublish_all")
    {
        for (auto& m_array_of_port : m_array_of_ports)
        {
            m_array_of_port->port.close();
            delete m_array_of_port;
            m_array_of_port=nullptr;
        }
        m_array_of_ports.clear();
        if (m_array_of_ports.size()==0) this->askToStop();
        out.addString("Operation complete");
    }
    else if (request == "unpublish_transform")
    {
        bool ret = false;
        string port_name = in.get(1).asString();
        if (port_name[0]=='/')  port_name.erase(port_name.begin());
        string full_port_name = m_local_name + "/" + port_name;
        for (auto it = m_array_of_ports.begin(); it != m_array_of_ports.end(); it++)
        {
            if ((*it)->port.getName() == port_name)
            {
                (*it)->port.close();
                delete (*it);
                (*it)=nullptr;
                 m_array_of_ports.erase(it);
                 ret = true;
                 break;
            }
        }
        if (ret)
        {
            out.addString("Port " + full_port_name + " has been closed.");
        }
        else
        {
            out.addString("Port " + full_port_name + " was not found.");
        }
        if (m_array_of_ports.size()==0) this->askToStop();
    }
    else
    {
        yCError(FRAMETRANSFORMCLIENT, "Invalid vocab received in FrameTransformClient");
        out.clear();
        out.addVocab32(VOCAB_ERR);
        out.addString("Invalid command name");
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }
    else
    {
        yCError(FRAMETRANSFORMCLIENT) << "Invalid return to sender";
    }
    return true;
}

bool FrameTransformClient::open(yarp::os::Searchable &config)
{
    yarp::os::Property cfg;
    cfg.fromString(config.toString());

    string configuration_to_open;
    string innerFilePath="config_xml/ftc_local_only.xml";
    auto fs = cmrc::frameTransformRC::get_filesystem();
    if(cfg.check("filexml_option")) { innerFilePath="config_xml/"+cfg.find("filexml_option").toString();}
    cfg.unput("filexml_option");
    auto xmlFile = fs.open(innerFilePath);
    for(const auto& lemma : xmlFile)
    {
        configuration_to_open += lemma;
    }

    string m_local_rpcUser = "/ftClient/rpc";
    if (cfg.check("local_rpc")) { m_local_rpcUser=cfg.find("local_rpc").toString();}
    cfg.unput("local_rpc");

    yarp::robotinterface::XMLReader reader;
    yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(configuration_to_open, cfg);
    yCAssert(FRAMETRANSFORMCLIENT, result.parsingIsSuccessful);

    m_robot = std::move(result.robot); // FIXME std::move non serve

    if (!result.robot.enterPhase(yarp::robotinterface::ActionPhaseStartup)) {
        return false;
    }

    if (!result.robot.enterPhase(yarp::robotinterface::ActionPhaseRun)) {
        return false;
    }

/*
    //probably useless, to be removed
    string setDevice = "foo";
    if (m_robot.hasParam("setDevice")) {setDevice = m_robot.findParam("setDevice");}
    yCAssert(FRAMETRANSFORMCLIENT, m_robot.hasDevice(setDevice));
    auto* polyset = m_robot.device(setDevice).driver();
    yCAssert(FRAMETRANSFORMCLIENT, polyset);
    polyset->view(m_ift_s);
    yCAssert(FRAMETRANSFORMCLIENT, m_ift_s);

    const string getDevice = "bar";
    string getDevice = "bar";
    if (robot.hasParam("getDevice")) {getDevice = robot.findParam("getDevice");}
    yCAssert(FRAMETRANSFORMCLIENT, m_robot.hasDevice(getDevice));
    auto* polyget = m_robot.device(getDevice).driver();
    yCAssert(FRAMETRANSFORMCLIENT, polyget);
    polyget->view(m_ift_g);
    yCAssert(FRAMETRANSFORMCLIENT, m_ift_g);
*/

    const string uDevice = "ftc_storage";
    yCAssert(FRAMETRANSFORMCLIENT, m_robot.hasDevice(uDevice));
    auto* polyu = m_robot.device(uDevice).driver();
    yCAssert(FRAMETRANSFORMCLIENT, polyu);
    polyu->view(m_ift_u);
    yCAssert(FRAMETRANSFORMCLIENT, m_ift_u);

/*
    if (config.check("period"))
    {
        m_period = config.find("period").asInt32() / 1000.0;
    }
    else
    {
        m_period = 0.010;
        yCWarning(FRAMETRANSFORMCLIENT, "Using default period of %f s" , m_period);
    }
*/


    if (!m_rpc_InterfaceToUser.open(m_local_rpcUser))
    {
        yCError(FRAMETRANSFORMCLIENT,"Failed to open rpc port");
    }

    m_rpc_InterfaceToUser.setReader(*this);

    return true;
}

bool FrameTransformClient::close()
{
    m_robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt1);
    m_robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown);
    m_rpc_InterfaceToUser.close();
    return true;
}

bool FrameTransformClient::allFramesAsString(string &all_frames)
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (br && p_cont)
    {
        for (auto it = p_cont->begin(); it != p_cont->end(); it++)
        all_frames += it->toString() + " ";
        return true;
    }
    return false;
}

FrameTransformClient::ConnectionType FrameTransformClient::priv_getConnectionType(const string &target_frame, const string &source_frame, string* commonAncestor = nullptr)
{
    if (target_frame == source_frame) {return IDENTITY;}

    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return DISCONNECTED; }

    size_t                     i, j;
    std::vector<string>   tar2root_vec;
    std::vector<string>   src2root_vec;
    string                ancestor, child;
    child = target_frame;
    std::lock_guard<std::recursive_mutex> l(p_cont->m_trf_mutex);
    while(getParent(child, ancestor))
    {
        if(ancestor == source_frame)
        {
            return DIRECT;
        }

        tar2root_vec.push_back(ancestor);
        child = ancestor;
    }
    child = source_frame;
    while(getParent(child, ancestor))
    {
        if(ancestor == target_frame)
        {
            return INVERSE;
        }

        src2root_vec.push_back(ancestor);
        child = ancestor;
    }

    for(i = 0; i < tar2root_vec.size(); i++)
    {
        string a;
        a = tar2root_vec[i];
        for(j = 0; j < src2root_vec.size(); j++)
        {
            if(a == src2root_vec[j])
            {
                if(commonAncestor)
                {
                    *commonAncestor = a;
                }
                return UNDIRECT;
            }
        }
    }

    return DISCONNECTED;
}

bool FrameTransformClient::canTransform(const string &target_frame, const string &source_frame)
{
    return priv_getConnectionType(target_frame, source_frame) != DISCONNECTED;
}

bool FrameTransformClient::clear()
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    p_cont->clear();
    return true;
}

bool FrameTransformClient::frameExists(const string &frame_id)
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        if (it->src_frame_id == frame_id) { return true; }
        if (it->dst_frame_id == frame_id) { return true; }
    }

    // condition reached if not found or container is empty
    return false;
}

bool FrameTransformClient::getAllFrameIds(std::vector< string > &ids)
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        bool found = false;
        for (const auto& id : ids)
        {
            if (it->src_frame_id == id) { found = true; break; }
        }
        if (found == false) ids.push_back(it->src_frame_id);
    }

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        bool found = false;
        for (const auto& id : ids)
        {
            if (it->dst_frame_id == id) { found = true; break; }
        }
        if (found == false) ids.push_back(it->dst_frame_id);
    }

    return true;
}

bool FrameTransformClient::getParent(const string &frame_id, string &parent_frame_id)
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        string par(it->dst_frame_id);
        if (it->dst_frame_id == frame_id)
        {

            parent_frame_id = it->src_frame_id;
            return true;
        }
    }
    return false;
}

bool FrameTransformClient::priv_canExplicitTransform(const string& target_frame_id, const string& source_frame_id) const
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (!br || p_cont==nullptr) {yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        if (it->dst_frame_id == target_frame_id && it->src_frame_id == source_frame_id)
        {
            return true;
        }
    }
    return false;
}

bool FrameTransformClient::priv_getChainedTransform(const string& target_frame_id, const string& source_frame_id, yarp::sig::Matrix& transform) const
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    std::lock_guard<std::recursive_mutex> l(p_cont->m_trf_mutex);

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        if (it->dst_frame_id == target_frame_id)
        {
            if (it->src_frame_id == source_frame_id)
            {
                transform = it->toMatrix();
                return true;
            }
            else
            {
                yarp::sig::Matrix m;
                if (priv_getChainedTransform(it->src_frame_id, source_frame_id, m))
                {
                    transform = m * it->toMatrix();
                    return true;
                }
            }
        }
    }
    return false;
}

bool FrameTransformClient::getTransform(const string& target_frame_id, const string& source_frame_id, yarp::sig::Matrix& transform)
{
    ConnectionType ct;
    string    ancestor;
    ct = priv_getConnectionType(target_frame_id, source_frame_id, &ancestor);
    if (ct == DIRECT)
    {
        return priv_getChainedTransform(target_frame_id, source_frame_id, transform);
    }
    else if (ct == INVERSE)
    {
        yarp::sig::Matrix m(4, 4);
        priv_getChainedTransform(source_frame_id, target_frame_id, m);
        transform = yarp::math::SE3inv(m);
        return true;
    }
    else if(ct == UNDIRECT)
    {
        yarp::sig::Matrix root2tar(4, 4), root2src(4, 4);
        priv_getChainedTransform(source_frame_id, ancestor, root2src);
        priv_getChainedTransform(target_frame_id, ancestor, root2tar);
        transform = yarp::math::SE3inv(root2src) * root2tar;
        return true;
    }
    else if (ct == IDENTITY)
    {
        yarp::sig::Matrix tmp(4, 4); tmp.eye();
        transform = tmp;
        return true;
    }

    yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "getTransform(): Frames " << source_frame_id << " and " << target_frame_id << " are not connected";
    return false;
}

bool FrameTransformClient::setTransform(const string& target_frame_id, const string& source_frame_id, const yarp::sig::Matrix& transform)
{
    if(target_frame_id == source_frame_id)
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "setTransform(): Invalid transform detected.\n" \
                    "\t Source frame and target frame are both equal to " << source_frame_id;
        return false;
    }

    if (!priv_canExplicitTransform(target_frame_id, source_frame_id) && canTransform(target_frame_id, source_frame_id))
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "setTransform(): Such transform already exist by chaining transforms";
        return false;
    }

    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    FrameTransform   tf;

    if (!tf.fromMatrix(transform))
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "setTransform(): Wrong matrix format, it has to be 4 by 4";
        return false;
    }

    tf.src_frame_id = source_frame_id;
    tf.dst_frame_id = target_frame_id;
    tf.isStatic = false;
    tf.timestamp = yarp::os::Time::now();

    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (br && p_cont) { br = p_cont->setTransform(tf); }
    return br;
}

bool FrameTransformClient::setTransformStatic(const string &target_frame_id, const string &source_frame_id, const yarp::sig::Matrix &transform)
{
    if(target_frame_id == source_frame_id)
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "setTransformStatic(): Invalid transform detected.\n" \
                    "\t Source frame and target frame are both equal to " << source_frame_id;
        return false;
    }

    if (canTransform(target_frame_id, source_frame_id))
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "setTransform(): Such static transform already exist, directly or by chaining transforms";
        return false;
    }

    FrameTransform   tf;
    if (!tf.fromMatrix(transform))
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "setTransform(): Wrong matrix format, it has to be 4 by 4";
        return false;
    }
    tf.src_frame_id = source_frame_id;
    tf.dst_frame_id = target_frame_id;
    tf.isStatic = true;
    tf.timestamp=-1;

    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_u->getInternalContainer(p_cont);
    if (br && p_cont) { br = p_cont->setTransform(tf); }
    return br;
}

bool FrameTransformClient::deleteTransform(const string &target_frame_id, const string &source_frame_id)
{
    /*yarp::os::Bottle b;
    yarp::os::Bottle resp;
    b.addVocab32(VOCAB_ITRANSFORM);
    b.addVocab32(VOCAB_TRANSFORM_DELETE);
    b.addString(target_frame_id);
    b.addString(source_frame_id);
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32()!=VOCAB_OK)
        {
            yCError(FRAMETRANSFORMCLIENT) << "Received error from server on deleting frame between "+source_frame_id+" and "+target_frame_id;
            return false;
        }
    }
    else
    {
        yCError(FRAMETRANSFORMCLIENT) << "deleteFrame(): Error on writing on rpc port";
        return false;
    }
    return true;*/

    yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "deleteFrame() Not yet implemented";
    return false;
}

bool FrameTransformClient::transformPoint(const string &target_frame_id, const string &source_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point)
{
    if (input_point.size() != 3)
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "Only 3 dimensional vector allowed.";
        return false;
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "No transform found between source '" << target_frame_id << "' and target '" << source_frame_id << "'";
        return false;
    }
    yarp::sig::Vector in = input_point;
    in.push_back(1);
    transformed_point = m * in;
    transformed_point.pop_back();
    return true;
}

bool FrameTransformClient::transformPose(const string &target_frame_id, const string &source_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose)
{
    if (input_pose.size() != 6)
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "Only 6 dimensional vector (3 axes + roll pith and yaw) allowed.";
        return false;
    }
    if (transformed_pose.size() != 6)
    {
        yCWarning(FRAMETRANSFORMCLIENT, "transformPose(): Performance warning: size transformed_pose should be 6, resizing.");
        transformed_pose.resize(6, 0.0);
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "No transform found between source '" << target_frame_id << "' and target '" << source_frame_id << "'";
        return false;
    }
    FrameTransform t;
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

bool FrameTransformClient::transformQuaternion(const string &target_frame_id, const string &source_frame_id, const yarp::math::Quaternion &input_quaternion, yarp::math::Quaternion &transformed_quaternion)
{
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "No transform found between source '" << target_frame_id << "' and target '" << source_frame_id <<"'";
        return false;
    }
    FrameTransform t;
    t.rotation=input_quaternion;
    transformed_quaternion.fromRotationMatrix(m * t.toMatrix());
    return true;
}

bool FrameTransformClient::waitForTransform(const string &target_frame_id, const string &source_frame_id, const double &timeout)
{
    //loop until canTransform == true or timeout expires
    double start = yarp::os::SystemClock::nowSystem();
    while (!canTransform(target_frame_id, source_frame_id))
    {
        if (yarp::os::SystemClock::nowSystem() - start > timeout)
        {
            yCError(FRAMETRANSFORMCLIENT) << "waitForTransform(): timeout expired";
            return false;
        }
        yarp::os::SystemClock::delaySystem(0.001);
    }
    return true;
}

FrameTransformClient::FrameTransformClient() : PeriodicThread(0.01),
    m_period(0.01)
{
}

FrameTransformClient::~FrameTransformClient() = default;

bool     FrameTransformClient::threadInit()
{
    yCTrace(FRAMETRANSFORMCLIENT, "Thread started");
    return true;
}

void     FrameTransformClient::threadRelease()
{
    yCTrace(FRAMETRANSFORMCLIENT, "Thread stopped");
}

void     FrameTransformClient::run()
{
    std::lock_guard<std::mutex> lock (m_rpc_mutex);
    if (m_array_of_ports.size()==0)
    {
        return;
    }

    for (auto& m_array_of_port : m_array_of_ports)
    {
        if (m_array_of_port)
        {
            string src = m_array_of_port->transform_src;
            string dst = m_array_of_port->transform_dst;
            yarp::sig::Matrix m;
            this->getTransform(src, dst, m);
            if (m_array_of_port->format == broadcast_port_t::format_t::matrix)
            {
                m_array_of_port->port.write(m);
            }
            else
            {
                yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "Unknown format requested: " << m_array_of_port->format;
            }
        }
    }
}
