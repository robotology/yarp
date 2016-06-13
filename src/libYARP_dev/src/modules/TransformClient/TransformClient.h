/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#ifndef TRANSFORM_CLIENT_H
#define TRANSFORM_CLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ITransform.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class TransformClient;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms
const int TRANSFORM_TIMEOUT_MS=100; //ms

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Transforms_client_storage : public yarp::os::BufferedPort<yarp::os::Bottle>
{
private:
    yarp::os::Bottle m_lastBottle;
    yarp::os::Mutex  m_mutex;
    yarp::os::Stamp  m_lastStamp;
    double           m_deltaT;
    double           m_deltaTMax;
    double           m_deltaTMin;
    double           m_prev;
    double           m_now;
    int              m_state;
    int              m_count;

    std::vector <Transform_t> m_transforms;

public:
    inline size_t   size()                                { return m_transforms.size(); }
    inline Transform_t& operator[]   (std::size_t idx)    { return m_transforms[idx]; };
    inline void clear()                                   { m_transforms.clear(); }

public:
    Transforms_client_storage (std::string port_name);
    ~Transforms_client_storage ( );
    bool     set_transform(Transform_t t);
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
class yarp::dev::TransformClient: public DeviceDriver,
                                  public ITransform
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
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
     bool     canTransform(const std::string &target_frame, const std::string &source_frame, std::string *error_msg = NULL) ;
     bool     clear() ;
     bool     frameExists(const std::string &frame_id) ;
     bool     getAllFrameIds(std::vector< std::string > &ids) ;
     bool     getParent(const std::string &frame_id, std::string &parent_frame_id) ;
     bool     getTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform) ;
     bool     setTransform(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform) ;
     bool     deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)     ;
     bool     transformPoint(const std::string &target_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point) ;
     bool     transformPose(const std::string &target_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose) ;
     bool     transformQuaternion(const std::string &target_frame_id, const yarp::sig::Vector &input_quaternion, yarp::sig::Vector &transformed_quaternion) ;
     bool     waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout) ;
};

#endif // TRANSFORM_CLIENT_H
