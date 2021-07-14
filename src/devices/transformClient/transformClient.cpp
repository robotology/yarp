/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "transformClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Math.h>
#include <mutex>

/*! \file transformClient.cpp */

//example: yarpdev --device transformClient --local /transformClient --remote /transformServer

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;


namespace {
YARP_LOG_COMPONENT(TRANSFORMCLIENT, "yarp.device.transformClient")
}

inline void Transforms_client_storage::resetStat()
{
    std::lock_guard<std::recursive_mutex> l(m_mutex);
}

void Transforms_client_storage::onRead(yarp::os::Bottle &b)
{
    m_now = Time::now();
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    if (m_count>0)
    {
        double tmpDT = m_now - m_prev;
        m_deltaT += tmpDT;
        if (tmpDT > m_deltaTMax) {
            m_deltaTMax = tmpDT;
        }
        if (tmpDT < m_deltaTMin) {
            m_deltaTMin = tmpDT;
        }

        //compare network time
        /*if (tmpDT*1000<TRANSFORM_TIMEOUT)
        {
            state = b.get(5).asInt32();
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
   // if ((1000 * (newStamp.getTime() - m_lastStamp.getTime()))<TRANSFORM_TIMEOUT)
    if (true)
    {
        m_state = IFrameTransform::TRANSFORM_OK;

        m_transforms.clear();
        int bsize= b.size();
        for (int i = 0; i < bsize; i++)
        {
            //this includes: timed yarp transforms, static yarp transforms, ros transforms
            Bottle* bt = b.get(i).asList();
            if (bt != nullptr)
            {
                FrameTransform t;
                t.src_frame_id = bt->get(0).asString();
                t.dst_frame_id = bt->get(1).asString();
                t.timestamp = bt->get(2).asFloat64();
                t.translation.tX = bt->get(3).asFloat64();
                t.translation.tY = bt->get(4).asFloat64();
                t.translation.tZ = bt->get(5).asFloat64();
                t.rotation.w() = bt->get(6).asFloat64();
                t.rotation.x() = bt->get(7).asFloat64();
                t.rotation.y() = bt->get(8).asFloat64();
                t.rotation.z() = bt->get(9).asFloat64();
                m_transforms.push_back(t);
            }
        }
    }
    else
    {
        m_state = IFrameTransform::TRANSFORM_TIMEOUT;
    }
    m_lastStamp = newStamp;
}

inline int Transforms_client_storage::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    int ret = m_state;
    if (ret != IFrameTransform::TRANSFORM_GENERAL_ERROR)
    {
        data = m_lastBottle;
        stmp = m_lastStamp;
    }

    return ret;
}

inline int Transforms_client_storage::getIterations()
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);
    int ret = m_count;
    return ret;
}

// time is in ms
void Transforms_client_storage::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);
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

void Transforms_client_storage::clear()
{
    std::lock_guard<std::recursive_mutex> l(m_mutex);
    m_transforms.clear();
}

Transforms_client_storage::Transforms_client_storage(std::string local_streaming_name)
{
    m_count = 0;
    m_deltaT = 0;
    m_deltaTMax = 0;
    m_deltaTMin = 1e22;
    m_now = Time::now();
    m_prev = m_now;

    if (!this->open(local_streaming_name))
    {
        yCError(TRANSFORMCLIENT, "open(): Could not open port %s, check network", local_streaming_name.c_str());
    }
    this->useCallback();
}

Transforms_client_storage::~Transforms_client_storage()
{
    this->interrupt();
    this->close();
}

size_t   Transforms_client_storage::size()
{
    std::lock_guard<std::recursive_mutex> l(m_mutex);
    return m_transforms.size();
}

yarp::math::FrameTransform& Transforms_client_storage::operator[]   (std::size_t idx)
{
    std::lock_guard<std::recursive_mutex> l(m_mutex);
    return m_transforms[idx];
};

//------------------------------------------------------------------------------------------------------------------------------
bool TransformClient::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> lock (m_rpc_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) {
        return false;
    }

    std::string request = in.get(0).asString();
    if (request == "help")
    {
        out.addVocab32("many");
        out.addString("'get_transform <src> <dst>: print the transform from <src> to <dst>");
        out.addString("'list_frames: print all the available reference frames");
        out.addString("'list_ports: print all the opened ports for transform broadcasting");
        out.addString("'publish_transform <src> <dst> <portname> <format>: opens a port to publish transform from src to dst");
        out.addString("'unpublish_transform <portname>: closes a previously opened port to publish a transform");
        out.addString("'unpublish_all <portname>: closes a all previously opened ports to publish a transform");
        out.addString("'is_connected'");
        out.addString("'reconnect'");
    }
    else if (request == "is_connected")
    {
        if (isConnectedWithServer())
        {
            out.addString("yes");
        }
        else
        {
            out.addString("no");
        }
    }
    else if (request == "reconnect")
    {
        if (reconnectWithServer())
        {
            out.addString("successful");
        }
        else
        {
            out.addString("unsuccessful");
        }
    }
    else if (request == "list_frames")
    {
        std::vector<std::string> v;
        this->getAllFrameIds(v);
        out.addVocab32("many");
        out.addString("List of available reference frames:");
        int count = 0;
        for (auto& vec : v)
        {
            count++;
            std::string str = std::to_string(count) + "- " + vec;
            out.addString(str.c_str());
        }
    }
    else if (request == "get_transform")
    {
        std::string src = in.get(1).asString();
        std::string dst = in.get(2).asString();
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
                std::string  s = m_array_of_port->port.getName() + " "+ m_array_of_port->transform_src +  " -> " + m_array_of_port->transform_dst;
                out.addString(s);
            }
        }
    }
    else if (request == "publish_transform")
    {
        out.addVocab32("many");
        std::string src  = in.get(1).asString();
        std::string dst  = in.get(2).asString();
        std::string port_name = in.get(3).asString();
        std::string format = "matrix";
        if (in.size() > 4)
            {format= in.get(4).asString();}
        if (port_name[0] == '/') {
            port_name.erase(port_name.begin());
        }
        std::string full_port_name = m_local_name + "/" + port_name;
        bool ret = true;
        for (auto& m_array_of_port : m_array_of_ports)
        {
            if (m_array_of_port && m_array_of_port->port.getName() == full_port_name)
            {
                ret = false;
                break;
            }
        }
        if (this->frameExists(src)==false)
        {
            out.addString("Requested src frame " + src + " does not exists.");
            yCWarning(TRANSFORMCLIENT, "Requested src frame %s does not exists.", src.c_str());
        }
        if (this->frameExists(dst)==false)
        {
            out.addString("Requested dst frame " + dst + " does not exists.");
            yCWarning(TRANSFORMCLIENT, "Requested fst frame %s does not exists.", dst.c_str());
        }
        if (ret == true)
        {
            auto* b = new broadcast_port_t;
            b->transform_src = src;
            b->transform_dst = dst;
            b->format = format;
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
        std::string port_name = in.get(1).asString();
        if (port_name[0] == '/') {
            port_name.erase(port_name.begin());
        }
        std::string full_port_name = m_local_name + "/" + port_name;
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
    else
    {
        yCError(TRANSFORMCLIENT, "Invalid vocab received in TransformClient");
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
        yCError(TRANSFORMCLIENT) << "Invalid return to sender";
    }
    return true;
}

bool TransformClient::open(yarp::os::Searchable &config)
{
    yCWarning(TRANSFORMCLIENT) << "The 'transformClient' device is deprecated in favour of 'frameTransformClient'.";
    yCWarning(TRANSFORMCLIENT) << "The old device is no longer supported, and it will be deprecated in YARP 3.6 and removed in YARP 4.";
    yCWarning(TRANSFORMCLIENT) << "Please update your scripts.";

    m_local_name.clear();
    m_remote_name.clear();

    m_local_name  = config.find("local").asString();
    m_remote_name = config.find("remote").asString();
    m_streaming_connection_type = "udp";

    if (m_local_name == "")
    {
        yCError(TRANSFORMCLIENT, "open(): Invalid local name");
        return false;
    }
    if (m_remote_name == "")
    {
        yCError(TRANSFORMCLIENT, "open(): Invalid remote name");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt32() / 1000.0;
    }
    else
    {
        m_period = 0.010;
        yCWarning(TRANSFORMCLIENT, "Using default period of %f s" , m_period);
    }

    m_local_rpcServer = m_local_name + "/rpc:o";
    m_local_rpcUser = m_local_name + "/rpc:i";
    m_remote_rpc = m_remote_name + "/rpc";
    m_remote_streaming_name = m_remote_name + "/transforms:o";
    m_local_streaming_name = m_local_name + "/transforms:i";

    if (!m_rpc_InterfaceToUser.open(m_local_rpcUser))
    {
        yCError(TRANSFORMCLIENT, "open(): Could not open rpc port %s, check network", m_local_rpcUser.c_str());
        return false;
    }

    if (!m_rpc_InterfaceToServer.open(m_local_rpcServer))
    {
        yCError(TRANSFORMCLIENT, "open(): Could not open rpc port %s, check network", m_local_rpcServer.c_str());
        return false;
    }

    m_transform_storage = new Transforms_client_storage(m_local_streaming_name);
    bool ok = Network::connect(m_remote_streaming_name.c_str(), m_local_streaming_name.c_str(), m_streaming_connection_type.c_str());
    if (!ok)
    {
        yCError(TRANSFORMCLIENT, "open(): Could not connect to %s", m_remote_streaming_name.c_str());
        return false;
    }

    ok = Network::connect(m_local_rpcServer, m_remote_rpc);
    if (!ok)
    {
        yCError(TRANSFORMCLIENT, "open(): Could not connect to %s", m_remote_rpc.c_str());
        return false;
    }


    m_rpc_InterfaceToUser.setReader(*this);
    return true;
}

bool TransformClient::close()
{
    m_rpc_InterfaceToServer.close();
    m_rpc_InterfaceToUser.close();
    if (m_transform_storage != nullptr)
    {
        delete m_transform_storage;
        m_transform_storage = nullptr;
    }
    return true;
}

bool TransformClient::allFramesAsString(std::string &all_frames)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        all_frames += (*m_transform_storage)[i].toString() + " ";
    }
    return true;
}

TransformClient::ConnectionType TransformClient::getConnectionType(const std::string &target_frame, const std::string &source_frame, std::string* commonAncestor = nullptr)
{
    if (target_frame == source_frame) {return IDENTITY;}

    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t                     i, j;
    std::vector<std::string>   tar2root_vec;
    std::vector<std::string>   src2root_vec;
    std::string                ancestor, child;
    child = target_frame;
    std::lock_guard<std::recursive_mutex> l(tfVec.m_mutex);
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
        std::string a;
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

bool TransformClient::canTransform(const std::string &target_frame, const std::string &source_frame)
{
    return getConnectionType(target_frame, source_frame) != DISCONNECTED;
}

bool TransformClient::clear()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    b.addVocab32(VOCAB_ITRANSFORM);
    b.addVocab32(VOCAB_TRANSFORM_DELETE_ALL);
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(TRANSFORMCLIENT) << "clear(): Received error from server";
            return false;
        }
    }
    else
    {
        yCError(TRANSFORMCLIENT) << "clear(): Error on writing on rpc port";
        return false;
    }

    m_transform_storage->clear();
    return true;
}

bool TransformClient::frameExists(const std::string &frame_id)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        if (((*m_transform_storage)[i].src_frame_id) == frame_id) { return true; }
        if (((*m_transform_storage)[i].dst_frame_id) == frame_id) { return true; }
    }
    return false;
}

bool TransformClient::getAllFrameIds(std::vector< std::string > &ids)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        bool found = false;
        for (const auto& id : ids)
        {
            if (((*m_transform_storage)[i].src_frame_id) == id) { found = true; break; }
        }
        if (found == false) {
            ids.push_back((*m_transform_storage)[i].src_frame_id);
        }
    }

    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        bool found = false;
        for (const auto& id : ids)
        {
            if (((*m_transform_storage)[i].dst_frame_id) == id) { found = true; break; }
        }
        if (found == false) {
            ids.push_back((*m_transform_storage)[i].dst_frame_id);
        }
    }

    return true;
}

bool TransformClient::getParent(const std::string &frame_id, std::string &parent_frame_id)
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

bool TransformClient::canExplicitTransform(const std::string& target_frame_id, const std::string& source_frame_id) const
{
    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t                     i, tfVec_size;
    std::lock_guard<std::recursive_mutex>         l(tfVec.m_mutex);

    tfVec_size = tfVec.size();
    for (i = 0; i < tfVec_size; i++)
    {
        if (tfVec[i].dst_frame_id == target_frame_id && tfVec[i].src_frame_id == source_frame_id)
        {
            return true;
        }
    }
    return false;
}

bool TransformClient::getChainedTransform(const std::string& target_frame_id, const std::string& source_frame_id, yarp::sig::Matrix& transform) const
{
    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t                     i, tfVec_size;
    std::lock_guard<std::recursive_mutex>         l(tfVec.m_mutex);

    tfVec_size = tfVec.size();
    for (i = 0; i < tfVec_size; i++)
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
                if (getChainedTransform(tfVec[i].src_frame_id, source_frame_id, m))
                {
                    transform = m * tfVec[i].toMatrix();
                    return true;
                }
            }
        }
    }
    return false;
}

bool TransformClient::getTransform(const std::string& target_frame_id, const std::string& source_frame_id, yarp::sig::Matrix& transform)
{
    ConnectionType ct;
    std::string    ancestor;
    ct = getConnectionType(target_frame_id, source_frame_id, &ancestor);
    if (ct == DIRECT)
    {
        return getChainedTransform(target_frame_id, source_frame_id, transform);
    }
    else if (ct == INVERSE)
    {
        yarp::sig::Matrix m(4, 4);
        getChainedTransform(source_frame_id, target_frame_id, m);
        transform = yarp::math::SE3inv(m);
        return true;
    }
    else if(ct == UNDIRECT)
    {
        yarp::sig::Matrix root2tar(4, 4), root2src(4, 4);
        getChainedTransform(source_frame_id, ancestor, root2src);
        getChainedTransform(target_frame_id, ancestor, root2tar);
        transform = yarp::math::SE3inv(root2src) * root2tar;
        return true;
    }
    else if (ct == IDENTITY)
    {
        yarp::sig::Matrix tmp(4, 4); tmp.eye();
        transform = tmp;
        return true;
    }

    yCError(TRANSFORMCLIENT) << "getTransform(): Frames " << source_frame_id << " and " << target_frame_id << " are not connected";
    return false;
}

bool TransformClient::setTransform(const std::string& target_frame_id, const std::string& source_frame_id, const yarp::sig::Matrix& transform)
{
    if(target_frame_id == source_frame_id)
    {
        yCError(TRANSFORMCLIENT) << "setTransform(): Invalid transform detected.\n" \
                    "\t Source frame and target frame are both equal to " << source_frame_id;
        return false;
    }

    if (!canExplicitTransform(target_frame_id, source_frame_id) && canTransform(target_frame_id, source_frame_id))
    {
        yCError(TRANSFORMCLIENT) << "setTransform(): Such transform already exist by chaining transforms";
        return false;
    }

    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    FrameTransform   tf;

    if (!tf.fromMatrix(transform))
    {
        yCError(TRANSFORMCLIENT) << "setTransform(): Wrong matrix format, it has to be 4 by 4";
        return false;
    }

    b.addVocab32(VOCAB_ITRANSFORM);
    b.addVocab32(VOCAB_TRANSFORM_SET);
    b.addString(source_frame_id);
    b.addString(target_frame_id);
    b.addFloat64(1000.0); //transform lifetime
    b.addFloat64(tf.translation.tX);
    b.addFloat64(tf.translation.tY);
    b.addFloat64(tf.translation.tZ);
    b.addFloat64(tf.rotation.w());
    b.addFloat64(tf.rotation.x());
    b.addFloat64(tf.rotation.y());
    b.addFloat64(tf.rotation.z());
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(TRANSFORMCLIENT) << "setTransform(): Received error from server on creating frame between " + source_frame_id + " and " + target_frame_id;
            return false;
        }
    }
    else
    {
        yCError(TRANSFORMCLIENT) << "setTransform(): Error on writing on rpc port";
        return false;
    }
    return true;
}

bool TransformClient::setTransformStatic(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform)
{
    if(target_frame_id == source_frame_id)
    {
        yCError(TRANSFORMCLIENT) << "setTransformStatic(): Invalid transform detected.\n" \
                    "\t Source frame and target frame are both equal to " << source_frame_id;
        return false;
    }

    if (canTransform(target_frame_id, source_frame_id))
    {
        yCError(TRANSFORMCLIENT) << "setTransform(): Such static transform already exist, directly or by chaining transforms";
        return false;
    }

    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    FrameTransform   tf;

    if (!tf.fromMatrix(transform))
    {
        yCError(TRANSFORMCLIENT) << "setTransform(): Wrong matrix format, it has to be 4 by 4";
        return false;
    }

    b.addVocab32(VOCAB_ITRANSFORM);
    b.addVocab32(VOCAB_TRANSFORM_SET);
    b.addString(source_frame_id);
    b.addString(target_frame_id);
    b.addFloat64(-1);
    b.addFloat64(tf.translation.tX);
    b.addFloat64(tf.translation.tY);
    b.addFloat64(tf.translation.tZ);
    b.addFloat64(tf.rotation.w());
    b.addFloat64(tf.rotation.x());
    b.addFloat64(tf.rotation.y());
    b.addFloat64(tf.rotation.z());
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(TRANSFORMCLIENT) << "setTransform(): Received error from server on creating frame between " + source_frame_id + " and " + target_frame_id;
            return false;
        }
    }
    else
    {
        yCError(TRANSFORMCLIENT) << "setTransform(): Error on writing on rpc port";
        return false;
    }
    return true;
}

bool TransformClient::deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)
{
    yarp::os::Bottle b;
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
            yCError(TRANSFORMCLIENT) << "Received error from server on deleting frame between "+source_frame_id+" and "+target_frame_id;
            return false;
        }
    }
    else
    {
        yCError(TRANSFORMCLIENT) << "deleteFrame(): Error on writing on rpc port";
        return false;
    }
    return true;
}

bool TransformClient::transformPoint(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point)
{
    if (input_point.size() != 3)
    {
        yCError(TRANSFORMCLIENT) << "Only 3 dimensional vector allowed.";
        return false;
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yCError(TRANSFORMCLIENT) << "No transform found between source '" << target_frame_id << "' and target '" << source_frame_id << "'";
        return false;
    }
    yarp::sig::Vector in = input_point;
    in.push_back(1);
    transformed_point = m * in;
    transformed_point.pop_back();
    return true;
}

bool TransformClient::transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose)
{
    if (input_pose.size() != 6)
    {
        yCError(TRANSFORMCLIENT) << "Only 6 dimensional vector (3 axes + roll pith and yaw) allowed.";
        return false;
    }
    if (transformed_pose.size() != 6)
    {
        yCWarning(TRANSFORMCLIENT, "transformPose(): Performance warning: size transformed_pose should be 6, resizing.");
        transformed_pose.resize(6, 0.0);
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yCError(TRANSFORMCLIENT) << "No transform found between source '" << target_frame_id << "' and target '" << source_frame_id << "'";
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

bool TransformClient::transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::math::Quaternion &input_quaternion, yarp::math::Quaternion &transformed_quaternion)
{
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yCError(TRANSFORMCLIENT) << "No transform found between source '" << target_frame_id << "' and target '" << source_frame_id <<"'";
        return false;
    }
    FrameTransform t;
    t.rotation=input_quaternion;
    transformed_quaternion.fromRotationMatrix(m * t.toMatrix());
    return true;
}

bool TransformClient::waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout)
{
    //loop until canTransform == true or timeout expires
    double start = yarp::os::SystemClock::nowSystem();
    while (!canTransform(target_frame_id, source_frame_id))
    {
        if (yarp::os::SystemClock::nowSystem() - start > timeout)
        {
            yCError(TRANSFORMCLIENT) << "waitForTransform(): timeout expired";
            return false;
        }
        yarp::os::SystemClock::delaySystem(0.001);
    }
    return true;
}

TransformClient::TransformClient() : PeriodicThread(0.01),
    m_transform_storage(nullptr),
    m_period(0.01)
{
}

TransformClient::~TransformClient() = default;

bool     TransformClient::threadInit()
{
    yCTrace(TRANSFORMCLIENT, "Thread started");
    return true;
}

void     TransformClient::threadRelease()
{
    yCTrace(TRANSFORMCLIENT, "Thread stopped");
}

void     TransformClient::run()
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
            std::string src = m_array_of_port->transform_src;
            std::string dst = m_array_of_port->transform_dst;
            yarp::sig::Matrix m;
            this->getTransform(src, dst, m);
            if (m_array_of_port->format == "matrix")
            {
                m_array_of_port->port.write(m);
            }
            else
            {
                yCError(TRANSFORMCLIENT) << "Unknown format requested: " << m_array_of_port->format;
            }
        }
    }
}

bool     TransformClient::isConnectedWithServer()
{
    bool ok1 = Network::isConnected(m_local_rpcServer.c_str(), m_remote_rpc.c_str());
    if (!ok1) {
        yCInfo(TRANSFORMCLIENT) << m_local_rpcServer << "is not connected to: " << m_remote_rpc;
    }

    bool ok2 = Network::isConnected(m_remote_streaming_name.c_str(), m_local_streaming_name.c_str(),m_streaming_connection_type.c_str());
    if (!ok2) {
        yCInfo(TRANSFORMCLIENT) << m_remote_streaming_name << "is not connected to: " << m_local_streaming_name;
    }

    return ok1 && ok2;
}

bool     TransformClient::reconnectWithServer()
{
    bool ok = Network::connect(m_remote_streaming_name.c_str(), m_local_streaming_name.c_str(), m_streaming_connection_type.c_str());
    if (!ok)
    {
        yCError(TRANSFORMCLIENT, "reconnectWithServer(): Could not connect to %s", m_remote_streaming_name.c_str());
        return false;
    }

    ok = Network::connect(m_local_rpcServer, m_remote_rpc);
    if (!ok)
    {
        yCError(TRANSFORMCLIENT, "reconnectWithServer(): Could not connect to %s", m_remote_rpc.c_str());
        return false;
    }
    return true;
}
