/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <fstream>

#include "MainWindow.h"
#include "moc_MainWindow.cpp"
#include "ui_MainWindow.h"
#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include "QGVSubGraph.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtPrintSupport/QPrinter>
#include <QInputDialog>

#include <yarp/os/Random.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include "NetworkProfiler.h"
#include "ggraph.h"

#include "informationdialog.h"
#include "qosconfigdialog.h"
#include "portloggerdialog.h"
#include "batchqosconfdialog.h"
#include <iomanip>

using namespace std;
using namespace yarp::os;
using namespace yarp::graph;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), scene(NULL), currentGraph(NULL), progressDlg(NULL)
{
    ui->setupUi(this);
    stringModel.setStringList(messages);
    ui->messageView->setModel(&stringModel);
    ui->messageView->setVisible(false);

    // initialize the scene
    initScene();

    connect(ui->actionProfile_YARP_network, SIGNAL(triggered()),this,SLOT(onProfileYarpNetwork()));
    connect(ui->actionHighlight_Loops, SIGNAL(triggered()),this,SLOT(onHighlightLoops()));
    connect(ui->actionHideConnectionsLable, SIGNAL(triggered()),this,SLOT(onUpdateGraph()));
    connect(ui->actionHideDisconnectedPorts, SIGNAL(triggered()),this,SLOT(onUpdateGraph()));
    connect(ui->actionDebugMode, SIGNAL(triggered()),this,SLOT(onUpdateGraph()));
    connect(ui->actionColorMode, SIGNAL(triggered()),this,SLOT(onUpdateGraph()));
    connect(ui->actionOrthogonal, SIGNAL(triggered()),this,SLOT(onLayoutOrthogonal()));
    connect(ui->actionCurved, SIGNAL(triggered()),this,SLOT(onLayoutCurved()));
    connect(ui->actionPolyline, SIGNAL(triggered()),this,SLOT(onLayoutPolyline()));
    connect(ui->actionLine, SIGNAL(triggered()),this,SLOT(onLayoutLine()));
    connect(ui->actionSubgraph, SIGNAL(triggered()),this,SLOT(onUpdateGraph()));
    connect(ui->actionHidePorts, SIGNAL(triggered()),this,SLOT(onUpdateGraph()));
    connect(ui->nodesTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
            SLOT(onNodesTreeItemClicked(QTreeWidgetItem *, int)));
    connect(ui->actionMessageBox, SIGNAL(triggered()),this,SLOT(onWindowMessageBox()));
    connect(ui->actionItemswindow, SIGNAL(triggered()),this,SLOT(onWindowItem()));
    connect(ui->actionExport_scene, SIGNAL(triggered()),this,SLOT(onExportScene()));
    connect(ui->actionExport_connections_list, SIGNAL(triggered()),this,SLOT(onExportConList()));
    connect(ui->actionConfigure_connections_QOS, SIGNAL(triggered()),this,SLOT(onConfigureConsQos()));
    connect(ui->actionUpdateConnectionQosStatus, SIGNAL(triggered()),this,SLOT(onUpdateQosStatus()));
    connect(ui->actionProfilePortsRate, SIGNAL(triggered()),this,SLOT(onProfilePortsRate()));
    connect(ui->actionAbout, SIGNAL(triggered()),this,SLOT(onAbout()));

    //progressDlg = new QProgressDialog("...", "Cancel", 0, 100, this);

    layoutStyle = "ortho";
    ui->actionOrthogonal->setChecked(true);
    layoutSubgraph = true;
    ui->actionSubgraph->setChecked(true);
    ui->actionMessageBox->setChecked(false);
    ui->actionItemswindow->setChecked(true);

    ui->actionHighlight_Loops->setEnabled(false);
    ui->actionHidePorts->setEnabled(false);
    ui->actionHideDisconnectedPorts->setEnabled(false);
    ui->actionHideConnectionsLable->setEnabled(false);
    ui->actionDebugMode->setEnabled(false);
    ui->actionUpdateConnectionQosStatus->setEnabled(false);

    ui->action_Save_project->setEnabled(false);
    ui->action_Load_project->setEnabled(false);
    ui->actionDocumentaion->setEnabled(false);
    ui->actionProfilePortsRate->setEnabled(false);
    moduleParentItem = new QTreeWidgetItem( ui->nodesTreeWidget,  QStringList("Modules"));
    portParentItem = new QTreeWidgetItem( ui->nodesTreeWidget,  QStringList("Ports"));
    machinesParentItem = new QTreeWidgetItem( ui->nodesTreeWidget,  QStringList("Machines"));
    moduleParentItem->setIcon(0, QIcon(":icons/resources/module.svg"));
    portParentItem->setIcon(0, QIcon(":icons/resources/port.svg"));
    machinesParentItem->setIcon(0, QIcon(":icons/resources/computer_B.svg"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initScene() {
    if(scene) {
        scene->clear();
        delete scene;
    }
    scene = new QGVScene("yarpviz", this);
    sceneNodeMap.clear();
    sceneSubGraphMap.clear();
    ui->graphicsView->setBackgroundBrush(QBrush(QColor("#2e3e56"), Qt::SolidPattern));
    ui->graphicsView->setScene(scene);
    connect(scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
    connect(scene, SIGNAL(subGraphContextMenu(QGVSubGraph*)), SLOT(onSubGraphContextMenuProcess(QGVSubGraph*)));
    connect(scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
    connect(scene, SIGNAL(edgeContextMenu(QGVEdge*)), SLOT(edgeContextMenu(QGVEdge*)));
}

void MainWindow::onProgress(unsigned int percentage) {
    //yInfo()<<percentage<<"%";
    if(progressDlg)
        progressDlg->setValue(percentage);
}

void MainWindow::drawGraph(Graph &graph)
{
    initScene();
    if(graph.nodesCount() == 0)
        return;

    layoutSubgraph = ui->actionSubgraph->isChecked();


    //Configure scene attributes
    //scene->setGraphAttribute("label", "yarp-viz");
    scene->setGraphAttribute("splines", layoutStyle.c_str()); //curved, polyline, line. ortho
    scene->setGraphAttribute("rankdir", "LR");
    scene->setGraphAttribute("bgcolor", "#2e3e56");
    //scene->setGraphAttribute("concentrate", "true"); //Error !
    scene->setGraphAttribute("nodesep", "0.4");
    scene->setGraphAttribute("ranksep", "0.5");
    //scene->setNodeAttribute("shape", "box");
    scene->setNodeAttribute("style", "filled");
    scene->setNodeAttribute("fillcolor", "gray");
    scene->setNodeAttribute("height", "1.0");
    scene->setEdgeAttribute("minlen", "2.0");
    //scene->setEdgeAttribute("dir", "both");

    // drawing nodes
    // create a map between graph nodes and their visualization
    //std::map<const Vertex*, QGVNode*> nodeSet;

    //adding all machine subgraphs
    pvertex_const_iterator itr;
    const pvertex_set& vertices = graph.vertices();
    int countChild =0;
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(NULL);
        const Property& prop = (*itr)->property;
        QGVSubGraph *sgraph;
        if(dynamic_cast<MachineVertex*>(*itr))
        {
            string hostname =  prop.find("hostname").asString();
            if(layoutSubgraph) {
                std::stringstream key;
                key<<hostname;
                if(sceneSubGraphMap[key.str()] == NULL)
                {
                    sgraph = scene->addSubGraph(prop.toString().c_str());
                    sceneSubGraphMap[key.str()] = sgraph;
                    dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(sgraph);
                    sgraph->setVertex(*itr);
                    //sgraph->setAttribute("label", prop.find("name").asString().c_str());
                    sgraph->setAttribute("color", "#FFFFFF");
                    sgraph->setAttribute("label", prop.find("hostname").toString().c_str());
                    string host = prop.find("os").asString();
                    if(host == "Linux")
                        sgraph->setIcon(QImage(":/icons/resources/Linux-icon.png"));
                    else if(host == "Windows")
                        sgraph->setIcon(QImage(":/icons/resources/Windows-icon.png"));
                    else if(host == "Mac")
                        sgraph->setIcon(QImage(":/icons/resources/Mac-icon.png"));
                    else
                        sgraph->setIcon(QImage(":/icons/resources/Gnome-System-Run-64.png"));
                    std::string endNodeName = key.str() + ".end";
                    QGVNode * node = sgraph->addNode(endNodeName.c_str());
                    node->setAttribute("shape", "circle");
                    node->setAttribute("height", "0.0000000000001"); //a subgraph cannot be empty, adding fake hidden node
                    node->setAttribute("fixedsize", "true");
                    node->setAttribute("label", "");
                    node->setAttribute("fillcolor", "#2e3e56");
                    node->setAttribute("color", "#2e3e56");
                    node->setAttribute("node_type", "end");
                    node->setAttribute("rawname", endNodeName.c_str());
                    sceneNodeMap[endNodeName] = node;
                }
            }
        }
    }


    // adding all process subgraphs

    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(NULL);
        const Property& prop = (*itr)->property;
        QGVSubGraph *sgraph;
        if(dynamic_cast<ProcessVertex*>(*itr) && !prop.find("hidden").asBool())
        {
            int randNum = rand()%(16777214 - 0 + 1) + 0;
            stringstream hexStream;
            hexStream<<std::hex<< randNum;
            string hexRandNum ="#" + hexStream.str();
            string name =  prop.find("name").asString() + countChild;
            if(layoutSubgraph)
            {
                std::stringstream key;
                key<<prop.find("hostname").asString();
                QGVSubGraph *sgraphParent = sceneSubGraphMap[key.str()];
                if(sgraphParent == NULL || (!ui->actionDebugMode->isChecked() && name.find("yarplogger") != string::npos))
                {
                    continue;
                }
                sgraph = sgraphParent->addSubGraph(name.c_str());
                countChild++;
            }
            else
            {
                continue;
            }

            std::stringstream label;
            label << "   " << prop.find("name").asString().c_str()
                  << " (" << prop.find("pid").asInt() << ")   ";
            sgraph->setAttribute("shape", "box");
            sgraph->setAttribute("label", label.str().c_str());
            if(prop.check("color")) {
                sgraph->setAttribute("fillcolor", prop.find("color").asString().c_str());
                sgraph->setAttribute("color", prop.find("color").asString().c_str());
            }else {
                sgraph->setAttribute("fillcolor", "#a5cf80");
                sgraph->setAttribute("color", "#a5cf80");
            }
            sgraph->setAttribute("colorOfTheProcess", hexRandNum.c_str());
            //nodeSet[*itr] = node;
            dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(sgraph);
            sgraph->setVertex(*itr);
            std::stringstream keyProcess;
            keyProcess<<prop.find("hostname").asString()<<prop.find("pid").asInt();
            std::string endNodeName = keyProcess.str() + ".end";
            QGVNode * node = sgraph->addNode(endNodeName.c_str());
            node->setAttribute("shape", "circle");
            node->setAttribute("height", "0.0000000000001"); //a subgraph cannot be empty, adding fake hidden node
            node->setAttribute("fixedsize", "true");
            node->setAttribute("label", "");
            node->setAttribute("fillcolor", "#2e3e56");
            node->setAttribute("color", "#2e3e56");
            node->setAttribute("node_type", "end");
            node->setAttribute("rawname", endNodeName.c_str());
            sceneNodeMap[endNodeName] = node;
            sceneSubGraphMap[keyProcess.str()]= sgraph;
        }
    }

    // adding port nodes
    //pvertex_const_iterator itr;
    //const pvertex_set& vertices = graph.vertices();
    int portCounts = 0;

    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Property& prop = (*itr)->property;
        string portName = prop.find("name").asString();
        if(dynamic_cast<PortVertex*>(*itr)) {
            PortVertex* pv = dynamic_cast<PortVertex*>(*itr);
            ProcessVertex* v = (ProcessVertex*) pv->getOwner();
            if(ui->actionHideDisconnectedPorts->isChecked() && pv->property.find("orphan").asBool())
                continue;
            if(!ui->actionDebugMode->isChecked() && (portName.find("/log") != string::npos || portName.find("/yarplogger") != string::npos ))
                continue;
            std::stringstream key;
            if(v->property.find("hidden").asBool())
            {
                pv->property.put("hidden",true);
                updateNodeWidgetItems();
                continue;
            }
            else if(prop.find("hidden").asBool())
                continue;
            QGVNode *node;
            QString colorProcess;
            if(layoutSubgraph) {
                key<<v->property.find("hostname").asString()<<v->property.find("pid").asInt();
                QGVSubGraph *sgraph = sceneSubGraphMap[key.str()];
                if(sgraph)
                {
                    node =  sgraph->addNode(portName.c_str());
                    if(ui->actionColorMode->isChecked())
                    {
                        QColor color(sgraph->getAttribute("colorOfTheProcess"));
                        if(color.lightness()<100)
                            node->setAttribute("labelfontcolor","#ffffff");
                        colorProcess = sgraph->getAttribute("colorOfTheProcess");
                    }
                }
                else
                    node =  scene->addNode(portName.c_str());
            }
            else
                node =  scene->addNode(portName.c_str());
            node->setAttribute("shape", "ellipse");
            if(prop.check("color")) {
                node->setAttribute("fillcolor", prop.find("color").asString().c_str());
                node->setAttribute("color", prop.find("color").asString().c_str());
            }
            else if(!colorProcess.isEmpty())
            {
                node->setAttribute("fillcolor", colorProcess);
                node->setAttribute("color", colorProcess);
            }
            else {
                node->setAttribute("fillcolor", "#edad56");
                node->setAttribute("color", "#edad56");
            }
            //nodeSet[*itr] = node;
            dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(node);
            node->setVertex(*itr);
            portCounts++;
        }
    }

    // arrange the nodes deifferently if they are not port nodes
    if(portCounts == 0) {
        scene->setGraphAttribute("nodesep", "0.5");
        scene->setGraphAttribute("ranksep", "1.5");
    }

    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Vertex &v1 = (**itr);
        for(int i=0; i<v1.outEdges().size(); i++) {
            const Edge& edge = v1.outEdges()[i];
            const Vertex &v2 = edge.second();
            string targetName = v2.property.find("name").asString();
            if(!ui->actionDebugMode->isChecked() && targetName.find("/yarplogger") != string::npos)
                continue;
            //yInfo()<<"Drawing:"<<v1.property.find("name").asString()<<" -> "<<v2.property.find("name").asString();
            // add ownership edges
            if(!v1.property.find("hidden").asBool() && !v2.property.find("hidden").asBool()) {
                if(edge.property.find("type").asString() == "ownership" &&
                        edge.property.find("dir").asString() != "unknown") {
                    continue;
                }

                if(edge.property.find("type").asString() == "connection") {                    
                    //QGVEdge* gve = scene->addEdge(nodeSet[&v1], nodeSet[&v2],
                    //                               edge.property.find("carrier").asString().c_str());
                    string lable="";
                    if(!ui->actionHideConnectionsLable->isChecked())
                        lable = edge.property.find("carrier").asString();
                    QGVEdge* gve = scene->addEdge((QGVNode*)((YarpvizVertex*)&v1)->getGraphicItem(),
                                                  (QGVNode*)((YarpvizVertex*)&v2)->getGraphicItem(),
                                                   lable.c_str());
                    QosStyle::PacketPriorityLevel level=
                            (QosStyle::PacketPriorityLevel)edge.property.find("FromPacketPriority").asInt();
                    switch (level) {
                    case QosStyle::PacketPriorityNormal:
                        gve->setAttribute("color", "white");
                        break;
                    case QosStyle::PacketPriorityHigh:
                        gve->setAttribute("color", "orange");
                        break;
                    case QosStyle::PacketPriorityCritical:
                        gve->setAttribute("color", "red");
                        break;
                    case QosStyle::PacketPriorityLow:
                        gve->setAttribute("color", "yellow");
                        break;
                    default:
                        gve->setAttribute("color", "white");
                        break;
                    }
                    //gve->setToolTip("hello!");
                    gve->setEdge(&edge);
                }
            }
        }
    }

    //Layout scene
    scene->applyLayout();

    //Fit in view
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    //QGVSubGraph *ssgraph = sgraph->addSubGraph("SUB2");
    //ssgraph->setAttribute("label", "DESK");
    //scene->addEdge(snode1, ssgraph->addNode("PC0155"), "S10");
}

void MainWindow::edgeContextMenu(QGVEdge* edge) {
    const Edge* e  = (const Edge*)edge->getEdge();
    if(e == NULL)
        return;

    //yInfo()<<"edge clicked!";
    //Context menu exemple
    QMenu menu(edge->label());
    menu.addSeparator();
    menu.addAction(tr("Information..."));
    menu.addAction(tr("Configure Qos..."));
    //menu.addAction(tr("Hide"));
    QAction *action = menu.exec(QCursor::pos());
    if(action == 0)
        return;
    if(action->text().toStdString() == "Information...") {
        InformationDialog dialog;
        dialog.setEdgeInfo((Edge*)e);
        dialog.setModal(true);
        dialog.exec();
    }
    else if(action->text().toStdString() == "Configure Qos...") {
        QosConfigDialog dialog((Edge*)e);
        dialog.setModal(true);
        dialog.exec();
    }
}

void MainWindow::nodeContextMenu(QGVNode *node)
{
    YarpvizVertex* v = (YarpvizVertex*) node->getVertex();
    yAssert(v != 0);
    if(v->property.find("type").asString() == "port")
        onNodeContextMenuPort(node, v);
    else
        yWarning()<<"nodeContextMenu(): Unknown node!";
}

void MainWindow::onSubGraphContextMenuProcess(QGVSubGraph *sgraph) {
    YarpvizVertex* vertex;
    vertex = (YarpvizVertex*) sgraph->getVertex();

    if(!vertex || vertex->property.find("type").asString() != "process")
        return;


    QMenu menu(sgraph->getAttribute("label"));
    menu.addSeparator();
    menu.addAction(tr("Information..."));
    menu.addAction(tr("Hide"));
    QAction *action = menu.exec(QCursor::pos());
    if(action == 0)
        return;
    if(action->text().toStdString() == "Information...") {
        InformationDialog dialog;
        dialog.setProcessVertexInfo((ProcessVertex*)vertex);
        dialog.setModal(true);
        dialog.exec();
    }
    else if(action->text().toStdString() == "Hide") {
        vertex->property.put("hidden", true);
        updateNodeWidgetItems();
        drawGraph(*currentGraph);
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "yarpviz (version 2.0.0)",
                       "A graphical tool for a graphical tool for profiling and visualizing Yarp network!\n\nAuthors:\n\t-Ali Paikan <ali.paikan@iit.it>\n\t-Nicolò Genesio <nicolo.genesio@iit.it>");
}

void MainWindow::onNodeContextMenuPort(QGVNode *node, YarpvizVertex* vertex) {
    //Context menu exemple
    QMenu menu(node->label());
    menu.addSeparator();
    menu.addAction(tr("Information..."));
    menu.addAction(tr("Hide"));
    QAction *action = menu.exec(QCursor::pos());
    if(action == 0)
        return;
    if(action->text().toStdString() == "Information...") {
        InformationDialog dialog;
        dialog.setPortVertexInfo((PortVertex*)vertex);
        dialog.setModal(true);
        dialog.exec();
    }
    else if(action->text().toStdString() == "Hide") {
        vertex->property.put("hidden", true);
        updateNodeWidgetItems();
        drawGraph(*currentGraph);
    }
}


void MainWindow::nodeDoubleClick(QGVNode *node)
{
    QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(node->label()));
}

