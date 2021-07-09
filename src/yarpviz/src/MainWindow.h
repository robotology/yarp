/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGVScene.h>
#include <QProgressDialog>
#include <QStringList>
#include <QStringListModel>
#include <QTreeWidgetItem>

#include <yarp/profiler/NetworkProfiler.h>

enum NodeItemType { UNKNOWN = 0,
                    MODULE = 1,
                    PORT = 2,
                    MACHINE = 3};


class NodeWidgetItem : public QTreeWidgetItem {
public:
    NodeWidgetItem(QTreeWidgetItem* parent, yarp::profiler::graph::Vertex* vertex, int type)
        : QTreeWidgetItem(parent, QStringList(vertex->property.find("name").asString().c_str()), type)
    {
        if(dynamic_cast<yarp::profiler::graph::ProcessVertex*>(vertex)) {
            std::stringstream label;
            label << vertex->property.find("name").asString().c_str()
                  << " (" << vertex->property.find("pid").asInt32() << ")";
            setText(0, label.str().c_str());
        }
        else if(dynamic_cast<yarp::profiler::graph::MachineVertex*> (vertex))
        {
            std::stringstream label;
            label << vertex->property.find("hostname").asString().c_str()
                  << " (" << vertex->property.find("os").asString() << ")";
            setText(0, label.str().c_str());
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
    yarp::profiler::graph::Vertex* getVertex() { return vertex; }

public:
    bool checkFlag;
    yarp::profiler::graph::Vertex* vertex;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public  yarp::profiler::NetworkProfiler::ProgressCallback
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void drawGraph(yarp::profiler::graph::Graph &graph);

public:
    void onProgress(unsigned int percentage) override;

private:
    void initScene();
    void onNodeContextMenuPort(QGVNode *node,  yarp::profiler::graph::GraphicVertex* vertex);
    void updateNodeWidgetItems();
    void populateTreeWidget();

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
    void onUpdateGraph();
    void onNodesTreeItemClicked(QTreeWidgetItem *item, int column);
    void onWindowMessageBox();
    void onWindowItem();
    void onExportScene();
    void onExportConList();
    void onUpdateQosStatus();
    void onConfigureConsQos();
    void onProfilePortsRate();
    void onSubGraphContextMenuProcess(QGVSubGraph *node);
    void onAbout();
    void onBackgroundGrid();

private:
    Ui::MainWindow *ui;
    QGVScene *scene;
    QProgressDialog* progressDlg;
    QStringList messages;
    QStringListModel stringModel;
    yarp::profiler::graph::Graph mainGraph;
    yarp::profiler::graph::Graph simpleGraph;
    yarp::profiler::graph::Graph* currentGraph;
    std::string layoutStyle;
    bool layoutSubgraph;
    QTreeWidgetItem *moduleParentItem;
    QTreeWidgetItem *portParentItem;
    QTreeWidgetItem *machinesParentItem;
    std::map<std::string, QGVSubGraph*> sceneSubGraphMap;
    std::map<std::string, QGVNode*> sceneNodeMap;
};

#endif // MAINWINDOW_H
