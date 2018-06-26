#include "QueueZone.h"
#include "ui/base/misccomponents.h"
#include "ui/base/tablewidget.h"
#include "message_queue/MessageClient.h"
#include "message_queue/RPCCommand.h"

QString QueueZone::mOperatorQueueNumber = "";

static QueueZone* sQueueZone = NULL;
static void EventCallback(MessageClient::EventType e, QMap<QString, QString>& eventMessage) {
    if (sQueueZone != NULL)
        sQueueZone->eventMessageReceived(eventMessage);
}
static QString sCallbackUuid = "";

void QueueZone::eventMessageReceived(QMap<QString, QString>& msg)
{
    QString event = msg.value("Event");
    if (event != "JoinEvent" && event != "LeaveEvent")
        return;
    QString queue = msg.value("Queue");
    if (queue != mOperatorQueueNumber)
        return;
    QString callerid = "";
    QString calleridname = "";
    if (event == "JoinEvent") {
         callerid = msg.value("CallerIdNum");
         calleridname = msg.value("CallerIdName");
         if (calleridname == "")
             calleridname = callerid;
    }
    QString channel = msg.value("Channel");
    emit queueEventSignal(event, callerid, calleridname, channel);
}

void QueueZone::queueEventSlot(QString event, QString callerid, QString calleridname, QString channel)
{
    if (event == "JoinEvent") {
        mQueueListModel->insertRow(0);
        mQueueListModel->setData(mQueueListModel->index(0, 0), calleridname + "(" + callerid + ")");
        mQueueListModel->setData(mQueueListModel->index(0, 0), channel, Qt::UserRole);
    }
    if (event == "LeaveEvent") {
        int i = 0;
        for (;i<mQueueListModel->rowCount();i++) {
            QString chan = mQueueListModel->data(mQueueListModel->index(i, 0), Qt::UserRole).toString();
            if (chan == channel)
                break;
        }
        if (i < mQueueListModel->rowCount())
            mQueueListModel->removeRow(i);
    }
}

QueueZone::QueueZone(QWidget *parent) :
    QWidget(parent)
{
    sQueueZone = this;
    mOperatorQueueNumber = RPCCommand::getOperatorQueueNumber();

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(MiscComponents::getSectionLabel(tr("排队信息"), this));
    TableWidget* widget = new TableWidget(1, this);
    rootBox->addWidget(widget);

    QGroupBox* grpBox = MiscComponents::getGroupBox(rootBox, this);
    QVBoxLayout* box = new QVBoxLayout;
    box->setMargin(0);
    box->setSpacing(0);
    box->addWidget(grpBox);
    setLayout(box);

    //队列列表
    mQueueListModel = new QStandardItemModel(0, 1, this);
    QTreeView* queueList = new QTreeView(this);
    queueList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    queueList->setFocusPolicy(Qt::NoFocus);
    queueList->setRootIsDecorated(false);
    queueList->setAlternatingRowColors(true);
    queueList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    queueList->setHeaderHidden(true);
    queueList->setStyleSheet(
                "QTreeView::item {height:50px; show-decoration-selected:0; font: 75 10pt;}"
                "QTreeView::item::selected {color: green;}"
                );
    queueList->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical { border: 2px solid grey; width: 30px; }"
                );
    queueList->setModel(mQueueListModel);
    rootBox->addWidget(queueList);

    connect(this, SIGNAL(queueEventSignal(QString,QString,QString,QString)), this, SLOT(queueEventSlot(QString,QString,QString,QString)));
    sCallbackUuid = MessageClient::Instance()->getEventMonitor()->addCallback(EventCallback);
}

QueueZone::~QueueZone()
{
    MessageClient::Instance()->getEventMonitor()->removeCallback(sCallbackUuid);
}
