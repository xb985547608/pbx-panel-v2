#include "QueueZone.h"
#include "ui/base/misccomponents.h"
#include "ui/base/tablewidget.h"


QueueZone::QueueZone(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(MiscComponents::getSectionLabel(tr("排队信息"), this));
    TableWidget* widget = new TableWidget(1, this);
    rootBox->addWidget(widget);
    for (int i=0;i<15;i++) {
        QLabel* label = new QLabel(tr("queue"), widget);
        widget->appendItem(label);
    }

    QGroupBox* grpBox = MiscComponents::getGroupBox(rootBox, this);
    QVBoxLayout* box = new QVBoxLayout;
    box->setMargin(0);
    box->setSpacing(0);
    box->addWidget(grpBox);

    setLayout(box);
}
