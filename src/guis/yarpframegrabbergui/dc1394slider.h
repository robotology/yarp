/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef DC1394SLIDER_H
#define DC1394SLIDER_H

#include <QWidget>
#include "dc1394SliderBase.h"

namespace Ui {
class DC1394Slider;
}

class DC1394Slider :  public DC1394SliderBase
{
    Q_OBJECT

public:

    explicit DC1394Slider(QWidget *parent = nullptr);
    ~DC1394Slider();
    bool init(yarp::dev::cameraFeature_id_t feature, char* label, DC1394Thread *controlThread);
    void Refresh();
    void Propagate();
    void updateSliders();
    void set_value(double val);
protected:
    void resizeEvent(QResizeEvent* event);


private:
    void connectWidgets();
    void disconnectWidgets();

protected:
    QString m_Name;
    double m_old_value,m_new_value;
    yarp::dev::cameraFeature_id_t m_Feature;

private:
    Ui::DC1394Slider *ui;
    DC1394Thread *controlThread;
    bool pressed;

private slots:
    void onSliderValueChanged(int value);
    void onOnePushClicked();
    void onRadioAuto(bool toggled);
    void onPower(bool checked);

    void onSliderPressed();
    void onSliderReleased();
    void onRefreshDone(QObject *slider,bool bON,bool bAuto,bool bHasAuto,bool bHasOnOff,bool bHasManual,bool bHasOnePush,double val);
    void onSliderSetFeatureDone(QObject *slider,double val);
    void onOnePushDone(QObject *slider, double val);
    void onRadioAutoDone(QObject *slider, bool bON, bool bAuto);
    void onPowerDone(QObject *slider, bool bON, bool hasAuto, bool hasManual, bool hasOnePush);
    void onHasFeatureDone(QObject *slider, bool hasFeature);
};

#endif // DC1394SLIDER_H
