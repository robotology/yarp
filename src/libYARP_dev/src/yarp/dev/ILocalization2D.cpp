/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ILocalization2D.h>

yarp::dev::ILocalization2D::~ILocalization2D() = default;

yarp::dev::myclass::~myclass() { delete a; a = nullptr; }
