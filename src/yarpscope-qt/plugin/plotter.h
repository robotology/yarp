#ifndef PLOTTER_H
#define PLOTTER_H

#include <QObject>
#include "yarp/os/BufferedPort.h"
#include "yarp/os/Network.h"
#include <QTimer>
#include <QVariant>
#include "qcustomplot.h"

#define GRAPH_TYPE_LINE     0
#define GRAPH_TYPE_BARS     1
#define GRAPH_TYPE_POINTS   2

class Graph : public QObject
{
    Q_OBJECT


public:
    Graph(int index, QString title,QString color,QString type,int size,int buffer_size,QObject *parent = 0);
    ~Graph();
    void appendPreviousValues();
    void appendValues(float y, float t);

    void setCustomGraphPoint(QCPGraph*);
    void setCustomGraph(QCPGraph*);

    void clearData();

    double lastX;
    double lastY;
    double lastT;


    int getType();
    QString getColor();
    int getLineSize();

    QCPGraph *customGraphPoint;
    QCPGraph *customGraph;

    int index;


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

class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(QString remotePortName, QString localPortName, QObject *parent = 0);

    ~Connection();
    void connect(const yarp::os::ContactStyle &style) ;

public:
    QString remotePortName;
    QString localPortName;

    yarp::os::BufferedPort<yarp::os::Bottle> *localPort;
    bool realTime;
    double initialTime;

    yarp::os::ContactStyle style;
};

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
    void init(QString remotePortName, QString localPortName, QString carrier, bool persistent);
    int addGraph(int index, QString title, QString color, QString type, int size);
    void clear();
    void rescale();
    void setPaintGeometry(QRectF);

public:
    QPixmap    *picture;
    QList<QObject*> graphList;
    QCustomPlot customPlot;
    QCPAxisRect *axisRect;
    QRectF paintRectGeometry;

    Connection *curr_connection;
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
    void newGraphValue(int graphIndex, double x, double y, double t);
    void duplicateLastGraphValue(int graphIndex);
public slots:
    void onInteract();
    void onTimeout();

};

#endif // PLOTTER_H
