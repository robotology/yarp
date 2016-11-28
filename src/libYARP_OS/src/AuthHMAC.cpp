/*
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/AuthHMAC.h>

#include <cstdlib>
#include <cstring> //needed by strcpy
#include <stdio.h>
#include <time.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/impl/NameClient.h>

#ifdef DEBUG_HMAC
void show_hmac_debug(unsigned char* hex, unsigned int length, std::string context)
{
    char *buf;
    int off = context.length();
    buf = new char[length*3+off+2];
    strcpy(buf, context.c_str());
    for (unsigned int i=0; i < length; i++)
        sprintf(&(buf[off+i*3]), "%X ", hex[i]);
    yDebug("%s\n", buf);
    delete [] buf;
}
#endif

using namespace yarp::os::impl;
using namespace yarp::os;

AuthHMAC::AuthHMAC() :
        authentication_enabled(false)
{
    static int auth_warning_shown = false;
    if(auth_warning_shown) {
        // If the warning was already shown, we have nothing to do.
        // return as soon as possible
        return;
    }

    ConstString key;
    ResourceFinder& rf = NameClient::getNameClient().getResourceFinder();
    ConstString fname;
    Network::lock();
    ResourceFinderOptions opt;
    opt.messageFilter = ResourceFinderOptions::ShowNone;
    fname = rf.findFile("auth.conf",opt);
    Network::unlock();


    if (fname.empty()) {
        //yInfo("Cannot find auth.conf file. Authentication disabled.\n");
        auth_warning_shown = true;
        return;
    }

    Property config;
    config.fromConfigFile(fname.c_str());
    Bottle group = config.findGroup("AUTH");

    if (group.isNull()) {
        yWarning("No \"AUTH\" group found in auth.conf file. Authentication disabled.\n");
        auth_warning_shown = true;
        return;
    }

    key = group.find("key").asString();
    if (!(key.length() > 0)) {
        yWarning("No \"key\" found in \"AUTH\" group in auth.conf file. Authentication disabled.\n");
        auth_warning_shown = true;
        return;
    }

    size_t key_len = key.length();
    unsigned char * tmp = new unsigned char[key_len];
    strcpy((char*) tmp, key.c_str());
    HMAC_INIT(&context, tmp, (unsigned int)key_len);
    srand((unsigned)time(YARP_NULLPTR));

    if(!authentication_enabled) {
        yInfo("Authentication enabled.\n");
        authentication_enabled = true;
    }
}


bool AuthHMAC::authSource(InputStream *streamIn, OutputStream *streamOut)
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
    if (! send_hmac(streamOut, nonce1, mac)) {
        return false;
    }

    /* ---------------
      * Receive and check second msg: B->A
      */
    if (! receive_hmac(streamIn, nonce2, mac)) {
        return false;
    }

    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_UPDATE(&context, nonce2, NONCE_LEN);
    HMAC_FINAL(&context, mac_check, DIGEST_SIZE);
    if (! check_hmac(mac, mac_check)) {
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
    if (! send_hmac(streamOut, nonce3, mac)) {
        return false;
    }

    return true;

}
bool AuthHMAC::authDest(InputStream *streamIn, OutputStream *streamOut)
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
    if (! receive_hmac(streamIn, nonce1, mac)) {
        return false;
    }
    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_FINAL(&context, mac_check, DIGEST_SIZE);
    if (! check_hmac(mac, mac_check)) {
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
    if (! send_hmac(streamOut, nonce2, mac)) {
        return false;
    }


    /* ---------------
     * Receive and check third msg: A->B
     */
    if (! receive_hmac(streamIn, nonce3, mac)) {
        return false;
    }
    HMAC_REINIT(&context);
    HMAC_UPDATE(&context, nonce1, NONCE_LEN);
    HMAC_UPDATE(&context, nonce2, NONCE_LEN);
    HMAC_UPDATE(&context, nonce3, NONCE_LEN);
    HMAC_FINAL(&context, mac_check, DIGEST_SIZE);
    if (! check_hmac(mac, mac_check)) {
        return false;
    }
    /* Authentication of A successful */

    return true;
}


bool AuthHMAC::send_hmac(OutputStream * stream, unsigned char* nonce, unsigned char* mac)
{
    Bytes nonce_bytes((char*) nonce, NONCE_LEN);
    Bytes mac_bytes((char*) mac, DIGEST_SIZE);
    stream->write(nonce_bytes);
    stream->write(mac_bytes);

#ifdef DEBUG_HMAC
    show_hmac_debug(nonce, NONCE_LEN, "send nonce ");
    show_hmac_debug(mac, DIGEST_SIZE, "send digest ");
#endif

    return stream->isOk();
}

bool AuthHMAC::receive_hmac(InputStream * stream, unsigned char * nonce, unsigned char * mac)
{
    Bytes nonce_bytes((char*) nonce, NONCE_LEN);
    Bytes mac_bytes((char*) mac, DIGEST_SIZE);
    stream->read(nonce_bytes);
    stream->read(mac_bytes);

#ifdef DEBUG_HMAC
    show_hmac_debug(nonce, NONCE_LEN, "got nonce ");
    show_hmac_debug(mac, DIGEST_SIZE, "got digest ");
#endif

    return stream->isOk();
}

bool AuthHMAC::check_hmac(unsigned char * mac, unsigned char * mac_check)
{
    int cmp = memcmp(mac, mac_check, DIGEST_SIZE);

#ifdef DEBUG_HMAC
    std::string check = "digest check ";
    if (cmp == 0) {
        check += "successful";
    } else {
        check += "FAILED";
    }
    show_hmac_debug(mac_check, DIGEST_SIZE, check.c_str());
#endif

    return (cmp == 0);
}


void AuthHMAC::fill_nonce(unsigned char* nonce) {
    for (unsigned int i=0; i < NONCE_LEN; i++)
        nonce[i] = static_cast<unsigned char>(rand()%256);
}