void MainWindow::onProfileYarpNetwork() {

    if(mainGraph.nodesCount()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Profiling: clear current project", "Running profiler will clear the current project.\n Are you sure?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }

    mainGraph.clear();
    QInputDialog* inputDialog = new QInputDialog(this);
    inputDialog->setOptions(QInputDialog::NoButtons);

    bool ok=false;

    float timeout =  inputDialog->getDouble(NULL ,"Profiling: yarp clean",
                                          "Do you want to run yarp clean before profiling?\n\n"
                                          "Be aware that yarp clean with a little timetout could\n"
                                          "unregister ports that are actually open.\n\n"
                                           "Timeout(seconds):", 0.3, 0, 2147483647, 1, &ok);
    if (ok)
    {
        messages.append("Cleaning death ports...");
        NetworkProfiler::yarpClean(timeout);
    }

    messages.append("Getting the ports list...");
    NetworkProfiler::ports_name_set ports;
    NetworkProfiler::yarpNameList(ports);


    messages.append("Getting the ports details...");
    NetworkProfiler::ports_detail_set portsInfo;
    progressDlg = new QProgressDialog("...", "Cancel", 0, 100, this);

    progressDlg->setLabelText("Getting the ports details...");
    progressDlg->reset();
    progressDlg->setRange(0, ports.size());
    progressDlg->setValue(0);
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->show();
    for(int i=0; i<ports.size(); i++) {
        NetworkProfiler::PortDetails info;
        std::string portname = ports[i].find("name").asString();
        std::string msg = string("Cheking ") + portname + "...";
        messages.append(QString(msg.c_str()));
        if(NetworkProfiler::getPortDetails(portname, info))
            portsInfo.push_back(info);
        progressDlg->setValue(i);
        if (progressDlg->wasCanceled())
            return;
    }
    //progressDlg->setValue(ports.size());
    stringModel.setStringList(messages);
    ui->messageView->update();

    NetworkProfiler::setProgressCallback(this);
    progressDlg->setLabelText("Generating the graph...");
    progressDlg->setRange(0, 100);
    progressDlg->setValue(0);
    NetworkProfiler::creatNetworkGraph(portsInfo, mainGraph);
    progressDlg->close();
    delete progressDlg;
    progressDlg = NULL;


    // update QoS
    NetworkProfiler::updateConnectionQosStatus(mainGraph);
    moduleParentItem->setExpanded(true);
    portParentItem->setExpanded(true);
    machinesParentItem->setExpanded(true);
    currentGraph = &mainGraph;

    // add process and port nodes to the tree
    populateTreeWidget();

    drawGraph(*currentGraph);
    ui->actionHighlight_Loops->setEnabled(true);
    ui->actionHidePorts->setEnabled(true);
    ui->actionHideDisconnectedPorts->setEnabled(true);
    ui->actionHideConnectionsLable->setEnabled(true);
    ui->actionDebugMode->setEnabled(true);
    ui->actionUpdateConnectionQosStatus->setEnabled(true);
    ui->actionProfilePortsRate->setEnabled(true);
}

