/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
