/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <fstream>
#include <QDir>
#include <QFileDialog>
#include <QProgressDialog>

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include "portloggerdialog.h"
#include "ui_portloggerdialog.h"
#include <yarp/profiler/NetworkProfiler.h>
#include <QMessageBox>

using namespace std;
using namespace yarp::os;
using namespace yarp::profiler;
using namespace yarp::profiler::graph;

PortLoggerDialog::PortLoggerDialog(Graph *graph, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortLoggerDialog), isStarted(false), timer(nullptr)
{
    yAssert(graph);
    ui->setupUi(this);


    connect(ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(addConnections()));
    connect(ui->pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeConnections()));
    connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(startStopLoggers()));
    connect(ui->toolButtonLogPath, SIGNAL(clicked()), this, SLOT(setLogPath()));
    connect(ui->pushButtonOpen, SIGNAL(clicked()), this, SLOT(openCons()));

    ui->pushButtonStart->setEnabled(false);
    ui->checkBoxCollect->setChecked(true);

    PortLoggerDialog::graph = graph;
    // adding all process nodes and subgraphs
    pvertex_const_iterator itr;
    const pvertex_set& vertices = graph->vertices();
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Vertex &v1 = (**itr);
        for(const auto& i : v1.outEdges()) {
            Edge& edge = (Edge&) i;
            const Vertex &v2 = edge.second();
            if(!v1.property.check("hidden") && !v2.property.check("hidden")) {
                if(edge.property.find("type").asString() == "connection") {
                    //yInfo()<<v1.property.find("name").asString()<<"->"<<v2.property.find("name").asString()<<label;
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
                    YARP_UNUSED(item);
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


void PortLoggerDialog::openCons()
{
    QString filters("Text files (*.txt);;All files (*.*)");
    QString defaultFilter("Connections list file (*.txt)");
    QString filename = QFileDialog::getOpenFileName(nullptr, "Load connections list",
                                                    QDir::homePath(),
                                                    filters, &defaultFilter);
    if (filename.size() == 0) {
        return;
    }

    fstream file;
    file.open(filename.toStdString().c_str());
    if (!file.is_open()) {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Cannot open the file for loading"));
        return;
    }

   ui->treeWidgetCons->clear();
    string line;
    unsigned int count = 0;
    while(getline(file, line)) {
        count++;
        Bottle sample(line);
        if(sample.size() == 3) {
            //data.addList() = sample;
            //yInfo()<<sample.toString();
            QTreeWidgetItem* item;
            QStringList prop;
            prop.clear();
            prop.append(sample.get(0).asString().c_str());
            prop.append(sample.get(1).asString().c_str());
            prop.append(sample.get(2).asString().c_str());
            item = new QTreeWidgetItem( ui->treeWidgetCons, prop);
            YARP_UNUSED(item);
        } else {
            yWarning() << "Wrong connection data at line" << count;
        }
    }
    file.close();
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
        YARP_UNUSED(newitem);
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
        YARP_UNUSED(newitem);
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
            //yarp::os::Property param;
            //param.put("log_raw", 1);
            //isStarted = isStarted && NetworkProfiler::setPortmonitorParams(item->text(1).toUtf8().constData(), param);
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
            messageBox.critical(nullptr,"Error","An error has occurred while starting the portrate plugin for some ports ! \n Please check if the LUA portmonitor carrier is enabled in YARP and portrate plugin can be found by the portmonitor.");
            messageBox.setFixedSize(500,200);
        }
    }
    else { // stop it
        QProgressDialog* progressDlg = new QProgressDialog("...", "Cancel", 0,
                                                           ui->treeWidgetSelectedCons->topLevelItemCount(), this);
        progressDlg->setLabelText("Collecting the results...");
        progressDlg->reset();
        progressDlg->setValue(0);
        progressDlg->setWindowModality(Qt::WindowModal);
        progressDlg->show();
        for( int i=0; i < ui->treeWidgetSelectedCons->topLevelItemCount(); ++i ){
            progressDlg->setValue(i);
            progressDlg->update();
            progressDlg->repaint();
            QTreeWidgetItem *item = ui->treeWidgetSelectedCons->topLevelItem(i);
            if(ui->checkBoxCollect->checkState() == Qt::Checked) { //collect the results
                yarp::os::Bottle param;
                std::string portname = item->text(1).toUtf8().constData();
                if(NetworkProfiler::getPortmonitorParams(portname, param)) {
                    Bottle* bt = param.get(0).asList();
                    if(!bt) {
                        yError()<<"Got wrong result format from portrate plugin at "<<portname;
                        continue;
                    }
                    //yInfo()<<bt->toString();
                    Bottle& data = bt->findGroup("data");
                    if(data.isNull() || data.size()<2) {
                        yError()<<"Got wrong result format from portrate plugin at "<<portname;
                        continue;
                    }
                    Bottle* samples = data.get(1).asList();
                    QString filename = portname.c_str();
                    filename.replace("/", "_");
                    filename = ui->lineEditLogPath->text() + "/port." +filename + ".log";
                    if (!saveLog(filename.toStdString(), samples)) {
                        yError() << "could not save the result into " << filename.toStdString();
                    }
                }
            }
            else {
                yarp::os::Property param;
                param.put("log_save", 1);
                NetworkProfiler::setPortmonitorParams(item->text(1).toUtf8().constData(), param);
            }

            NetworkProfiler::detachPortmonitorPlugin(item->text(1).toUtf8().constData());
        }
        delete progressDlg;
        if(timer) {
            timer->stop();
            delete timer;
            timer = nullptr;
        }
        isStarted = false;
        ui->pushButtonStart->setText("&Start");
    }
    ui->treeWidgetCons->setEnabled(!isStarted);
    ui->treeWidgetSelectedCons->setEnabled(!isStarted);
    ui->pushButtonClose->setEnabled(!isStarted);
    ui->pushButtonAdd->setEnabled(!isStarted);
    ui->pushButtonRemove->setEnabled(!isStarted);
    ui->checkBoxCollect->setEnabled(!isStarted);
}

void PortLoggerDialog::MyTimerSlot() {
    ui->lcdNumberLogTime->display(logTime.elapsed()/1000.0);
}

bool PortLoggerDialog::saveLog(std::string filename, yarp::os::Bottle* samples) {
    ofstream file;
    file.open(filename.c_str());
    if(!file.is_open()) {
        return false;
    }

    for(size_t k=0; k<samples->size(); k++) {
        Bottle* smp = samples->get(k).asList();
        file<<smp->toString().c_str()<<endl;
    }

    file.close();
    return true;
}

void PortLoggerDialog::setLogPath() {
    //QString filters("Log files (*.log);;Text files (*.txt);;All files (*.*)");
    //QString defaultFilter("Log file (*.log)");
    QString filename = QFileDialog::getExistingDirectory(nullptr, "Set the log files path",
                                                    QDir::homePath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (filename.size() == 0) {
        return;
    }
    ui->lineEditLogPath->setText(filename);
}
