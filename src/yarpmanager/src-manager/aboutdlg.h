/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>

namespace Ui {
class AboutDlg;
}

class AboutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDlg(QString appName, QString version, QString copyright, QString website, QWidget *parent = 0);
    ~AboutDlg();

private:
    Ui::AboutDlg *ui;

private slots:
    void onCredits();
    void onLicence();
};

#endif // ABOUTDLG_H
