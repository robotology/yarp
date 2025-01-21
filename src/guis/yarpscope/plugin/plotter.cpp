/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plotter.h"
#include "yarp/os/ContactStyle.h"
#include "yarp/os/Time.h"
#include "yarp/os/Stamp.h"
#include <QDebug>
#include <utility>

/*! \brief Constructor of the class.
 *
 *  \param title the title of the plotter
 *  \param gridx the x pos in the grid
 *  \param gridy the y pos in the grid
 *  \param hspan the horizonatl span
 *  \param vspan the vertical span
 *  \param minval the minimum scale value
 *  \param maxval the maximium scale value
 *  \param size number of sample of datas in plotter
 *  \param bgcolor the background color of the plotter
 *  \param autorescale not used
 */
Plotter::Plotter(const QString &title, int gridx, int gridy, int hspan, int vspan, float minval, float maxval, int size, const QString &bgcolor, bool autorescale,  QObject *parent) :
    QObject(parent),
    title(title),
    gridx(gridx),
    gridy(gridy),
    hspan(hspan),
    vspan(vspan),
    minval(minval),
    maxval(maxval),
    size(size),
    bgcolor(bgcolor),
    autorescale(autorescale),
    initialSize(size),
    start(0),
    interact(false)
{
    connect(customPlot.axisRect(),SIGNAL(zoomRequest()),this,SLOT(onInteract()));
    connect(customPlot.axisRect(),SIGNAL(dragStarted()),this,SLOT(onInteract()));

    customPlot.axisRect()->setBackground(QBrush(QColor(bgcolor)));
    customPlot.axisRect()->setupFullAxesBox(true);
#if !defined(QCUSTOMPLOT_VERSION) || (QCUSTOMPLOT_VERSION < 0x020000)
    customPlot.axisRect()->axis(QCPAxis::atBottom)->setTickLabelType(QCPAxis::ltNumber);
    customPlot.axisRect()->axis(QCPAxis::atBottom)->setAutoTickStep(false);
    customPlot.axisRect()->axis(QCPAxis::atBottom)->setTickStep(25);
#else
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    customPlot.axisRect()->axis(QCPAxis::atBottom)->setTicker(fixedTicker);
    fixedTicker->setTickStep(25);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
#endif
    customPlot.axisRect()->axis(QCPAxis::atBottom)->setRange(0,size);
    customPlot.axisRect()->axis(QCPAxis::atLeft)->setRange(minval, maxval);
    customPlot.setInteractions( QCP::iRangeDrag | QCP::iRangeZoom  );
    auto* textLabel = new QCPItemText(&customPlot);
#if !defined(QCUSTOMPLOT_VERSION) || (QCUSTOMPLOT_VERSION < 0x020000)
    customPlot.addItem(textLabel);
#else
    // Nothing to do
#endif
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
    textLabel->setText(title);

    connect(customPlot.xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot.xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot.yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot.yAxis2, SLOT(setRange(QCPRange)));

}

/*! \brief Sets the interaction mode to true. Called when the user interact with the plotter using mouse*/
void Plotter::onInteract()
{
    interact = true;
}



Plotter::~Plotter()
{
    for (int i=0;i<graphList.count(); i++) {
        auto* idx = (Graph*)graphList.at(i);
        if (idx) {
            delete idx;
            idx = nullptr;
        }
    }
    graphList.clear();

}

/*! \brief Rescales the plotter and its graphs */
void Plotter::rescale()
{
    interact = false;
    customPlot.rescaleAxes(true);
}

/*! \brief Sets the paint geometry for this Plotter */
void Plotter::setPaintGeometry(QRectF r)
{
    paintRectGeometry = r;
}


