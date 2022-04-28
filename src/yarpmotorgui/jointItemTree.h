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
#include <QMenu>

#include "jointitem.h"

namespace Ui {
class jointItemTree;
}

class JointItemTree : public QWidget
{
    Q_OBJECT

public:
    explicit JointItemTree(int index, QWidget *parent = nullptr);
    virtual ~JointItemTree();

    void setJointName(const QString& name);

    QString jointName() const;

    void setJointMode(const JointItem::JointState& mode);

    JointItem::JointState jointMode() const;

    void mouseDoubleClickEvent(QMouseEvent *) override;

    QSize sizeHint() const override;

    void setDesiredSize(int w, int h);

signals:

    void sig_jointClicked(int index);

    void sig_homeClicked(int index);

    void sig_runClicked(int index);

    void sig_idleClicked(int index);

    void sig_PIDClicked(int index);

public slots:

    void onShowContextMenu(QPoint pos);

    void onHomeClicked();

    void onRunClicked();

    void onIdleClicked();

    void onPIDClicked();

private:

    void setColor(const QColor& color, const QColor& background);

    Ui::jointItemTree *m_ui;
    QMenu m_rightClickMenu;
    QAction* m_rightClickMenuTitle;
    QAction* m_homeAction;
    QAction* m_idleAction;
    QAction* m_runAction;
    QAction* m_pidAction;
    int m_desiredHeight{-1};
    int m_desiredWidth{-1};
    int m_index;
    bool m_modeSet{false};
    JointItem::JointState m_mode{JointItem::Idle};
};

#endif // JOINTITEMTREE_H
