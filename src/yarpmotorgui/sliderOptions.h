/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
