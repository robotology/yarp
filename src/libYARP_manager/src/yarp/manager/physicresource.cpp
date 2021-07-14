/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/physicresource.h>

#include <cstring>


using namespace yarp::manager;
using namespace std;


/**
 * Class GPU
 */
GPU::GPU() : GenericResource("GPU")
{
    cores = (size_t)0;
    frequency = (double)0.0;
    globalMemory = (Capacity)0;
    sharedMemory = (Capacity)0;
    constantMemory = (Capacity)0;
    registerPerBlock = (size_t)0;
    threadPerBlock = (size_t)0;
    bOverlap = false;
}


GPU::GPU(const char* szName) : GenericResource("GPU")
{
    setName(szName);
    cores = (size_t)0;
    frequency = (double)0.0;
    globalMemory = (Capacity)0;
    sharedMemory = (Capacity)0;
    constantMemory = (Capacity)0;
    registerPerBlock = (size_t)0;
    threadPerBlock = (size_t)0;
    bOverlap = false;
}


GPU::GPU(const GPU &resource) : GenericResource(resource)
{
    compCompatibility = resource.compCompatibility;
    cores = resource.cores;
    frequency = resource.frequency;
    globalMemory = resource.globalMemory;
    sharedMemory = resource.sharedMemory;
    constantMemory = resource.constantMemory;
    registerPerBlock = resource.registerPerBlock;
    threadPerBlock = resource.threadPerBlock;
    bOverlap = resource.bOverlap;
}


Node* GPU::clone()
{
    GPU* resource = new GPU(*this);
    return resource;
}


bool GPU::satisfy(GenericResource* resource)
{
    if (!getAvailability() || getDisable()) {
        return false;
    }

    GPU* gpu = dynamic_cast<GPU*>(resource);
    if (!gpu) {
        return false;
    }
    bool ret = (!strlen(gpu->getCompCompatibility()))? true : (compCompatibility == string(gpu->getCompCompatibility()));
    ret &= (cores >= gpu->getCores());
    ret &= (frequency >= gpu->getFrequency());
    ret &= (globalMemory >= gpu->getGlobalMemory());
    ret &= (sharedMemory >= gpu->getSharedMemory());
    ret &= (constantMemory >= gpu->getConstantMemory());
    //ret &= (registerPerBlock == gpu->getRegisterPerBlock());
    //ret &= (threadPerBlock == gpu->getThreadPerBlock());
    ret &= (!gpu->getOverlap())? true : (bOverlap == gpu->getOverlap());
    return ret;
}

GPU::~GPU() = default;
