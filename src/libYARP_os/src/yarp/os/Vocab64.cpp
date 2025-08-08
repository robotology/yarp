/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Vocab64.h>

using namespace yarp::os;

NetInt64 Vocab64::encode64(const std::string& str)
{
    char a = '\0';
    char b = '\0';
    char c = '\0';
    char d = '\0';
    char e = '\0';
    char f = '\0';
    char g = '\0';
    char h = '\0';
    if (str.length() >= 1) {
        a = str[0];
        if (str.length() >= 2) {
            b = str[1];
            if (str.length() >= 3) {
                c = str[2];
                if (str.length() >= 4) {
                    d = str[3];
                    if (str.length() >= 5) {
                       e = str[4];
                       if (str.length() >= 6) {
                          f = str[5];
                          if (str.length() >= 7) {
                             g = str[6];
                             if (str.length() >= 8) {
                                 h = str[7];
                             }
                          }
                       }
                    }
                }
            }
        }
    }
    return createVocab64(a, b, c, d, e, f, g, h);
}


std::string Vocab64::decode64(NetInt64 code)
{
    std::string str;
    for (int i = 0; i < 8; i++) {
        int ch = code % 256;
        if (ch > 0) {
            str += ((char)ch);
        }
        code /= 256;
    }
    return str;
}
