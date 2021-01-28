/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
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

#ifndef SLIDEROPTIONS_H
#define SLIDEROPTIONS_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QModelIndex>
#include <QLineEdit>
#include <QDoubleValidator>

namespace Ui {
    class sliderOptions;
}

class sliderOptions : public QDialog
{
    Q_OBJECT

public:
    explicit sliderOptions(QWidget *parent = 0);
    ~sliderOptions();

private:
    Ui::sliderOptions *ui;
    double val_pos_custom_step;
    double val_vel_custom_step;
    double val_trq_custom_step;
    double val_vel_limit;
    int val_pos_choice;
    int val_vel_choice;
    int val_trq_choice;
    QDoubleValidator* pos_step_validator;
    QDoubleValidator* vel_step_validator;
    QDoubleValidator* trq_step_validator;
    QDoubleValidator* vel_lims_validator;


signals:
    void sig_setPosSliderOptionSO(int, double);
    void sig_setVelSliderOptionSO(int, double);
    void sig_setTrqSliderOptionSO(int, double);
};


#endif // SLIDEROPTIONS_H
