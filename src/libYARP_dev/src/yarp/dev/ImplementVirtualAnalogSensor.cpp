/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ImplementVirtualAnalogSensor.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include "yarp/sig/Vector.h"
#include <cstdio>

using namespace yarp::dev;

ImplementVirtualAnalogSensor::ImplementVirtualAnalogSensor(IVirtualAnalogSensorRaw *virt)
{
    iVASRaw = virt;
    m_helper=nullptr;
}

ImplementVirtualAnalogSensor::~ImplementVirtualAnalogSensor()
{
    uninitialize();
}

bool ImplementVirtualAnalogSensor::initialize(int size, const int *amap, const double *userToRaw)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, nullptr, nullptr, nullptr, nullptr, userToRaw, nullptr));
    yAssert (m_helper != nullptr);

    return true;
}

bool ImplementVirtualAnalogSensor::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }
    return true;
}

yarp::dev::VAS_status ImplementVirtualAnalogSensor::getVirtualAnalogSensorStatus(int ch)
{
    if (ch >= castToMapper(m_helper)->axes())
    {
        return yarp::dev::VAS_status::VAS_ERROR;
    }
    else
    {
        int ch_raw = castToMapper(m_helper)->toHw(ch);
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
    castToMapper(m_helper)->voltageV2S(measure.data(), measure_raw.data());
    bool ret = iVASRaw->updateVirtualAnalogSensorMeasureRaw(measure_raw);
    return ret;
}

bool ImplementVirtualAnalogSensor::updateVirtualAnalogSensorMeasure(int ch, double &measure)
{
    int ch_raw;
    bool ret;
    double measure_raw;
    castToMapper(m_helper)->voltageV2S(measure, ch, measure_raw, ch_raw);
    ret = iVASRaw->updateVirtualAnalogSensorMeasureRaw(ch_raw, measure_raw);
    return ret;
}
