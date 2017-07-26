/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef CLUSTERWIDGET_H
#define CLUSTERWIDGET_H

#include <QWidget>

namespace Ui {
class ClusterWidget;
}

class ClusterWidget : public QWidget
{
    Q_OBJECT
private slots:
//    void onBrowseFile();
//    void onFileNameEditChanged(QString text);
public:
    explicit ClusterWidget(QWidget *parent = 0);
    ~ClusterWidget();

private:
    void addRow(std::string name="", std::string display="none", std::string user="",
                               bool onOff=false, bool log=true, bool select=true);
    std::string getSSHCmd(std::string user, std::string host, std::string ssh_options);
    Ui::ClusterWidget *ui;
};

#endif // CLUSTERWIDGET_H
