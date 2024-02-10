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
* | YARP device name |
* |:-----------------:|
* | `fakeLaserWithMotor` |
*
* \section fakeLaser_device_parameters Description of input parameters
*
* Parameters accepted in the config argument of the open method:
* | Parameter name      | Type   | Units | Default Value | Required | Description | Notes |
* |:-------------------:|:------:|:-----:|:-------------:|:--------:|:-----------:|:-----:|
* | test                | string |   -   |       -       | Yes      | Choose the modality   | It can be one of the following: no_obstacles, use_pattern, use_mapfile |
* | map_file            | string |   -   |       -       | No       | Full path to a .map file   | Mandatory if --test use_mapfile option has been set |
* | clip_max            | double |   m   | 3.5           | No       | Maximum detectable distance for an obstacle | - |
* | clip_min            | double |   m   | 0.1           | No       | Minimum detectable distance for an obstacle | - |
* | max_angle           | double |  deg  | 360           | No       | Angular range of the sensor  | - |
* | min_angle           | double |  deg  |   0           | No       | Angular range of the sensor  | - |
* | resolution          | double |  deg  | 1.0           | No       | Device resolution          | - |
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

    double m_period;
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
    double m_const_value=1;

    yarp::os::Port  m_rpcPort;

public:
    FakeLaserWithMotor(double period = 0.02) : PeriodicThread(period),
        m_period(period),
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
    bool setDistanceRange    (double min, double max) override;
    bool setScanLimits        (double min, double max) override;
    bool setHorizontalResolution      (double step) override;
    bool setScanRate         (double rate) override;

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

    bool alloc(int njoints);
    bool dealloc();

public:
    //motor interfaces
    bool resetEncoderRaw(int j) override;
    bool resetEncodersRaw() override;
    bool setEncoderRaw(int j, double val) override;
    bool setEncodersRaw(const double* vals) override;
    bool getEncoderRaw(int j, double* v) override;
    bool getEncodersRaw(double* encs) override;
    bool getEncoderSpeedRaw(int j, double* sp) override;
    bool getEncoderSpeedsRaw(double* spds) override;
    bool getEncoderAccelerationRaw(int j, double* spds) override;
    bool getEncoderAccelerationsRaw(double* accs) override;

    bool getEncodersTimedRaw(double* encs, double* stamps) override;
    bool getEncoderTimedRaw(int j, double* encs, double* stamp) override;

    // POSITION CONTROL INTERFACE RAW
    bool getAxes(int* ax) override;
    bool positionMoveRaw(int j, double ref) override;
    bool positionMoveRaw(const double* refs) override;
    bool relativeMoveRaw(int j, double delta) override;
    bool relativeMoveRaw(const double* deltas) override;
    bool checkMotionDoneRaw(bool* flag) override;
    bool checkMotionDoneRaw(int j, bool* flag) override;
    bool setRefSpeedRaw(int j, double sp) override;
    bool setRefSpeedsRaw(const double* spds) override;
    bool setRefAccelerationRaw(int j, double acc) override;
    bool setRefAccelerationsRaw(const double* accs) override;
    bool getRefSpeedRaw(int j, double* ref) override;
    bool getRefSpeedsRaw(double* spds) override;
    bool getRefAccelerationRaw(int j, double* acc) override;
    bool getRefAccelerationsRaw(double* accs) override;
    bool stopRaw(int j) override;
    bool stopRaw() override;

    bool positionMoveRaw(const int n_joint, const int* joints, const double* refs) override;
    bool relativeMoveRaw(const int n_joint, const int* joints, const double* deltas) override;
    bool checkMotionDoneRaw(const int n_joint, const int* joints, bool* flags) override;
    bool setRefSpeedsRaw(const int n_joint, const int* joints, const double* spds) override;
    bool setRefAccelerationsRaw(const int n_joint, const int* joints, const double* accs) override;
    bool getRefSpeedsRaw(const int n_joint, const int* joints, double* spds) override;
    bool getRefAccelerationsRaw(const int n_joint, const int* joints, double* accs) override;
    bool stopRaw(const int n_joint, const int* joints) override;
    bool getTargetPositionRaw(const int joint, double* ref) override;
    bool getTargetPositionsRaw(double* refs) override;
    bool getTargetPositionsRaw(const int n_joint, const int* joints, double* refs) override;

    //IControlMode
    bool getControlModeRaw(int j, int* v) override;
    bool getControlModesRaw(int* v) override;
    bool getControlModesRaw(const int n_joint, const int* joints, int* modes) override;
    bool setControlModeRaw(const int j, const int mode) override;
    bool setControlModesRaw(const int n_joint, const int* joints, int* modes) override;
    bool setControlModesRaw(int* modes) override;

    //IVelocityControl
    bool velocityMoveRaw(int j, double sp) override;
    bool velocityMoveRaw(const double* sp) override;
    bool velocityMoveRaw(const int n_joint, const int* joints, const double* spds) override;
    bool getRefVelocityRaw(const int joint, double* ref) override;
    bool getRefVelocitiesRaw(double* refs) override;
    bool getRefVelocitiesRaw(const int n_joint, const int* joints, double* refs) override;

    bool getAxisNameRaw(int axis, std::string& name) override;
    bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type) override;
public:
    //Lidar2DDeviceBase
    bool acquireDataFromHW() override final;

public:
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif
