#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QtGui>
#include "dingobutton.h"

DingoButton::DingoButton(QWidget *parent) :
    QGroupBox(parent)
{
    mToggleable = true;
    mToggled = false;
    mStyleSheetToggled = "QGroupBox { border : 2px solid gray; border-radius: 6px; background-color: gray; }";
    mStyleSheetNormal = "QGroupBox { border : 2px solid gray; border-radius: 6px; }";

    QVBoxLayout* rootBox = new QVBoxLayout(this);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    QGroupBox::setLayout(rootBox);
    mContainer = new QWidget(this);
    rootBox->addWidget(mContainer);
}

void DingoButton::setBackgroundStyleSheet(ToggleState state, const QString &styleSheet)
{
    if (state == Toggled)
        mStyleSheetToggled = styleSheet;
    else
        mStyleSheetNormal = styleSheet;
    refreshBackground();
}

void DingoButton::setLayout(QLayout *layout)
{
    mContainer->setLayout(layout);
}

void DingoButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && mToggleable) {
        mToggled = !mToggled;
        refreshBackground();
        emit widgetClickedSignal();
        return;
    }
    QWidget::mousePressEvent(e);
}

void DingoButton::toggle(bool toggle)
{
    if (!mToggleable)
        return;
    mToggled = toggle;
    refreshBackground();
}

void DingoButton::refreshBackground()
{
    if (mToggled)
        setStyleSheet(mStyleSheetToggled);
    else
        setStyleSheet(mStyleSheetNormal);
}
