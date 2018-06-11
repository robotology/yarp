/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef CUSTOMTREE_H
#define CUSTOMTREE_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <yarp/conf/compiler.h>

class CustomTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTree(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:

public slots:

};

#endif // CUSTOMTREE_H
