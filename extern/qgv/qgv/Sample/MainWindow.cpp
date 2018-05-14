/***************************************************************
QGVCore Sample
Copyright (c) 2014, Bergont Nicolas, All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
***************************************************************/
#include "MainWindow.h"
#include "moc_MainWindow.cpp"
#include "ui_MainWindow.h"
#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include "QGVSubGraph.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _scene = new QGVScene("DEMO", this);
    ui->graphicsView->setScene(_scene);

    connect(_scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
    connect(_scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawGraph()
{
    /*
    _scene->loadLayout("digraph test{node [style=filled,fillcolor=white];N1 -> N2;N2 -> N3;N3 -> N4;N4 -> N1;}");
    connect(_scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
    connect(_scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
    ui->graphicsView->setScene(_scene);
    return;
    */

    //Configure scene attributes
    _scene->setGraphAttribute("label", "DEMO");

    _scene->setGraphAttribute("splines", "ortho");
    _scene->setGraphAttribute("rankdir", "LR");
    //_scene->setGraphAttribute("concentrate", "true"); //Error !
    _scene->setGraphAttribute("nodesep", "0.4");

    _scene->setNodeAttribute("shape", "box");
    _scene->setNodeAttribute("style", "filled");
    _scene->setNodeAttribute("fillcolor", "white");
    _scene->setNodeAttribute("height", "1.2");
    _scene->setEdgeAttribute("minlen", "3");
    //_scene->setEdgeAttribute("dir", "both");

    //Add some nodes
    QGVNode *node1 = _scene->addNode("BOX");
    node1->setIcon(QImage(":/icons/Gnome-System-Run-64.png"));
    QGVNode *node2 = _scene->addNode("SERVER0");
    node2->setIcon(QImage(":/icons/Gnome-Network-Transmit-64.png"));
    QGVNode *node3 = _scene->addNode("SERVER1");
    node3->setIcon(QImage(":/icons/Gnome-Network-Transmit-64.png"));
    QGVNode *node4 = _scene->addNode("USER");
    node4->setIcon(QImage(":/icons/Gnome-Stock-Person-64.png"));
    QGVNode *node5 = _scene->addNode("SWITCH");
    node5->setIcon(QImage(":/icons/Gnome-Network-Server-64.png"));

    //Add some edges
    _scene->addEdge(node1, node2, "TTL")->setAttribute("color", "red");
    _scene->addEdge(node1, node2, "SERIAL");
    _scene->addEdge(node1, node3, "RAZ")->setAttribute("color", "blue");
    _scene->addEdge(node2, node3, "SECU");

    _scene->addEdge(node2, node4, "STATUS")->setAttribute("color", "red");

    _scene->addEdge(node4, node3, "ACK")->setAttribute("color", "red");

    _scene->addEdge(node4, node2, "TBIT");
    _scene->addEdge(node4, node2, "ETH");
    _scene->addEdge(node4, node2, "RS232");

    _scene->addEdge(node4, node5, "ETH1");
    _scene->addEdge(node2, node5, "ETH2");

    QGVSubGraph *sgraph = _scene->addSubGraph("SUB1");
    sgraph->setAttribute("label", "OFFICE");

    QGVNode *snode1 = sgraph->addNode("PC0152");
    QGVNode *snode2 = sgraph->addNode("PC0153");

    _scene->addEdge(snode1, snode2, "RT7");

    _scene->addEdge(node3, snode1, "GB8");
    _scene->addEdge(node3, snode2, "TS9");


    QGVSubGraph *ssgraph = sgraph->addSubGraph("SUB2");
    ssgraph->setAttribute("label", "DESK");
    _scene->addEdge(snode1, ssgraph->addNode("PC0155"), "S10");

    //Layout scene
    _scene->applyLayout();

    //Fit in view
    ui->graphicsView->fitInView(_scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::nodeContextMenu(QGVNode *node)
{
    //Context menu exemple
    QMenu menu(node->label());

    menu.addSeparator();
    menu.addAction(tr("Informations"));
    menu.addAction(tr("Options"));

    QAction *action = menu.exec(QCursor::pos());
    if(action == 0)
        return;
}

void MainWindow::nodeDoubleClick(QGVNode *node)
{
    QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(node->label()));
}
