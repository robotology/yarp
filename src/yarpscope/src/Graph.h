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