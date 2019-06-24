/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_DEV_MULTIPLEANALOGSENSORSSERVER_MULTIPLEANALOGSENSORSSERVER_H
#define YARP_DEV_MULTIPLEANALOGSENSORSSERVER_MULTIPLEANALOGSENSORSSERVER_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

// Thrift-generated classes
#include "SensorStreamingData.h"
#include "MultipleAnalogSensorsMetadata.h"


/**
 * @ingroup dev_impl_wrapper
 *
 * \brief The server side of the MultipleAnalogSensorsClient, useful to expose device implementing MultipleAnalogSensors interfaces over the YARP network.
 *
 * | YARP device name |
 * |:-----------------:|
 * | `multipleanalogsensorsserver` |
 *
 * The parameters accepted by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   -           | Yes                         | Prefix of the port opened by this device                          | MUST start with a '/' character |
 * | period         |      -         | int     | ms             |   -           | Yes                          | Refresh period of the broadcasted values in ms                    |  |
 */
class MultipleAnalogSensorsServer :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public MultipleAnalogSensorsMetadata
{
    double m_periodInS{0.01};
    std::string m_streamingPortName;
    std::string m_RPCPortName;
    yarp::os::BufferedPort<SensorStreamingData> m_streamingPort;
    yarp::os::Port m_rpcPort;
    // Generic vector buffer
    yarp::sig::Vector m_buffer;

    // Interface of the wrapped device
    yarp::dev::IThreeAxisGyroscopes* m_iThreeAxisGyroscopes{nullptr};
    yarp::dev::IThreeAxisLinearAccelerometers* m_iThreeAxisLinearAccelerometers{nullptr};
    yarp::dev::IThreeAxisMagnetometers* m_iThreeAxisMagnetometers{nullptr};
    yarp::dev::IOrientationSensors* m_iOrientationSensors{nullptr};
    yarp::dev::ITemperatureSensors* m_iTemperatureSensors{nullptr};
    yarp::dev::ISixAxisForceTorqueSensors* m_iSixAxisForceTorqueSensors{nullptr};
    yarp::dev::IContactLoadCellArrays* m_iContactLoadCellArrays{nullptr};
    yarp::dev::IEncoderArrays* m_iEncoderArrays{nullptr};
    yarp::dev::ISkinPatches* m_iSkinPatches{nullptr};

    // Metadata to be server via the RPC port
    SensorRPCData m_sensorMetadata;
    bool populateAllSensorsMetadata();
    template<typename Interface>
    bool populateSensorsMetadata(Interface * wrappedDeviceInterface,
                                 std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                                 size_t (Interface::*getNrOfSensorsMethodPtr)() const,
                                 bool (Interface::*getNameMethodPtr)(size_t, std::string&) const,
                                 bool (Interface::*getFrameNameMethodPtr)(size_t, std::string&) const);
    template<typename Interface>
    bool populateSensorsMetadataNoFrameName(Interface * wrappedDeviceInterface,
                                            std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                                            size_t (Interface::*getNrOfSensorsMethodPtr)() const,
                                            bool (Interface::*getNameMethodPtr)(size_t, std::string&) const);

    template<typename Interface>
    bool genericStreamData(Interface* wrappedDeviceInterface,
                           const std::vector< SensorMetadata >& metadataVector,
                           std::vector< SensorMeasurement >& streamingDataVector,
                           yarp::dev::MAS_status (Interface::*getStatusMethodPtr)(size_t) const,
                           bool (Interface::*getMeasureMethodPtr)(size_t, yarp::sig::Vector&, double&) const);

public:
    MultipleAnalogSensorsServer();
    ~MultipleAnalogSensorsServer();

    /* DevideDriver methods */
    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    /* IMultipleWrapper methods */
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    /* RateThread methods */
    void threadRelease() override;
    void run() override;

    /* MultipleAnalogSensorsMetadata */
    SensorRPCData getMetadata() override;
};

#endif

