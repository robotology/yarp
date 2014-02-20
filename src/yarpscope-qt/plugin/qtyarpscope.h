/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef QTYARPSCOPE_H
#define QTYARPSCOPE_H

#include <QtQuick>
#include <yarp/os/Network.h>
#include "simpleloader.h"
#include "plotmanager.h"
#include <QTimer>
#include <QMutex>
#include "qcustomplot.h"

/*! \class QtYARPScope
    \brief The plugin Core class

    this is the plugin core class wich acts as bridge between the QML and c++.
    in the C++ code is implemented the backend logic, instead in the QML is
    implemented the Visual part.
    This Core plugin derived from QQuickPaintedItem and has the flag ItemHasContents
    setted to true, so it can paint itself what it needs.
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
    void paint(QPainter *painter);




protected:
    void routeMouseEvents( QMouseEvent* event );
    void routeMouseEvents( QWheelEvent* event );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void wheelEvent(QWheelEvent* event);

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

