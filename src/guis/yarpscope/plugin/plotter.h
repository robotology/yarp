/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLOTTER_H
#define PLOTTER_H

#include <QObject>
#include "yarp/os/BufferedPort.h"
#include "yarp/os/Network.h"
#include <QTimer>
#include <QVariant>
#include <qcustomplot.h>

#define GRAPH_TYPE_LINE     0
#define GRAPH_TYPE_BARS     1
#define GRAPH_TYPE_POINTS   2

class Connection;

/*! \class Graph
    \brief Class representing a Graph
*/
class Graph : public QObject
{
    Q_OBJECT


public:
    Graph(int index, QString title,QString color,QString type,int size,double graph_y_scale, int buffer_size,QObject *parent = 0);
    ~Graph();
    void init(QString remotePortName,
                       QString localPortName,
                       QString carrier,
                       bool persistent);

    void appendPreviousValues();
    void appendValues(float y, float t);

    void setCustomGraphPoint(QCPGraph*);
    void setCustomGraph(QCPGraph*);

    void clearData();

    Connection *getConnection();

    double lastX;
    double lastY;
    double lastT;


    int getType();
    QString getColor();
    int getLineSize();
    bool deleteConnection;

    QCPGraph *customGraphPoint;
    QCPGraph *customGraph;

    int index;
    double graph_y_scale;

    Connection *curr_connection;


private:
    int buffer_size;
    qint64 numberAcquiredData;
    int lastIndex;
    QList<double> X;
    QList<double> Y;
    QList<double> T;
    QString type;
    QString color;
    int lineSize;
    QString title;



signals:
    void valuesChanged();
    void acquiredDataChanged();

};

/*! \class Connection
    \brief Class representing a Connection
*/
class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(QString remotePortName, QString localPortName, QObject *parent = 0);

    ~Connection();
    void connect(const yarp::os::ContactStyle &style);
    void freeResources();

public:
    QString remotePortName;
    QString localPortName;

    yarp::os::BufferedPort<yarp::os::Bottle> *localPort;
    bool realTime;
    double initialTime;

    yarp::os::ContactStyle style;
};

/*! \class Plotter
    \brief Class representing a Plotter
*/
class Plotter : public QObject
{
    Q_OBJECT

public:
    explicit Plotter(const QString &title,
                     int gridx,
                     int gridy,
                     int hspan,
                     int vspan,
                     float minval,
                     float maxval,
                     int size,
                     const QString &bgcolor,
                     bool autorescale,
                     QObject *parent = 0);
    ~Plotter();
    Graph *addGraph(QString remotePort, QString localPort, int index, QString title, QString color, QString type, int size, double graph_y_scale=1.0);
    void clear();
    void rescale();
    void setPaintGeometry(QRectF);

public:
    //QPixmap    *picture;
    QList<QObject*> graphList;
    QCustomPlot customPlot;
    //QCPAxisRect *axisRect;
    QRectF paintRectGeometry;

    //Connection *curr_connection;
    QString title;
    int gridx;
    int gridy;
    int hspan;
    int vspan;
    float minval;
    float maxval;
    int size;
    QString bgcolor;
    bool autorescale;
    int initialSize;
    int start;
    bool interact;

signals:
    void maxValueChanged();
    void minValueChanged();
    void internalAppendGraph(int index, QString title, QString color, QString type, int size, int buffer_size);
    void plotSampleSizeChanged();
    void duplicateLastGraphValue(int graphIndex);
public slots:
    void onInteract();
    void onTimeout();

};

#endif // PLOTTER_H
