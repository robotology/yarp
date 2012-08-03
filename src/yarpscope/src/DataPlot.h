/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */



#ifndef YARPSCOPE_DATAPLOT_H
#define YARPSCOPE_DATAPLOT_H

#include <gtkdataboxmm/databox.h>

#include <gdkmm/color.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>


namespace Gtk
{
class Table;
}

namespace YarpScope {

class DataPlot : public GDatabox::Databox
{
public:
    DataPlot(Gtk::Table **table,
             float minval,
             float maxval,
             int size,
             const Glib::ustring &bgcolor,
             bool autorescale);

    virtual ~DataPlot();

    void toggleAcquire(bool toggled);
    int size() const;

private:
    class Private;
    Private * const mPriv;
};

} // YarpScope

#endif // YARPSCOPE_DATAPLOT_H
