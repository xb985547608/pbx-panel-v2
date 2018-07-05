#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "SectionZone.h"
#include "misccomponents.h"
#include "tablewidget.h"

SectionZone::SectionZone(
        QString title,
        QWidget* content,
        QPushButton* btnOk,
        QPushButton* btnCancel,
        QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(10);
    hbox->addStretch();
    hbox->addWidget(btnOk);
    if (btnCancel != NULL)
        hbox->addWidget(btnCancel);
    hbox->addStretch();
    QWidget* footer = new QWidget(this);
    footer->setLayout(hbox);

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);

    TableWidget* tableWidget = new TableWidget(1, this);
    tableWidget->setHeader(MiscComponents::getSectionLabel(title, this));
    tableWidget->setFooter(footer);
    tableWidget->appendItem(content);

    rootBox->addWidget(tableWidget);
}
