/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SIMPLELOADER_H
#define SIMPLELOADER_H

#include <QObject>
#include "yarp/os/ResourceFinder.h"
#include "plotmanager.h"
#include "genericloader.h"

/*! \class SimpleLoader
    \brief Reads a configuration from a xml file
*/
class SimpleLoader : public GenericLoader
{
    Q_OBJECT
public:
    explicit SimpleLoader(/* FIXME const */ yarp::os::ResourceFinder *options, PlotManager *plotManager,bool *ok, QObject *parent = 0);

private:
    PlotManager *plotManager;

    QString plot_bgcolor;
    int plot_size;
    float plot_minval;
    float plot_maxval;
    bool plot_autorescale;
    bool plot_realtime;
    bool plot_triggermode;
    QString plot_title;

    QString graph_remote;
    int graph_index;
    QString graph_localport;
    QString graph_title;
    QString graph_color;
    QString graph_type;
    int graph_size;
};

#endif // SIMPLELOADER_H
