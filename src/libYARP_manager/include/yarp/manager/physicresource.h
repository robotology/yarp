/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_PHYSICRESOURCE
#define YARP_MANAGER_PHYSICRESOURCE

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/primresource.h>
#include <yarp/manager/resource.h>

namespace yarp {
namespace manager {


class GPU : public GenericResource {

public:
    GPU(void);
    GPU(const char* szName);
    GPU(const GPU &res);
    virtual ~GPU();
    virtual Node* clone(void) override;
    virtual bool satisfy(GenericResource* resource) override;

    void setCores(size_t n) { cores = n; }
    void setFrequency(double f) { frequency = f; }
    void setCompCompatibility(const char* cap) { compCompatibility = cap; }
    void setGlobalMemory(Capacity c) { globalMemory = c; }
    void setSharedMemory(Capacity c) { sharedMemory = c; }
    void setConstantMemory(Capacity c) { constantMemory = c; }
    void setResgisterPerBlock(size_t val) { registerPerBlock = val; }
    void setThreadPerBlock(size_t val) { threadPerBlock = val; }
    void setOverlap(bool flag) { bOverlap = flag;}

    size_t getCores(void) { return cores; }
    double getFrequency(void) { return frequency; }
    const char* getCompCompatibility(void) { return compCompatibility.c_str(); }
    Capacity getGlobalMemory(void) { return globalMemory; }
    Capacity getSharedMemory(void) { return sharedMemory; }
    Capacity getConstantMemory(void) { return constantMemory; }
    size_t getRegisterPerBlock(void) { return registerPerBlock; }
    size_t getThreadPerBlock(void) { return threadPerBlock; }
    bool getOverlap(void) { return bOverlap; }

protected:

private:
    size_t cores;
    double frequency;
    std::string compCompatibility;
    Capacity globalMemory;
    Capacity sharedMemory;
    Capacity constantMemory;
    size_t registerPerBlock;
    size_t threadPerBlock;
    bool bOverlap;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_PHYSICRESOURCE__
