// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __DATA__
#define __DATA__

#include "ymm-types.h" 
#include "node.h"
#include "utility.h"

using namespace std; 

//namespace ymm {

/**
 * Class InputData  
 */
class InputData : public Node{

public: 
    InputData(void);
    InputData(const char* szName);
    InputData(const InputData &input);
    virtual ~InputData();
    virtual Node* clone(void);
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
    
    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner(void) { return modOwner; }

    bool operator==(const InputData& input) {       
        return (strName == input.strName); 
    }
    
protected:

private:
    string strName;
    string strPort; 
    string carrier;
    string strDescription;
    bool bWithPriority;
    bool bRequired;     
    Node*  modOwner; 
};


class OutputData : public Node{

public: 
    OutputData(void);
    OutputData(const char* szName);
    OutputData(const OutputData &input);
    virtual ~OutputData();
    virtual Node* clone(void);
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
    
    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner(void) { return modOwner; }

   
    bool operator==(const OutputData& output) {     
        return (strName == output.strName); 
    }
    
protected:

private:
    string strName;
    string strPort; 
    string carrier;
    string strDescription;
    Node*  modOwner; 
};
 
 
//}

#endif //__MODULE__
