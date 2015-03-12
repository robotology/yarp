/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef LOG_H
#define LOG_H

#ifdef QT_DEBUG
#define LOG(...) qDebug(__VA_ARGS__)
#else
#define LOG(...) fprintf(stdout,__VA_ARGS__)
#endif

#ifdef QT_DEBUG
#define LOG_ERROR(...) qDebug(__VA_ARGS__)
#else
#define LOG_ERROR(...) fprintf(stderr,__VA_ARGS__)
#endif

#endif // LOG_H
