/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_RESOURCE
#define YARP_MANAGER_RESOURCE

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>
#include <yarp/conf/api.h>

namespace yarp {
namespace manager {

class GenericResource : public Node
{
public:
    GenericResource(const char* szTypeName);
    ~GenericResource() override;

    void setAvailability(bool flag) { bAvailable = flag; }
    bool getAvailability() { return bAvailable; }
    void setDisable(bool flag) { bDisabled = flag; }
    bool getDisable() { return bDisabled; }

    void setName(const char* szName) { if(szName) { strName = szName; } }
    const char* getName() { return strName.c_str(); }
    void setDescription(const char* szDesc) { if(szDesc) { strDescription = szDesc; } }
    const char* getDescription() { return strDescription.c_str(); }
    const char* getTypeName() { return strTypeName.c_str(); }

    void setOwner(Node* owner) { modOwner = owner; }
    Node* owner() { return modOwner; }
    void setXmlFile(const char* szFilename) { if(szFilename) { strXmlFile = szFilename; } }
    const char* getXmlFile() { return strXmlFile.c_str(); }

    //virtual Node* clone();
    virtual bool satisfy(GenericResource* resource) = 0;

    bool operator==(const GenericResource& res) const {
        return (strName == res.strName);
    }

protected:

private:
    bool bAvailable;
    bool bDisabled;
    std::string strName;
    std::string strTypeName;
    std::string strDescription;
    Node*  modOwner;
    std::string strXmlFile;

};

typedef std::vector<GenericResource*> ResourcePContainer;
typedef std::vector<GenericResource*>::iterator ResourcePIterator;



/**
* Class MultiResource
*/
class MultiResource : public GenericResource
{
public:
    MultiResource();
    MultiResource(const char* szName);
    MultiResource(const MultiResource& rhs);
    MultiResource& operator=(const MultiResource& rhs);
    ~MultiResource() override;
    Node* clone() override;
    bool satisfy(GenericResource* resource) override;

    int resourceCount() const { return resources.size(); }
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
