/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <QObject>
#include "plotter.h"
#include <QTimer>

/*! \class PlotManager
    \brief The Manager of the plotters

    This class is used to manage the various plotters
*/
class PlotManager : public QObject
{
    Q_OBJECT

public:
    explicit PlotManager(QObject *parent = 0);
    static PlotManager *instance();
    ~PlotManager();
    Plotter * addPlot(const QString &title,
                      int gridx,
                      int gridy,
                      int hspan,
                      int vspan,
                      float minval,
                      float maxval,
                      int size,
                      const QString &bgcolor,
                      bool autorescale);



    QList <QObject*>* getPlotters();
    void setInterval(int interval);
    void setMainCustomPlot(QCustomPlot*);
    void playPressed(bool check);
    void clear();
    void rescale();


private:
    QList <QObject*> plotterList;
    static PlotManager *self;
    QTimer timer;

signals:
    void requestRepaint();
    void plottersChanged();
public slots:
    void onTimeout();

};

#endif // PLOTMANAGER_H
