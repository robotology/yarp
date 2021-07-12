/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
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
class YARP_os_impl_API AuthHMAC
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
    static bool send_hmac(yarp::os::OutputStream* stream, unsigned char* nonce, unsigned char* mac);
    static bool receive_hmac(yarp::os::InputStream* stream, unsigned char* nonce, unsigned char* mac);
    static bool check_hmac(unsigned char* mac, unsigned char* mac_check);
    static void fill_nonce(unsigned char* nonce);

    bool authentication_enabled;
    HMAC_CONTEXT context;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_AUTHHMAC_H
