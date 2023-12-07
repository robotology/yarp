/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ANALOGWRAPPER_ANALOGWRAPPER_H
#define YARP_DEV_ANALOGWRAPPER_ANALOGWRAPPER_H

 //#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/api.h>


#define DEFAULT_THREAD_PERIOD 20 //ms

class AnalogServerHandler;
class AnalogPortEntry;

/**
 *  @ingroup dev_impl_wrapper dev_impl_deprecated
 *
 * \brief `analogServer` *deprecated*: Device that expose an AnalogSensor (using the IAnalogSensor interface) on the YARP network.
 *
 * \section analogServer_device_parameters Description of input parameters
 *
 * It reads the data from an analog sensor and sends them on one or more ports.
 * It creates one rpc port and its related handler for every output port..
 *
 *
 * Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   -           | Yes                         | full name of the port opened by the device, like /robotName/part/ | MUST start with a '/' character |
 * | period         |      -         | int     | ms             |   20          | No                          | refresh period of the broadcasted values in ms                    | optional, default 20ms |
 * | subdevice      |      -         | string  | -              |   -           | alternative to netwok group | name of the subdevice to instantiate                              | when used, parameters for the subdevice must be provided as well |
 * | ports          |      -         | group   | -              |   -           | alternative to subdevice    | this is expected to be a group parameter in xml format, a list in .ini file format. SubParameter are mandatory if this is used| - |
 * | -              | portName_1     | 4 * int | channel number |   -           |   if ports is used          | describe how to match subdevice_1 channels with the wrapper channels. First 2 numbers indicate first/last wrapper channel, last 2 numbers are subdevice first/last channel | The channels are intended to be consequent |
 * | -              |      ...       | 4 * int | channel number |   -           |   if ports is used          | same as above                                                     | The channels are intended to be consequent |
 * | -              | portName_n     | 4 * int | channel number |   -           |   if ports is used          | same as above                                                     | The channels are intended to be consequent |
 * | -              | channels       |  int    |  -             |   -           |   if ports is used          | total number of channels handled by the wrapper                   | MUST match the sum of channels from all the ports |
 *
 * for example:
 *              3forces + 3torques + temperature + 3forces + 3torques + temperature ... -> offset 0, padding 1 (the temperature entry is one sample to skip)
 *              3forces + 3torques + temperature + gravity + 3forces + 3torques + temperature + gravity ... -> offset 0, padding is 4 (1 temperature + 3 for gravity)
 *              temperature + 3forces + 3torques + temperature + 3forces + 3torques ... -> offset 1, padding is 0 (the F/T are the last ones)
 *              temperature + 3forces + 3torques + gravity + temperature + 3forces + 3torques + gravity ... -> offset 1, padding is 3 (skip 1 sample, get F/T and skip remaining 3)
 *
 * Some example of configuration files:
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}
 *  device analogServer
 *  subdevice fakeAnalogSensor
 *  name /myAnalogSensor
 *
 * ** parameter for 'fakeAnalogSensor' subdevice follows here **
 * ...
 * \endcode
 *
 * Configuration file using .ini format, using ports keyword
 *
 * \code{.unparsed}
 *  device analogServer
 *  name  /myAnalogServer
 *  period 20
 *  ports (FirstSetOfChannels SecondSetOfChannels ThirdSetOfChannels)
 *  channels 1344
 *  FirstSetOfChannels  0   191  0 191
 *  SecondSetOfChannels 192 575  0 383
 *  ThirdSetOfChannels  576 1343 0 767
 *
 * \endcode
 *
 * Configuration file using .xml format.
 *
 * \code{.xml}
 *    <device name="/myAnalogServer" type="analogServer">
 *        <param name="period">   20   </param>
 *        <param name="channels"> 1344 </param>
 *
 *        <paramlist name="ports">
 *            <elem name="FirstSetOfChannels">  0   191  0 191</elem>
 *            <elem name="SecondSetOfChannels"> 192 575  0 383</elem>
 *            <elem name="ThirdSetOfChannels">  576 1343 0 767</elem>
 *        </paramlist>
 *
 *        <action phase="startup" level="5" type="attach">
 *            <paramlist name="networks">
 *                <!-- The param value must match the device name in the corresponding analogSensor config file. AnalogWrapper is able to attach to only one subdevice. -->
 *                <elem name="myAnalogSensor">  my_analog_sensor </elem>
 *            </paramlist>
 *        </action>
 *
 *        <action phase="shutdown" level="5" type="detach" />
 *    </device>
 * \endcode
 *
 * */

class AnalogWrapper :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper
{
public:
    AnalogWrapper();

    AnalogWrapper(const AnalogWrapper&) = delete;
    AnalogWrapper(AnalogWrapper&&) = delete;
    AnalogWrapper& operator=(const AnalogWrapper&) = delete;
    AnalogWrapper& operator=(AnalogWrapper&&) = delete;

    ~AnalogWrapper() override;

    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    yarp::os::Bottle getOptions();

    /**
      * Specify which analog sensor this thread has to read from.
      */
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IAnalogSensor *s);
    void detach();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    std::string streamingPortName;
    std::string rpcPortName;
    yarp::dev::IAnalogSensor *analogSensor_p{nullptr};   // the analog sensor to read from
    std::vector<AnalogPortEntry> analogPorts;   // the list of output ports
    std::vector<AnalogServerHandler*> handlers; // the list of rpc port handlers
    yarp::os::Stamp lastStateStamp;             // the last reading time stamp
    yarp::sig::Vector lastDataRead;             // the last vector of data read from the attached IAnalogSensor
    int _rate{DEFAULT_THREAD_PERIOD};
    std::string sensorId;

    bool ownDevices{false};
    // Open the wrapper only, the attach method needs to be called before using it
    bool openDeferredAttach(yarp::os::Searchable &prop);

    // For the simulator, if a subdevice parameter is given to the wrapper, it will
    // open it and attach to it immediately.
    yarp::dev::PolyDriver *subDeviceOwned{nullptr};
    bool openAndAttachSubDevice(yarp::os::Searchable &prop);

    bool initialize_YARP(yarp::os::Searchable &config);
    void setHandlers();
    void removeHandlers();

    // Function used when there is only one output port
    bool createPort(const char* name, int rate=20);
    // Function used when one or more output ports are specified
    bool createPorts(const std::vector<AnalogPortEntry>& _analogPorts, int rate=20);
};

#endif // YARP_DEV_ANALOGWRAPPER_ANALOGWRAPPER_H
