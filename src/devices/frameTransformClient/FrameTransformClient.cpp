/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include "FrameTransformClient.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/math/Math.h>

#include <cmrc/cmrc.hpp>
#include <mutex>
CMRC_DECLARE(frameTransformRC);

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
    if (!ok) {
        return false;
    }

    string request = in.get(0).asString();
    if (request == "help")
    {
        out.addVocab32("many");
        out.addString("get_transform <src> <dst>: print the transform from <src> to <dst>");
        out.addString("list_transforms: print all the stored frame transforms");
        out.addString("list_frames: print all the available reference frames");
        out.addString("list_ports: print all the opened ports for transform broadcasting");
        out.addString("publish_transform <src> <dst> <portname> [format]: opens a port to publish transform from src to dst");
        out.addString("unpublish_transform <portname>: closes a previously opened port to publish a transform");
        out.addString("unpublish_all: closes a all previously opened ports to publish a transform");
        out.addString("set_static_transform_rad <src> <dst> <x> <y> <z> <roll> <pitch> <yaw>': create a static transform (angles in radians)");
        out.addString("set_static_transform_deg <src> <dst> <x> <y> <z> <roll> <pitch> <yaw>': create a static transform (angles in degrees)");
        out.addString("delete_static_transform <src> <dst>': delete a static transform");
        out.addString("generate_view <option>': generate a frames.pdf file showing the transform tree diagram.");
        out.addString("     The following values are valid for option (default=none)");
        out.addString("    'show_rpy': show rotation as rpy angles");
        out.addString("    'show_quaterion:'show rotation as a quaternion");
        out.addString("    'show_matrix:'show rotation as a 3x3 rotation matrix");
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

        if (port_name[0] == '/') {
            port_name.erase(port_name.begin());
        }
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
                if (m_array_of_ports.size() == 1) {
                    this->start();
                }
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
        if (m_array_of_ports.size() == 0) {
            this->askToStop();
        }
        out.addString("Operation complete");
    }
    else if (request == "unpublish_transform")
    {
        bool ret = false;
        string port_name = in.get(1).asString();
        if (port_name[0] == '/') {
            port_name.erase(port_name.begin());
        }
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
        if (m_array_of_ports.size() == 0) {
            this->askToStop();
        }
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
        {
            t.rotFromRPY(in.get(6).asFloat64(),
                         in.get(7).asFloat64(),
                         in.get(8).asFloat64());
        }
        else if (request == "set_static_transform_deg")
        {
            t.rotFromRPY(in.get(6).asFloat64() * 180 / M_PI,
                         in.get(7).asFloat64() * 180 / M_PI,
                         in.get(8).asFloat64() * 180 / M_PI);
        }
        t.timestamp = yarp::os::Time::now();
        bool ret = this->setTransformStatic(t.src_frame_id,t.dst_frame_id,t.toMatrix());
        if (ret == true)
        {
            yCInfo(FRAMETRANSFORMCLIENT) << "set_static_transform done";
            out.addString("set_static_transform done");
        }
        else
        {
            yCError(FRAMETRANSFORMCLIENT) << "read(): something strange happened";
            out.addString("error");
        }
    }
    else if (request == "generate_view")
    {
        m_show_transforms_in_diagram = show_transforms_in_diagram_t::do_not_show;
        if (in.get(1).asString() == "show_quaternion") {
            m_show_transforms_in_diagram = show_transforms_in_diagram_t::show_quaternion;
        } else if (in.get(1).asString() == "show_matrix") {
            m_show_transforms_in_diagram = show_transforms_in_diagram_t::show_matrix;
        } else if (in.get(1).asString() == "show_rpy") {
            m_show_transforms_in_diagram = show_transforms_in_diagram_t::show_rpy;
        }
        priv_generate_view();
        out.addString("ok");
    }
    else if (request == "delete_static_transform")
    {
        std::string src = in.get(1).asString();
        std::string dst = in.get(2).asString();
        //@@@check if the transform is static?
        this->deleteTransform(src,dst);
        out.addString("delete_static_transform done");
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
    yCWarning(FRAMETRANSFORMCLIENT) << "The 'FrameTransformClient' device is experimental and could be modified without any warning";

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

    m_robot = std::move(result.robot);

    if (!m_robot.enterPhase(yarp::robotinterface::ActionPhaseStartup)) {
        return false;
    }

    if (!m_robot.enterPhase(yarp::robotinterface::ActionPhaseRun)) {
        return false;
    }

    string setdeviceName = "ftc_storage";
    if (m_robot.hasParam("setDeviceName")) { setdeviceName = m_robot.findParam("setDeviceName");}
    yCAssert(FRAMETRANSFORMCLIENT, m_robot.hasDevice(setdeviceName));
    auto* polyset = m_robot.device(setdeviceName).driver();
    yCAssert(FRAMETRANSFORMCLIENT, polyset);
    polyset->view(m_ift_set);
    yCAssert(FRAMETRANSFORMCLIENT, m_ift_set);

    string getdeviceName = "ftc_storage";
    if (m_robot.hasParam("getDeviceName")) {getdeviceName = m_robot.findParam("getDeviceName");}
    yCAssert(FRAMETRANSFORMCLIENT, m_robot.hasDevice(getdeviceName));
    auto* polyget = m_robot.device(getdeviceName).driver();
    yCAssert(FRAMETRANSFORMCLIENT, polyget);
    polyget->view(m_ift_get);
    yCAssert(FRAMETRANSFORMCLIENT, m_ift_get);
    polyget->view(m_ift_util);
    yCAssert(FRAMETRANSFORMCLIENT, m_ift_util);

    if (config.check("period"))
    {
        m_period = config.find("period").asFloat64();
        this->setPeriod(m_period);
    }

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
    bool br = m_ift_util->getInternalContainer(p_cont);
    if (br && p_cont)
    {
        for (auto it = p_cont->begin(); it != p_cont->end(); it++) {
            all_frames += it->toString() + " ";
        }
        return true;
    }
    return false;
}

