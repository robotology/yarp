/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LOG_H
#define LOG_H

#ifdef QT_DEBUG
#define LOG(...) qDebug(__VA_ARGS__)
#else
#define LOG(...) yDebug(__VA_ARGS__)
#endif

#ifdef QT_DEBUG
#define LOG_ERROR(...) qDebug(__VA_ARGS__)
#else
#define LOG_ERROR(...) yError(__VA_ARGS__)
#endif

#ifdef QT_DEBUG
#define LOG_INFO(...) qDebug(__VA_ARGS__)
#else
#define LOG_INFO(...) yInfo(__VA_ARGS__)
#endif

#endif // LOG_H
