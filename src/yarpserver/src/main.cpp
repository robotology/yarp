/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/yarpserversql.h>

int main(int argc, char *argv[]) {
    int ret=yarpserver_main(argc, argv);
    return (ret!=0?1:0);
}
