#include "BaseWidget.h"
#include "misc/Config.h"
#include "ui/MainWindow.h"
#include "ui/base/misccomponents.h"

BaseWidget::BaseWidget( QString title,
                        QString titleImage,
                        QColor titleBgColor,
                        Activity activityId,
                        Activity activityBack,
                        MainWindow* mainWindow,
                        QWidget *parent) :
    QWidget(parent),
    mTitleLabel(new QLabel(title, this)),
    mTitleBGColor(titleBgColor),
    mTitleImage(titleImage),
    mActivityId(activityId),
    mActivityBackId(activityBack),
    mMainWindow(mainWindow),
    mHeaderWidget(NULL),
    mBtnBack(NULL)
{
    //setMinimumSize(1280, 768);
    QVBoxLayout* rootBox = new QVBoxLayout;
    setLayout(rootBox);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    mHeaderBox = new QVBoxLayout;
    mHeaderBox->setMargin(0);
    mHeaderBox->setSpacing(0);
    rootBox->addLayout(mHeaderBox, 1);

    mFrameRootBox = new QVBoxLayout;
    mFrameRootBox->setMargin(0);
    mFrameRootBox->setSpacing(0);
    rootBox->addLayout(mFrameRootBox, 6);
    startTimer(1000);
    //initFrames();
}

/*
void BaseWidget::initFrames()
{
    QWidget* widget = new QWidget(this);
    mFrameRootBox->addWidget(widget);
}
*/

QWidget *BaseWidget::getTopFrame()
{
    Config* cfg = Config::Instance();
    QWidget* topFrame = new QWidget(this);

    QVBoxLayout* rootBox = new QVBoxLayout;
    topFrame->setLayout(rootBox);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    mHeaderWidget = MiscComponents::getHeader(this);
    rootBox->addWidget(mHeaderWidget);
    QHBoxLayout* hbox = new QHBoxLayout;
    rootBox->addLayout(hbox);
    hbox->setMargin(0);
    hbox->setSpacing(0);

    //返回按钮
    if (mActivityBackId != eNone) {
        mBtnBack = new QPushButton;
        MiscComponents::setPushButton(mBtnBack, cfg->Res_icon_back, QString(""), QSize(48, 48));
        connect(mBtnBack, SIGNAL(clicked()), this, SLOT(onBtnBackClicked()));
    }
    hbox->addWidget(initHeader());
    return topFrame;
}

void BaseWidget::onBtnBackClicked()
{
    QMap<QString, QVariant> params;
    params.insert("back-from", getActivityId());
    if (mCallerActivityId == eNone)
        mMainWindow->loadActivity(mActivityBackId, params);
    else
        mMainWindow->loadActivity(mCallerActivityId, params);
}

void BaseWidget::setTitle(QString title)
{
    mTitleLabel->setText(title);
}

QWidget* BaseWidget::initHeader()
{
    QWidget* headWidget = new QWidget(this);
    QPalette palette;
    palette.setBrush(headWidget->backgroundRole(), QBrush(mTitleBGColor));
    headWidget->setAutoFillBackground(true);
    headWidget->setPalette(palette);
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    headWidget->setLayout(rootBox);

    if (mBtnBack != NULL)
        rootBox->addWidget(mBtnBack);

    rootBox->addStretch();

    /*
    QLabel* logo = new QLabel(headWidget);
    if (imgRes == "")
        logo->setPixmap(Config::Instance()->Res_icon_empty);
    else
        logo->setPixmap(QPixmap(imgRes));
    rootBox->addWidget(logo);
    */
    mTitleLabel->setObjectName("headerLabel");
    rootBox->addWidget(mTitleLabel);
    rootBox->addStretch();
    return headWidget;
}

void BaseWidget::onResume(QMap<QString, QVariant> &params)
{
    if (params.contains("caller-activity"))
        mCallerActivityId = params.value("caller-activity").toInt();
    else
        mCallerActivityId = eNone;
}

void BaseWidget::onPause(QMap<QString, QVariant> &)
{
    mCallerActivityId = eNone;
}

void BaseWidget::timerEvent(QTimerEvent *)
{
    if (mHeaderWidget == NULL)
        return;
    QDateTime now = QDateTime::currentDateTime();
//    MiscComponents::setTextToHeader((QLabel*)mHeaderWidget, now.toString("yyyy年MM月dd日 ddd HH:mm:ss"));
    MiscComponents::setTextToHeader((QLabel*)mHeaderWidget, now.toString("yyyy年MM月dd日 HH:mm:ss"));
}
