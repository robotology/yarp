/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGVScene.h>
#include <QProgressDialog>
#include <QStringList>
#include <QStringListModel>
#include <QTreeWidgetItem>

#include "NetworkProfiler.h"

enum NodeItemType { UNKNOWN = 0,
                    MODULE = 1,
                    PORT = 2 };


class NodeWidgetItem : public QTreeWidgetItem {
public:
    NodeWidgetItem(QTreeWidgetItem* parent, yarp::graph::Vertex* vertex, int type)
        : QTreeWidgetItem(parent, QStringList(vertex->property.find("name").asString().c_str()), type)
    {
        if(dynamic_cast<ProcessVertex*>(vertex)) {
            std::stringstream lable;
            lable << vertex->property.find("name").asString().c_str()
                  << " (" << vertex->property.find("pid").asInt() << ")";
            setText(0, lable.str().c_str());
        }
        checkFlag = false;
        NodeWidgetItem::vertex = vertex;
    }

    void check(bool flag) {
        checkFlag = flag;
        setCheckState( 0, (flag == true) ? Qt::Checked : Qt::Unchecked);
        if(!checkFlag)
            vertex->property.put("hidden", true);
        else
            vertex->property.put("hidden",false);
    }

    bool checked() { return checkFlag; }
    yarp::graph::Vertex* getVertex() { return vertex; }

public:
    bool checkFlag;
    yarp::graph::Vertex* vertex;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public NetworkProfiler::ProgressCallback
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void drawGraph(yarp::graph::Graph &graph);

public:
    virtual void onProgress(unsigned int percentage);

private:
    void initScene();
    void onNodeContextMenuPort(QGVNode *node, YarpvizVertex* vertex);
    void updateNodeWidgetItems();

private slots:
    void nodeContextMenu(QGVNode* node);
    void edgeContextMenu(QGVEdge* edge);
    void nodeDoubleClick(QGVNode* node);
    void onProfileYarpNetwork();
    void onHighlightLoops();
    void onLayoutOrthogonal();
    void onLayoutPolyline();
    void onLayoutLine();
    void onLayoutCurved();
    void onLayoutSubgraph();
    void onHidePorts();
    void onHideConnectionsLable();
    void onNodesTreeItemClicked(QTreeWidgetItem *item, int column);
    void onWindowMessageBox();
    void onWindowItem();
    void onExportScene();
    void onExportConList();
    void onUpdateQosStatus();
    void onConfigureConsQos();
    void onProfilePortsRate();
    void onSubGraphContextMenuProccess(QGVSubGraph *node);

private:
    Ui::MainWindow *ui;
    QGVScene *scene;
    QProgressDialog* progressDlg;
    QStringList messages;
    QStringListModel stringModel;
    yarp::graph::Graph mainGraph;
    yarp::graph::Graph simpleGraph;
    yarp::graph::Graph* currentGraph;
    std::string layoutStyle;
    bool layoutSubgraph;
    QTreeWidgetItem *moduleParentItem;
    QTreeWidgetItem *portParentItem;
    QTreeWidgetItem *machinesParentItem;
    std::map<std::string, QGVSubGraph*> sceneSubGraphMap;
    std::map<std::string, QGVNode*> sceneNodeMap;
};

#endif // MAINWINDOW_H
