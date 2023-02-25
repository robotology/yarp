/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef SLIDER_WITH_TARGET_H
#define SLIDER_WITH_TARGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QStyleOptionSlider>
#include <yarp/conf/compiler.h>

class SliderWithTarget : public QSlider
{
public:
    bool enableViewLabel = true;
    bool enableViewTargetBox = true;
    bool enableViewTargetValue = false;
    int  number_of_decimals = 0;
    bool disableClickOutOfHandle;
    SliderWithTarget(QWidget * parent = 0);
    ~SliderWithTarget();
    void updateSliderTarget(double val);
    void resetTarget();
    void setSliderStep(double val);
    double getSliderStep();
    void setValue(double val);
    void setIsDouble(bool b);
    bool getIsDouble();

protected:
    bool isDouble;
    QLabel* sliderCurrentLabel = nullptr;
    QLabel* sliderTargetLabel = nullptr;
    double target;
    double width_at_target;
    double sliderStep = 1;
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent * event) override;
};


#endif // JOINTITEM_H
