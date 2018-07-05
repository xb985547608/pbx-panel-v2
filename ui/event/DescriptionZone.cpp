#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "DescriptionZone.h"
#include "ui/base/tablewidget.h"
#include "ui/base/misccomponents.h"
#include "time.h"

DescriptionZone::DescriptionZone(EventWidget* ew, QWidget *parent) :
    QWidget(parent),
    mEventWidget(ew)
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);

    QWidget* title = MiscComponents::getSectionLabel(tr("事件描述"), this);

    QLabel* lbTitleName = new QLabel(tr("事件标题:"), this);
    mTitleText = new QLineEdit(this);

    mLbDateTime = new QLabel(this);
    QLabel* lbRecorder = new QLabel(tr("记录人员："), this);
    mEditRecorder = new QLineEdit(this);

    mEditContent = new QPlainTextEdit(this);

    mBtnSave = new QPushButton(tr("保存"), this);
    connect(mBtnSave, SIGNAL(clicked()), this, SLOT(onBtnSaveClicked()));

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(title, 0, 0, 1, 5);

    gridLayout->addWidget(lbTitleName, 1, 0, 1, 2);
    gridLayout->addWidget(mTitleText, 1, 2, 1, 3);

    gridLayout->addWidget(mLbDateTime, 2, 0, 1, 2);
    gridLayout->addWidget(lbRecorder, 2, 2, 1, 1);
    gridLayout->addWidget(mEditRecorder, 2, 3, 1, 2);

    gridLayout->addWidget(mEditContent, 3, 0, 4, 5);

    gridLayout->addWidget(mBtnSave, 7, 2, 1, 1);

    rootBox->addLayout(gridLayout);

    startTimer(1000);
}

void DescriptionZone::onBtnSaveClicked()
{
    mEventWidget->saveEvent();
}

void DescriptionZone::timerEvent(QTimerEvent *)
{
    time_t timeStamp = time(NULL);
    QDateTime now = QDateTime::fromTime_t(timeStamp);
    mLbDateTime->setText(tr("日期时间:") + now.toString("yyyy-MM-dd hh:mm:ss"));
}
