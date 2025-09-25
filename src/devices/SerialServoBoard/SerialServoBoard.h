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

    bool getAxes(int* ax) override;
    bool positionMove(int j, double ref) override;
    bool positionMove(const double* refs) override;
    bool relativeMove(int j, double delta) override;
    bool relativeMove(const double* deltas) override;
    bool checkMotionDone(int j, bool* flag) override;
    bool checkMotionDone(bool* flag) override;
    bool setTrajSpeed(int j, double sp) override;
    bool setTrajSpeeds(const double* spds) override;
    bool setTrajAcceleration(int j, double acc) override;
    bool setTrajAccelerations(const double* accs) override;
    bool getTrajSpeed(int j, double* ref) override;
    bool getTrajSpeeds(double* spds) override;
    bool getTrajAcceleration(int j, double* acc) override;
    bool getTrajAccelerations(double* accs) override;
    bool stop(int j) override;
    bool stop() override;
    bool positionMove(const int n_joint, const int* joints, const double* refs) override;
    bool relativeMove(const int n_joint, const int* joints, const double* deltas) override;
    bool checkMotionDone(const int n_joint, const int* joints, bool* flags) override;
    bool setTrajSpeeds(const int n_joint, const int* joints, const double* spds) override;
    bool setTrajAccelerations(const int n_joint, const int* joints, const double* accs) override;
    bool getTrajSpeeds(const int n_joint, const int* joints, double* spds) override;
    bool getTrajAccelerations(const int n_joint, const int* joints, double* accs) override;
    bool stop(const int n_joint, const int* joints) override;

    bool open(Searchable& config) override;
    bool close() override;
};
