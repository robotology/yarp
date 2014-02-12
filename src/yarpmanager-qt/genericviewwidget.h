#ifndef GENERICVIEWWIDGET_H
#define GENERICVIEWWIDGET_H

#include <QWidget>
#include "manager.h"
#include "safe_manager.h"

class GenericViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GenericViewWidget(QWidget *parent = 0);
    NodeType getType();

protected:
    NodeType type;
signals:

public slots:

};

#endif // GENERICVIEWWIDGET_H
