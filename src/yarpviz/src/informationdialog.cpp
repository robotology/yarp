#include "informationdialog.h"
#include "ui_informationdialog.h"

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
