/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "MainWindow.h"
#include "moc_MainWindow.cpp"
#include "ui_MainWindow.h"
#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include "QGVSubGraph.h"
#include <QMessageBox>
#include <yarp/os/Time.h>

#include <yarp/os/LogStream.h>
#include "NetworkProfiler.h"
#include "ggraph.h"
#include "informationdialog.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::graph;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), scene(NULL)
{
    ui->setupUi(this);
    stringModel.setStringList(messages);
    ui->messageView->setModel(&stringModel);
    ui->messageView->setVisible(false);

    // initialize the scene
    initScene();

    connect(ui->actionProfile_YARP_network, SIGNAL(triggered()),this,SLOT(onProfileYarpNetwork()));
    connect(ui->actionOrthogonal, SIGNAL(triggered()),this,SLOT(onLayoutOrthogonal()));
    connect(ui->actionCurved, SIGNAL(triggered()),this,SLOT(onLayoutCurved()));
    connect(ui->actionPolyline, SIGNAL(triggered()),this,SLOT(onLayoutPolyline()));
    connect(ui->actionLine, SIGNAL(triggered()),this,SLOT(onLayoutLine()));
    connect(ui->actionSubgraph, SIGNAL(triggered()),this,SLOT(onLayoutSubgraph()));
    connect(ui->nodesTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
            SLOT(onNodesTreeItemClicked(QTreeWidgetItem *, int)));
    connect(ui->actionMessageBox, SIGNAL(triggered()),this,SLOT(onWindowMessageBox()));
    connect(ui->actionItemswindow, SIGNAL(triggered()),this,SLOT(onWindowItem()));

    progressDlg = new QProgressDialog("...", "Cancel", 0, 100, this);

    layoutStyle = "ortho";
    ui->actionOrthogonal->setChecked(true);
    layoutSubgraph = true;
    ui->actionSubgraph->setChecked(true);
    ui->actionMessageBox->setChecked(false);
    ui->actionItemswindow->setChecked(true);

    ui->action_Save_project->setEnabled(false);
    ui->action_Load_project->setEnabled(false);
    ui->actionDocumentaion->setEnabled(false);
    moduleParentItem = new QTreeWidgetItem( ui->nodesTreeWidget,  QStringList("Modules"));
    portParentItem = new QTreeWidgetItem( ui->nodesTreeWidget,  QStringList("Ports"));
    moduleParentItem->setIcon(0, QIcon(":icons/resources/module.svg"));
    portParentItem->setIcon(0, QIcon(":icons/resources/port.svg"));

}

MainWindow::~MainWindow()
{
    delete progressDlg;
    delete ui;
}

void MainWindow::initScene() {
    if(scene) {
        scene->clear();
        delete scene;
    }
    scene = new QGVScene("yarpviz", this);
    ui->graphicsView->setBackgroundBrush(QBrush(QColor("#2e3e56"), Qt::SolidPattern));
    ui->graphicsView->setScene(scene);
    connect(scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
    connect(scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));

}

void MainWindow::onProgress(unsigned int percentage) {
    //yInfo()<<percentage<<"%";
    progressDlg->setValue(percentage);
}

