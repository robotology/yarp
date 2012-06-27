/*
 *  This file is part of Yarp Port Scope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef YARPSCOPE_PORTREADER_H
#define YARPSCOPE_PORTREADER_H

namespace Glib {
class ustring;
}

namespace YarpScope
{

class PortReader
{
protected:
    PortReader();

public:
    virtual ~PortReader();

    static PortReader& instance();

    void setInterval(int interval);
    int interval() const;

    void toggleAcquire(bool acquire);
    void toggleAcquire();
    bool isAcquiring() const;

    void clearData();

    void acquireData(const Glib::ustring &remotePortName, int index);

    float* X(const Glib::ustring &remotePortName, int index) const;
    float* Y(const Glib::ustring &remotePortName, int index) const;
    float* T(const Glib::ustring &remotePortName, int index) const;

    int bufSize() const;

private:

    // not implemented
    PortReader& operator=(const PortReader &other);
    PortReader(const PortReader &other);

    class Private;
    Private * const mPriv;
};

} // namespace YarpScope


#endif // YARPSCOPE_PORTREADER_H
