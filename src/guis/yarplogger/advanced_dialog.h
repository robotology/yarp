/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef ADVANCED_DIALOG_H
#define ADVANCED_DIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <yarp/logger/YarpLogger.h>

namespace Ui {
class advanced_dialog;
}

class advanced_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit advanced_dialog(yarp::yarpLogger::LoggerEngine* logger, QWidget *parent = 0);
    ~advanced_dialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_radio_log_unlimited_clicked();

    void on_radio_log_limited_clicked();

    void on_radio_ports_unlimited_clicked();

    void on_radio_ports_limited_clicked();

private:
    Ui::advanced_dialog *ui;
    yarp::yarpLogger::LoggerEngine* theLogger;
};

#endif // ADVANCED_DIALOG_H
