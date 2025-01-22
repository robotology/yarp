/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_DEV_MULTIPLEANALOGSENSORSSERVER_MULTIPLEANALOGSENSORSSERVER_H
#define YARP_DEV_MULTIPLEANALOGSENSORSSERVER_MULTIPLEANALOGSENSORSSERVER_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

// Thrift-generated classes
#include "SensorStreamingData.h"
#include "MultipleAnalogSensorsMetadata.h"

#include "MultipleAnalogSensorsServer_ParamsParser.h"

/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `multipleanalogsensorsserver`: The server side of the MultipleAnalogSensorsClient, useful to expose device implementing MultipleAnalogSensors interfaces over the YARP network.
 * This device opens two ports: /${name}/measures:o that streams the data of the sensors, and /${name}/rpc:o that is a YARP RPC port that exposes the metadata.
 * The data on the /${name}/measures:o is streamed every ${period} milliseconds, and an envelope to each data is added with a timestamp obtained by calling the
 * yarp::os::Time::now() method when the message is written on the port.
 *
 * Parameters required by this device are shown in class: MultipleAnalogSensorsServer_ParamsParser
 */
class MultipleAnalogSensorsServer :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public MultipleAnalogSensorsMetadata,
        public MultipleAnalogSensorsServer_ParamsParser
{
    double m_periodInS{0.01};
    yarp::os::Stamp m_stamp;
    std::string m_streamingPortName;
    std::string m_RPCPortName;
    yarp::os::BufferedPort<SensorStreamingData> m_streamingPort;
    yarp::os::Port m_rpcPort;
    // Generic vector buffer
    yarp::sig::Vector m_buffer;

    // Interface of the wrapped device
    yarp::dev::IThreeAxisGyroscopes* m_iThreeAxisGyroscopes{nullptr};
    yarp::dev::IThreeAxisLinearAccelerometers* m_iThreeAxisLinearAccelerometers{nullptr};
    yarp::dev::IThreeAxisAngularAccelerometers* m_iThreeAxisAngularAccelerometers{nullptr};
    yarp::dev::IThreeAxisMagnetometers* m_iThreeAxisMagnetometers{nullptr};
    yarp::dev::IPositionSensors* m_iPositionSensors{nullptr};
    yarp::dev::ILinearVelocitySensors* m_iLinearVelocitySensors{nullptr};
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


    // Helper methods to resize measure buffers
    template<typename Interface>
    bool resizeMeasureVectors(Interface* wrappedDeviceInterface,
                              const std::vector< SensorMetadata >& metadataVector,
                              std::vector< SensorMeasurement >& streamingDataVector,
                              size_t (Interface::*getMeasureSizePtr)(size_t) const);
    template<typename Interface>
    bool resizeMeasureVectors(Interface* wrappedDeviceInterface,
                              const std::vector< SensorMetadata >& metadataVector,
                              std::vector< SensorMeasurement >& streamingDataVector,
                              size_t measureSize);
    bool resizeAllMeasureVectors(SensorStreamingData& streamingData);


    // Helper method used to copy the sensor measure to the measure buffers
    template<typename Interface>
    bool genericStreamData(Interface* wrappedDeviceInterface,
                           const std::vector< SensorMetadata >& metadataVector,
                           std::vector< SensorMeasurement >& streamingDataVector,
                           yarp::dev::MAS_status (Interface::*getStatusMethodPtr)(size_t) const,
                           bool (Interface::*getMeasureMethodPtr)(size_t, yarp::sig::Vector&, double&) const,
                           const char* sensorType);


public:
    MultipleAnalogSensorsServer();
    ~MultipleAnalogSensorsServer();

    /* DevideDriver methods */
    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    /* IWrapper methods */
    bool attach(yarp::dev::PolyDriver* p) override;
    bool detach() override;

    /* RateThread methods */
    void threadRelease() override;
    void run() override;

    /* MultipleAnalogSensorsMetadata */
    SensorRPCData getMetadata() override;
};

#endif
