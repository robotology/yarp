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


#ifndef YARPSCOPE_GRAPH_H
#define YARPSCOPE_GRAPH_H

#include <glibmm/refptr.h>


namespace Glib
{
class ustring;
}

namespace GDatabox
{
class Graph;
}

namespace YarpScope
{
class PlotManager;

class Graph
{
public:
    enum Type
    {
        TypeUnset = 0,
        TypePoints,
        TypeLines,
        TypeBars
    };

    Graph(const Glib::ustring &remote,
          int index,
          const Glib::ustring &title,
          const Glib::ustring &color,
          const Glib::ustring &type,
          int size,
          int plotSize);

    virtual ~Graph();

    Glib::RefPtr<GDatabox::Graph> graph() const;
    const Glib::ustring& title() const;

protected:
    void update(bool increment);

private:
    class Private;
    Private * const mPriv;

    friend class YarpScope::PlotManager;
};

} // namespace YarpScope

#endif // YARPSCOPE_GRAPH_H