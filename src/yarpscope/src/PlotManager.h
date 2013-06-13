/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARPSCOPE_PLOTMANAGER_H
#define YARPSCOPE_PLOTMANAGER_H


namespace Glib {
class ustring;
}

namespace Gtk {
class Widget;
}

namespace YarpScope
{

class PlotManager
{
protected:
    PlotManager();

public:
    virtual ~PlotManager();

    static PlotManager& instance();

    void setupTable(int rows, int columns);

    int addPlot(const Glib::ustring &title,
                int gridx,
                int gridy,
                int hspan,
                int vspan,
                float minval,
                float maxval,
                int size,
                const Glib::ustring &bgcolor,
                bool autorescale);

    int addGraph(int portIndex,
                 const Glib::ustring &remote,
                 int index,
                 const Glib::ustring &title,
                 const Glib::ustring &color,
                 const Glib::ustring &type,
                 int size);

    Gtk::Widget *getPlotWidget() const;
    void redraw(bool increment = true) const;
    void autoRescale() const;

private:

    // not implemented
    PlotManager& operator=(const PlotManager &other);
    PlotManager(const PlotManager &other);

    class Private;
    Private * const mPriv;
};

} // namespace YarpScope


#endif // YARPSCOPE_PLOTMANAGER_H
