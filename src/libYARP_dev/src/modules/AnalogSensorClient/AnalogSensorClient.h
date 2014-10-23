/*
* Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
* Author:  Alberto Cardellino
* email:   alberto.cardellino@iit.it
* website: www.robotcub.org
* Permission is granted to copy, distribute, and/or modify this program
* under the terms of the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
*
* A copy of the license can be found at
* http://www.robotcub.org/icub/license/gpl.txt
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details
*/

#ifndef ANALOGSENSORCLIENT_H
#define ANALOGSENSORCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class AnalogSensorClient;
    }
}

/**
*  @ingroup yarp_dev_modules
*  \defgroup analogsensorclient analogsensorclient
*
*
*  This device will connect to the proper analogServer and read the data broadcasted making them available to use for the user application. It also made available some function to check and control the state of the remote sensor.
*
* Parameters accepted in the config argument of the open method:
* | Parameter name | Type   | Units | Default Value | Required  | Description   | Notes |
* |:--------------:|:------:|:-----:|:-------------:|:--------: |:-------------:|:-----:|
* | local          | string |       |               | Yes       | full name if the port opened by the device  | must start with a '/' character |
* | remote         | string |       |               | Yes       | full name of the port the device need to connect to | must start with a '/' character |
* | period         | int    | ms    | 20            | No        | Publication period (in ms) of the sensor reading on the Can Bus | - |
*  The device will create a port with name <local> and will connect to a port colled <remote> at startup,
* ex: <b> /myModule/left_arm/ForceTorque </b>, and will connect to a port called <b> /icub/left_arm/ForceTorque<b>.
*
**/

#define DEFAULT_THREAD_PERIOD 20 //ms
const int ANALOG_TIMEOUT=100; //ms

class InputPortProcessor : public yarp::os::BufferedPort<yarp::sig::Vector>
{
    yarp::sig::Vector lastVector;
    yarp::os::Semaphore mutex;
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

    virtual void onRead(yarp::sig::Vector &v);

    inline int getLast(yarp::sig::Vector &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    int getState();

    int getChannels();
};

/**
* @ingroup dev_impl_wrapper
*
* The client side of any IAnalogSensor capable device.
* Still single thread! concurrent access is unsafe.
*/
class yarp::dev::AnalogSensorClient: public DeviceDriver,
                          public IPreciselyTimed,
                          public IAnalogSensor
{
protected:
    InputPortProcessor inputPort;
    yarp::os::Port rpcPort;
    yarp::os::ConstString local;
    yarp::os::ConstString remote;
    yarp::os::Stamp lastTs; //used by IPreciselyTimed
    std::string robotName;
    std::string deviceId;
    std::string sensorType;
    std::string portPrefix;
    int _rate;

    void  removeLeadingTrailingSlashesOnly(std::string &name);

public:

    /* DevideDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    /* IAnalogSensor methods*/
    int read(yarp::sig::Vector &out);

    /* Check the state value of a given channel.
    * @param ch: channel number.
    * @return status.
    */
    int getState(int ch);

    /* Get the number of channels of the sensor.
     * @return number of channels (0 in case of errors).
     */
    int getChannels();

    /* Calibrates the whole sensor.
     * @return status.
     */
    int calibrateSensor();

    /* Calibrates the whole sensor, using a vector of calibration values.
     * @param value: a vector of calibration values.
     * @return status.
     */
    int calibrateSensor(const yarp::sig::Vector& value);

    /* Calibrates one single channel.
     * @param ch: channel number.
     * @return status.
     */
    int calibrateChannel(int ch);

    /* Calibrates one single channel.
     * @param ch: channel number.
     * @param value: calibration value.
     * @return status.
     */
    int calibrateChannel(int ch, double value);

    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp();
};

#endif // ANALOGSENSORCLIENT_H
