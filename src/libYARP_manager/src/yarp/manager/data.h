/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    InputData();
    InputData(const char* szName);
    InputData(const InputData &input);
    ~InputData() override;
    Node* clone() override;
    void setName(const char* szName) {
        if(szName){
            strName = szName;
        }
    }
    const char* getName() { return strName.c_str(); }
    void setPort(const char* szPort) { if(szPort) { strPort = szPort; } }
    const char* getPort() { return strPort.c_str(); }
    void setCarrier(const char* szCr) { if(szCr) { carrier = szCr; } }
    const char* getCarrier() { return carrier.c_str(); }
    void setPriority(bool prio) { bWithPriority = prio; }
    void setRequired(bool req) { bRequired = req; }
    bool isRequired() { return bRequired; }
    bool withPriority() { return bWithPriority; }
    void setDescription(const char* szDesc) { if(szDesc) { strDescription = szDesc; } }
    const char* getDescription() { return strDescription.c_str(); }
    void setPortType(NodeType type) { portType = type; }
    NodeType getPortType() { return portType; }
    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner() { return modOwner; }

    bool operator==(const InputData& input) const {
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
    OutputData();
    OutputData(const char* szName);
    OutputData(const OutputData &input);
    ~OutputData() override;
    Node* clone() override;
    void setName(const char* szName) {
        if(szName){
            strName = szName;
        }
    }
    const char* getName() { return strName.c_str(); }
    void setPort(const char* szPort) { if(szPort) { strPort = szPort; } }
    const char* getPort() { return strPort.c_str(); }
    void setCarrier(const char* szCr) { if(szCr) { carrier = szCr; } }
    const char* getCarrier() { return carrier.c_str(); }
    void setDescription(const char* szDesc) { if(szDesc) { strDescription = szDesc; } }
    const char* getDescription() { return strDescription.c_str(); }
    void setPortType(NodeType type) { portType = type; }
    NodeType getPortType() { return portType; }

    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner() { return modOwner; }


    bool operator==(const OutputData& output) const {
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
