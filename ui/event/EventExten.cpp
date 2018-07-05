#include "EventExten.h"

EventExten::EventExten(PBX::Extension e, QWidget *parent) :
    UiExtensionBase(e, parent)
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);

    QLabel* lbNumber = new QLabel(e.number, this);
    lbNumber->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    mIcon->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(mIcon);
    hbox->addWidget(lbNumber);
    rootBox->addLayout(hbox);

    QLabel* lbName = new QLabel(e.name, this);
    lbNumber->setAlignment(Qt::AlignCenter);
    rootBox->addWidget(lbName);

    setLayout(rootBox);
    setToggleable(false);

    connect(this, SIGNAL(onStateChangedSignal(int)), this, SLOT(onStateChangedSlot(int)));
}

void EventExten::onStateChanged(int state)
{
    emit onStateChangedSignal(state);
}

void EventExten::onStateChangedSlot(int state)
{
    mIcon->setPixmap(QPixmap(mStateIconsRes.value(state)));
}

void EventExten::clickedSlot()
{
}