void MainWindow::onHighlightLoops() {
    if(!currentGraph)
        return;

    if(ui->actionHighlight_Loops->isChecked()) {        
        graph_subset scc;
        Algorithm::calcSCC(*currentGraph, scc);

        for(int i=0; i<scc.size(); i++) {
            pvertex_set &vset = scc[i];
            QColor color(Random::uniform(128,255), Random::uniform(0,128), Random::uniform(128,255));
            for(int j=0; j<vset.size(); j++)
                vset[j]->property.put("color", color.name().toStdString().c_str());
        }
    }
    else {
        pvertex_const_iterator itr;
        const pvertex_set& vertices = currentGraph->vertices();
        for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
            (*itr)->property.unput("color");
        }
    }
    drawGraph(*currentGraph);
}


void MainWindow::updateNodeWidgetItems() {

    NodeWidgetItem* item= NULL;
    for (int i= moduleParentItem->childCount()-1; i>-1; i--) {
        item = (NodeWidgetItem*) moduleParentItem->child(i);
        yAssert(item != NULL);
        item->check(!item->getVertex()->property.find("hidden").asBool());
    }
    for (int i= portParentItem->childCount()-1; i>-1; i--) {
        item = (NodeWidgetItem*) portParentItem->child(i);
        yAssert(item != NULL);
        item->check(!item->getVertex()->property.find("hidden").asBool());
    }
    for (int i= machinesParentItem->childCount()-1; i>-1; i--) {
        item = (NodeWidgetItem*) machinesParentItem->child(i);
        yAssert(item != NULL);
        item->check(!item->getVertex()->property.find("hidden").asBool());
    }
}

