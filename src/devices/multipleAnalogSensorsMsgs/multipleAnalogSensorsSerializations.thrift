/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
  3: SensorMeasurements ThreeAxisMagnetometers;
  4: SensorMeasurements OrientationSensors;
  5: SensorMeasurements TemperatureSensors;
  6: SensorMeasurements SixAxisForceTorqueSensors;
  7: SensorMeasurements ContactLoadCellArrays;
  8: SensorMeasurements EncoderArrays;
  9: SensorMeasurements SkinPatches;
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
  3: list<SensorMetadata> ThreeAxisMagnetometers;
  4: list<SensorMetadata> OrientationSensors;
  5: list<SensorMetadata> TemperatureSensors;
  6: list<SensorMetadata> SixAxisForceTorqueSensors;
  7: list<SensorMetadata> ContactLoadCellArrays;
  8: list<SensorMetadata> EncoderArrays;
  9: list<SensorMetadata> SkinPatches;
}

service MultipleAnalogSensorsMetadata
{
  /**
   * Read the sensor metadata necessary to configure the MultipleAnalogSensorsClient device.
   */
  SensorRPCData getMetadata();
}
