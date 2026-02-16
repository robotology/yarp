/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTJOINTFAULT_H
#define YARP_DEV_IMPLEMENTJOINTFAULT_H

#include <yarp/dev/IJointFault.h>
#include <yarp/dev/api.h>
#include <vector>

namespace yarp::dev {
class ImplementJointFault;
}

class YARP_dev_API yarp::dev::ImplementJointFault: public IJointFault
{
protected:
    yarp::dev::IJointFaultRaw* m_iraw=nullptr;
    void*                      m_helper=nullptr;
    std::mutex                 m_imp_mutex;
    std::vector<int>           m_buffer_ints;
    std::vector<double>        m_buffer_doubles;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementJointFault(yarp::dev::IJointFaultRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementJointFault();

    yarp::dev::ReturnValue getLastJointFault(int j, int& fault, std::string& message) override;

};

#endif // YARP_DEV_IMPLEMENTJOINTFAULT_H
