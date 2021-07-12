/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/AuthHMAC.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/LogComponent.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <random>
#include <string>

namespace {
YARP_OS_LOG_COMPONENT(AUTHHMAC, "yarp.os.impl.AuthHMAC")
} // namespace

void show_hmac_debug(unsigned char* hex, unsigned int length, const std::string& context)
{
    char* buf;
    int off = context.length();
    buf = new char[length * 3 + off + 2];
    strcpy(buf, context.c_str());
    for (unsigned int i = 0; i < length; i++) {
        sprintf(&(buf[off + i * 3]), "%X ", hex[i]);
    }
    yCDebug(AUTHHMAC, "%s\n", buf);
    delete[] buf;
}

using namespace yarp::os::impl;
using namespace yarp::os;

AuthHMAC::AuthHMAC() :
        authentication_enabled(false)
{
    memset(&context, 0, sizeof(HMAC_CONTEXT));
    static bool auth_warning_shown = false;
    if (auth_warning_shown) {
        // If the warning was already shown, we have nothing to do.
        // return as soon as possible
        return;
    }
    std::string key;
    ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
    std::string fname;
    Network::lock();
    ResourceFinderOptions opt;
    opt.messageFilter = ResourceFinderOptions::ShowNone;
    fname = rf.findFile("auth.conf", opt);
    Network::unlock();


    if (fname.empty()) {
        yCDebug(AUTHHMAC, "Cannot find auth.conf file. Authentication disabled.\n");
        auth_warning_shown = true;
        return;
    }

    Property config;
    config.fromConfigFile(fname);
    Bottle group = config.findGroup("AUTH");

    if (group.isNull()) {
        yCWarning(AUTHHMAC, "No \"AUTH\" group found in auth.conf file. Authentication disabled.\n");
        auth_warning_shown = true;
        return;
    }

    key = group.find("key").asString();
    if (!(key.length() > 0)) {
        yCWarning(AUTHHMAC, "No \"key\" found in \"AUTH\" group in auth.conf file. Authentication disabled.\n");
        auth_warning_shown = true;
        return;
    }

    size_t key_len = key.length();
    auto* tmp = new unsigned char[key_len];
    strcpy(reinterpret_cast<char*>(tmp), key.c_str());
    HMAC_INIT(&context, tmp, static_cast<unsigned int>(key_len));
    delete[] tmp;
    srand(static_cast<unsigned>(time(nullptr)));

    if (!authentication_enabled) {
        yCInfo(AUTHHMAC, "Authentication enabled.\n");
        authentication_enabled = true;
    }
}


bool AuthHMAC::authSource(InputStream* streamIn, OutputStream* streamOut)
{

    if (!authentication_enabled) {
        return true;
    }

    /* ---------------
      * 3-way auth
      * Port A
      * ---------------
      */

    unsigned char nonce1[NONCE_LEN];
    unsigned char nonce2[NONCE_LEN];
    unsigned char nonce3[NONCE_LEN];

    unsigned char mac[DIGEST_SIZE];
    unsigned char mac_check[DIGEST_SIZE];

    /* ---------------
      * Send first msg: A->B
      */
    fill_nonce(nonce1);
    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_FINAL(&context, mac, DIGEST_SIZE);
    if (!send_hmac(streamOut, nonce1, mac)) {
        return false;
    }

    /* ---------------
      * Receive and check second msg: B->A
      */
    if (!receive_hmac(streamIn, nonce2, mac)) {
        return false;
    }

    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_UPDATE(&context, nonce2, NONCE_LEN);
    HMAC_FINAL(&context, mac_check, DIGEST_SIZE);
    if (!check_hmac(mac, mac_check)) {
        return false;
    }
    /* Authentication of B successful */


    /* ---------------
      * Send third msg: A->B
      */
    fill_nonce(nonce3);
    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_UPDATE(&context, nonce2, NONCE_LEN);
    HMAC_UPDATE(&context, nonce3, NONCE_LEN);
    HMAC_FINAL(&context, mac, DIGEST_SIZE);
    return send_hmac(streamOut, nonce3, mac);
}
bool AuthHMAC::authDest(InputStream* streamIn, OutputStream* streamOut)
{

    if (!authentication_enabled) {
        return true;
    }

    /* ---------------
     * 3-way auth
     * Port B
     * ---------------
     */

    unsigned char nonce1[NONCE_LEN];
    unsigned char nonce2[NONCE_LEN];
    unsigned char nonce3[NONCE_LEN];

    unsigned char mac[DIGEST_SIZE];
    unsigned char mac_check[DIGEST_SIZE];

    /* ---------------
     * Receive and check first msg: A->B
     */
    if (!receive_hmac(streamIn, nonce1, mac)) {
        return false;
    }
    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_FINAL(&context, mac_check, DIGEST_SIZE);
    if (!check_hmac(mac, mac_check)) {
        return false;
    }

    /* ---------------
     * Send second msg: B->A
     */
    fill_nonce(nonce2);
    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_UPDATE(&context, nonce2, NONCE_LEN);
    HMAC_FINAL(&context, mac, DIGEST_SIZE);
    if (!send_hmac(streamOut, nonce2, mac)) {
        return false;
    }


    /* ---------------
     * Receive and check third msg: A->B
     */
    if (!receive_hmac(streamIn, nonce3, mac)) {
        return false;
    }
    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_UPDATE(&context, nonce2, NONCE_LEN);
    HMAC_UPDATE(&context, nonce3, NONCE_LEN);
    HMAC_FINAL(&context, mac_check, DIGEST_SIZE);
    if (!check_hmac(mac, mac_check)) {
        return false;
    }
    /* Authentication of A successful */

    return true;
}


bool AuthHMAC::send_hmac(OutputStream* stream, unsigned char* nonce, unsigned char* mac)
{
    Bytes nonce_bytes(reinterpret_cast<char*>(nonce), NONCE_LEN);
    Bytes mac_bytes(reinterpret_cast<char*>(mac), DIGEST_SIZE);
    stream->write(nonce_bytes);
    stream->write(mac_bytes);

    show_hmac_debug(nonce, NONCE_LEN, "send nonce ");
    show_hmac_debug(mac, DIGEST_SIZE, "send digest ");

    return stream->isOk();
}

bool AuthHMAC::receive_hmac(InputStream* stream, unsigned char* nonce, unsigned char* mac)
{
    Bytes nonce_bytes(reinterpret_cast<char*>(nonce), NONCE_LEN);
    Bytes mac_bytes(reinterpret_cast<char*>(mac), DIGEST_SIZE);
    stream->read(nonce_bytes);
    stream->read(mac_bytes);

    show_hmac_debug(nonce, NONCE_LEN, "got nonce ");
    show_hmac_debug(mac, DIGEST_SIZE, "got digest ");

    return stream->isOk();
}

bool AuthHMAC::check_hmac(unsigned char* mac, unsigned char* mac_check)
{
    int cmp = memcmp(mac, mac_check, DIGEST_SIZE);

    std::string check = "digest check ";
    if (cmp == 0) {
        check += "successful";
    } else {
        check += "FAILED";
    }
    show_hmac_debug(mac_check, DIGEST_SIZE, check);

    return (cmp == 0);
}


void AuthHMAC::fill_nonce(unsigned char* nonce)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (unsigned int i = 0; i < NONCE_LEN; i++) {
        nonce[i] = static_cast<unsigned char>(dist(mt));
    }
}
