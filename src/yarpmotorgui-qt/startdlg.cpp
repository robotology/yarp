#include "startdlg.h"
#include "ui_startdlg.h"
#include "flowlayout.h"


StartDlg::StartDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDlg)
{
    ui->setupUi(this);

    setWindowTitle("Qt Robot Motor GUI V1.0 - Select Parts");
}

StartDlg::~StartDlg()
{
    delete ui;
}


void StartDlg::init(QString robotName,QStringList partsName,QList <int> ENA)
{

    ui->lineEdit->setText(robotName);
    FlowLayout *layout = new FlowLayout(ui->groupBox);

    for(int i=0;i<partsName.count();i++){
        QCheckBox *check = new QCheckBox(partsName.at(i),ui->groupBox);
        check->setChecked(ENA.at(i));
        check->setMinimumSize(QSize(100,check->height()));
        layout->addWidget(check);
        checkList.append(check);
    }

    ui->groupBox->setLayout(layout);
}


QList <int> StartDlg::getEnabledParts()
{
    QList <int> enabled;
    for(int i=0; i<checkList.count();i++){
        if(checkList.at(i)->isChecked()){
            enabled.append(1);
        }else{
            enabled.append(0);
        }
    }
    return enabled;
}

QString StartDlg::getRobotName()
{
    return ui->lineEdit->text();
}
