/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <string.h>
#include "primresource.h"

/**
 * Class Memory
 */
Memory::Memory(void) : GenericResource("Memory") 
{
    totalSpace = (Capacity)0;
    freeSpace = (Capacity)0;
}


Memory::Memory(const char* szName) : GenericResource("Memory")
{
    setName(szName);
    totalSpace = (Capacity)0;
    freeSpace = (Capacity)0;
}


Memory::Memory(const Memory &resource) : GenericResource(resource)
{
    totalSpace = resource.totalSpace;
    freeSpace = resource.freeSpace;
}

bool Memory::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    Memory* mem = dynamic_cast<Memory*>(resource);
    if(!mem)
        return false;
    return ( (freeSpace >= mem->getFreeSpace()) && 
             (totalSpace >= mem->getTotalSpace()) );
}

Node* Memory::clone(void)
{
    Memory* resource = new Memory(*this);
    return resource; 
}


Memory::~Memory() { }



/**
 * Class Storage
 */
Storage::Storage(void) : GenericResource("Storage") 
{
    totalSpace = (Capacity)0;
    freeSpace = (Capacity)0;
}


Storage::Storage(const char* szName) : GenericResource("Storage")
{
    setName(szName);
    totalSpace = (Capacity)0;
    freeSpace = (Capacity)0;
}


Storage::Storage(const Storage &resource) : GenericResource(resource)
{
    totalSpace = resource.totalSpace;
    freeSpace = resource.freeSpace;
}

bool Storage::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    Storage* mem = dynamic_cast<Storage*>(resource);
    if(!mem)
        return false;
    return ( (freeSpace >= mem->getFreeSpace()) && 
             (totalSpace >= mem->getTotalSpace()) );
}

Node* Storage::clone(void)
{
    Storage* resource = new Storage(*this);
    return resource; 
}


Storage::~Storage() { }



/**
 * Class Network
 */
Network::Network(void) : GenericResource("Network") 
{
}


Network::Network(const char* szName) : GenericResource("Network")
{
    setName(szName);
}


Network::Network(const Network &resource) : GenericResource(resource)
{
    strIP4 = resource.strIP4;
    strIP6 = resource.strIP6;
    strMAC = resource.strMAC;
}

bool Network::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    Network* net = dynamic_cast<Network*>(resource);
    if(!net)
        return false;
    bool ret = (!strlen(net->getIP4()))? true : (strIP4 == string(net->getIP4()));
    ret &= (!strlen(net->getIP6()))? true : (strIP6 == string(net->getIP6()));
    ret &= (!strlen(net->getMAC()))? true : (strMAC == string(net->getMAC()));
    return ret;
}

Node* Network::clone(void)
{
    Network* resource = new Network(*this);
    return resource; 
}


Network::~Network() { }



/**
 * Class Processor
 */
Processor::Processor(void) : GenericResource("Processor") 
{
    cores = (size_t)0;
    frequency = (double)0.0;
    siblings = (size_t)0;
    cpuload.loadAverage1 = (double)0.0;
    cpuload.loadAverage5 = (double)0.0;
    cpuload.loadAverage15 = (double)0.0;
}


Processor::Processor(const char* szName) : GenericResource("Processor")
{
    setName(szName);
    cores = (size_t)0;
    frequency = (double)0.0;
    cpuload.loadAverage1 = (double)0.0;
    cpuload.loadAverage5 = (double)0.0;
    cpuload.loadAverage15 = (double)0.0;
}


Processor::Processor(const Processor &resource) : GenericResource(resource)
{
    strArchitecure = resource.strArchitecure;
    strModel = resource.strModel;
    cores = resource.cores;
    siblings = resource.siblings;
    frequency = resource.frequency;
    cpuload = resource.cpuload;
}

bool Processor::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    Processor* proc = dynamic_cast<Processor*>(resource);
    if(!proc)
        return false;
    
    bool ret = (!strlen(proc->getArchitecture()))? true : (strArchitecure == string(proc->getArchitecture()));
    ret &= (!strlen(proc->getModel()))? true : (strModel == string(proc->getModel()));
    ret &= (cores >= proc->getCores());
    ret &= (siblings >= proc->getSiblings());
    ret &= (frequency >= proc->getFrequency());
    return ret;
}


Node* Processor::clone(void)
{
    Processor* resource = new Processor(*this);
    return resource; 
}


Processor::~Processor() { }



/**
 * Class Computer
 */

Computer::Computer(void) : GenericResource("Computer") 
{
}


Computer::Computer(const char* szName) : GenericResource("Computer")
{
    setName(szName);
    network.setName(szName);
}


Computer::Computer(const Computer &resource) : GenericResource(resource)
{
    Computer::swap(resource);
}


Computer& Computer::operator=(const Computer& rhs)
{
    GenericResource::operator=(rhs);
    Computer::swap(rhs);
    return *this;
}


bool Computer::addPeripheral(GenericResource& res)
{
    GenericResource* newres = (GenericResource*) res.clone();
    peripheralResources.push_back(newres);    
    return true;
}


bool Computer::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    MultiResource* mres = dynamic_cast<MultiResource*>(resource);
    if(mres)
    {
        if(!mres->resourceCount())
            return true;
        for(int i=0; i<mres->resourceCount(); i++)
        {
            Computer* comp = dynamic_cast<Computer*>(&mres->getResourceAt(i));            
            if(comp &&satisfyComputer(comp))
                return true;                    
            else if(satisfyComputerResource(&mres->getResourceAt(i)))
                    return true;                               
        }
        return false;
    }

    Computer* comp = dynamic_cast<Computer*>(resource);
    if(comp)
        return satisfyComputer(comp);

    return satisfyComputerResource(resource);
}


bool Computer::satisfyComputer(Computer* comp)
{
    bool ret = satisfyComputerResource(&comp->getMemory());
    ret &= satisfyComputerResource(&comp->getStorage());
    ret &= satisfyComputerResource(&comp->getProcessor());
    ret &= satisfyComputerResource(&comp->getNetwork());
    ret &= satisfyComputerResource(&comp->getPlatform());
    for(int i=0; i<comp->peripheralCount(); i++)
        ret &= satisfyComputerResource(&comp->getPeripheralAt(i));
    return ret;
}


bool Computer::satisfyComputerResource(GenericResource* resource)
{
    if(memory.satisfy(resource))
        return true;
    if(storage.satisfy(resource))
        return true;
    if(network.satisfy(resource))
        return true;
    if(processor.satisfy(resource))
        return true; 
    if(platform.satisfy(resource))
        return true;

    ResourcePIterator itr;
    for(itr=peripheralResources.begin(); itr!=peripheralResources.end(); itr++)
        if((*itr)->satisfy(resource))
            return true;
    return false;
}

Node* Computer::clone(void)
{
    Computer* resource = new Computer(*this);
    return resource; 
}

void Computer::swap(const Computer &comp)
{
    clear();
    memory = comp.memory;
    storage = comp.storage;
    processor = comp.processor;
    network = comp.network;
    platform = comp.platform;
    processes = comp.processes;
    // deep copy    
    for(int i=0; i<comp.peripheralCount(); i++)
        addPeripheral(comp.getPeripheralAt(i));
}

void Computer::clear(void)
{
    for(unsigned int i=0; i<peripheralResources.size(); i++)
    {
        delete peripheralResources[i];
        peripheralResources[i] = NULL;
    }
    peripheralResources.clear();
    processes.clear();
}


Computer::~Computer()
{
    clear();
}



