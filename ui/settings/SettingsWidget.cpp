#include "SettingsWidget.h"
#include "ui/MainWindow.h"
#include "ui/base/misccomponents.h"
#include "ui/base/BaseWidget.h"
#include "OpSettingsZone.h"
#include "PlanListZone.h"
#include "RecordingFileZone.h"
#include "TimingBroadcastZone.h"
#include "misc/Config.h"


SettingsWidget::SettingsWidget(MainWindow* mw, QWidget *parent) :
    BaseWidget(tr("系统设置"), Config::Instance()->Res_icon_settings, QColor(172,211,217),
               BaseWidget::eSettings, BaseWidget::eMainWidget, mw, parent)
{
    initFrames();
}

SettingsWidget::SettingsWidget(
        QString title,
        QString titleImage,
        QColor titleBgColor,
        Activity activityBack,
        MainWindow* mainWindow,
        QWidget *parent) :
    BaseWidget(title, titleImage, titleBgColor, BaseWidget::ePhoneBook, activityBack, mainWindow, parent)
{
    setMinimumSize(1280, 768);
    QVBoxLayout* rootBox = new QVBoxLayout;
    setLayout(rootBox);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(getTopFrame(), 1);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    rootBox->addLayout(hbox, 6);
    hbox->addWidget(getLeftFrame(), 1);
    hbox->addWidget(getRightFrame(), 3);
}

void SettingsWidget::initFrames()
{
    mHeaderBox->addWidget(BaseWidget::getTopFrame());
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    mFrameRootBox->addLayout(hbox);
    hbox->addWidget(getLeftFrame(), 1);
#ifndef EEPBX
    hbox->addWidget(getCenterFrame(), 1);
#endif
    hbox->addWidget(getRightFrame(), 3);
}

QWidget* SettingsWidget::getTopFrame()
{
    Config* cfg = Config::Instance();
    QWidget* topFrame = new QWidget(this);

    QVBoxLayout* rootBox = new QVBoxLayout;
    topFrame->setLayout(rootBox);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(MiscComponents::getHeader(this));
    QHBoxLayout* hbox = new QHBoxLayout;
    rootBox->addLayout(hbox);
    hbox->setMargin(0);
    hbox->setSpacing(0);

    //返回按钮
    QPushButton* btnBack = new QPushButton(this);
    MiscComponents::setPushButton(btnBack, Config::Instance()->Res_icon_back, QString(""), QSize(48, 48));
    connect(btnBack, SIGNAL(clicked()), this, SLOT(onBtnBackClicked()));

    QWidget* title = MiscComponents::gettitle(QColor(172,211,217), cfg->Res_icon_settings, tr("系统设置"), btnBack, this);
    hbox->addWidget(title);
    return topFrame;
}

QWidget* SettingsWidget::getLeftFrame()
{
    //Config* cfg = Config::Instance();
    QWidget* leftFrame = new QWidget(this);

    QVBoxLayout* rootBox = new QVBoxLayout;
    leftFrame->setLayout(rootBox);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);

    rootBox->addWidget(new OpSettingsZone(this, this), 1);
    mNormalPlansZone = new PlanListZone(EventModel::eNormal, this);
    mEmergencyPlansZone = new PlanListZone(EventModel::eEmergency, this);
    rootBox->addWidget(mNormalPlansZone, 2);
    rootBox->addWidget(mEmergencyPlansZone, 2);
    return leftFrame;
}

//创建定时广播显示区域
QWidget* SettingsWidget::getCenterFrame()
{
    QWidget* centerFrame = new QWidget(this);

    QVBoxLayout* rootBox = new QVBoxLayout;
    centerFrame->setLayout(rootBox);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);

    mTimedEventPlansZone = new TimingBroadcastZone(EventModel::eTimedEvent, this);
    rootBox->addWidget(mTimedEventPlansZone);
    return centerFrame;
}

QWidget* SettingsWidget::getRightFrame()
{
    //Config* cfg = Config::Instance();
    QWidget* rightFrame = new QWidget(this);

    QVBoxLayout* rootBox = new QVBoxLayout;
    rightFrame->setLayout(rootBox);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);

    RecordingFileZone* recordingFileZone = new RecordingFileZone(this);
    rootBox->addWidget(recordingFileZone);
    return rightFrame;
}

void SettingsWidget::onBtnBackClicked()
{
    QMap<QString, QVariant> params;
    mMainWindow->loadActivity(BaseWidget::eMainWidget, params, getActivityId());
}

MainWindow* SettingsWidget::getMainWindow()
{
    return mMainWindow;
}

void SettingsWidget::onResume(QMap<QString, QVariant> &)
{
    QMap<QString, QVariant> p;
    BaseWidget::onResume(p);
    mNormalPlansZone->loadPlans();
    mEmergencyPlansZone->loadPlans();
#ifndef EEPBX
    mTimedEventPlansZone->loadPlans();
#endif
}
