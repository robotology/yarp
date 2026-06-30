/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(IMPLEMENT_LAYER_COMPONENT, "yarp.ImplementPositionDirect")
}

ImplementPositionDirect::ImplementPositionDirect(IPositionDirectRaw *y):
    m_iraw(y),
    m_helper(nullptr)
{;}


ImplementPositionDirect::~ImplementPositionDirect()
{
    uninitialize();
}

bool ImplementPositionDirect::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

bool ImplementPositionDirect::uninitialize()
{
    if(m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementPositionDirect::getAxes(int *axes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,axes);

    (*axes)=castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementPositionDirect::setPosition(int j, double ref)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(IMPLEMENT_LAYER_COMPONENT,j)

    int k;
    double enc;
    castToMapper(m_helper)->posA2E(ref, j, enc, k);
    return m_iraw->setPositionRaw(k, enc);
}

ReturnValue ImplementPositionDirect::setPositions(const int n_joints, const int *joints, const double *refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,joints);
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,refs);
    JOINTSIDCHECK(IMPLEMENT_LAYER_COMPONENT,n_joints, joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx]= castToMapper(m_helper)->toHw(joints[idx]);
        m_buffer_doubles[idx] = castToMapper(m_helper)->posA2E(refs[idx], joints[idx]);
    }

    ReturnValue ret = m_iraw->setPositionsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionDirect::setPositions(const double *refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,refs);

    castToMapper(m_helper)->posA2E(refs, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setPositionsRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionDirect::getRefPosition(const int j, double* ref)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(IMPLEMENT_LAYER_COMPONENT,j)
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,ref);

    int k;
    double tmp;
    k=castToMapper(m_helper)->toHw(j);

    ReturnValue ret = m_iraw->getRefPositionRaw(k, &tmp);

    *ref=(castToMapper(m_helper)->posE2A(tmp, k));
    return ret;
}

ReturnValue ImplementPositionDirect::getRefPositions(const int n_joints, const int* joints, double* refs)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTSIDCHECK(IMPLEMENT_LAYER_COMPONENT,n_joints, joints)
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,joints);
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,refs);

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }

    ReturnValue ret = m_iraw->getRefPositionsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    for(int idx=0; idx<n_joints; idx++)
    {
        refs[idx]=castToMapper(m_helper)->posE2A(m_buffer_doubles[idx], m_buffer_ints[idx]);
    }

    return ret;
}

ReturnValue ImplementPositionDirect::getRefPositions(double* refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(IMPLEMENT_LAYER_COMPONENT,refs);

    ReturnValue ret = m_iraw->getRefPositionsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->posE2A(m_buffer_doubles.data(), refs);

    return ret;
}
