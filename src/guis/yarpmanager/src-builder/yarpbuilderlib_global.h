/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARPBUILDERLIB_GLOBAL_H
#define YARPBUILDERLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(YARPBUILDERLIB_LIBRARY)
#  define YARPBUILDERLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define YARPBUILDERLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // YARPBUILDERLIB_GLOBAL_H
