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

public:
    QString graph_remote;
    QString local_port;
    QString portscope_carrier;
    QString plot_title;
    QString plot_bgcolor;
    bool portscope_persistent;
    int plot_size;
    float plot_minval;
    float plot_maxval;
    bool plot_autorescale;
    bool plot_realtime;
    bool plot_triggermode;
    QString graph_title;
    QString graph_color;
    QString graph_type;
    int graph_index;
    int graph_size;
    int portscope_rows;
    int portscope_columns;
   /* int plot_gridx;
    int plot_gridy;
    int plot_hspan;
    int plot_vspan;*/

protected:
    QString default_bg_color;
    int default_portscope_rows;
    int default_portscope_columns;
    QString default_portscope_carrier;
    bool default_portscope_persistent;
    int default_plot_gridx;
    int default_plot_gridy;
    int default_plot_hspan;
    int default_plot_vspan;
    float default_plot_minval;
    float default_plot_maxval;
    int default_plot_size;
    bool default_plot_autorescale;
    bool default_plot_realtime;
    bool default_plot_triggermode;
    int default_graph_size;
    QString default_graph_type;
    QString default_local_port;
    QString default_graph_color;

signals:

public slots:

};

#endif // GENERICLOADER_H
