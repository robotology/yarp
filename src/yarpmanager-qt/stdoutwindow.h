#ifndef STDOUTWINDOW_H
#define STDOUTWINDOW_H

#include <QWidget>

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
    void closeEvent(QCloseEvent * event);

private:
    Ui::StdoutWindow *ui;
    int id;



signals:
    void closeStdOut(int);
};

#endif // STDOUTWINDOW_H
