
#include <yarp/os/api.h>
#include <string>

#ifndef YARP_OS_RFPLUGINo_H
#define YARP_OS_RFPLUGINo_H

namespace yarp {
namespace os {

class YARP_OS_API RFPlugin
{
    struct RFPlugin_Private;
    RFPlugin_Private* impl{ nullptr };

public:
    RFPlugin();

    virtual ~RFPlugin();

    virtual bool open(const std::string& command);

    virtual void close();

    virtual bool isRunning();

    virtual std::string getCmd();

    virtual std::string getAlias();

    virtual int getThreadKey();
};


}
}

#endif // YARP_OS_RFPLUGINo_H