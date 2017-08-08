/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "FrameTransformClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file FrameTransformClient.cpp */

//example: yarpdev --device transformClient --local /transformClient --remote /transformServer

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;


inline void Transforms_client_storage::resetStat()
{
    RecursiveLockGuard l(m_mutex);
}

void Transforms_client_storage::onRead(yarp::os::Bottle &b)
{
    m_now = Time::now();
    RecursiveLockGuard guard(m_mutex);

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
   // if ((1000 * (newStamp.getTime() - m_lastStamp.getTime()))<TRANSFORM_TIMEOUT)
    if (1)
    {
        m_state = IFrameTransform::TRANSFORM_OK;

        m_transforms.clear();
        int bsize= b.size();
        for (int i = 0; i < bsize; i++)
        {
            //this includes: timed yarp transforms, static yarp transforms, ros transforms
            Bottle* bt = b.get(i).asList();
            if (bt != 0)
            {
                FrameTransform t;
                t.src_frame_id = bt->get(0).asString();
                t.dst_frame_id = bt->get(1).asString();
                t.timestamp = bt->get(2).asDouble();
                t.translation.tX = bt->get(3).asDouble();
                t.translation.tY = bt->get(4).asDouble();
                t.translation.tZ = bt->get(5).asDouble();
                t.rotation.w() = bt->get(6).asDouble();
                t.rotation.x() = bt->get(7).asDouble();
                t.rotation.y() = bt->get(8).asDouble();
                t.rotation.z() = bt->get(9).asDouble();
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
    RecursiveLockGuard guard(m_mutex);

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
    RecursiveLockGuard guard(m_mutex);
    int ret = m_count;
    return ret;
}

// time is in ms
void Transforms_client_storage::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    RecursiveLockGuard guard(m_mutex);
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
    RecursiveLockGuard l(m_mutex);
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

    if (!this->open(local_streaming_name.c_str()))
    {
        yError("FrameTransformClient::open() error could not open port %s, check network", local_streaming_name.c_str());
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
    RecursiveLockGuard l(m_mutex);
    return m_transforms.size();
}

yarp::math::FrameTransform& Transforms_client_storage::operator[]   (std::size_t idx)
{
    RecursiveLockGuard l(m_mutex);
    return m_transforms[idx];
};

//------------------------------------------------------------------------------------------------------------------------------
bool yarp::dev::FrameTransformClient::read(yarp::os::ConnectionReader& connection)
{
    LockGuard lock (m_rpc_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    std::string request = in.get(0).asString();
    if (request == "help")
    {
        out.addVocab(Vocab::encode("many"));
        out.addString("'get_transform <src> <dst>: print the transform from <src> to <dst>");
        out.addString("'list_frames: print all the available refence frames");
        out.addString("'list_ports: print all the opened ports for tranform broadcasting");
        out.addString("'publish_transform <src> <dst> <portname> <format>: opens a port to publish transform from src to dst");
        out.addString("'unpublish_transform <portname>: closes a previously opened port to publish a transform");
        out.addString("'unpublish_all <portname>: closes a all previously opened ports to publish a transform");
    }
    else if (request == "list_frames")
    {
        std::vector<std::string> v;
        this->getAllFrameIds(v);
        out.addVocab(Vocab::encode("many"));
        out.addString("List of available reference frames:");
        int count = 0;
        for (auto vec = v.begin(); vec != v.end(); vec++)
        {
            count++;
            std::string str = std::to_string(count) + "- " + *vec;
            out.addString(str.c_str());
        }
    }
    else if (request == "get_transform")
    {
        std::string src = in.get(1).asString();
        std::string dst = in.get(2).asString();
        out.addVocab(Vocab::encode("many"));
        yarp::sig::Matrix m;
        this->getTransform(src, dst, m);
        out.addString("Tranform from " + src + " to " + dst + " is: ");
        out.addString(m.toString());
    }
    else if (request == "list_ports")
    {
        out.addVocab(Vocab::encode("many"));
        if (m_array_of_ports.size()==0)
        {
            out.addString("No ports are currently active");
        }
        for (auto it = m_array_of_ports.begin(); it != m_array_of_ports.end(); it++)
        {
            if (*it)
            {
                std::string  s = (*it)->port.getName() + " "+ (*it)->transform_src +  " -> " + (*it)->transform_dst;
                out.addString(s);
            }
        }
    }
    else if (request == "publish_transform")
    {
        out.addVocab(Vocab::encode("many"));
        std::string src  = in.get(1).asString();
        std::string dst  = in.get(2).asString();
        std::string port_name = in.get(3).asString();
        std::string format = "matrix";
        if (in.size() > 4)
            {format= in.get(4).asString();}
        if (port_name[0]=='/')  port_name.erase(port_name.begin());
        std::string full_port_name = m_local_name + "/" + port_name;
        bool ret = true;
        for (auto it = m_array_of_ports.begin(); it != m_array_of_ports.end(); it++)
        {
            if ((*it) && (*it)->port.getName() == full_port_name)
            {
                ret = false;
                break;
            }
        }
        if (this->frameExists(src)==false)
        {
            out.addString("Requested src frame " + src + " does not exists.");
            yWarning("Requested src frame %s does not exists.", src.c_str());
        }
        if (this->frameExists(dst)==false)
        {
            out.addString("Requested dst frame " + dst + " does not exists.");
            yWarning("Requested fst frame %s does not exists.", dst.c_str());
        }
        if (ret == true)
        {
            broadcast_port_t* b = new broadcast_port_t;
            b->transform_src = src;
            b->transform_dst = dst;
            b->format = format;
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
        for (auto it = m_array_of_ports.begin(); it != m_array_of_ports.end(); it++)
        {
            (*it)->port.close();
            delete (*it);
            (*it)=0;
        }
        m_array_of_ports.clear();
        if (m_array_of_ports.size()==0) this->askToStop();
        out.addString("Operation complete");
    }
    else if (request == "unpublish_transform")
    {
        bool ret = false;
        std::string port_name = in.get(1).asString();
        if (port_name[0]=='/')  port_name.erase(port_name.begin());
        std::string full_port_name = m_local_name + "/" + port_name;
        for (auto it = m_array_of_ports.begin(); it != m_array_of_ports.end(); it++)
        {
            if ((*it)->port.getName() == port_name)
            {
                (*it)->port.close();
                delete (*it);
                (*it)=0;
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
        yError("Invalid vocab received in FrameTransformClient");
        out.clear();
        out.addVocab(VOCAB_ERR);
        out.addString("Invalid command name");
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL)
    {
        out.write(*returnToSender);
    }
    else
    {
        yError() << "FrameTransformClient: invalid return to sender";
    }
    return true;
}

bool yarp::dev::FrameTransformClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name  = config.find("local").asString().c_str();
    m_remote_name = config.find("remote").asString().c_str();

    if (m_local_name == "")
    {
        yError("FrameTransformClient::open() error you have to provide valid local name");
        return false;
    }
    if (m_remote_name == "")
    {
        yError("FrameTransformClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt();
    }
    else
    {
        m_period = 10;
        yWarning("FrameTransformClient: using default period of %d ms" , m_period);
    }

    ConstString local_rpcServer = m_local_name;
    local_rpcServer += "/rpc:o";
    ConstString local_rpcUser = m_local_name;
    local_rpcUser += "/rpc:i";
    ConstString remote_rpc = m_remote_name;
    remote_rpc += "/rpc";
    ConstString remote_streaming_name = m_remote_name;
    remote_streaming_name += "/transforms:o";
    ConstString local_streaming_name = m_local_name;
    local_streaming_name += "/transforms:i";

    if (!m_rpc_InterfaceToUser.open(local_rpcUser.c_str()))
    {
        yError("FrameTransformClient::open() error could not open rpc port %s, check network", local_rpcUser.c_str());
        return false;
    }

    if (!m_rpc_InterfaceToServer.open(local_rpcServer.c_str()))
    {
        yError("FrameTransformClient::open() error could not open rpc port %s, check network", local_rpcServer.c_str());
        return false;
    }

    m_transform_storage = new Transforms_client_storage(local_streaming_name);
    bool ok = Network::connect(remote_streaming_name.c_str(), local_streaming_name.c_str(), "udp");
    if (!ok)
    {
        yError("FrameTransformClient::open() error could not connect to %s", remote_streaming_name.c_str());
        return false;
    }

    ok = Network::connect(local_rpcServer.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("FrameTransformClient::open() error could not connect to %s", remote_rpc.c_str());
        return false;
    }


    m_rpc_InterfaceToUser.setReader(*this);
    return true;
}

bool yarp::dev::FrameTransformClient::close()
{
    m_rpc_InterfaceToServer.close();
    m_rpc_InterfaceToUser.close();
    if (m_transform_storage != 0)
    {
        delete m_transform_storage;
        m_transform_storage = 0;
    }
    return true;
}

bool yarp::dev::FrameTransformClient::allFramesAsString(std::string &all_frames)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        all_frames += (*m_transform_storage)[i].toString() + " ";
    }
    return true;
}

yarp::dev::FrameTransformClient::ConnectionType yarp::dev::FrameTransformClient::getConnectionType(const std::string &target_frame, const std::string &source_frame, std::string* commonAncestor = NULL)
{
    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t                     i, j;
    std::vector<std::string>   tar2root_vec;
    std::vector<std::string>   src2root_vec;
    std::string                ancestor, child;
    child = target_frame;
    RecursiveLockGuard l(tfVec.m_mutex);
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

bool yarp::dev::FrameTransformClient::canTransform(const std::string &target_frame, const std::string &source_frame)
{
    return getConnectionType(target_frame, source_frame) != DISCONNECTED;
}

bool yarp::dev::FrameTransformClient::clear()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    b.addVocab(VOCAB_ITRANSFORM);
    b.addVocab(VOCAB_TRANSFORM_DELETE_ALL);
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "FrameTransformClient::clear() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "FrameTransformClient::clear() error on writing on rpc port";
        return false;
    }

    m_transform_storage->clear();
    return true;
}

bool yarp::dev::FrameTransformClient::frameExists(const std::string &frame_id)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        if (((*m_transform_storage)[i].src_frame_id) == frame_id) { return true; }
        if (((*m_transform_storage)[i].dst_frame_id) == frame_id) { return true; }
    }
    return false;
}

bool yarp::dev::FrameTransformClient::getAllFrameIds(std::vector< std::string > &ids)
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

bool yarp::dev::FrameTransformClient::getParent(const std::string &frame_id, std::string &parent_frame_id)
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

bool yarp::dev::FrameTransformClient::canExplicitTransform(const std::string& target_frame_id, const std::string& source_frame_id) const
{
    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t                     i, tfVec_size;
    RecursiveLockGuard         l(tfVec.m_mutex);

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

bool yarp::dev::FrameTransformClient::getChainedTransform(const std::string& target_frame_id, const std::string& source_frame_id, yarp::sig::Matrix& transform) const
{
    Transforms_client_storage& tfVec = *m_transform_storage;
    size_t                     i, tfVec_size;
    RecursiveLockGuard         l(tfVec.m_mutex);

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

bool yarp::dev::FrameTransformClient::getTransform(const std::string& target_frame_id, const std::string& source_frame_id, yarp::sig::Matrix& transform)
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

    yError() << "FrameTransformClient::getTransform() frames " << source_frame_id << " and " << target_frame_id << " are not connected";
    return false;
}

bool yarp::dev::FrameTransformClient::setTransform(const std::string& target_frame_id, const std::string& source_frame_id, const yarp::sig::Matrix& transform)
{

    if (!canExplicitTransform(target_frame_id, source_frame_id) && canTransform(target_frame_id, source_frame_id))
    {
        yError() << "FrameTransformClient::setTransform() such transform already exist by chaining transforms";
        return false;
    }

    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    FrameTransform   tf;

    if (!tf.fromMatrix(transform))
    {
        yError() << "FrameTransformClient::setTransform() wrong matrix format, it has to be 4 by 4";
        return false;
    }

    b.addVocab(VOCAB_ITRANSFORM);
    b.addVocab(VOCAB_TRANSFORM_SET);
    b.addString(source_frame_id);
    b.addString(target_frame_id);
    b.addDouble(1000.0); //transform lifetime
    b.addDouble(tf.translation.tX);
    b.addDouble(tf.translation.tY);
    b.addDouble(tf.translation.tZ);
    b.addDouble(tf.rotation.w());
    b.addDouble(tf.rotation.x());
    b.addDouble(tf.rotation.y());
    b.addDouble(tf.rotation.z());
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "FrameTransformClient::setTransform() recived error from server on creating frame between " + source_frame_id + " and " + target_frame_id;
            return false;
        }
    }
    else
    {
        yError() << "FrameTransformClient::setTransform() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::FrameTransformClient::setTransformStatic(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform)
{
    if (canTransform(target_frame_id, source_frame_id))
    {
        yError() << "FrameTransformClient::setTransform() such static transform already exist, directly or by chaining transforms";
        return false;
    }

    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    FrameTransform   tf;

    if (!tf.fromMatrix(transform))
    {
        yError() << "FrameTransformClient::setTransform() wrong matrix format, it has to be 4 by 4";
        return false;
    }

    b.addVocab(VOCAB_ITRANSFORM);
    b.addVocab(VOCAB_TRANSFORM_SET);
    b.addString(source_frame_id);
    b.addString(target_frame_id);
    b.addDouble(-1);
    b.addDouble(tf.translation.tX);
    b.addDouble(tf.translation.tY);
    b.addDouble(tf.translation.tZ);
    b.addDouble(tf.rotation.w());
    b.addDouble(tf.rotation.x());
    b.addDouble(tf.rotation.y());
    b.addDouble(tf.rotation.z());
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "FrameTransformClient::setTransform() recived error from server on creating frame between " + source_frame_id + " and " + target_frame_id;
            return false;
        }
    }
    else
    {
        yError() << "FrameTransformClient::setTransform() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::FrameTransformClient::deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    b.addVocab(VOCAB_ITRANSFORM);
    b.addVocab(VOCAB_TRANSFORM_DELETE);
    b.addString(target_frame_id);
    b.addString(source_frame_id);
    bool ret = m_rpc_InterfaceToServer.write(b, resp);
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

bool yarp::dev::FrameTransformClient::transformPoint(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point)
{
    if (input_point.size() != 3)
    {
        yError() << "sorry.. only 3 dimensional vector allowed my dear..";
        return false;
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yError() << "no transform found between source '" << target_frame_id << "' and target '" << source_frame_id << "'";
        return false;
    }
    yarp::sig::Vector in = input_point;
    in.push_back(1);
    transformed_point = m * in;
    transformed_point.pop_back();
    return true;
}

bool yarp::dev::FrameTransformClient::transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose)
{
    if (input_pose.size() != 6)
    {
        yError() << "sorry.. only 6 dimensional vector (3 axes + roll pith and yaw) allowed, dear friend of mine..";
        return false;
    }
    if (transformed_pose.size() != 6)
    {
        yWarning("FrameTransformClient::transformPose() performance warning: size transformed_pose should be 6, resizing");
        transformed_pose.resize(6, 0.0);
    }
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yError() << "no transform found between source '" << target_frame_id << "' and target '" << source_frame_id << "'";
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

bool yarp::dev::FrameTransformClient::transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::math::Quaternion &input_quaternion, yarp::math::Quaternion &transformed_quaternion)
{
    yarp::sig::Matrix m(4, 4);
    if (!getTransform(target_frame_id, source_frame_id, m))
    {
        yError() << "no transform found between source '" << target_frame_id << "' and target '" << source_frame_id <<"'";
        return false;
    }
    FrameTransform t;
    t.rotation=input_quaternion;
    transformed_quaternion.fromRotationMatrix(m * t.toMatrix());
    return true;
}

bool yarp::dev::FrameTransformClient::waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout)
{
    //loop until canTransform == true or timeout expires
    double start = yarp::os::SystemClock::nowSystem();
    while (!canTransform(target_frame_id, source_frame_id))
    {
        if (yarp::os::SystemClock::nowSystem() - start > timeout)
        {
            yError() << "FrameTransformClient::waitForTransform() timeout expired";
            return false;
        }
        yarp::os::SystemClock::delaySystem(0.001);
    }
    return true;
}

FrameTransformClient::FrameTransformClient() : RateThread(0.010)
{
    m_transform_storage = 0;
}

FrameTransformClient::~FrameTransformClient()
{
}

bool     yarp::dev::FrameTransformClient::threadInit()
{
    yInfo("Thread started");
    return true;
}

void     yarp::dev::FrameTransformClient::threadRelease()
{
    yInfo("Thread stopped");
}

void     yarp::dev::FrameTransformClient::run()
{
    LockGuard lock (m_rpc_mutex);
    if (m_array_of_ports.size()==0)
    {
        return;
    }

    for (auto it=m_array_of_ports.begin(); it!=m_array_of_ports.end(); it++)
    {
        if (*it)
        {
            std::string src = (*it)->transform_src;
            std::string dst = (*it)->transform_dst;
            yarp::sig::Matrix m;
            this->getTransform(src, dst, m);
            if ((*it)->format == "matrix")
            {
                (*it)->port.write(m);
            }
            else
            {
                yError() << "Unknown format requested: " << (*it)->format;
            }
        }
    }
}

yarp::dev::DriverCreator *createFrameTransformClient()
{
    return new DriverCreatorOf<FrameTransformClient>
               (
                   "transformClient",
                   "",
                   "transformClient"
               );
}
