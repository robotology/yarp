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

namespace {
YARP_LOG_COMPONENT(IMPLEMENT_LAYER_COMPONENT, "yarp.ImplementInteractionMode")
}

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

ReturnValue ImplementInteractionMode::getInteractionMode(int axis, yarp::dev::InteractionModeEnum& mode)
{
    std::lock_guard lock(m_imp_mutex);

    int j = castToMapper(m_helper)->toHw(axis);
    return m_iraw->getInteractionModeRaw(j, mode);
}

ReturnValue ImplementInteractionMode::getInteractionModes(const std::vector<int>& joints, std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_GET_SOME(IMPLEMENT_LAYER_COMPONENT,joints,modes);

    std::vector<int> vectorInt_tmp(joints.size());
    for (int i = 0; i < joints.size(); i++)
    {
        vectorInt_tmp[i] = castToMapper(m_helper)->toHw(joints[i]);
    }
    ReturnValue ret = m_iraw->getInteractionModesRaw(vectorInt_tmp, modes);

    return ret;
}

ReturnValue ImplementInteractionMode::getInteractionModes(std::vector < yarp::dev::InteractionModeEnum>& modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_GET_ALL(IMPLEMENT_LAYER_COMPONENT,modes);

    ReturnValue ret = m_iraw->getInteractionModesRaw(m_buffer_enums);
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

ReturnValue ImplementInteractionMode::setInteractionModes(const std::vector<int>& joints, const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_SET_SOME(IMPLEMENT_LAYER_COMPONENT,joints, modes)

    std::vector<int> vectorInt_tmp(joints.size());
    for(int idx=0; idx<joints.size(); idx++)
    {
        vectorInt_tmp[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }
    ReturnValue ret = m_iraw->setInteractionModesRaw(vectorInt_tmp, modes);

    return ret;
}

ReturnValue ImplementInteractionMode::setInteractionModes(const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_SET_ALL(IMPLEMENT_LAYER_COMPONENT,modes)

    for(int idx=0; idx< castToMapper(m_helper)->axes(); idx++)
    {
        int j = castToMapper(m_helper)->toHw(idx);
        m_buffer_enums[j] = modes[idx];
    }

    ReturnValue ret = m_iraw->setInteractionModesRaw(m_buffer_enums);

    return ret;
}
