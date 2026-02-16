/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ImplementAxisInfo.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include <cstdio>
using namespace yarp::dev;

////////////////////////
// Encoder Interface Timed Implementation
ImplementAxisInfo::ImplementAxisInfo(yarp::dev::IAxisInfoRaw *y)
{
    m_iraw=y;
    m_helper = nullptr;
}

ImplementAxisInfo::~ImplementAxisInfo()
{
    uninitialize();
}

bool ImplementAxisInfo::initialize(int size, const int *amap)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementAxisInfo::uninitialize()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }



    return true;
}

ReturnValue ImplementAxisInfo::getAxes(int* ax)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(ax)

    ReturnValue ret=ReturnValue_ok;
    (*ax) = castToMapper(m_helper)->axes();
    return ret;
}

ReturnValue ImplementAxisInfo::getAxisName(int axis, std::string& name)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(axis);

    ReturnValue ret;
    int k = castToMapper(m_helper)->toHw(axis);
    ret = m_iraw->getAxisNameRaw(k, name);
    return ret;
}

ReturnValue ImplementAxisInfo::getJointType(int axis, yarp::dev::JointTypeEnum& type)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(axis);

    ReturnValue ret;
    int k = castToMapper(m_helper)->toHw(axis);
    ret = m_iraw->getJointTypeRaw(k, type);
    return ret;
}
