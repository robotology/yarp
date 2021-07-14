/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ImplementVirtualAnalogSensor.h"
#include <yarp/dev/ControlBoardHelper.h>
#include "yarp/sig/Vector.h"
#include <cstdio>

using namespace yarp::dev;
#define JOINTIDCHECK if (ch >= castToMapper(helper)->axes()){yError("channel id out of bound"); return false;}

ImplementVirtualAnalogSensor::ImplementVirtualAnalogSensor(IVirtualAnalogSensorRaw *virt)
{
    iVASRaw = virt;
    helper=nullptr;
}

ImplementVirtualAnalogSensor::~ImplementVirtualAnalogSensor()
{
    uninitialize();
}

bool ImplementVirtualAnalogSensor::initialize(int size, const int *amap, const double *userToRaw)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap, nullptr, nullptr, nullptr, nullptr, userToRaw, nullptr));
    yAssert (helper != nullptr);

    return true;
}

bool ImplementVirtualAnalogSensor::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }
    return true;
}

yarp::dev::VAS_status ImplementVirtualAnalogSensor::getVirtualAnalogSensorStatus(int ch)
{
    if (ch >= castToMapper(helper)->axes())
    {
        return yarp::dev::VAS_status::VAS_ERROR;
    }
    else
    {
        int ch_raw = castToMapper(helper)->toHw(ch);
        return iVASRaw->getVirtualAnalogSensorStatusRaw(ch_raw);
    }
}

int  ImplementVirtualAnalogSensor::getVirtualAnalogSensorChannels()
{
    return iVASRaw->getVirtualAnalogSensorChannelsRaw();
}

bool ImplementVirtualAnalogSensor::updateVirtualAnalogSensorMeasure(yarp::sig::Vector &measure)
{
    yarp::sig::Vector measure_raw;
    castToMapper(helper)->voltageV2S(measure.data(), measure_raw.data());
    bool ret = iVASRaw->updateVirtualAnalogSensorMeasureRaw(measure_raw);
    return ret;
}

bool ImplementVirtualAnalogSensor::updateVirtualAnalogSensorMeasure(int ch, double &measure)
{
    JOINTIDCHECK
    int ch_raw;
    bool ret;
    double measure_raw;
    castToMapper(helper)->voltageV2S(measure, ch, measure_raw, ch_raw);
    ret = iVASRaw->updateVirtualAnalogSensorMeasureRaw(ch_raw, measure_raw);
    return ret;
}
