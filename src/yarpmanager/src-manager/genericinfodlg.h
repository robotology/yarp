/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef GENERICINFODLG_H
#define GENERICINFODLG_H

#include <QDialog>

namespace Ui {
class GenericInfoDlg;
}

class GenericInfoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GenericInfoDlg(QString title, QString description, QString text, QWidget *parent = 0);
    ~GenericInfoDlg();

private:
    Ui::GenericInfoDlg *ui;
};

#endif // GENERICINFODLG_H
