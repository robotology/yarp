/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    void acquireData(const Glib::ustring &remotePortName,
                     int index,
                     const Glib::ustring &localPortName,
                     const Glib::ustring &carrier,
                     bool persistent);

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
