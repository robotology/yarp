/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
    void onLicence();
};

#endif // ABOUTDLG_H
