/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
