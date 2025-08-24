/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
  10: list<SensorMetadata> PositionSensors;
  11: list<SensorMetadata> LinearVelocitySensors;
  12: list<SensorMetadata> ThreeAxisAngularAccelerometers;
}

service MultipleAnalogSensorsMetadata
{
  /**
   * Read the sensor metadata necessary to configure the MultipleAnalogSensorsClient device.
   */
  SensorRPCData getMetadata();
}
