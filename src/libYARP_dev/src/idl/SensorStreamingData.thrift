/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct YarpVector {
  1: list<double> content;
} (
  yarp.name = "yarp::sig::Vector"
  yarp.includefile="yarp/sig/Vector.h"
)

struct SensorMeasurement {
  1: YarpVector measurement;
  2: double timestamp;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)

struct SensorMeasurements {
  1: list<SensorMeasurement> measurements;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)

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
  10: SensorMeasurements PositionSensors;
  11: SensorMeasurements LinearVelocitySensors;
  12: SensorMeasurements ThreeAxisAngularAccelerometers;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
