#include "stdoutwindow.h"
#include "ui_stdoutwindow.h"

StdoutWindow::StdoutWindow(int id, QString title,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StdoutWindow)
{
    ui->setupUi(this);
    this->id = id;
    if(!title.isEmpty()){
        setWindowTitle(title);
    }else{
        setWindowTitle("STDOUT");
    }

}

StdoutWindow::~StdoutWindow()
{
    delete ui;
}

void StdoutWindow::addMessage(QString text)
{
    ui->plainTextEdit->appendPlainText(text);
}

int StdoutWindow::getId()
{
    return id;
}

void StdoutWindow::closeEvent(QCloseEvent * event)
{
    Q_UNUSED(event);
    closeStdOut(id);
}
