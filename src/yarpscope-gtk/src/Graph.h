/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARPSCOPE_GRAPH_H
#define YARPSCOPE_GRAPH_H

#include <glibmm.h>


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