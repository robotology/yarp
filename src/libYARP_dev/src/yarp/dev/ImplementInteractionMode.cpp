/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/ImplementInteractionMode.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;

ImplementInteractionMode::ImplementInteractionMode(yarp::dev::IInteractionModeRaw *class_p) :
    m_iraw(class_p),
    m_helper(nullptr)
{;}


ImplementInteractionMode::~ImplementInteractionMode()
{
    uninitialize();
}

/**
 * Allocate memory for internal data
 * @param size the number of joints
 * @param amap axis map for this device wrapper
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementInteractionMode::initialize(int size, const int *amap)
{
    return initialize(size, amap, nullptr, nullptr);
}

bool ImplementInteractionMode::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);
    m_buffer_enums.resize(size);

    return true;
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementInteractionMode::uninitialize()
{
    if(m_helper != nullptr)
    {
        delete castToMapper(m_helper);
        m_helper = nullptr;
    }

    return true;
}

ReturnValue ImplementInteractionMode::getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(mode)

    int j = castToMapper(m_helper)->toHw(axis);
    return m_iraw->getInteractionModeRaw(j, mode);
}

ReturnValue ImplementInteractionMode::getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(modes)
    POINTERCHECK(joints)
    JOINTSIDCHECK(n_joints, joints);

    for (int i = 0; i < n_joints; i++)
    {
        m_buffer_ints[i] = castToMapper(m_helper)->toHw(joints[i]);
    }
    ReturnValue ret = m_iraw->getInteractionModesRaw(n_joints, m_buffer_ints.data(), modes);

    return ret;
}

ReturnValue ImplementInteractionMode::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(modes)

    ReturnValue ret = m_iraw->getInteractionModesRaw(m_buffer_enums.data());
    if(!ret)
    {
        return ret;
    }
    for(int idx=0; idx<castToMapper(m_helper)->axes(); idx++)
    {
        int j = castToMapper(m_helper)->toUser(idx);
        modes[j] = m_buffer_enums[idx];
    }

    return ReturnValue_ok;
}

ReturnValue ImplementInteractionMode::setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)
{
    std::lock_guard lock(m_imp_mutex);

    int j = castToMapper(m_helper)->toHw(axis);
    return m_iraw->setInteractionModeRaw(j, mode);
}

ReturnValue ImplementInteractionMode::setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints)
    POINTERCHECK(modes)
    JOINTSIDCHECK(n_joints, joints);

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }
    ReturnValue ret = m_iraw->setInteractionModesRaw(n_joints, m_buffer_ints.data(), modes);

    return ret;
}

ReturnValue ImplementInteractionMode::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(modes)

    for(int idx=0; idx< castToMapper(m_helper)->axes(); idx++)
    {
        int j = castToMapper(m_helper)->toHw(idx);
        m_buffer_enums[j] = modes[idx];
    }

    ReturnValue ret = m_iraw->setInteractionModesRaw(m_buffer_enums.data());

    return ret;
}
