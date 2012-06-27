/*
 *  This file is part of gPortScope
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


#ifndef GPORTSCOPE_DATAPLOT_H
#define GPORTSCOPE_DATAPLOT_H

#include <gtkdataboxmm/databox.h>

#include <gdkmm/color.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>


namespace GPortScope {

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

} // GPortScope

#endif // GPORTSCOPE_DATAPLOT_H
