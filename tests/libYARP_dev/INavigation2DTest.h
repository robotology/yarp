/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef INAVIGATION2DTEST_H
#define INAVIGATION2DTEST_H

#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

void exec_iNav2D_test_1(INavigation2D* inav, IMap2D* imap);
void exec_iNav2D_test_2(INavigation2D* inav, IMap2D* imap);

#endif
