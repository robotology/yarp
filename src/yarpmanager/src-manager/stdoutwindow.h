/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef STDOUTWINDOW_H
#define STDOUTWINDOW_H

#include <QWidget>
#include <yarp/conf/compiler.h>

namespace Ui {
class StdoutWindow;
}

class StdoutWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StdoutWindow(int id, QString title, QWidget *parent = 0);
    ~StdoutWindow();

    int getId();
    void addMessage(QString text);

protected:
    void closeEvent(QCloseEvent * event) override;

private:
    Ui::StdoutWindow *ui;
    int id;



signals:
    void closeStdOut(int);
    void internalAddMessage(QString msg);

public slots:
    void onInternalAddMessage(QString msg);
};

#endif // STDOUTWINDOW_H
