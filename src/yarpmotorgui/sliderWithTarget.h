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
    bool enableViewLabel;
    bool enableViewTarget;
    bool disableClickOutOfHandle;
    SliderWithTarget(QWidget * parent = 0, bool _hasTargetOption = true);
    ~SliderWithTarget() { if (sliderLabel) { delete sliderLabel; sliderLabel = 0; } }
    void updateSliderTarget(double val);
    void resetTarget();
    void setSliderStep(double val);
    double getSliderStep();
    void setValue(double val);
    void setIsDouble(bool b);
    bool getIsDouble();

protected:
    bool isDouble;
    QLabel* sliderLabel;
    double target;
    double width_at_target;
    double sliderStep;
    bool hasTargetOption;
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent * event) override;
};


#endif // JOINTITEM_H
