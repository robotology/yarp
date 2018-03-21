/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_DATA
#define YARP_MANAGER_DATA

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>
#include <yarp/conf/api.h>


namespace yarp {
namespace manager {


/**
 * Class InputData
 */
class InputData : public Node{

public:
    InputData(void);
    InputData(const char* szName);
    InputData(const InputData &input);
    virtual ~InputData();
    virtual Node* clone(void) override;
    void setName(const char* szName) {
        if(szName){
            strName = szName;
        }
    }
    const char* getName(void) { return strName.c_str(); }
    void setPort(const char* szPort) { if(szPort) strPort = szPort; }
    const char* getPort(void) { return strPort.c_str(); }
    void setCarrier(const char* szCr) { if(szCr) carrier = szCr; }
    const char* getCarrier(void) { return carrier.c_str(); }
    void setPriority(bool prio) { bWithPriority = prio; }
    void setRequired(bool req) { bRequired = req; }
    bool isRequired(void) { return bRequired; }
    bool withPriority(void) { return bWithPriority; }
    void setDescription(const char* szDesc) { if(szDesc) strDescription = szDesc; }
    const char* getDescription(void) { return strDescription.c_str(); }
    void setPortType(NodeType type) { portType = type; }
    NodeType getPortType(void) { return portType; }
    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner(void) { return modOwner; }

    bool operator==(const InputData& input) {
        return (strName == input.strName);
    }

protected:

private:
    std::string strName;
    std::string strPort;
    std::string carrier;
    std::string strDescription;
    bool bWithPriority;
    bool bRequired;
    Node*  modOwner;
    NodeType portType;
};


class OutputData : public Node{

public:
    OutputData(void);
    OutputData(const char* szName);
    OutputData(const OutputData &input);
    virtual ~OutputData();
    virtual Node* clone(void) override;
    void setName(const char* szName) {
        if(szName){
            strName = szName;
        }
    }
    const char* getName(void) { return strName.c_str(); }
    void setPort(const char* szPort) { if(szPort) strPort = szPort; }
    const char* getPort(void) { return strPort.c_str(); }
    void setCarrier(const char* szCr) { if(szCr) carrier = szCr; }
    const char* getCarrier(void) { return carrier.c_str(); }
    void setDescription(const char* szDesc) { if(szDesc) strDescription = szDesc; }
    const char* getDescription(void) { return strDescription.c_str(); }
    void setPortType(NodeType type) { portType = type; }
    NodeType getPortType(void) { return portType; }

    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner(void) { return modOwner; }


    bool operator==(const OutputData& output) {
        return (strName == output.strName);
    }

protected:

private:
    std::string strName;
    std::string strPort;
    std::string carrier;
    std::string strDescription;
    Node*  modOwner;
    NodeType portType;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_MODULE__
