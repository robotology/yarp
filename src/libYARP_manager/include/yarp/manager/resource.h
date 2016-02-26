/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_MANAGER_RESOURCE
#define YARP_MANAGER_RESOURCE

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>

using namespace std;

namespace yarp {
namespace manager {

class GenericResource : public Node
{
public:
    GenericResource(const char* szTypeName);
    GenericResource(const GenericResource &res);
    virtual ~GenericResource();

    void setAvailability(bool flag) { bAvailable = flag; }
    bool getAvailability(void) { return bAvailable; }
    void setDisable(bool flag) { bDisabled = flag; }
    bool getDisable(void) { return bDisabled; }

    void setName(const char* szName) { if(szName) strName = szName; }
    const char* getName(void) { return strName.c_str(); }
    void setDescription(const char* szDesc) { if(szDesc) strDescription = szDesc; }
    const char* getDescription(void) { return strDescription.c_str(); }
    const char* getTypeName(void) { return strTypeName.c_str(); }

    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner(void) { return modOwner; }
    void setXmlFile(const char* szFilename) { if(szFilename) strXmlFile = szFilename;}
    const char* getXmlFile(void) { return strXmlFile.c_str(); }

    //virtual Node* clone(void);
    virtual bool satisfy(GenericResource* resource) = 0;

    bool operator==(const GenericResource& res) {
        return (strName == res.strName);
    }

protected:

private:
    bool bAvailable;
    bool bDisabled;
    string strName;
    string strTypeName;
    string strDescription;
    Node*  modOwner;
    string strXmlFile;

};

typedef vector<GenericResource*> ResourcePContainer;
typedef vector<GenericResource*>::iterator ResourcePIterator;



/**
* Class MultiResource
*/
class MultiResource : public GenericResource
{
public:
    MultiResource(void);
    MultiResource(const char* szName);
    MultiResource(const MultiResource& rhs);
    MultiResource& operator=(const MultiResource& rhs);
    virtual ~MultiResource();
    virtual Node* clone(void);
    virtual bool satisfy(GenericResource* resource);

    int resourceCount(void) const { return resources.size(); }
    GenericResource& getResourceAt(int index) const { return *(resources[index]); }
    bool addResource(GenericResource& res);
    void clear();

protected:

private:
    ResourcePContainer resources;
    void swap(const MultiResource &res);
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_RESOURCE__
