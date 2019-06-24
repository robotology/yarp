/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_AUTHHMAC_H
#define YARP_OS_IMPL_AUTHHMAC_H

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>

#include <hmac_sha2.h>

#define DIGEST_SIZE SHA256_DIGEST_SIZE
#define HMAC_CONTEXT hmac_sha256_ctx
#define HMAC_INIT hmac_sha256_init
#define HMAC_REINIT hmac_sha256_reinit
#define HMAC_UPDATE hmac_sha256_update
#define HMAC_FINAL hmac_sha256_final
#define NONCE_LEN 8

namespace yarp {
namespace os {
namespace impl {

/**
 * 3-way authentication via HMAC
 */
class YARP_OS_impl_API AuthHMAC
{
public:
    /**
     * Constructor.
     */
    AuthHMAC();

    /**
     * Destructor.
     */
    virtual ~AuthHMAC() = default;

    bool authSource(yarp::os::InputStream* streamIn, yarp::os::OutputStream* streamOut);
    bool authDest(yarp::os::InputStream* streamIn, yarp::os::OutputStream* streamOut);

private:
    bool send_hmac(yarp::os::OutputStream* stream, unsigned char* nonce, unsigned char* mac);
    bool receive_hmac(yarp::os::InputStream* stream, unsigned char* nonce, unsigned char* mac);
    bool check_hmac(unsigned char* mac, unsigned char* mac_check);
    void fill_nonce(unsigned char* nonce);

    bool authentication_enabled;
    HMAC_CONTEXT context;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_AUTHHMAC_H
