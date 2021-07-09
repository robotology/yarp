/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Vocab.h>

using namespace yarp::os;

NetInt32 Vocab32::encode(const std::string& str)
{
    char a = '\0';
    char b = '\0';
    char c = '\0';
    char d = '\0';
    if (str.length() >= 1) {
        a = str[0];
        if (str.length() >= 2) {
            b = str[1];
            if (str.length() >= 3) {
                c = str[2];
                if (str.length() >= 4) {
                    d = str[3];
                }
            }
        }
    }
    return createVocab32(a, b, c, d);
}


std::string Vocab32::decode(NetInt32 code)
{
    std::string str;
    for (int i = 0; i < 4; i++) {
        int ch = code % 256;
        if (ch > 0) {
            str += ((char)ch);
        }
        code /= 256;
    }
    return str;
}
