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



#ifndef YARPSCOPE_DATAPLOT_H
#define YARPSCOPE_DATAPLOT_H

#include <gtkdataboxmm/databox.h>

#include <gdkmm/color.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>


namespace YarpScope {

class DataPlot : public GDatabox::Databox
{
public:
    DataPlot(const Glib::ustring &title,
             int minval,
             int maxval,
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
