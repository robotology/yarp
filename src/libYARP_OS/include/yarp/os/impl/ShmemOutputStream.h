/*
 * Author: Alessandro Scalzo alessandro@liralab.it
 * Copyright (C) 2007 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_OS_IMPL_SHMEMOUTPUTSTREAM_H
#define YARP_OS_IMPL_SHMEMOUTPUTSTREAM_H

#include <ace/config.h>
#include <ace/Mutex.h>
#include <ace/Process_Mutex.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#if defined(ACE_LACKS_SYSV_SHMEM)
#include <ace/Shared_Memory_MM.h>
#else
#include <ace/Shared_Memory_SV.h>
#endif

#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/OutputStream.h>

#include <yarp/os/impl/ShmemTypes.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ShmemOutputStreamImpl;
        }
    }
}

class yarp::os::impl::ShmemOutputStreamImpl {
public:
    ShmemOutputStreamImpl()
    {
        m_bOpen=false;

        m_pAccessMutex=m_pWaitDataMutex=0;
        m_pMap=0;
        m_pData=0;
        m_pHeader=0;
        m_ResizeNum=0;
        m_Port=0;
    }

    ~ShmemOutputStreamImpl()
    {
        close();
    }

    bool isOk() { return m_bOpen; }
    bool open(int port,int size=SHMEM_DEFAULT_SIZE);
    bool write(const Bytes& b);
    void close();

protected:
    bool Resize(int newsize);

    bool m_bOpen;

    int m_ResizeNum;
    int m_Port;

#if defined(_ACE_USE_SV_SEM)
    ACE_Mutex *m_pAccessMutex,*m_pWaitDataMutex;
#else
    ACE_Process_Mutex *m_pAccessMutex,*m_pWaitDataMutex;
#endif

    ACE_Shared_Memory *m_pMap;
    char *m_pData;
    ShmemHeader_t *m_pHeader;
};

#endif
