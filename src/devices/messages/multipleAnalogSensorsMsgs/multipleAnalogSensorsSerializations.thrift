/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct YarpVector {
  1: list<double> content;
} (
  yarp.name = "yarp::sig::Vector"
  yarp.includefile="yarp/sig/Vector.h"
)

struct SensorMeasurement {
  1: YarpVector measurement;
  2: double timestamp;
}

struct SensorMeasurements {
  1: list<SensorMeasurement> measurements;
}

struct SensorStreamingData
{
  1: SensorMeasurements ThreeAxisGyroscopes;
  2: SensorMeasurements ThreeAxisLinearAccelerometers;
  3: SensorMeasurements ThreeAxisAngularAccelerometers;
  4: SensorMeasurements ThreeAxisMagnetometers;
  5: SensorMeasurements OrientationSensors;
  6: SensorMeasurements TemperatureSensors;
  7: SensorMeasurements SixAxisForceTorqueSensors;
  8: SensorMeasurements ContactLoadCellArrays;
  9: SensorMeasurements EncoderArrays;
  10: SensorMeasurements SkinPatches;
  11: SensorMeasurements PositionSensors;
  12: SensorMeasurements LinearVelocitySensors;
}

struct SensorMetadata {
  1: string name;
  2: string frameName;
  3: string additionalMetadata;
}

struct SensorRPCData
{
  1: list<SensorMetadata> ThreeAxisGyroscopes;
  2: list<SensorMetadata> ThreeAxisLinearAccelerometers;
  3: list<SensorMetadata> ThreeAxisAngularAccelerometers;
  4: list<SensorMetadata> ThreeAxisMagnetometers;
  5: list<SensorMetadata> OrientationSensors;
  6: list<SensorMetadata> TemperatureSensors;
  7: list<SensorMetadata> SixAxisForceTorqueSensors;
  8: list<SensorMetadata> ContactLoadCellArrays;
  9: list<SensorMetadata> EncoderArrays;
  10: list<SensorMetadata> SkinPatches;
  11: list<SensorMetadata> PositionSensors;
  12: list<SensorMetadata> LinearVelocitySensors;
}

service MultipleAnalogSensorsMetadata
{
  /**
   * Read the sensor metadata necessary to configure the MultipleAnalogSensorsClient device.
   */
  SensorRPCData getMetadata();
}
