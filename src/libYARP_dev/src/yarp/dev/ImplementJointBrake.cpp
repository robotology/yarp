/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementJointBrake.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

ImplementJointBrake::ImplementJointBrake(IJointBrakeRaw* r) :
    m_helper(nullptr),
    m_iraw(r)
{}

bool ImplementJointBrake::initialize(int size, const int* amap)
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

ImplementJointBrake::~ImplementJointBrake()
{
    uninitialize();
}

bool ImplementJointBrake::uninitialize()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementJointBrake::isJointBraked(int j, bool& braked) const
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k = castToMapper(m_helper)->toHw(j);
    return m_iraw->isJointBrakedRaw(k, braked);
}

ReturnValue ImplementJointBrake::setManualBrakeActive(int j, bool active)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k = castToMapper(m_helper)->toHw(j);
    return m_iraw->setManualBrakeActiveRaw(k, active);
}

ReturnValue ImplementJointBrake::setAutoBrakeEnabled(int j, bool enabled)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k = castToMapper(m_helper)->toHw(j);
    return m_iraw->setAutoBrakeEnabledRaw(k, enabled);
}

ReturnValue ImplementJointBrake::getAutoBrakeEnabled(int j, bool& enabled) const
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k = castToMapper(m_helper)->toHw(j);
    return m_iraw->getAutoBrakeEnabledRaw(k, enabled);
}
