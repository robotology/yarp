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

struct SensorMeasurementData {
  1: YarpVector measurement;
  2: double timestamp;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)

struct SensorMeasurementsData {
  1: list<SensorMeasurementData> measurements;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)

struct SensorStreamingData
{
  1: SensorMeasurementsData ThreeAxisGyroscopes;
  2: SensorMeasurementsData ThreeAxisLinearAccelerometers;
  3: SensorMeasurementsData ThreeAxisMagnetometers;
  4: SensorMeasurementsData OrientationSensors;
  5: SensorMeasurementsData TemperatureSensors;
  6: SensorMeasurementsData SixAxisForceTorqueSensors;
  7: SensorMeasurementsData ContactLoadCellArrays;
  8: SensorMeasurementsData EncoderArrays;
  9: SensorMeasurementsData SkinPatches;
  10: SensorMeasurementsData PositionSensors;
  11: SensorMeasurementsData LinearVelocitySensors;
  12: SensorMeasurementsData ThreeAxisAngularAccelerometers;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
