/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "qosconfigdialog.h"
#include "ui_qosconfigdialog.h"
#include <qmessagebox.h>

QosConfigDialog::QosConfigDialog(yarp::profiler::graph::Edge *edge, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QosConfigDialog)
{
    QosConfigDialog::edge = edge;
    ui->setupUi(this);
    ui->treeWidgetProperty->setStyleSheet("QTreeWidget::item { border-right: 1px dotted grey;}");
    ui->treeWidgetProperty->setSelectionMode(QAbstractItemView::NoSelection);
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    createGui();
}


QosConfigDialog::~QosConfigDialog()
{
    delete ui;
}

void QosConfigDialog::createGui() {
    QTreeWidgetItemIterator it(ui->treeWidgetProperty);
    // source packet priority
    srcPacketProCombo = new QComboBox(this);
    srcPacketProCombo->addItems(QStringList() << "LOW" << "NORMAL" << "HIGH" << "CRITIC");
    yarp::os::QosStyle::PacketPriorityLevel level = yarp::os::QosStyle::PacketPriorityNormal;
    if(edge)
        level = (yarp::os::QosStyle::PacketPriorityLevel)edge->property.find("FromPacketPriority").asInt32();
    switch(level) {
    case yarp::os::QosStyle::PacketPriorityNormal :
        srcPacketProCombo->setCurrentIndex(1);
        break;
    case yarp::os::QosStyle::PacketPriorityLow :
        srcPacketProCombo->setCurrentIndex(0);
        break;
    case yarp::os::QosStyle::PacketPriorityHigh :
        srcPacketProCombo->setCurrentIndex(2);
        break;
    case yarp::os::QosStyle::PacketPriorityCritical :
        srcPacketProCombo->setCurrentIndex(3);
        break;
    default:
        srcPacketProCombo->setCurrentIndex(1);
    }
    ui->treeWidgetProperty->setItemWidget(*it++, 1, srcPacketProCombo);

    // source thread priority and policy
    srcThreadPro = new QLineEdit;
    srcThreadPro->setValidator( new QIntValidator(-100, 100, this) );
    if(edge)
        srcThreadPro->setText(QString::number(edge->property.find("FromThreadPriority").asInt32()));
    else
        srcThreadPro->setText(QString::number(0));
    ui->treeWidgetProperty->setItemWidget(*it++, 1, srcThreadPro);

    srcThreadPolicy = new QLineEdit;
    srcThreadPolicy->setValidator( new QIntValidator(-100, 100, this) );
    if(edge)
        srcThreadPolicy->setText(QString::number(edge->property.find("FromThreadPolicy").asInt32()));
    else
        srcThreadPolicy->setText(QString::number(0));
    ui->treeWidgetProperty->setItemWidget(*it++, 1, srcThreadPolicy);


    // destination packet priority
    dstPacketProCombo = new QComboBox(this);
    dstPacketProCombo->addItems(QStringList() << "LOW" << "NORMAL" << "HIGH" << "CRITIC");
    if(edge)
        level = (yarp::os::QosStyle::PacketPriorityLevel)edge->property.find("ToPacketPriority").asInt32();
    else
        level = yarp::os::QosStyle::PacketPriorityNormal;

    switch(level) {
    case yarp::os::QosStyle::PacketPriorityNormal :
        dstPacketProCombo->setCurrentIndex(1);
        break;
    case yarp::os::QosStyle::PacketPriorityLow :
        dstPacketProCombo->setCurrentIndex(0);
        break;
    case yarp::os::QosStyle::PacketPriorityHigh :
        dstPacketProCombo->setCurrentIndex(2);
        break;
    case yarp::os::QosStyle::PacketPriorityCritical :
        dstPacketProCombo->setCurrentIndex(3);
        break;
    default:
        dstPacketProCombo->setCurrentIndex(1);
    }
    ui->treeWidgetProperty->setItemWidget(*it++, 1, dstPacketProCombo);

    // destination thread priority and policy
    dstThreadPro = new QLineEdit;
    dstThreadPro->setValidator( new QIntValidator(-100, 100, this) );
    if(edge)
        dstThreadPro->setText(QString::number(edge->property.find("ToThreadPriority").asInt32()));
    else
        dstThreadPro->setText(QString::number(0));
    ui->treeWidgetProperty->setItemWidget(*it++, 1, dstThreadPro);

    dstThreadPolicy = new QLineEdit;
    dstThreadPolicy->setValidator( new QIntValidator(-100, 100, this) );
    if(edge)
        dstThreadPolicy->setText(QString::number(edge->property.find("ToThreadPolicy").asInt32()));
    else
        dstThreadPolicy->setText(QString::number(0));
    ui->treeWidgetProperty->setItemWidget(*it++, 1, dstThreadPolicy);
}

void QosConfigDialog::reset() {
    srcPacketProCombo->setCurrentIndex(1);
    srcThreadPro->setText("0");
    srcThreadPolicy->setText("0");
    dstPacketProCombo->setCurrentIndex(1);
    dstThreadPro->setText("0");
    dstThreadPolicy->setText("0");
}

bool QosConfigDialog::getUserQosStyles(yarp::os::QosStyle& srcStyle, yarp::os::QosStyle& dstStyle) {
    yarp::os::QosStyle::PacketPriorityLevel level;
    int itemLevel = srcPacketProCombo->currentIndex();
    switch(itemLevel) {
    case 0:
        level = yarp::os::QosStyle::PacketPriorityLow;
        break;
    case 1:
        level = yarp::os::QosStyle::PacketPriorityNormal;
        break;
    case 2:
        level = yarp::os::QosStyle::PacketPriorityHigh;
        break;
    case 3:
        level = yarp::os::QosStyle::PacketPriorityCritical;
        break;
    default:
        level = yarp::os::QosStyle::PacketPriorityInvalid;
    }
    srcStyle.setPacketPriorityByLevel(level);
    srcStyle.setThreadPriority(srcThreadPro->text().toInt());
    srcStyle.setThreadPolicy(srcThreadPolicy->text().toInt());

    itemLevel = dstPacketProCombo->currentIndex();
    switch(itemLevel) {
    case 0:
        level = yarp::os::QosStyle::PacketPriorityLow;
        break;
    case 1:
        level = yarp::os::QosStyle::PacketPriorityNormal;
        break;
    case 2:
        level = yarp::os::QosStyle::PacketPriorityHigh;
        break;
    case 3:
        level = yarp::os::QosStyle::PacketPriorityCritical;
        break;
    default:
        level = yarp::os::QosStyle::PacketPriorityInvalid;
    }
    dstStyle.setPacketPriorityByLevel(level);
    dstStyle.setThreadPriority(dstThreadPro->text().toInt());
    dstStyle.setThreadPolicy(dstThreadPolicy->text().toInt());
    return true;
}

void QosConfigDialog::apply() {
    if(!edge) {
        close();
        setResult(QDialog::Accepted);
        return;
    }
    yarp::os::QosStyle srcStyle, dstStyle;
    getUserQosStyles(srcStyle, dstStyle);

    std::string from  = edge->first().property.find("name").asString();
    std::string to  = edge->second().property.find("name").asString();

    if(yarp::os::Network::setConnectionQos(from, to, srcStyle, dstStyle)) {
        close();
        return;
    }

    // something went wrong
    QMessageBox messageBox;
    messageBox.critical(nullptr,"Error","An error has occurred while applying the QoS configuration! \n Please check the QoS parameters carefully.");
    messageBox.setFixedSize(500,200);
}