FrameTransformClient::ConnectionType FrameTransformClient::priv_getConnectionType(const string &target_frame, const string &source_frame, string* commonAncestor = nullptr)
{
    if (target_frame == source_frame) {return ConnectionType::IDENTITY;}

    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_util->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return ConnectionType::DISCONNECTED; }

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
            return ConnectionType::DIRECT;
        }

        tar2root_vec.push_back(ancestor);
        child = ancestor;
    }
    child = source_frame;
    while(getParent(child, ancestor))
    {
        if(ancestor == target_frame)
        {
            return ConnectionType::INVERSE;
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
                return ConnectionType::UNDIRECT;
            }
        }
    }

    return ConnectionType::DISCONNECTED;
}

bool FrameTransformClient::canTransform(const string &target_frame, const string &source_frame)
{
    return priv_getConnectionType(target_frame, source_frame) != ConnectionType::DISCONNECTED;
}

bool FrameTransformClient::clear()
{
    if (m_ift_set)
    {
        return m_ift_set->clearAll();
    }
    yCError(FRAMETRANSFORMCLIENT, "clear(): interface not available");
    return false;
}

bool FrameTransformClient::frameExists(const string &frame_id)
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_util->getInternalContainer(p_cont);
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
    bool br = m_ift_util->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        bool found = false;
        for (const auto& id : ids)
        {
            if (it->src_frame_id == id) { found = true; break; }
        }
        if (found == false) {
            ids.push_back(it->src_frame_id);
        }
    }

    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        bool found = false;
        for (const auto& id : ids)
        {
            if (it->dst_frame_id == id) { found = true; break; }
        }
        if (found == false) {
            ids.push_back(it->dst_frame_id);
        }
    }

    return true;
}

bool FrameTransformClient::getParent(const string &frame_id, string &parent_frame_id)
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_util->getInternalContainer(p_cont);
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
    bool br = m_ift_util->getInternalContainer(p_cont);
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
    bool br = m_ift_util->getInternalContainer(p_cont);
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
    if (ct == ConnectionType::DIRECT)
    {
        return priv_getChainedTransform(target_frame_id, source_frame_id, transform);
    }
    else if (ct == ConnectionType::INVERSE)
    {
        yarp::sig::Matrix m(4, 4);
        priv_getChainedTransform(source_frame_id, target_frame_id, m);
        transform = yarp::math::SE3inv(m);
        return true;
    }
    else if(ct == ConnectionType::UNDIRECT)
    {
        yarp::sig::Matrix root2tar(4, 4), root2src(4, 4);
        priv_getChainedTransform(source_frame_id, ancestor, root2src);
        priv_getChainedTransform(target_frame_id, ancestor, root2tar);
        transform = yarp::math::SE3inv(root2src) * root2tar;
        return true;
    }
    else if (ct == ConnectionType::IDENTITY)
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

    if (m_ift_set)
    {
        return m_ift_set->setTransform(tf);
    }
    yCError(FRAMETRANSFORMCLIENT, "setTransform(): interface not available");
    return false;
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

    if (m_ift_set)
    {
        return m_ift_set->setTransform(tf);
    }
    yCError(FRAMETRANSFORMCLIENT, "setTransformStatic(): interface not available");
    return false;
}

