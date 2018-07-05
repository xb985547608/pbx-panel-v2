#include "EventLog.h"
#include "ui/base/misccomponents.h"
#include "pbx/EventModel.h"
#include "message_queue/RPCCommand.h"
#include "misc/Config.h"
#include "misc/logger.h"

EventLog::EventLog(MainWindow* mw, QWidget* parent) :
    BaseWidget(tr("事件记录"), Config::Instance()->Res_icon_calendar, QColor(234,160,115),
               BaseWidget::eEventLog, BaseWidget::eMainWidget, mw, parent)
{
    initFrames();
}

void EventLog::initFrames()
{
    mHeaderBox->addWidget(getTopFrame(), 1);
    QHBoxLayout* hbox = new QHBoxLayout;
    mFrameRootBox->addLayout(hbox);
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(getLeftFrame(), 4);
    hbox->addWidget(getRightFrame(), 6);
}

void EventLog::onResume(QMap<QString, QVariant>& params)
{
    loadHistoryZone();
    BaseWidget::onResume(params);
}


QWidget* EventLog::getLeftFrame()
{
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    //历史记录列表窗口
    mHistoryZone = new MultiTableWidgets(9, 1, this);
    mHistoryZone->setHeader(MiscComponents::getSectionLabel(tr("历史记录"), mHistoryZone));
    vbox->addWidget(mHistoryZone);
    //查询窗口
    mQueryZone = new QueryZone(this);
    vbox->addWidget(mQueryZone);
    QWidget* leftFrameWidget = new QWidget(this);
    leftFrameWidget->setLayout(vbox);
    return leftFrameWidget;
}

EventDetailZone* EventLog::getRightFrame() {
    mEventDetailZone = new EventDetailZone(this);
    return mEventDetailZone;
}

void EventLog::loadHistoryZone(bool listAll)
{
    QString qTitle;
    QDateTime qDate;
    QString qDescription;
    if (!listAll) {
        qTitle = mQueryZone->getQueryTitle();
        qDate = mQueryZone->getQueryDate();
        qDescription = mQueryZone->getQueryDescription();
    }
    QList<EventModel::Event> events = RPCCommand::getEvent();
    mHistoryZone->clear(false, true);
    foreach (EventModel::Event event, events) {
        if (!listAll) {
            if (qTitle != "" && !event.title.contains(qTitle))
                continue;
            LOG(Logger::Debug, "qDate=%s\n", qDate.toString("yyyy-MM-dd").toStdString().c_str());
            LOG(Logger::Debug, "created_date=%s\n", event.created_date.toString("yyyy-MM-dd").toStdString().c_str());
            if (qDate.date() > event.created_date.date())
                continue;
            if (qDescription != "" && !event.description.contains(qDescription))
                continue;
        }
        QString text = QString("%1 %2").arg(event.created_date.toString("yyyy年MM月dd日")).arg(event.title);
        QPushButton* btnEvent = new QPushButton(text);
        connect(btnEvent, SIGNAL(clicked()), this, SLOT(onBtnEventClicked()));
        btnEvent->setStyleSheet("QPushButton {font: 12pt;}");
        EventModel::EventData* data = new EventModel::EventData(event);
        btnEvent->setUserData(0, data);
        mHistoryZone->appendWidget(btnEvent);
    }
    mHistoryZone->refresh();
}

void EventLog::onBtnEventClicked()
{
    QPushButton* sender = qobject_cast<QPushButton *>(QObject::sender());
    EventModel::EventData* data = (EventModel::EventData*)sender->userData(0);
    EventModel::Event event = data->getData();
    mEventDetailZone->setDetail(event);
}


