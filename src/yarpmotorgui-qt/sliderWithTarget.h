/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef SLIDER_WITH_TARGET_H
#define SLIDER_WITH_TARGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <qstyleoption>

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
    void paintEvent(QPaintEvent *ev) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event);
};


#endif // JOINTITEM_H
