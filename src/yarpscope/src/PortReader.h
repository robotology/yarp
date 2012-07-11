/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
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