void MainWindow::populateTreeWidget(){
    QTreeWidgetItem* item= NULL;
    for (int i= moduleParentItem->childCount()-1; i>-1; i--) {
        item = moduleParentItem->child(i);
        delete item;
    }
    for (int i= portParentItem->childCount()-1; i>-1; i--) {
        item = portParentItem->child(i);
        delete item;
    }
    for (int i= machinesParentItem->childCount()-1; i>-1; i--) {
        item = machinesParentItem->child(i);
        delete item;
    }
    pvertex_const_iterator itr;
    const pvertex_set& vertices = currentGraph->vertices();
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Property& prop = (*itr)->property;
        if(dynamic_cast<ProcessVertex*>(*itr)) {
            string processName = prop.find("name").asString();
            if(!ui->actionDebugMode->isChecked() && processName.find("yarplogger") != string::npos)
            {
                continue;
            }
            NodeWidgetItem *moduleItem =  new NodeWidgetItem(moduleParentItem, (*itr), MODULE);
            moduleItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
            moduleItem->check(true);
        }
        else if(dynamic_cast<PortVertex*>(*itr) && !ui->actionHidePorts->isChecked()) {
            string portName = prop.find("name").asString();
            if(ui->actionHideDisconnectedPorts->isChecked()){
                if(prop.check("orphan"))
                    continue;
            }
            if(!ui->actionDebugMode->isChecked() && (portName.find("/log") != string::npos || portName.find("/yarplogger") != string::npos ))
                continue;
            NodeWidgetItem *portItem =  new NodeWidgetItem(portParentItem, (*itr), PORT);
            portItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
            portItem->check(true);
        }
        else if(dynamic_cast<MachineVertex*>(*itr)) {
            NodeWidgetItem *machineItem =  new NodeWidgetItem(machinesParentItem, (*itr), MACHINE);
            machineItem->setFlags( /*Qt::ItemIsSelectable | */Qt::ItemIsEnabled /*| Qt::ItemIsUserCheckable */);
            machineItem->check(true);
        }
    }
    moduleParentItem->setExpanded(true);
    portParentItem->setExpanded(true);
    machinesParentItem->setExpanded(true);
}

