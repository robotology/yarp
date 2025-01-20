/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
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
    QTimer timer;

signals:
    void requestRepaint();
    void plottersChanged();
public slots:
    void onTimeout();

};

#endif // PLOTMANAGER_H
