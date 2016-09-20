#include "informationdialog.h"
#include "ui_informationdialog.h"

#include <yarp/os/Network.h>

InformationDialog::InformationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InformationDialog)
{
    ui->setupUi(this);
    ui->treeWidgetProperty->setStyleSheet("QTreeWidget::item { border-right: 1px dotted grey;}");
    ui->treeWidgetProperty->setSelectionMode(QAbstractItemView::NoSelection);
}

InformationDialog::~InformationDialog()
{
    delete ui;
}

Ui::InformationDialog* InformationDialog::getUi() {
    return ui;
}

void InformationDialog::setProcessVertexInfo(ProcessVertex* vertex) {
    QTreeWidgetItem* item;
    QStringList prop;
    prop.clear();
    prop.append("Name");
    prop.append(vertex->property.find("name").asString().c_str());
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("Arguments");
    prop.append(vertex->property.find("arguments").asString().c_str());
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("Hostname");
    prop.append(vertex->property.find("hostname").asString().c_str());
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("Operating system");
    prop.append(vertex->property.find("os").asString().c_str());
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("PID");
    prop.append(QString::number(vertex->property.find("pid").asInt()));
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("Priority");
    prop.append(QString::number(vertex->property.find("priority").asInt()));
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("Policy");
    prop.append(QString::number(vertex->property.find("policy").asInt()));
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);
}


void InformationDialog::setPortVertexInfo(PortVertex* vertex) {
     QTreeWidgetItem* item;
     QStringList prop;
     prop.clear();
     prop.append("Name");
     prop.append(vertex->property.find("name").asString().c_str());
     item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

     prop.clear();
     prop.append("Type");
     prop.append(vertex->property.find("dir").asString().c_str());
     item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);
}

std::string packetPrioToString(yarp::os::QosStyle::PacketPriorityLevel level) {
    std::string name;
    switch(level) {
    case yarp::os::QosStyle::PacketPriorityNormal : {
        name = "NORMAL";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityLow : {
        name = "LOW";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityHigh : {
        name = "HIGH";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityCritical : {
        name = "CRITICAL";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityInvalid : {
        name = "INVALID";
        break;
    }
    default: {
        name = "UNDEFINED";
    }
    };
    return name;
}

void InformationDialog::setEdgeInfo(const yarp::graph::Edge* edge) {
    QTreeWidgetItem* item;
    std::string from  = edge->first().property.find("name").asString();
    std::string to  = edge->second().property.find("name").asString();

    QStringList prop;
    prop.clear();
    prop.append("Source");
    prop.append(from.c_str());
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("Destination");
    prop.append(to.c_str());
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    prop.clear();
    prop.append("Carrier");
    prop.append(edge->property.find("carrier").asString().c_str());
    item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

    yarp::os::QosStyle fromStyle, toStyle;
    if(yarp::os::NetworkBase::getConnectionQos(from, to, fromStyle, toStyle)) {
        // source
        prop.clear();
        prop.append("Source thread priority");
        prop.append(QString::number(fromStyle.getThreadPriority()));
        item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

        prop.clear();
        prop.append("Source thread policy");
        prop.append(QString::number(fromStyle.getThreadPolicy()));
        item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

        yarp::os::QosStyle::PacketPriorityLevel level = fromStyle.getPacketPriorityAsLevel();
        prop.clear();
        prop.append("Source packet priority");
        prop.append(packetPrioToString(level).c_str());
        item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

        // destination
        prop.clear();
        prop.append("Destination thread priority");
        prop.append(QString::number(toStyle.getThreadPriority()));
        item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

        prop.clear();
        prop.append("Destination thread policy");
        prop.append(QString::number(toStyle.getThreadPolicy()));
        item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);

        level = toStyle.getPacketPriorityAsLevel();
        prop.clear();
        prop.append("Destination packet priority");
        prop.append(packetPrioToString(level).c_str());
        item = new QTreeWidgetItem( ui->treeWidgetProperty, prop);
    }
}