void MainWindow::drawGraph(Graph &graph)
{
    if(graph.nodesCount() == 0)
        return;

    initScene();

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
    std::map<const string, QGVSubGraph*> subgraphSet;


    // adding all process nodes and subgraphs
    pvertex_const_iterator itr;
    const pvertex_set& vertices = graph.vertices();
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(NULL);
        const Property& prop = (*itr)->property;
        QGVNode *node;
        if(dynamic_cast<ProcessVertex*>(*itr) && !prop.check("hidden"))
        {
            if(layoutSubgraph) {
                QGVSubGraph *sgraph = scene->addSubGraph(prop.toString().c_str());
                //sgraph->setAttribute("label", prop.find("name").asString().c_str());
                sgraph->setAttribute("color", "#2e3e56"); // hidden!
                //sgraph->setAttribute("fillcolor", "#0180B5");
                subgraphSet[prop.toString()] = sgraph;
                node = sgraph->addNode(prop.find("name").asString().c_str());
            }else
                node = scene->addNode(prop.find("name").asString().c_str());

            std::stringstream label;
            label << "   " << prop.find("name").asString().c_str()
                  << " (" << prop.find("pid").asInt() << ")   ";
            node->setAttribute("shape", "box");
            node->setAttribute("label", label.str().c_str());
            node->setAttribute("fillcolor", "#a5cf80");
            node->setAttribute("color", "#a5cf80");
            node->setIcon(QImage(":/icons/resources/Gnome-System-Run-64.png"));
            //nodeSet[*itr] = node;
            dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(node);
            node->setVertex(*itr);
        }
    }

    // adding port nodes
    //pvertex_const_iterator itr;
    //const pvertex_set& vertices = graph.vertices();
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Property& prop = (*itr)->property;
        if(dynamic_cast<PortVertex*>(*itr) && !prop.check("hidden")) {
            if(!prop.check("orphan")) {
                QGVNode *node;
                if(layoutSubgraph) {
                    PortVertex* pv = dynamic_cast<PortVertex*>(*itr);
                    ProcessVertex* v = (ProcessVertex*) pv->getOwner();
                    QGVSubGraph *sgraph = subgraphSet[v->property.toString()];
                    if(sgraph)
                        node =  sgraph->addNode(prop.find("name").asString().c_str());                    
                    else
                        node =  scene->addNode(prop.find("name").asString().c_str());
                }
                else
                    node =  scene->addNode(prop.find("name").asString().c_str());
                node->setAttribute("shape", "ellipse");
                node->setAttribute("fillcolor", "#edad56");
                node->setAttribute("color", "#edad56");
                //nodeSet[*itr] = node;
                dynamic_cast<YarpvizVertex*>(*itr)->setGraphicItem(node);
                node->setVertex(*itr);
            }
        }
    }

    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Vertex &v1 = (**itr);
        for(int i=0; i<v1.outEdges().size(); i++) {
            const Edge& edge = v1.outEdges()[i];
            const Vertex &v2 = edge.second();
            // add ownership edges
            if(!v1.property.check("hidden") && !v2.property.check("hidden")) {
                if(edge.property.find("type").asString() == "ownership" &&
                        edge.property.find("dir").asString() != "unknown") {
                    //QGVEdge* gve = scene->addEdge(nodeSet[&v1], nodeSet[&v2], "");
                    QGVEdge* gve = scene->addEdge((QGVNode*)((YarpvizVertex*)&v1)->getGraphicItem(),
                                                  (QGVNode*)((YarpvizVertex*)&v2)->getGraphicItem(), "");
                    gve->setAttribute("color", "grey");
                    gve->setAttribute("style", "dashed");
                }

                if(edge.property.find("type").asString() == "connection") {
                    //QGVEdge* gve = scene->addEdge(nodeSet[&v1], nodeSet[&v2],
                    //                               edge.property.find("carrier").asString().c_str());
                    QGVEdge* gve = scene->addEdge((QGVNode*)((YarpvizVertex*)&v1)->getGraphicItem(),
                                                  (QGVNode*)((YarpvizVertex*)&v2)->getGraphicItem(),
                                                   edge.property.find("carrier").asString().c_str());
                    gve->setAttribute("color", "white");
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

void MainWindow::nodeContextMenu(QGVNode *node)
{
    YarpvizVertex* v = (YarpvizVertex*) node->getVertex();
    YARP_ASSERT(v != 0);
    if(v->property.find("type").asString() == "process")
        onNodeContextMenuProccess(node, v);
    else if(v->property.find("type").asString() == "port")
        onNodeContextMenuPort(node, v);
    else
        yWarning()<<"nodeContextMenu(): Unknown node!";
}

void MainWindow::onNodeContextMenuProccess(QGVNode *node, YarpvizVertex* vertex) {
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
        dialog.setProcessVertexInfo((ProcessVertex*)vertex);
        dialog.exec();
    }
    else if(action->text().toStdString() == "Hide") {
        vertex->property.put("hidden", true);
        updateNodeWidgetItems();
        drawGraph(mainGraph);
    }
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
        dialog.exec();
    }
    else if(action->text().toStdString() == "Hide") {
        vertex->property.put("hidden", true);
        updateNodeWidgetItems();
        drawGraph(mainGraph);
    }
}


void MainWindow::nodeDoubleClick(QGVNode *node)
{
    QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(node->label()));
}

