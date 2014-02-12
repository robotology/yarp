#include "genericviewwidget.h"

GenericViewWidget::GenericViewWidget(QWidget *parent) :
    QWidget(parent)
{
}

NodeType GenericViewWidget::getType()
{
    return type;
}