void MainWindow::onLayoutOrthogonal() {
    ui->actionPolyline->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "ortho";
    if(currentGraph)
        drawGraph(*currentGraph);
}

void MainWindow::onLayoutPolyline() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "polyline";
    if(currentGraph)
        drawGraph(*currentGraph);
}

void MainWindow::onLayoutLine() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "line";
    if(currentGraph)
        drawGraph(*currentGraph);
}

void MainWindow::onLayoutCurved() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionLine->setChecked(false);
    layoutStyle = "curved";
    drawGraph(*currentGraph);
}


void MainWindow::onUpdateGraph() {
    if(currentGraph)
    {
        if(ui->actionHidePorts->isChecked()){
            NetworkProfiler::creatSimpleModuleGraph(mainGraph, simpleGraph);
            currentGraph = &simpleGraph;
        }
        else{
            currentGraph = &mainGraph;
        }
        populateTreeWidget();
        drawGraph(*currentGraph);
    }
}

void MainWindow::onNodesTreeItemClicked(QTreeWidgetItem *item, int column){
    if(item->type() != MODULE && item->type() != PORT )
        return;

    bool state = (item->checkState(column) == Qt::Checked);
    bool needUpdate = state != ((NodeWidgetItem*)(item))->checked();
    ((NodeWidgetItem*)(item))->check(state);
    if(needUpdate)
        drawGraph(*currentGraph);

    QList<QGraphicsItem *> items = scene->selectedItems();
    foreach( QGraphicsItem *item, items )
        item->setSelected(false);
    YarpvizVertex* yv = (YarpvizVertex*)((NodeWidgetItem*)(item))->getVertex();
    QGraphicsItem* graphicItem = (QGraphicsItem*) yv->getGraphicItem();
    if(graphicItem) {
        graphicItem->setSelected(true);
        if(state){
            ui->graphicsView->centerOn(graphicItem);
        }
    }
}