void MainWindow::onProfileYarpNetwork() {

    mainGraph.clear();

    messages.append("Cleaning death ports...");
    NetworkProfiler::yarpClean(0.1);

    messages.append("Getting the ports list...");
    NetworkProfiler::ports_name_set ports;
    NetworkProfiler::yarpNameList(ports);


    messages.append("Getting the ports details...");
    NetworkProfiler::ports_detail_set portsInfo;
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

    // add process and port nodes to the tree
    QTreeWidgetItem* item= NULL;
    for (int i= moduleParentItem->childCount()-1; i>-1; i--) {
        item = moduleParentItem->child(i);
        delete item;
    }
    for (int i= portParentItem->childCount()-1; i>-1; i--) {
        item = portParentItem->child(i);
        delete item;
    }
    pvertex_const_iterator itr;
    const pvertex_set& vertices = mainGraph.vertices();
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Property& prop = (*itr)->property;
        if(dynamic_cast<ProcessVertex*>(*itr)) {
            NodeWidgetItem *moduleItem =  new NodeWidgetItem(moduleParentItem, (*itr), MODULE);
            moduleItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
            moduleItem->check(true);
        }
        else if(dynamic_cast<PortVertex*>(*itr) && !(*itr)->property.check("orphan")) {
            NodeWidgetItem *portItem =  new NodeWidgetItem(portParentItem, (*itr), PORT);
            portItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
            portItem->check(true);
            //moduleItem->setIcon(0, QIcon(":/Gnome-System-Run-64.png"));
        }
    }

    moduleParentItem->setExpanded(true);
    portParentItem->setExpanded(true);
    drawGraph(mainGraph);
}

void MainWindow::updateNodeWidgetItems() {

    NodeWidgetItem* item= NULL;
    for (int i= moduleParentItem->childCount()-1; i>-1; i--) {
        item = (NodeWidgetItem*) moduleParentItem->child(i);
        YARP_ASSERT(item != NULL);
        item->check(!item->getVertex()->property.check("hidden"));
    }
    for (int i= portParentItem->childCount()-1; i>-1; i--) {
        item = (NodeWidgetItem*) portParentItem->child(i);
        YARP_ASSERT(item != NULL);
        item->check(!item->getVertex()->property.check("hidden"));
    }
}

void MainWindow::onLayoutOrthogonal() {
    ui->actionPolyline->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "ortho";
    drawGraph(mainGraph);
}

void MainWindow::onLayoutPolyline() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "polyline";
    drawGraph(mainGraph);
}

void MainWindow::onLayoutLine() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "line";
    drawGraph(mainGraph);
}

void MainWindow::onLayoutCurved() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionLine->setChecked(false);
    layoutStyle = "curved";
    drawGraph(mainGraph);
}

void MainWindow::onLayoutSubgraph() {
    layoutSubgraph = ui->actionSubgraph->isChecked();
    drawGraph(mainGraph);
}

void MainWindow::onNodesTreeItemClicked(QTreeWidgetItem *item, int column){
    if(item->type() != MODULE && item->type() != PORT )
        return;

    bool state = (item->checkState(column) == Qt::Checked);
    bool needUpdate = state != ((NodeWidgetItem*)(item))->checked();
    ((NodeWidgetItem*)(item))->check(state);
    if(needUpdate)
        drawGraph(mainGraph);

    QList<QGraphicsItem *> items = scene->selectedItems();
    foreach( QGraphicsItem *item, items )
        item->setSelected(false);
    YarpvizVertex* yv = (YarpvizVertex*)((NodeWidgetItem*)(item))->getVertex();
    if(yv->getGraphicItem()) {
        ((QGVNode*)yv->getGraphicItem())->setSelected(true);
    }
}

void MainWindow::onWindowMessageBox() {
    ui->messageView->setVisible(ui->actionMessageBox->isChecked());
}

void MainWindow::onWindowItem() {
    ui->nodesTreeWidget->setVisible(ui->actionItemswindow->isChecked());
}
