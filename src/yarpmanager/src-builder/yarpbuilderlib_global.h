/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
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
