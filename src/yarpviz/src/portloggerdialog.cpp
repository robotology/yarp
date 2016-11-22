#include <yarp/os/LogStream.h>
#include "portloggerdialog.h"
#include "ui_portloggerdialog.h"
#include "NetworkProfiler.h"
#include <QMessageBox>
#include <QDir>

using namespace yarp::os;
using namespace yarp::graph;

PortLoggerDialog::PortLoggerDialog(yarp::graph::Graph *graph, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortLoggerDialog), isStarted(false), timer(NULL)
{
    yAssert(graph);
    ui->setupUi(this);


    connect(ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(addConnections()));
    connect(ui->pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeConnections()));
    connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(startStopLoggers()));

    ui->pushButtonStart->setEnabled(false);

    PortLoggerDialog::graph = graph;
    // adding all process nodes and subgraphs
    pvertex_const_iterator itr;
    const pvertex_set& vertices = graph->vertices();
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Vertex &v1 = (**itr);
        for(unsigned int i=0; i<v1.outEdges().size(); i++) {
            Edge& edge = (Edge&) v1.outEdges()[i];
            const Vertex &v2 = edge.second();
            if(!v1.property.check("hidden") && !v2.property.check("hidden")) {
                if(edge.property.find("type").asString() == "connection") {
                    //yInfo()<<v1.property.find("name").asString()<<"->"<<v2.property.find("name").asString()<<lable;
                    std::string source = v1.property.find("name").asString();
                    std::string destination = v2.property.find("name").asString();
                    std::string carrier = edge.property.find("carrier").asString();

                    QTreeWidgetItem* item;
                    QStringList prop;
                    prop.clear();
                    prop.append(source.c_str());
                    prop.append(destination.c_str());
                    prop.append(carrier.c_str());
                    item = new QTreeWidgetItem( ui->treeWidgetCons, prop);
                }
            }
        }
    }
    ui->pushButtonAdd->setEnabled(ui->treeWidgetCons->topLevelItemCount() > 0);
    ui->pushButtonRemove->setEnabled(ui->treeWidgetSelectedCons->topLevelItemCount() > 0);
    ui->lineEditLogPath->setText(QDir::homePath());
}

PortLoggerDialog::~PortLoggerDialog() {
    delete ui;
}

void PortLoggerDialog::addConnections() {
    QList<QTreeWidgetItem *> itemList;
    itemList = ui->treeWidgetCons->selectedItems();
    foreach(QTreeWidgetItem *item, itemList) {
        QTreeWidgetItem* newitem;
        QStringList prop;
        prop.clear();
        prop.append(item->text(0));
        prop.append(item->text(1));
        prop.append(item->text(2));
        newitem = new QTreeWidgetItem( ui->treeWidgetSelectedCons, prop);
        delete item;
    }
    ui->pushButtonStart->setEnabled(ui->treeWidgetSelectedCons->topLevelItemCount() > 0);
    ui->pushButtonAdd->setEnabled(ui->treeWidgetCons->topLevelItemCount() > 0);
    ui->pushButtonRemove->setEnabled(ui->treeWidgetSelectedCons->topLevelItemCount() > 0);
}

void PortLoggerDialog::removeConnections() {
    QList<QTreeWidgetItem *> itemList;
    itemList = ui->treeWidgetSelectedCons->selectedItems();
    foreach(QTreeWidgetItem *item, itemList) {
        QTreeWidgetItem* newitem;
        QStringList prop;
        prop.clear();
        prop.append(item->text(0));
        prop.append(item->text(1));
        prop.append(item->text(2));
        newitem = new QTreeWidgetItem( ui->treeWidgetCons, prop);
        delete item;
    }
    ui->pushButtonStart->setEnabled(ui->treeWidgetSelectedCons->topLevelItemCount() > 0);
    ui->pushButtonAdd->setEnabled(ui->treeWidgetCons->topLevelItemCount() > 0);
    ui->pushButtonRemove->setEnabled(ui->treeWidgetSelectedCons->topLevelItemCount() > 0);
}

void PortLoggerDialog::startStopLoggers() {
    if(!isStarted) {
        yarp::os::Property prop;
        prop.put("context", "yarpviz");
        prop.put("file", "portrate");

        isStarted = true;
        for( int i=0; i < ui->treeWidgetSelectedCons->topLevelItemCount(); ++i ){
            QTreeWidgetItem *item = ui->treeWidgetSelectedCons->topLevelItem(i);
            //yInfo()<<item->text(1).toUtf8().constData();
            isStarted = isStarted && NetworkProfiler::attachPortmonitorPlugin(item->text(1).toUtf8().constData(), prop);
        }

        if(isStarted) {
            timer = new QTimer(this);
            // setup signal and slot
            connect(timer, SIGNAL(timeout()), this, SLOT(MyTimerSlot()));
            logTime.start();
            logTime.restart();
            timer->start(1000);
            ui->pushButtonStart->setText("Sto&p");
        }
        else
        {
            // something went wrong
            QMessageBox messageBox;
            messageBox.critical(0,"Error","An error has occured while starting the portrate plugin for some ports ! \n Please check if the LUA portmonitor carrier is enabled in YARP and portrate plugin can be found by the portmonitor.");
            messageBox.setFixedSize(500,200);
        }
    }
    else { // stop it
        for( int i=0; i < ui->treeWidgetSelectedCons->topLevelItemCount(); ++i ){
            QTreeWidgetItem *item = ui->treeWidgetSelectedCons->topLevelItem(i);
            NetworkProfiler::detachPortmonitorPlugin(item->text(1).toUtf8().constData());
        }

        if(timer) {
            timer->stop();
            delete timer;
            timer = NULL;
        }
        isStarted = false;
        ui->pushButtonStart->setText("&Start");
    }
    ui->treeWidgetCons->setEnabled(!isStarted);
    ui->treeWidgetSelectedCons->setEnabled(!isStarted);
    ui->pushButtonClose->setEnabled(!isStarted);
    ui->pushButtonAdd->setEnabled(!isStarted);
    ui->pushButtonRemove->setEnabled(!isStarted);
}

void PortLoggerDialog::MyTimerSlot() {
    ui->lcdNumberLogTime->display(logTime.elapsed()/1000);
}
