/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKE_LASER_WITH_MOTOR_H
#define FAKE_LASER_WITH_MOTOR_H


#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/ImplementPositionControl.h>
#include <yarp/dev/ImplementVelocityControl.h>
#include <yarp/dev/ImplementInteractionMode.h>
#include <yarp/dev/ImplementEncodersTimed.h>
#include <yarp/dev/ImplementTorqueControl.h>
#include <yarp/dev/ImplementControlMode.h>
#include <yarp/dev/ImplementAxisInfo.h>
#include <yarp/dev/Lidar2DDeviceBase.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>

#include <mutex>
#include <random>
#include <string>

#include "FakeLaserWithMotor_ParamsParser.h"

/**
* @ingroup dev_impl_fake dev_impl_lidar
*
* @brief `fakeLaserWithMotor` : fake sensor device driver for testing purposes and reference for IRangefinder2D devices.
*
* Parameters required by this device are shown in class: FakeLaserWithMotor_ParamsParser
*
* \section Usage examples:
* yarpdev --device fakeLaser --help
* yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /ikart/laser:o --test no_obstacles
* yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /ikart/laser:o --test use_pattern
* yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /ikart/laser:o --test use_mapfile --map_file mymap.map
* yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /ikart/laser:o --test use_mapfile --map_file mymap.map
* yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /ikart/laser:o --test use_mapfile --map_file mymap.map
* yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /ikart/laser:o --test use_mapfile --map_file mymap.map
*/

