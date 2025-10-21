/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2008 Giacomo Spigler
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/dev/PolyDriver.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define round(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x)-0.5))
#define FABS(x) (x >= 0 ? x : -x)

#define SSC32 1
#define MINISSC 2
#define PONTECHSV203X 3
#define MONDOTRONICSMI 4
#define POLOLUUSB16 6
#define PICOPIC 7


using namespace yarp::os;
using namespace yarp::dev;


bool movessc32(int j, double ref, double* positions, double* speeds, ISerialDevice* serial);
bool moveminissc(int j, double ref, double* positions, double* speeds, ISerialDevice* serial);
bool movepontech(int j, double ref, double* positions, double* speeds, ISerialDevice* serial);
bool movemondotronic(int j, double ref, double* positions, double* speeds, ISerialDevice* serial);
bool movepololu(int j, double ref, double* positions, double* speeds, ISerialDevice* serial);
bool movepicopic(int j, double ref, double* positions, double* speeds, ISerialDevice* serial);

/**
* @ingroup dev_impl_motor
*
* \brief `SerialServoBoard`: Documentation to be added
*/
class SerialServoBoard : public DeviceDriver, public IPositionControl
{
public:
    int servoboard;

    ISerialDevice* serial;

    PolyDriver dd;

    bool (*move)(int j, double ref, double* positions, double* speeds, ISerialDevice* serial);

    double* positions;
    double* speeds;

    yarp::dev::ReturnValue getAxes(int* ax) override;
    yarp::dev::ReturnValue positionMove(int j, double ref) override;
    yarp::dev::ReturnValue positionMove(const double* refs) override;
    yarp::dev::ReturnValue relativeMove(int j, double delta) override;
    yarp::dev::ReturnValue relativeMove(const double* deltas) override;
    yarp::dev::ReturnValue checkMotionDone(int j, bool* flag) override;
    yarp::dev::ReturnValue checkMotionDone(bool* flag) override;
    yarp::dev::ReturnValue setTrajSpeed(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeeds(const double* spds) override;
    yarp::dev::ReturnValue setTrajAcceleration(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerations(const double* accs) override;
    yarp::dev::ReturnValue getTrajSpeed(int j, double* ref) override;
    yarp::dev::ReturnValue getTrajSpeeds(double* spds) override;
    yarp::dev::ReturnValue getTrajAcceleration(int j, double* acc) override;
    yarp::dev::ReturnValue getTrajAccelerations(double* accs) override;
    yarp::dev::ReturnValue stop(int j) override;
    yarp::dev::ReturnValue stop() override;
    yarp::dev::ReturnValue positionMove(const int n_joint, const int* joints, const double* refs) override;
    yarp::dev::ReturnValue relativeMove(const int n_joint, const int* joints, const double* deltas) override;
    yarp::dev::ReturnValue checkMotionDone(const int n_joint, const int* joints, bool* flags) override;
    yarp::dev::ReturnValue setTrajSpeeds(const int n_joint, const int* joints, const double* spds) override;
    yarp::dev::ReturnValue setTrajAccelerations(const int n_joint, const int* joints, const double* accs) override;
    yarp::dev::ReturnValue getTrajSpeeds(const int n_joint, const int* joints, double* spds) override;
    yarp::dev::ReturnValue getTrajAccelerations(const int n_joint, const int* joints, double* accs) override;
    yarp::dev::ReturnValue stop(const int n_joint, const int* joints) override;
    yarp::dev::ReturnValue getTargetPosition(const int,double *);
    yarp::dev::ReturnValue getTargetPositions(double *);
    yarp::dev::ReturnValue getTargetPositions(const int,const int *,double *);

    bool open(Searchable& config) override;
    bool close() override;
};
