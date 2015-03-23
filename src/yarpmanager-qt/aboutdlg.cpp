#include "aboutdlg.h"
#include "ui_aboutdlg.h"
#include "genericinfodlg.h"

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
            <a href=\""+ webSite + "\"><span style=\" text-decoration: underline; color:#0000ff;\">"+ webSite +"</span></a></p>";
    ui->website->setHtml(auxWebSite);

    connect(ui->btnCredits,SIGNAL(clicked()),this,SLOT(onCredits()));
    connect(ui->btnLicence,SIGNAL(clicked()),this,SLOT(onLicence()));
    connect(ui->btnClose,SIGNAL(clicked()),this,SLOT(accept()));
}

AboutDlg::~AboutDlg()
{
    delete ui;
}

void AboutDlg::onCredits()
{
    QString text = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> \
            p, li { white-space: pre-wrap; } \
            </style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\"> \
            <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8.25pt;\">Elena Ceseracciu &lt;</span><a href=\"mailto:vadim.tikhanoff@iit.it\"><span style=\" text-decoration: underline; color:#0000ff;\">vadim.tikhanoff@iit.it</span></a><span style=\" font-size:8.25pt;\">&gt;</span></p> \
            <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8.25pt;\">Ali Paikan &lt;</span><a href=\"mailto:elena.ceseracciu@iit.it\"><span style=\" text-decoration: underline; color:#0000ff;\">ali.paikan@iit.it</span></a><span style=\" font-size:8.25pt;\">&gt;</span></p> \
            <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8.25pt;\">Alessandro Roncone &lt;</span><a href=\"mailto:Alessandro.Roncone@iit.it\"><span style=\" text-decoration: underline; color:#0000ff;\">ali.paikan@iit.it</span></a><span style=\" font-size:8.25pt;\">&gt;</span></p> \
            <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8.25pt;\">Davide Perrone &lt;</span><a href=\"mailto:aitek4iit@aitek.it\"><span style=\" text-decoration: underline; color:#0000ff;\">aitek4iit@aitek.it</span></a><span style=\" font-size:8.25pt;\">&gt;</span></p> \</body></html>";
    GenericInfoDlg dlg("Credits","Written by:",text);
    dlg.exec();
}

void AboutDlg::onLicence()
{
    QString text = "Permission is granted to copy, distribute, and/or modify this program "
            "under the terms of the GNU General Public License,version 2 or any "
            "later version published by the Free Software Foundation."
            "<br><br>A copy of the license can be found at<br>"
            "<a href=\"http://www.robotcub.org/icub/license/gpl.txt\"><span style=\" text-decoration: underline; color:#0000ff;\">http://www.robotcub.org/icub/license/gpl.txt</span>";
    GenericInfoDlg dlg("Licence","",text);
    dlg.exec();
}