class FakeLaserWithMotor : public yarp::os::PeriodicThread,
                  public yarp::dev::Lidar2DDeviceBase,
                  public yarp::dev::DeviceDriver,
                  public yarp::os::PortReader,
                  public yarp::dev::IEncodersTimedRaw,
                  public yarp::dev::IPositionControlRaw,
                  public yarp::dev::IVelocityControlRaw,
                  public yarp::dev::IControlModeRaw,
                  public yarp::dev::IAxisInfoRaw,
                //  public yarp::dev::StubImplTorqueControlRaw,
                  public yarp::dev::StubImplInteractionModeRaw,
                  public yarp::dev::ImplementPositionControl,
                  public yarp::dev::ImplementVelocityControl,
                  public yarp::dev::ImplementControlMode,
                  public yarp::dev::ImplementInteractionMode,
                  public yarp::dev::ImplementEncodersTimed,
                  public yarp::dev::ImplementAxisInfo,
                  public FakeLaserWithMotor_ParamsParser
{
protected:
    enum test_mode_t { NO_OBSTACLES = 0, USE_PATTERN =1, USE_MAPFILE =2, USE_CONSTANT_VALUE =3, USE_SQUARE_TRAP };

    test_mode_t m_test_mode;

    yarp::dev::Nav2D::MapGrid2D   m_originally_loaded_map;
    yarp::dev::Nav2D::MapGrid2D   m_map;

    //motor stuff
    size_t m_njoints=3;

    //this is the position of the localized robot in the map
    double m_robot_loc_x=0;
    double m_robot_loc_y=0;
    double m_robot_loc_t=0;

    std::random_device* m_rd;
    std::mt19937* m_gen;
    std::uniform_real_distribution<>* m_dis;

    yarp::os::Port  m_rpcPort;

public:
    FakeLaserWithMotor(double period = 0.02) : PeriodicThread(period),
        m_test_mode(test_mode_t::NO_OBSTACLES),
        ImplementPositionControl(this),
        ImplementVelocityControl(this),
        ImplementEncodersTimed(this),
        ImplementControlMode(this),
        ImplementInteractionMode(this),
        ImplementAxisInfo(this)
    {
        //default parameters
        m_min_distance = 0.1;  //m
        m_max_distance = 8.0;  //m
        m_min_angle = 0;       //degrees
        m_max_angle = 360;     //degrees
        m_resolution = 1.0;    //degrees
        m_scan_rate = period;  //s

        //noise generator
        m_rd  = new std::random_device;
        m_gen = new std::mt19937((*m_rd)());
        m_dis = new std::uniform_real_distribution<>(0, 0.01);
    }

    ~FakeLaserWithMotor()
    {
        delete m_rd;
        delete m_gen;
        delete m_dis;
        m_rd = 0;
        m_gen = 0;
        m_dis = 0;
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    void drawStraightLine(yarp::dev::Nav2D::XYCell src, yarp::dev::Nav2D::XYCell dst);
    void wall_the_robot(double siz = 1.0, double dist = 1.0);
    void obst_the_robot(double siz = 1.0, double dist = 1.0);
    void trap_the_robot(double siz = 1.0);
    void free_the_robot();

    double checkStraightLine(yarp::dev::Nav2D::XYCell src, yarp::dev::Nav2D::XYCell dst);
    bool   LiangBarsky_clip(int edgeLeft, int edgeRight, int edgeTop, int edgeBottom,
                            yarp::dev::Nav2D::XYCell_unbounded src, yarp::dev::Nav2D::XYCell_unbounded dst,
                            yarp::dev::Nav2D::XYCell& src_clipped, yarp::dev::Nav2D::XYCell& dst_clipped);

public:
    //IRangefinder2D interface
    yarp::dev::ReturnValue setDistanceRange    (double min, double max) override;
    yarp::dev::ReturnValue setScanLimits        (double min, double max) override;
    yarp::dev::ReturnValue setHorizontalResolution      (double step) override;
    yarp::dev::ReturnValue setScanRate         (double rate) override;

public:
    // internal stuff
    std::string* _axisName = nullptr;                          // axis name
    yarp::dev::JointTypeEnum* _jointType = nullptr;            // axis type
    int* _controlModes = nullptr;
    double* _encoders = nullptr;             // encoders
    double* _posCtrl_references = nullptr;   // used for position control.
    double* _ref_speeds = nullptr;           // used for position control.
    double* _command_speeds = nullptr;       // used for velocity control.
    double* _ref_accs = nullptr;             // used for velocity control.
    std::vector<yarp::dev::SelectableControlModeEnum>* _availableControlModes = nullptr;

    bool alloc(int njoints);
    bool dealloc();

public:
    //motor interfaces
    yarp::dev::ReturnValue resetEncoderRaw(int j) override;
    yarp::dev::ReturnValue resetEncodersRaw() override;
    yarp::dev::ReturnValue setEncoderRaw(int j, double val) override;
    yarp::dev::ReturnValue setEncodersRaw(const double* vals) override;
    yarp::dev::ReturnValue getEncoderRaw(int j, double* v) override;
    yarp::dev::ReturnValue getEncodersRaw(double* encs) override;
    yarp::dev::ReturnValue getEncoderSpeedRaw(int j, double* sp) override;
    yarp::dev::ReturnValue getEncoderSpeedsRaw(double* spds) override;
    yarp::dev::ReturnValue getEncoderAccelerationRaw(int j, double* spds) override;
    yarp::dev::ReturnValue getEncoderAccelerationsRaw(double* accs) override;

    yarp::dev::ReturnValue getEncodersTimedRaw(double* encs, double* stamps) override;
    yarp::dev::ReturnValue getEncoderTimedRaw(int j, double* encs, double* stamp) override;

    // POSITION CONTROL INTERFACE RAW
    yarp::dev::ReturnValue getAxes(int* ax) override;
    yarp::dev::ReturnValue positionMoveRaw(int j, double ref) override;
    yarp::dev::ReturnValue positionMoveRaw(const double* refs) override;
    yarp::dev::ReturnValue relativeMoveRaw(int j, double delta) override;
    yarp::dev::ReturnValue relativeMoveRaw(const double* deltas) override;
    yarp::dev::ReturnValue checkMotionDoneRaw(bool* flag) override;
    yarp::dev::ReturnValue checkMotionDoneRaw(int j, bool* flag) override;
    yarp::dev::ReturnValue setTrajSpeedRaw(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeedsRaw(const double* spds) override;
    yarp::dev::ReturnValue setTrajAccelerationRaw(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerationsRaw(const double* accs) override;
    yarp::dev::ReturnValue getTrajSpeedRaw(int j, double* ref) override;
    yarp::dev::ReturnValue getTrajSpeedsRaw(double* spds) override;
    yarp::dev::ReturnValue getTrajAccelerationRaw(int j, double* acc) override;
    yarp::dev::ReturnValue getTrajAccelerationsRaw(double* accs) override;
    yarp::dev::ReturnValue stopRaw(int j) override;
    yarp::dev::ReturnValue stopRaw() override;

    yarp::dev::ReturnValue positionMoveRaw(const int n_joint, const int* joints, const double* refs) override;
    yarp::dev::ReturnValue relativeMoveRaw(const int n_joint, const int* joints, const double* deltas) override;
    yarp::dev::ReturnValue checkMotionDoneRaw(const int n_joint, const int* joints, bool* flags) override;
    yarp::dev::ReturnValue setTrajSpeedsRaw(const int n_joint, const int* joints, const double* spds) override;
    yarp::dev::ReturnValue setTrajAccelerationsRaw(const int n_joint, const int* joints, const double* accs) override;
    yarp::dev::ReturnValue getTrajSpeedsRaw(const int n_joint, const int* joints, double* spds) override;
    yarp::dev::ReturnValue getTrajAccelerationsRaw(const int n_joint, const int* joints, double* accs) override;
    yarp::dev::ReturnValue stopRaw(const int n_joint, const int* joints) override;
    yarp::dev::ReturnValue getTargetPositionRaw(const int joint, double* ref) override;
    yarp::dev::ReturnValue getTargetPositionsRaw(double* refs) override;
    yarp::dev::ReturnValue getTargetPositionsRaw(const int n_joint, const int* joints, double* refs) override;

    //IControlMode
    yarp::dev::ReturnValue getAvailableControlModesRaw(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail) override;
    yarp::dev::ReturnValue getControlModeRaw(int j, yarp::dev::ControlModeEnum& mode) override;
    yarp::dev::ReturnValue getControlModesRaw(std::vector<yarp::dev::ControlModeEnum>& mode) override;
    yarp::dev::ReturnValue getControlModesRaw(std::vector<int> j, std::vector<yarp::dev::ControlModeEnum>& mode) override;
    yarp::dev::ReturnValue setControlModeRaw(int j, yarp::dev::SelectableControlModeEnum mode) override;
    yarp::dev::ReturnValue setControlModesRaw(std::vector<int> j, std::vector<yarp::dev::SelectableControlModeEnum> mode) override;
    yarp::dev::ReturnValue setControlModesRaw(const std::vector<yarp::dev::SelectableControlModeEnum> mode) override;

    //IVelocityControl
    yarp::dev::ReturnValue velocityMoveRaw(int j, double sp) override;
    yarp::dev::ReturnValue velocityMoveRaw(const double* sp) override;
    yarp::dev::ReturnValue velocityMoveRaw(const int n_joint, const int* joints, const double* spds) override;
    yarp::dev::ReturnValue getTargetVelocityRaw(const int joint, double* ref) override;
    yarp::dev::ReturnValue getTargetVelocitiesRaw(double* refs) override;
    yarp::dev::ReturnValue getTargetVelocitiesRaw(const int n_joint, const int* joints, double* refs) override;

    yarp::dev::ReturnValue getAxisNameRaw(int axis, std::string& name) override;
    yarp::dev::ReturnValue getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type) override;
public:
    //Lidar2DDeviceBase
    bool acquireDataFromHW() override final;

public:
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif
