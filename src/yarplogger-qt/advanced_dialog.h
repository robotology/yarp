/* 
 * Copyright (C)2014  iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
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
