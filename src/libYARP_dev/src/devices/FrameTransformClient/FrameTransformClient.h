/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_FRAMETRANSFORMCLIENT_FRAMETRANSFORMCLIENT_H
#define YARP_DEV_FRAMETRANSFORMCLIENT_FRAMETRANSFORMCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/math/FrameTransform.h>
#include <yarp/os/RecursiveMutex.h>

namespace yarp {
    namespace dev {
        class FrameTransformClient;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms
const int TRANSFORM_TIMEOUT_MS = 100; //ms

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Transforms_client_storage : public yarp::os::BufferedPort<yarp::os::Bottle>
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
    yarp::os::RecursiveMutex  m_mutex;
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
    virtual void onRead(yarp::os::Bottle &v);
    inline int getLast(yarp::os::Bottle &data, yarp::os::Stamp &stmp);
    inline int getIterations();
    void getEstFrequency(int &ite, double &av, double &min, double &max);
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
* @ingroup dev_impl_wrapper
*
* The client side of any IBattery capable device.
* Still single thread! concurrent access is unsafe.
*/
class yarp::dev::FrameTransformClient: public DeviceDriver,
                                  public IFrameTransform
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    enum ConnectionType {DISCONNECTED = 0, DIRECT, INVERSE, UNDIRECT};

    yarp::dev::FrameTransformClient::ConnectionType getConnectionType(const std::string &target_frame, const std::string &source_frame, std::string* commonAncestor);
    
    bool canExplicitTransform(const std::string& target_frame_id, const std::string& source_frame_id) const;
    bool getChainedTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) const;

protected:

    yarp::os::Port                m_rpcPort;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_remote_name;
    Transforms_client_storage*    m_transform_storage;
    int                           m_period;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();


    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp();

     bool     allFramesAsString(std::string &all_frames);
     bool     canTransform(const std::string &target_frame, const std::string &source_frame);
     bool     clear() ;
     bool     frameExists(const std::string &frame_id) ;
     bool     getAllFrameIds(std::vector< std::string > &ids) ;
     bool     getParent(const std::string &frame_id, std::string &parent_frame_id) ;
     bool     getTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) ;
     bool     setTransform(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) ;
     bool     setTransformStatic(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform);
     bool     deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)     ;
     bool     transformPoint(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point);
     bool     transformPose(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose);
     bool     transformQuaternion(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::math::Quaternion &input_quaternion, yarp::math::Quaternion &transformed_quaternion);
     bool     waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) ;
};

#endif // YARP_DEV_FRAMETRANSFORMCLIENT_FRAMETRANSFORMCLIENT_H
