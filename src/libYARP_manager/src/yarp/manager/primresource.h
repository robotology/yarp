/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_PRIMRESOURCE
#define YARP_MANAGER_PRIMRESOURCE

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/resource.h>
#include <yarp/manager/logicresource.h>
#include <yarp/conf/api.h>

namespace yarp {
namespace manager {

typedef size_t Capacity;

/**
 * Class Memory
 */
class Memory : public GenericResource {
public:
    Memory(void);
    Memory(const char* szName);
    virtual ~Memory();
    Node* clone(void) override;
    bool satisfy(GenericResource* resource) override;

    void setTotalSpace(Capacity c) { totalSpace = c; }
    Capacity getTotalSpace(void) { return totalSpace; }
    void setFreeSpace(Capacity c) { freeSpace = c; }
    Capacity getFreeSpace(void) { return freeSpace; }

protected:

private:
    Capacity totalSpace;
    Capacity freeSpace;

};


/*
 * class Storage
 */
class Storage : public GenericResource {
public:
    Storage(void);
    Storage(const char* szName);
    virtual ~Storage();
    Node* clone(void) override;
    bool satisfy(GenericResource* resource) override;

    void setTotalSpace(Capacity c) { totalSpace = c; }
    Capacity getTotalSpace(void) { return totalSpace; }
    void setFreeSpace(Capacity c) { freeSpace = c; }
    Capacity getFreeSpace(void) { return freeSpace; }

protected:

private:
    Capacity totalSpace;
    Capacity freeSpace;
};


/*
 * Class Network
 */
class Network : public GenericResource {
public:
    Network(void);
    Network(const char* szName);
    virtual ~Network();
    Node* clone(void) override;
    bool satisfy(GenericResource* resource) override;

    void setIP4(const char* ip) { if(ip) strIP4 = ip; }
    void setIP6(const char* ip) { if(ip) strIP6 = ip; }
    void setMAC(const char* mac) { if(mac) strMAC = mac; }
    const char* getIP4(void) { return strIP4.c_str(); }
    const char* getIP6(void) { return strIP6.c_str(); }
    const char* getMAC(void) { return strMAC.c_str(); }

protected:

private:
    std::string strIP4;
    std::string strIP6;
    std::string strMAC;
};


typedef struct _LoadAvg {
    double loadAverageInstant;
    double loadAverage1;
    double loadAverage5;
    double loadAverage15;
} LoadAvg;


/*
 * Class Processor
 */
class Processor : public GenericResource {
public:
    Processor(void);
    Processor(const char* szName);
    virtual ~Processor();
    Node* clone(void) override;
    bool satisfy(GenericResource* resource) override;

    void setArchitecture(const char* arch) {if(arch) strArchitecure = arch; }
    void setModel(const char* model) {if(model) strModel = model; }
    void setCores(size_t n) { cores = n; }
    void setSiblings(size_t n) { siblings = n; }
    void setFrequency(double f) { frequency = f; }
    void setCPULoad(const LoadAvg &avg) { cpuload = avg; }
    const char* getArchitecture(void) { return strArchitecure.c_str(); }
    const char* getModel(void) { return strModel.c_str(); }
    size_t getCores(void) { return cores; }
    size_t getSiblings(void) { return siblings; }
    double getFrequency(void) { return frequency; }
    const LoadAvg& getCPULoad(void) { return cpuload; }

protected:

private:
    std::string strArchitecure;
    std::string strModel;
    size_t cores;
    size_t siblings;
    double frequency;
    LoadAvg cpuload;
};


/**
 * Class Computer
 */
typedef struct _Process {
    std::string command;
    int pid;
} Process;

typedef std::vector<Process> ProcessContainer;
typedef std::vector<Process>::iterator ProcessIterator;

class Computer : public GenericResource {
public:
    Computer(void);
    Computer(const char* szName);
    Computer(const Computer& rhs);
    virtual ~Computer();
    Computer& operator=(const Computer& rhs);
    Node* clone(void) override;
    bool satisfy(GenericResource* resource) override;

    void setMemory(Memory& mem) { memory = mem; }
    void setStorage(Storage& stg) { storage = stg; }
    void setProcessor(Processor& proc) { processor = proc; }
    void setNetwork(Network& net) { network = net; }
    void setPlatform(Platform& os) {platform = os; }

    Memory& getMemory(void) { return memory; }
    Storage& getStorage(void) { return storage; }
    Processor& getProcessor(void) { return processor; }
    Network& getNetwork(void) { return network; }
    Platform& getPlatform(void) { return platform; }

    int peripheralCount(void) const { return peripheralResources.size(); }
    GenericResource& getPeripheralAt(int index) const { return *(peripheralResources[index]); }
    bool addPeripheral(GenericResource& res);
    void clear(void);

    // processes list
    ProcessContainer& getProcesses(void) { return processes; }

protected:

private:
    Memory memory;
    Storage storage;
    Processor processor;
    Network network;
    Platform platform;
    ResourcePContainer peripheralResources;

    // Processes are not actually part of a resource and only
    // will be used by yarpmanager for listing processes running
    // on Computer
    ProcessContainer processes;

    bool satisfyComputer(Computer* comp);
    bool satisfyComputerResource(GenericResource* resource);
    void swap(const Computer &comp);

};

typedef std::vector<Computer> ComputerContainer;
typedef std::vector<Computer>::iterator ComputerIterator;

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_PRIMRESOURCE__
