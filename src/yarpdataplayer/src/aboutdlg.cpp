/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "include/aboutdlg.h"
#include "ui_aboutdlg.h"
#include "include/genericinfodlg.h"

AboutDlg::AboutDlg(QString appName,QString version,QString copyright,QString webSite,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);
    setWindowTitle("About");

    ui->lblName->setText(appName);
    ui->lblVersion->setText(version);
    ui->copyright->setText(copyright);

    QString auxWebSite = "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> \
            <a href=\""+ webSite + R"("><span style=" text-decoration: underline; color:#0000ff;">)"+ webSite +"</span></a></p>";
    ui->website->setHtml(auxWebSite);

    connect(ui->btnLicence,SIGNAL(clicked()),this,SLOT(onLicence()));
    connect(ui->btnClose,SIGNAL(clicked()),this,SLOT(accept()));
}

AboutDlg::~AboutDlg()
{
    delete ui;
}

void AboutDlg::onLicence()
{
    QString text = "\
BSD-3-Clause<br />\
<pre>\
 Redistribution and use in source and binary forms, with or without\n\
 modification, are permitted provided that the following conditions are met:\n\
\n\
     1. Redistributions of source code must retain the above copyright notice,\n\
        this list of conditions and the following disclaimer.\n\
     2. Redistributions in binary form must reproduce the above copyright\n\
        notice, this list of conditions and the following disclaimer in the\n\
        documentation and/or other materials provided with the distribution.\n\
     3. Neither the name of the copyright holder nor the names of its\n\
        contributors may be used to endorse or promote products derived from\n\
        this software without specific prior written permission.\n\
\n\
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\n\
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n\
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n\
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n\
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n\
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n\
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n\
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n\
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n\
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\
</pre>\
<br />\
<br />\
LGPL-2.1+<br />\
<pre>\
 This library is free software; you can redistribute it and/or modify it under\n\
 the terms of the GNU Lesser General Public License as published by the Free\n\
 Software Foundation; either version 2.1 of the License, or (at your option) any\n\
 later version.\n\
\n\
 This library is distributed in the hope that it will be useful, but WITHOUT ANY\n\
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
 PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more\n\
 details.\n\
\n\
 You should have received a copy of the GNU Lesser General Public License along\n\
 with this library; if not, write to the Free Software Foundation, Inc., 51\n\
 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA\n\
</pre>\
";
    GenericInfoDlg dlg("Licence","Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)",text);
    dlg.exec();
}
