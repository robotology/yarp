/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "portreader.h"

PortReader *PortReader::self = nullptr;

PortReader::PortReader(QObject *parent) :
    QObject(parent)
{
    self = this;
}

PortReader * PortReader::instance()
{
    if(self == nullptr){
        return  new PortReader();
    }
    return self;
}

void PortReader::acquireData(const QString &remotePortName,
                     int index,
                     const QString &localPortName,
                     const QString &carrier,
                     bool persistent)
{

}
