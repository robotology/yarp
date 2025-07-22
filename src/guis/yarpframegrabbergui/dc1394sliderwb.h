/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef DC1394SLIDERWB_H
#define DC1394SLIDERWB_H

#include <QWidget>
#include "dc1394SliderBase.h"

namespace Ui {
class DC1394SliderWB;
}

class DC1394SliderWB :  public DC1394SliderBase
{
    Q_OBJECT

public:
    explicit DC1394SliderWB(QWidget *parent = 0);
    ~DC1394SliderWB();
    bool init(DC1394Thread *controlThread);
    void Refresh();
    void Propagate();
    void updateSliders();
    void set_value(double blue,double red);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::DC1394SliderWB *ui;
    double m_old_red,m_new_red,m_old_blu,m_new_blu;
    DC1394Thread *controlThread;


private:
    void connectWidgets();
    void disconnectWidgets();

private slots:
    void onSliderRedReleased();
    void onSliderBlueReleased();
    void onSliderRedValueChanged(int value);
    void onSliderBlueValueChanged(int value);
    void onOnePushClicked();
    void onRadioAuto(bool toggled);
    void onPower(bool checked);

    void onHasFeatureDone(QObject *slider, bool hasFeature);
    void onRefreshDone(QObject *slider,bool bON,bool bAuto,bool bHasOnOff,bool bHasAuto,bool bHasManual,bool bHasOnePush,double redVal, double blueVal);
    void onSliderWBSetFeatureDone(QObject*slider, double redVal, double blueVal);
    void onRadioAutoDone(QObject *slider,bool bON, bool bAuto);
    void onPowerDone(QObject *slider, bool bON,bool hasAuto, bool hasManual, bool hasOnePush);
    void onOnePushDone(QObject *slider, double redVal, double blueVal);

};

#endif // DC1394SLIDERWB_H
