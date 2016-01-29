/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef SLIDEROPTIONS_H
#define SLIDEROPTIONS_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QModelIndex>
#include <QLineEdit>

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
    int val_pos_choice;
    int val_vel_choice;
    int val_trq_choice;

signals:
    void sig_setPosSliderOptionSO(int, double);
    void sig_setVelSliderOptionSO(int, double);
    void sig_setTrqSliderOptionSO(int, double);
};


#endif // SLIDEROPTIONS_H
