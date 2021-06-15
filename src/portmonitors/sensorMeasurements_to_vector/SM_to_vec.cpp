/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "SM_to_vec.h"

#include <algorithm>
#include <cmath>

#include <yarp/os/LogComponent.h>
#include "SensorStreamingData.h"

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(SM2VEC,
                   "yarp.carrier.portmonitor.sensorMeasurements_to_vector",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}


bool SensorMeasurements_to_vector::create(const yarp::os::Property& options)
{
    return true;
}

void SensorMeasurements_to_vector::destroy()
{
}

bool SensorMeasurements_to_vector::setparam(const yarp::os::Property& params)
{
    return false;
}

bool SensorMeasurements_to_vector::getparam(yarp::os::Property& params)
{
    return false;
}

bool SensorMeasurements_to_vector::accept(yarp::os::Things& thing)
{
    SensorStreamingData* ssd = thing.cast_as<SensorStreamingData>();
    if(ssd == nullptr ||
       ssd->OrientationSensors.measurements.size() != 1 ||
       ssd->OrientationSensors.measurements[0].measurement.size() != 3 ||
       ssd->PositionSensors.measurements.size() != 1 ||
       ssd->PositionSensors.measurements[0].measurement.size() != 3)
    {
        yCError(SM2VEC, "SensorMeasurements_to_vector: received invalid data type!");
        return false;
    }

    out.resize(3 + 3);
    return true;
}

yarp::os::Things& SensorMeasurements_to_vector::update(yarp::os::Things& thing)
{
    SensorStreamingData* ssd = thing.cast_as<SensorStreamingData>();
    const size_t sensor_id = 0;
    if (ssd)
    {
        out[0] = ssd->PositionSensors.measurements[sensor_id].measurement[0];
        out[1] = ssd->PositionSensors.measurements[sensor_id].measurement[1];
        out[2] = ssd->PositionSensors.measurements[sensor_id].measurement[2];
        out[3] = ssd->OrientationSensors.measurements[sensor_id].measurement[0];
        out[4] = ssd->OrientationSensors.measurements[sensor_id].measurement[1];
        out[5] = ssd->OrientationSensors.measurements[sensor_id].measurement[2];
    }

    th.setPortWriter(&out);
    return th;
}