QueryZone::QueryZone(EventLog *eventLogWidget, QWidget *parent) :
    QWidget(parent), mEventLogWidget(eventLogWidget)
{
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    setLayout(vbox);

    QVBoxLayout* rootbox = new QVBoxLayout;
    vbox->addWidget(MiscComponents::getGroupBox(rootbox, this));
    rootbox->setMargin(0);
    rootbox->setSpacing(0);
    rootbox->addWidget(MiscComponents::getSectionLabel(tr("查询条件"), this));

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    QLabel* label = new QLabel(tr("事件标题"), this);
    label->setObjectName("normalLabel");
    mLETitle = new QLineEdit(this);
    hbox->addWidget(label);
    hbox->addWidget(mLETitle);
    rootbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    label = new QLabel(tr("日期"), this);
    label->setObjectName("normalLabel");
    mDateTimeEditor = new QDateTimeEdit(this);
    mDateTimeEditor->setDateTime(QDateTime::currentDateTime());
    hbox->addWidget(label);
    hbox->addWidget(mDateTimeEditor);
    rootbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    label = new QLabel(tr("文字内容"), this);
    label->setObjectName("normalLabel");
    mLEText = new QLineEdit(this);
    mLEText->setStyleSheet("QLineEdit {font 12;}");
    hbox->addWidget(label);
    hbox->addWidget(mLEText);
    rootbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    QPushButton* btnQuery = new QPushButton(tr("查询"), this);
    //btnQuery->setObjectName("normalBtn");
    connect(btnQuery, SIGNAL(clicked()), this, SLOT(onBtnQueryClicked()));
    QPushButton* btnShowAll = new QPushButton(tr("所有记录"), this);
    //btnShowAll->setObjectName("normalBtn");
    connect(btnShowAll, SIGNAL(clicked()), this, SLOT(onBtnQueryAllClicked()));
    hbox->addWidget(btnQuery);
    hbox->addWidget(btnShowAll);
    rootbox->addLayout(hbox);
    rootbox->addStretch();
}

void QueryZone::onBtnQueryClicked()
{
    mEventLogWidget->loadHistoryZone(false);
}

void QueryZone::onBtnQueryAllClicked()
{
    mEventLogWidget->loadHistoryZone();
}

EventDetailZone::EventDetailZone(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    setLayout(vbox);
    QVBoxLayout* rootbox = new QVBoxLayout;
    rootbox->setMargin(0);
    rootbox->setSpacing(0);
    vbox->addWidget(MiscComponents::getGroupBox(rootbox, this));

    rootbox->addWidget(MiscComponents::getSectionLabel(tr("事件记录"), this));

    QGroupBox* groupbox;
    QHBoxLayout* hbox = new QHBoxLayout;
    groupbox = MiscComponents::getGroupBox(hbox, this);
    groupbox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    rootbox->addWidget(groupbox);
    hbox->addWidget(new QLabel(tr("事件标题:"), this), 2);
    mLETitle = new QLineEdit(this);
    hbox->addWidget(mLETitle, 8);

    QLabel* label;
    hbox = new QHBoxLayout;
    groupbox = MiscComponents::getGroupBox(hbox, this);
    groupbox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    rootbox->addWidget(groupbox);
    label = new QLabel(tr("日期时间:"), this);
//    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    hbox->addWidget(label, 2);
    mDateTimeEditor = new QDateTimeEdit(this);
//    mDateTimeEditor->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    hbox->addWidget(mDateTimeEditor, 3);
    label = new QLabel(tr("记录人员:"), this);
//    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    hbox->addWidget(label, 2);
    mLERecorder = new QLineEdit(this);
//    mLERecorder->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    hbox->addWidget(mLERecorder, 3);

    hbox = new QHBoxLayout;
    rootbox->addWidget(MiscComponents::getGroupBox(hbox, this));
    mDescription = new QPlainTextEdit(this);
    hbox->addWidget(mDescription);
    mDescription->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void EventDetailZone::setDetail(EventModel::Event &event)
{
    mLETitle->setText(event.title);
    mDateTimeEditor->setDateTime(event.created_date);
    mLERecorder->setText(event.recorder);
    mDescription->setPlainText(event.description);
}
