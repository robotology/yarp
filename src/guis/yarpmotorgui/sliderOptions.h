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

    int    val_pos_digits;
    int    val_vel_digits;
    int    val_acc_digits;
    int    val_trq_digits;
    int    val_cur_digits;

    double val_pos_custom_step;
    double val_vel_custom_step;
    double val_acc_custom_step;
    double val_trq_custom_step;
    double val_cur_custom_step;

    double val_vel_limit;
    double val_acc_limit;

    int val_pos_choice;
    int val_vel_choice;
    int val_acc_choice;
    int val_trq_choice;
    int val_cur_choice;

    QIntValidator*    pos_digits_validator = nullptr;
    QIntValidator*    vel_digits_validator = nullptr;
    QIntValidator*    acc_digits_validator = nullptr;
    QIntValidator*    trq_digits_validator = nullptr;
    QIntValidator*    cur_digits_validator = nullptr;

    QDoubleValidator* pos_step_validator = nullptr;
    QDoubleValidator* vel_step_validator = nullptr;
    QDoubleValidator* acc_step_validator = nullptr;
    QDoubleValidator* trq_step_validator = nullptr;
    QDoubleValidator* cur_step_validator = nullptr;

    QDoubleValidator* vel_lims_validator = nullptr;
    QDoubleValidator* acc_lims_validator = nullptr;


signals:
    void sig_setPosSliderOptionSO(int, double, int);
    void sig_setVelSliderOptionSO(int, double, int);
    void sig_setAccSliderOptionSO(int, double, int);
    void sig_setTrqSliderOptionSO(int, double, int);
    void sig_setCurSliderOptionSO(int, double, int);
};


#endif // SLIDEROPTIONS_H