bool FrameTransformClient::deleteTransform(const string &target_frame_id, const string &source_frame_id)
{
    if (m_ift_set)
    {
        return m_ift_set->deleteTransform(target_frame_id, source_frame_id);
    }
    yCError(FRAMETRANSFORMCLIENT, "deleteTransform(): interface not available");
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
                yCErrorThrottle(FRAMETRANSFORMCLIENT, LOG_THROTTLE_PERIOD) << "Unknown format requested: " << (int)(m_array_of_port->format);
            }
        }
    }
}

string FrameTransformClient::priv_get_matrix_as_text(FrameTransform* t)
{
    if (t == nullptr) {
        return "";
    }

    if (m_show_transforms_in_diagram == do_not_show)
    {
        return "";
    }
    else if (m_show_transforms_in_diagram == show_quaternion)
    {
        return string(",label=\" ") + t->toString(FrameTransform::display_transform_mode_t::rotation_as_quaternion) + "\"";
    }
    else if (m_show_transforms_in_diagram == show_matrix)
    {
        return string(",label=\" ") + t->toString(FrameTransform::display_transform_mode_t::rotation_as_matrix) + "\"";
    }
    else if (m_show_transforms_in_diagram == show_rpy)
    {
        return string(",label=\" ") + t->toString(FrameTransform::display_transform_mode_t::rotation_as_rpy) + "\"";
    }

    yCError(FRAMETRANSFORMCLIENT) << "get_matrix_as_text() invalid option";
    return "";
    /*
        //this is a test to use Latek display
        string s = "\\begin{ bmatrix } \
        1 & 2 & 3\\ \
        a & b & c \
        \\end{ bmatrix }";
    */
}


bool FrameTransformClient::priv_generate_view()
{
    FrameTransformContainer* p_cont = nullptr;
    bool br = m_ift_util->getInternalContainer(p_cont);
    if (!br || p_cont == nullptr) { yCError(FRAMETRANSFORMCLIENT) << "Failure"; return false; }

    string dot_string = "digraph G { ";
    for (auto it = p_cont->begin(); it != p_cont->end(); it++)
    {
        FrameTransform t = *it;
        string edge_text = priv_get_matrix_as_text(&t);
        string trf_text = it->src_frame_id + "->" +
            it->dst_frame_id + " " +
            "[color = black]";
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
            </table>>]\n\
          key2[label=<<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\" cellborder=\"0\">\n\
            <tr><td port = \"i1\">&nbsp;</td></tr>\n\
            <tr><td port = \"i2\">&nbsp;</td></tr>\n\
            </table>>]\n\
          key:i1:e -> key2:i1:w [color = blue]\n\
          key:i2:e -> key2:i2:w [color = blue, style=dashed]\n\
        } }";

    string command_string = "printf '" + dot_string + legend + "' | dot -Tpdf > frames.pdf";
#if defined (__linux__)
    int ret = std::system("dot -V");
    if (ret != 0)
    {
        yCError(FRAMETRANSFORMCLIENT) << "dot executable not found. Please install graphviz.";
        return false;
    }

    yCDebug(FRAMETRANSFORMCLIENT) << "Command string is:" << command_string;
    ret = std::system(command_string.c_str());
    if (ret != 0)
    {
        yCError(FRAMETRANSFORMCLIENT) << "The following command failed to execute:" << command_string;
        return false;
    }
#else
    yCError(FRAMETRANSFORMCLIENT) << "Not yet implemented. Available only Linux";
    return false;
#endif
    return true;
}