/*! \brief Add a Graph to the current Plotter
    \param index the index of the graph
    \param title the title of the graph
    \param color the color of the graph
    \param type the type of the graph (bar, lines, points)
    \param the tickness of the graph
    \param graph_y_scale to multiply the all data points for a scale factor
*/
Graph * Plotter::addGraph(QString remotePort,QString localPort,int index, QString title, QString color, QString type, int size, double graph_y_scale)
{
    Graph *graph = nullptr;
    graph = new Graph(index,title,color,type,size,graph_y_scale,this->size);


    for(int i=0;i<graphList.count();i++) {
        auto* g = (Graph *)graphList.at(i);
        Connection *con = g->getConnection();
        if(!con){
            continue;
        }
        if(con->remotePortName == remotePort && con->localPortName == localPort){
            graph->curr_connection = g->curr_connection;
            graph->deleteConnection = false;
            break;
        }
    }
    graphList.append(graph);


    QCPGraph *customGraph = customPlot.addGraph(); // line
    customGraph->setPen(QPen(QColor(color),size));
    customGraph->setAntialiased(false);
    customGraph->setLineStyle(QCPGraph::lsLine);

    if(type == "points"){
        customGraph->setLineStyle(QCPGraph::lsNone);
        customGraph->setScatterStyle(QCPScatterStyle::ssDot);
    }

    if(type == "bars"){
        customGraph->setLineStyle(QCPGraph::lsImpulse);
        customGraph->setScatterStyle(QCPScatterStyle::ssNone);
    }


    QCPGraph *customGraphPoint = customPlot.addGraph(); // dot
    customGraphPoint->setPen(QPen(QColor(color)));
    customGraphPoint->setLineStyle(QCPGraph::lsNone);
    customGraphPoint->setScatterStyle(QCPScatterStyle::ssDisc);

    graph->setCustomGraph(customGraph);
    graph->setCustomGraphPoint(customGraphPoint);
    customPlot.replot();


    return graph;
}


/*! \brief Timeout on which the data is acquired */
void Plotter::onTimeout()
{
    if (graphList.empty()) {
        // Do not read data from this port if we don't need it
        return;
    }

    int c = graphList.count();
    for (int j=0;j < c; j++) {
        auto* graph = (Graph*)graphList.at(j);
        yarp::os::Bottle *b;
        if(graph->deleteConnection){
            b = graph->curr_connection->localPort->read(false);
        }else{
            b = graph->curr_connection->localPort->lastRead();
        }
        if (!b) {
//             qDebug("No data received. Using previous values.");
            graph->appendPreviousValues();

        } else {
            if (b->size() == 1 && b->get(0).isList()) {
                b = b->get(0).asList();
            }
            yarp::os::Stamp stmp;
            graph->curr_connection->localPort->getEnvelope(stmp);

            if (b->size() - 1 < (size_t) graph->index) {
                qWarning() << "bottle size =" << b->size() << " requested index =" << graph->index;
                continue;
            }


            double y = (float)(b->get(graph->index).asFloat64());

            float t;
            if (graph->curr_connection->realTime && stmp.isValid()) {
                t = (float)(stmp.getTime() - graph->curr_connection->initialTime);
            } else {
                t = -1.0;
            }

            graph->appendValues(y,t);

        }


    }

    // if the user did not interact with the plotter, it remains aligned to the right
    // else, there is no alignment and the user has the freedom to pan and zoom it
    if(!interact){
        auto* graph = (Graph*)graphList.at(0);
        if(graph){
            customPlot.xAxis->setRange(graph->lastX+5, size, Qt::AlignRight);
        }
    }

    customPlot.replot();

}


/*! \brief Clears the graphs data */
void Plotter::clear()
{
    for (int j=0;j < graphList.count(); j++) {
        auto* graph = (Graph*)graphList.at(j);
        graph->clearData();
    }
    customPlot.replot();
}


/***********************************************************/
Graph::Graph(int index, QString title, QString color, QString type, int size, double graph_y_scale, int buffer_size, QObject *parent) :
    QObject(parent),
    lastX(0),
    lastY(0),
    lastT(0),
    deleteConnection(true),
    customGraphPoint(nullptr),
    customGraph(nullptr),
    index(index),
    graph_y_scale(graph_y_scale),
    curr_connection(nullptr),
    buffer_size(buffer_size),
    numberAcquiredData(0),
    lastIndex(0),
    type(std::move(type)),
    color(std::move(color)),
    lineSize(size),
    title(std::move(title))
{}


void Graph::init(QString remotePortName,
                   QString localPortName,
                   QString carrier,
                   bool persistent)
{

    yarp::os::ContactStyle  style;

    if(!curr_connection){
        if (persistent) {
            style.persistent = persistent;
            style.persistenceType = yarp::os::ContactStyle::END_WITH_TO_PORT;
        }
        style.carrier = carrier.toLatin1().data();

        curr_connection = new Connection(remotePortName, localPortName);
        curr_connection->connect(style);
    }
}

Graph::~Graph()
{
    if(curr_connection && deleteConnection){
        delete curr_connection;
        curr_connection = nullptr;
    }
    clearData();
}
Connection *Graph::getConnection()
{
    return curr_connection;
}

/*! \brief Return the graph type */
int Graph::getType()
{
    if(type.compare("lines") == 0){
        return GRAPH_TYPE_LINE;
    }
    if(type.compare("bars") == 0){
        return GRAPH_TYPE_BARS;
    }
    if(type.compare("points") == 0){
        return GRAPH_TYPE_POINTS;
    }

    return GRAPH_TYPE_LINE;
}

