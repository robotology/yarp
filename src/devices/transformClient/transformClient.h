/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_FRAMETRANSFORMCLIENT_H
#define YARP_DEV_FRAMETRANSFORMCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/IFrameTransformClientControl.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/math/FrameTransform.h>
#include <yarp/os/PeriodicThread.h>
#include <mutex>


#define DEFAULT_THREAD_PERIOD 20 //ms
const int TRANSFORM_TIMEOUT_MS = 100; //ms
const int MAX_PORTS = 5;


class Transforms_client_storage :
        public yarp::os::BufferedPort<yarp::os::Bottle>
{
private:
    yarp::os::Bottle m_lastBottle;
    yarp::os::Stamp  m_lastStamp;
    double           m_deltaT;
    double           m_deltaTMax;
    double           m_deltaTMin;
    double           m_prev;
    double           m_now;
    int              m_state;
    int              m_count;

    std::vector <yarp::math::FrameTransform> m_transforms;

public:
    std::recursive_mutex  m_mutex;
    size_t   size();
    yarp::math::FrameTransform& operator[]   (std::size_t idx);
    void clear();

public:
    Transforms_client_storage (std::string port_name);
    ~Transforms_client_storage ( );
    bool     set_transform(yarp::math::FrameTransform t);
    bool     delete_transform(std::string t1, std::string t2);

    inline void resetStat();
    using yarp::os::BufferedPort<yarp::os::Bottle>::onRead;
    void onRead(yarp::os::Bottle &v) override;
    inline int getLast(yarp::os::Bottle &data, yarp::os::Stamp &stmp);
    inline int getIterations();
    void getEstFrequency(int &ite, double &av, double &min, double &max);
};


/**
* @ingroup dev_impl_network_clients dev_impl_deprecated
*
* \brief `transformClient` *deprecated*: Documentation to be added
*/
class TransformClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IFrameTransform,
        public yarp::dev::IFrameTransformClientControl,
        public yarp::os::PortReader,
        public yarp::os::PeriodicThread
{
private:
    enum ConnectionType {DISCONNECTED = 0, DIRECT, INVERSE, UNDIRECT, IDENTITY};

    TransformClient::ConnectionType getConnectionType(const std::string &target_frame, const std::string &source_frame, std::string* commonAncestor);

    bool canExplicitTransform(const std::string& target_frame_id, const std::string& source_frame_id) const;
    bool getChainedTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) const;

protected:

    yarp::os::Port                m_rpc_InterfaceToServer;
    yarp::os::Port                m_rpc_InterfaceToUser;
    std::string         m_local_name;
    std::string         m_remote_name;

    std::string         m_local_rpcServer;
    std::string         m_local_rpcUser;
    std::string         m_remote_rpc;
    std::string         m_remote_streaming_name;
    std::string         m_local_streaming_name;

    std::string         m_streaming_connection_type;
    Transforms_client_storage*    m_transform_storage;
    double                        m_period;
    std::mutex               m_rpc_mutex;
    struct broadcast_port_t
    {
        std::string format;
        yarp::os::Port port;
        std::string transform_src;
        std::string transform_dst;
    };
    std::vector<broadcast_port_t*>  m_array_of_ports;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;

    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp();

     bool     allFramesAsString(std::string &all_frames) override;
     bool     canTransform(const std::string &target_frame, const std::string &source_frame) override;
     bool     clear() override;
     bool     frameExists(const std::string &frame_id) override;
     bool     getAllFrameIds(std::vector< std::string > &ids) override;
     bool     getParent(const std::string &frame_id, std::string &parent_frame_id) override;
     bool     getTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) override;
     bool     setTransform(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) override;
     bool     setTransformStatic(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) override;
     bool     deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id) override;
     bool     transformPoint(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point) override;
     bool     transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose) override;
     bool     transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::math::Quaternion &input_quaternion, yarp::math::Quaternion &transformed_quaternion) override;
     bool     waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) override;

     bool     isConnectedWithServer() override;
     bool     reconnectWithServer() override;

     TransformClient();
    ~TransformClient();
     bool     threadInit() override;
     void     threadRelease() override;
     void     run() override;
};

#endif // YARP_DEV_FRAMETRANSFORMCLIENT_H
