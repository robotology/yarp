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

#ifndef QTYARPSCOPE_H
#define QTYARPSCOPE_H

#include <QtQuick>
#include <yarp/os/Network.h>
#include "simpleloader.h"
#include "plotmanager.h"
#include <QTimer>
#include <QMutex>
#include <qcustomplot.h>

/*! \class QtYARPScope
    \brief The plugin Core class

    this is the plugin core class which acts as bridge between the QML and c++.
    in the C++ code is implemented the backend logic, instead in the QML is
    implemented the Visual part.
    This Core plugin derived from QQuickPaintedItem and has the flag ItemHasContents
    set to true, so it can paint itself what it needs.
*/
class QtYARPScope : public QQuickPaintedItem
{
    Q_OBJECT
    Q_DISABLE_COPY(QtYARPScope)

public:
    QtYARPScope(QQuickItem *parent = 0);
    Q_INVOKABLE bool parseParameters(QStringList params);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void rescale();
    Q_INVOKABLE void changeInterval(int interval);
    Q_INVOKABLE void playPressed(int check);
    ~QtYARPScope();
    void usage();
    void paint(QPainter *painter) override;




protected:
    void routeMouseEvents( QMouseEvent* event);
    void routeMouseEvents( QWheelEvent* event);

    void mousePressEvent( QMouseEvent* event) override;
    void mouseReleaseEvent( QMouseEvent* event) override;
    void mouseMoveEvent( QMouseEvent* event) override;
    void mouseDoubleClickEvent( QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    //QCustomPlot m_CustomPlot;
    QTimer timer;
    int i;

    yarp::os::Network yarp;
    GenericLoader *loader;
    PlotManager *plotManager;
    QObject *topLevel;
    QMutex mutex;
    QRectF zoomRect;
    bool bPressed;
    QRectF currentSelectedPlotterRect;
    Plotter *currentSelectedPlotter;


signals:
    void setWindowTitle(QString title);
    void setWindowPosition(int x, int y);
    void setWindowSize(int w, int h);
    void intervalLoaded(int interval);

private slots:
    void graphClicked( QCPAbstractPlottable* plottable );
    void updateCustomPlotSize();
    void onRepaint();
};

#endif // QTYARPSCOPE_H
