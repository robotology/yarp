/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    GPU();
    GPU(const char* szName);
    GPU(const GPU &res);
    ~GPU() override;
    Node* clone() override;
    bool satisfy(GenericResource* resource) override;

    void setCores(size_t n) { cores = n; }
    void setFrequency(double f) { frequency = f; }
    void setCompCompatibility(const char* cap) { compCompatibility = cap; }
    void setGlobalMemory(Capacity c) { globalMemory = c; }
    void setSharedMemory(Capacity c) { sharedMemory = c; }
    void setConstantMemory(Capacity c) { constantMemory = c; }
    void setResgisterPerBlock(size_t val) { registerPerBlock = val; }
    void setThreadPerBlock(size_t val) { threadPerBlock = val; }
    void setOverlap(bool flag) { bOverlap = flag;}

    size_t getCores() { return cores; }
    double getFrequency() { return frequency; }
    const char* getCompCompatibility() { return compCompatibility.c_str(); }
    Capacity getGlobalMemory() { return globalMemory; }
    Capacity getSharedMemory() { return sharedMemory; }
    Capacity getConstantMemory() { return constantMemory; }
    size_t getRegisterPerBlock() { return registerPerBlock; }
    size_t getThreadPerBlock() { return threadPerBlock; }
    bool getOverlap() { return bOverlap; }

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