void MainWindow::onWindowMessageBox() {
    ui->messageView->setVisible(ui->actionMessageBox->isChecked());
}

void MainWindow::onWindowItem() {
    ui->nodesTreeWidget->setVisible(ui->actionItemswindow->isChecked());
}

void MainWindow::onUpdateQosStatus() {
    NetworkProfiler::updateConnectionQosStatus(*currentGraph);
    drawGraph(*currentGraph);
}

void MainWindow::onProfilePortsRate() {
    yAssert(currentGraph!=NULL);
    PortLoggerDialog dialog(currentGraph);
    dialog.setModal(false);
    dialog.exec();
}

void MainWindow::onConfigureConsQos() {
    BatchQosConfDialog dialog;
    dialog.setModal(false);
    dialog.exec();
}

void MainWindow::onExportConList() {
    QString filters("Text files (*.txt);;All files (*.*)");
    QString defaultFilter("Image file (*.txt)");
    QString filename = QFileDialog::getSaveFileName(0, "Export connections list",
                                                    QDir::homePath(),
                                                    filters, &defaultFilter);
    if(filename.size() == 0)
        return;

    ofstream file;
    file.open(filename.toStdString().c_str());
    if(!file.is_open()) {
        QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr("Cannot open the file for saving"));
        return;
    }

    // adding all process nodes and subgraphs
    pvertex_const_iterator itr;
    const pvertex_set& vertices = currentGraph->vertices();
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Vertex &v1 = (**itr);
        for(int i=0; i<v1.outEdges().size(); i++) {
            Edge& edge = (Edge&) v1.outEdges()[i];
            const Vertex &v2 = edge.second();
            if(!v1.property.find("hidden").asBool() && !v2.property.find("hidden").asBool()) {
                if(edge.property.find("type").asString() == "connection") {
                    Bottle bt;
                    bt.addString(v1.property.find("name").asString());
                    bt.addString(v2.property.find("name").asString());
                    bt.addString(edge.property.find("carrier").asString());
                    file<<bt.toString().c_str()<<endl;
                    //yInfo()<<v1.property.find("name").asString()<<"->"<<v2.property.find("name").asString();
                }
            }
        }
    }
    file.close();

}

void MainWindow::onExportScene() {
    QString filters("Image files (*.png);;All files (*.*)");
    QString defaultFilter("Image file (*.png)");
    QString filename = QFileDialog::getSaveFileName(0, "Export scene",
                                                    QDir::homePath(),
                                                    filters, &defaultFilter);
    if(filename.size() == 0)
        return;

    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
    image.fill(QColor("#2e3e56"));
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter);
    if(!image.save(filename))
        yError()<<"Cannot save scene to"<<filename.toStdString();

    /*
    QPrinter printer( QPrinter::HighResolution );
    //printer.setPageSize( QPrinter::A4 );
    printer.setOrientation( QPrinter::Landscape );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setOutputFileName( filename ); // file will be created in your build directory (where debug/release directories are)
    QPainter p;
    if(!p.begin(&printer)){
        yError() << "Error!";
        return;
    }
    this->scene->render( &p );
    p.end();
    */
}
