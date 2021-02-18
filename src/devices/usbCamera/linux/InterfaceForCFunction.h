#pragma once

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <libudev.h>
#include <linux/media.h>
#include <linux/v4l2-subdev.h>
#include <linux/videodev2.h>

class InterfaceForCFunction
{
    public:
    virtual int open_c(const char* __path, int __oflag)
    {
        return open(__path, __oflag);
    }
    virtual void open_d(){};

    virtual ~InterfaceForCFunction(){};
};