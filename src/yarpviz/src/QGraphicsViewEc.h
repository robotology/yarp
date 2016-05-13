/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */
#ifndef QGRAPHICSVIEWEC_H
#define QGRAPHICSVIEWEC_H

#include <QGraphicsView>

/**
 * @brief Graphics view with wheel zoom
 *
 */
class QGraphicsViewEc : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsViewEc(QWidget *parent = 0);

protected:
    virtual void wheelEvent(QWheelEvent* event);
};

#endif // QGRAPHICSVIEWEC_H