/*! \brief Return tickness of the graph */
int Graph::getLineSize()
{
    return lineSize;
}

/*! \brief Return color of the grpah */
QString Graph::getColor()
{
    return color;
}

/*! \brief Append the previous values acquired */
void Graph::appendPreviousValues()
{
    appendValues(lastY,lastT);
}

/*! \brief Append the new values acquired */
void Graph::appendValues(float y, float t)
{
    float _t = t;
    if(t == -1){
        _t = (float)numberAcquiredData;
    }

    //apply the y scale factor
    y=y*graph_y_scale;

    lastX = numberAcquiredData;
    lastY = y;
    lastT = _t;

    if(customGraph && customGraphPoint){
        customGraph->addData(lastX,lastY);
#if !defined(QCUSTOMPLOT_VERSION) || (QCUSTOMPLOT_VERSION < 0x020000)
        customGraphPoint->clearData();
#else
        customGraphPoint->data()->clear();
#endif
        customGraphPoint->addData(lastX,lastY);
#if !defined(QCUSTOMPLOT_VERSION) || (QCUSTOMPLOT_VERSION < 0x020000)
        customGraph->removeDataBefore(lastX - 4*buffer_size);
#else
        customGraph->data()->removeBefore(lastX - 4*buffer_size);
#endif
        numberAcquiredData++;
    }

}

/*! \brief Sets the Custom Graph from the QCustomPlot class to this graph
    \param g the Custom Graph
*/
void Graph::setCustomGraph(QCPGraph *g)
{
    customGraph = g;
}

/*! \brief Sets the Custom Graph Point from the QCustomPlot class to this graph
    \param g the Custom Graph
*/
void Graph::setCustomGraphPoint(QCPGraph *g)
{
    customGraphPoint = g;
}


/*! \brief Clears the custom graph datas */
void Graph::clearData()
{
    if(customGraph){
#if !defined(QCUSTOMPLOT_VERSION) || (QCUSTOMPLOT_VERSION < 0x020000)
        customGraph->clearData();
#else
        customGraph->data()->clear();
#endif
    }
    if(customGraphPoint){
#if !defined(QCUSTOMPLOT_VERSION) || (QCUSTOMPLOT_VERSION < 0x020000)
        customGraphPoint->clearData();
#else
        customGraphPoint->data()->clear();
#endif
    }

}


/***************************************************************/

Connection::Connection(QString remotePortName,  QString localPortName, QObject *parent): QObject(parent)
{

    this->remotePortName = remotePortName;
    this->localPortName = localPortName;
    localPort = new yarp::os::BufferedPort<yarp::os::Bottle>();
    realTime = false;
    initialTime = 0.0;



    // Open the local port
    if (localPortName.isEmpty()) {
        localPort->open("...");
    } else {
        localPort->open(localPortName.toLatin1().data());
    }

    realTime = true;
    initialTime = yarp::os::Time::now();
}

Connection::~Connection()
{
    freeResources();
}

/*! \brief Connect local port to remote port */
void Connection::connect(const yarp::os::ContactStyle &style) {

    //Get the name of the port after the port is open (and therefore the real name assigned)
    const QString &realLocalPortName = localPort->getName().c_str();

    // Connect local port to remote port
    if (!yarp::os::Network::connect(remotePortName.toLatin1().data(), realLocalPortName.toLatin1().data(), style)) {
        qWarning() << "Connection from port" << realLocalPortName.toLatin1().data() <<  "to port" << remotePortName.toLatin1().data()
                   << "was NOT successful. Waiting from explicit connection from user.";
    } else {
        qDebug("Listening to port %s from port %s",remotePortName.toLatin1().data(),realLocalPortName.toLatin1().data());
        // Connection was successful. Save the ContactStyle in order to reuse it for disconnecting;
        this->style = style;
    }

    yarp::os::Stamp stmp;
    localPort->getEnvelope(stmp);
    if (stmp.isValid()) {
        qDebug("will use real time for port %s",remotePortName.toLatin1().data());
        realTime = true;
        initialTime = stmp.getTime();
    } else {
        qDebug("will NOT use real time for port %s",remotePortName.toLatin1().data());
        realTime = false;
    }
}

void Connection::freeResources()
{
    if(localPort)
    {
        yarp::os::Network::disconnect(remotePortName.toLatin1().data(), localPort->getName(), style);
        delete localPort;
        localPort = nullptr;
    }
}
