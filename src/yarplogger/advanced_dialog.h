/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
