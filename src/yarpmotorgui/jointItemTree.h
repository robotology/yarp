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

namespace Ui {
class jointItemTree;
}

class JointItemTree : public QWidget
{
    Q_OBJECT

public:
    explicit JointItemTree(int index, QWidget *parent = nullptr);
    ~JointItemTree();

    QLabel* jointLabel();

    QLabel* modeLabel();

    void setColor(const QColor& color, const QColor& background);

    void mouseDoubleClickEvent(QMouseEvent *event) override;

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
    Ui::jointItemTree *m_ui;
    QMenu m_rightClickMenu;
    QAction* m_rightClickMenuTitle;
    int m_desiredHeight{-1};
    int m_desiredWidth{-1};
    int m_index;
};

#endif // JOINTITEMTREE_H
