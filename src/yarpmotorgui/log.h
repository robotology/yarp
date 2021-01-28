/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
