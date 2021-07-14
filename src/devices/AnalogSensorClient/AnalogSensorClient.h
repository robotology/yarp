/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ANALOGSENSORCLIENT_ANALOGSENSORCLIENT_H
#define YARP_DEV_ANALOGSENSORCLIENT_ANALOGSENSORCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>


const int ANALOG_TIMEOUT=100; //ms


class InputPortProcessor : public yarp::os::BufferedPort<yarp::sig::Vector>
{
    yarp::sig::Vector lastVector;
    std::mutex mutex;
    yarp::os::Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    int state;
    int count;

public:

    inline void resetStat();

    InputPortProcessor();

    using yarp::os::BufferedPort<yarp::sig::Vector>::onRead;
    void onRead(yarp::sig::Vector &v) override;

    inline int getLast(yarp::sig::Vector &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    int getState();

    int getChannels();
};

/*
 * The client side of any IAnalogSensor capable device.
 * Still single thread! concurrent access is unsafe.
 */

/**
 * @ingroup dev_impl_network_clients dev_impl_deprecated
 *
 * \brief `analogsensorclient` *deprecated*: Device that reads an AnalogSensor (using the IAnalogSensor interface) from the YARP network.
 *
 * \section analogsensorclient_device_parameters Description of input parameters
 *
 * This device will connect to a port opened by the AnalogWrapper device and read the data broadcasted
 * making them available to use for the user application. It also made available some function to check and control the state of the remote sensor.
 *
 * Parameters accepted in the config argument of the open method:
 * | Parameter name | Type   | Units | Default Value | Required  | Description   | Notes |
 * |:--------------:|:------:|:-----:|:-------------:|:--------: |:-------------:|:-----:|
 * | local          | string |       |               | Yes       | full name if the port opened by the device  | must start with a '/' character |
 * | remote         | string |       |               | Yes       | full name of the port the device need to connect to | must start with a '/' character |
 * | carrier        | string |       | udp           | No        | type of carrier to use, like tcp, udp and so on ...  | - |
 *
 *  The device will create a port with name <local> and will connect to a port colled <remote> at startup,
 * ex: <b> /myModule/left_arm/ForceTorque </b>, and will connect to a port called <b> /icub/left_arm/ForceTorque<b>.
 **/
class AnalogSensorClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IAnalogSensor
{
protected:
    InputPortProcessor inputPort;
    yarp::os::Port rpcPort;
    std::string local;
    std::string remote;
    yarp::os::Stamp lastTs; //used by IPreciselyTimed
    std::string robotName;
    std::string deviceId;
    std::string sensorType;
    std::string portPrefix;

    void  removeLeadingTrailingSlashesOnly(std::string &name);

public:
    AnalogSensorClient() = default;
    AnalogSensorClient(const AnalogSensorClient&) = delete;
    AnalogSensorClient(AnalogSensorClient&&) = delete;
    AnalogSensorClient& operator=(const AnalogSensorClient&) = delete;
    AnalogSensorClient& operator=(AnalogSensorClient&&) = delete;
    ~AnalogSensorClient() override = default;

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IAnalogSensor methods*/
    int read(yarp::sig::Vector &out) override;

    /* Check the state value of a given channel.
    * @param ch: channel number.
    * @return status.
    */
    int getState(int ch) override;

    /* Get the number of channels of the sensor.
     * @return number of channels (0 in case of errors).
     */
    int getChannels() override;

    /* Calibrates the whole sensor.
     * @return status.
     */
    int calibrateSensor() override;

    /* Calibrates the whole sensor, using a vector of calibration values.
     * @param value: a vector of calibration values.
     * @return status.
     */
    int calibrateSensor(const yarp::sig::Vector& value) override;

    /* Calibrates one single channel.
     * @param ch: channel number.
     * @return status.
     */
    int calibrateChannel(int ch) override;

    /* Calibrates one single channel.
     * @param ch: channel number.
     * @param value: calibration value.
     * @return status.
     */
    int calibrateChannel(int ch, double value) override;

    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp() override;
};

#endif // YARP_DEV_ANALOGSENSORCLIENT_ANALOGSENSORCLIENT_H
