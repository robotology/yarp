/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef GENERICLOADER_H
#define GENERICLOADER_H

#include <QObject>
#include "plotmanager.h"

#include <yarp/os/ContactStyle.h>

/*! \class GenericLoader
    \brief Base Class for the Loaders. It stores some common features
    that could be red from xml or command line
*/
class GenericLoader : public QObject
{
    Q_OBJECT
public:
    explicit GenericLoader(QObject *parent = 0);

protected:
    const int default_portscope_rows;
    const int default_portscope_columns;
    const QString default_portscope_carrier;
    const bool default_portscope_persistent;
    const QString default_portscope_localport;
    const int default_plot_gridx;
    const int default_plot_gridy;
    const int default_plot_hspan;
    const int default_plot_vspan;
    const float default_plot_minval;
    const float default_plot_maxval;
    const int default_plot_size;
    const QString default_plot_bgcolor;
    const bool default_plot_autorescale;
    const bool default_plot_realtime;
    const bool default_plot_triggermode;
    const QString default_plot_title;
    const int default_graph_size;
    const QString default_graph_type;
    const QString default_graph_color;
    const QString default_graph_title;

public:
    int portscope_rows;
    int portscope_columns;
    QString portscope_carrier;
    bool portscope_persistent;
};

#endif // GENERICLOADER_H
