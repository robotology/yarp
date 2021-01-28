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

#ifndef PORTREADER_H
#define PORTREADER_H

#include <QObject>

class PortReader : public QObject
{
    Q_OBJECT
public:
    explicit PortReader(QObject *parent = 0);
    static PortReader *instance();

    void acquireData(const QString &remotePortName,
                     int index,
                     const QString &localPortName,
                     const QString &carrier,
                     bool persistent);

private:
    static PortReader *self;
signals:

public slots:

};

#endif // PORTREADER_H
