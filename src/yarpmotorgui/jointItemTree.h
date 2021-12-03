/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef JOINTITEMTREE_H
#define JOINTITEMTREE_H

#include <QWidget>
#include <QLabel>
#include <QColor>

namespace Ui {
class jointItemTree;
}

class JointItemTree : public QWidget
{
    Q_OBJECT

public:
    explicit JointItemTree(QWidget *parent = nullptr);
    ~JointItemTree();

    QLabel* jointLabel();

    QLabel* modeLabel();

    void setColor(const QColor& color, const QColor& background);

private:
    Ui::jointItemTree *m_ui;
};

#endif // JOINTITEMTREE_H
