#ifndef YSCOPEWINDOW_H
#define YSCOPEWINDOW_H

#include <QDialog>

namespace Ui {
class YscopeWindow;
}

class YscopeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit YscopeWindow(QWidget *parent = 0);
    ~YscopeWindow();
    int getIndex();

private:
    Ui::YscopeWindow *ui;
    int index;

private slots:
    void onInspect();
    void onCancel();
    void onIndexChanged(int);
};

#endif // YSCOPEWINDOW_H
