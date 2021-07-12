/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RFPLUGIN_H
#define YARP_OS_RFPLUGIN_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

class YARP_os_API RFPlugin
{
public:
    RFPlugin();

    virtual ~RFPlugin();

    /**
     * opens the plugin
     * @param command a string containing the equivalent command line
     * @return true if opened succesfully
     */
    virtual bool open(const std::string& command);

    /**
     * closes the plugin
     */
    virtual void close();

    /**
     * check if the plugin is running
     */
    virtual bool isRunning();

    /**
     * get the command line used to open the plugin
     */
    virtual std::string getCmd();

    /**
     * get the thread id
     */
    virtual int getThreadKey();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};


}
}

#endif // YARP_OS_RFPLUGIN_H
