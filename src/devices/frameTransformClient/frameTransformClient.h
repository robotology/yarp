/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_DEV_FRAMETRANSFORMCLIENT_H
#define YARP_DEV_FRAMETRANSFORMCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <frameTransformContainer.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/IFrameTransformStorage.h>
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
#include <yarp/robotinterface/experimental/Param.h>
#include <yarp/robotinterface/experimental/XMLReader.h>

#define DEFAULT_THREAD_PERIOD 20 //ms
const int MAX_PORTS = 5;

/**
 * @ingroup dev_impl_network_clients
 * @brief A client to manage FrameTransforms for a robot
 *
 * This is a client for the frametransform device.\n
 * it has various configurations as below.
 * \section clients that doesn't need the FrameTransformServer
 * - **LOCAL ONLY** The first one is the local only, that maintain a storage inside
 * keeps all the transforms there and has no connection with the network.\n
 * it is useful when you don't need to communicate with the network but you need to have
 * the transforms locally.\n
 * The code to launch this client is:
 * \code{.unparsed}
 * yarpdev --device frameTransformClient --filexml_option ftc_local_only.xml
 * \endcode
 * \image  html frametransform/FTC_Local_only.png width=35%
 * - **ROS ONLY** The previous one and this one, the ros publisher only,
 * are the only two clients that doesn't need a server.\n
 * The ros publisher only is composed as below and is
 * useful  when you need to communicate to a ros server without the yarp server.\n
 * The code to launch this client is:
 * \code{.unparsed}
 * yarpdev --device frameTransformClient --filexml_option ftc_ros.xml
 * \endcode
 * \image  html frametransform/FTC_ROS_Only.png width=35%
 *   - **ROS PUB ONLY** and **ROS SUB ONLY** There are two other version of the previous client as below,\n
 *   these two clients are to publish only or to receive only the transforms.\n
 *   They are useful when you need to interact but you don't want the whole client.\n
 *   The codes to launch those clients are:
 *   \code{.unparsed}
 *   yarpdev --device frameTransformClient --filexml_option ftc_pub_ros.xml
 *   \endcode
 *   \code{.unparsed}
 *   yarpdev --device frameTransformClient --filexml_option ftc_sub_ros.xml
 *   \endcode
 *   \image  html frametransform/FTC_ROS_Pub_Only.png width=35%
 *   \image  html frametransform/FTC_ROS_Sub_Only.png width=35%
 *
 * \section clients that need the FrameTransformServer
 * All the clients inside this section needs a server active, the composition
 * of the server depends from the type of client. The various clients are:
 * - **YARP ONLY** This is the full client that uses only yarp.\n
 * N.B. The client use only yarp but the server attached can be both ONLY YARP or YARP + ROS\n
 * The code to launch this client is:
 * \code{.unparsed}
 * yarpdev --device frameTransformClient --filexml_option ftc_yarp_only.xml
 * \endcode
 * \image  html frametransform/FTC_YARP_only.png width=35%
 *   - **YARP PUB ONLY** and **YARP SUB ONLY** As same as ROS client, there are also two other versions of this client,\n
 *   the one to publish only the transforms and the one to receive only the transforms.\n
 *   Like the one above this can be attached to both ONLY YARP or YARP + ROS\n
 *   The codes to launch those clients are:
 *   \code{.unparsed}
 *   yarpdev --device frameTransformClient --filexml_option ftc_pub_yarp_only.xml
 *   \endcode
 *   \code{.unparsed}
 *   yarpdev --device frameTransformClient --filexml_option ftc_sub_yarp_only.xml
 *   \endcode
 *   \image  html frametransform/FTC_PUB_YARP_only.png width=35%
 *   \image  html frametransform/FTC_SUB_YARP_only.png width=35%
 *   - **YARP UPDATE ONLY** There is a third version of the client that is useful when someone wants to receive only the\n
 *   updates from another client and is the one below:
 *   \code{.unparsed}
 *   yarpdev --device frameTransformClient --filexml_option ftc_yarp_only_single_client.xml
 *   \endcode
 *   \image  html frametransform/FTC_YARP_only_single_client.png width=35%
 *
 * - **YARP AND ROS** The last version available of the client is the one that has inside bot ros and yarp.\n
 * It is useful when you have both yarp and ros frametransform server and you need to keep  both updated.\n
 * the code to launch this client is:
 * \code{.unparsed}
 * yarpdev --device frameTransformClient --filexml_option ftc_full_ros.xml
 * \endcode
 * \image  html frametransform/FTC_YARP_+_ROS.png width=35%
 * .
*/

class FrameTransformClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IFrameTransform,
        public yarp::os::PortReader,
        public yarp::os::PeriodicThread
{
private:
    enum ConnectionType {DISCONNECTED = 0, DIRECT, INVERSE, UNDIRECT, IDENTITY};

    FrameTransformClient::ConnectionType priv_getConnectionType(const std::string &target_frame, const std::string &source_frame, std::string* commonAncestor);

    bool priv_canExplicitTransform(const std::string& target_frame_id, const std::string& source_frame_id) const;
    bool priv_getChainedTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) const;

protected:

    yarp::os::Port      m_rpc_InterfaceToUser;
    std::string         m_local_name;
    double              m_period;
    std::mutex          m_rpc_mutex;

    //ports to broadcast stuff...
    struct broadcast_port_t
    {
        enum format_t {matrix=0} format;
        yarp::os::Port port;
        std::string transform_src;
        std::string transform_dst;
    };
    std::vector<broadcast_port_t*>  m_array_of_ports;

    //new stuff
    yarp::robotinterface::experimental::Robot m_robot;
    yarp::dev::IFrameTransformStorageGet* m_ift_g = nullptr;
    yarp::dev::IFrameTransformStorageSet* m_ift_s = nullptr;
    yarp::dev::IFrameTransformStorageUtils* m_ift_u = nullptr;


public:
    FrameTransformClient();
    ~FrameTransformClient();

public:
    // DeviceDriver methods
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;

    // IPreciselyTimed methods
    yarp::os::Stamp getLastInputStamp();

    //IFrameTransform
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

    //PeriodicThread
    bool     threadInit() override;
    void     threadRelease() override;
    void     run() override;
};

#endif // YARP_DEV_FRAMETRANSFORMCLIENT_H
