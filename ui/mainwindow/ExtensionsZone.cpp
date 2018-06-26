#include "ExtensionsZone.h"
#include "SelectedExtensZone.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "misc/Config.h"

static const long CALL_NOT_CONNECTED = -1;

ExtensionsZone::ExtensionsZone(MainWindow* mainWindow, QWidget *parent) :
    QWidget(parent), mMainwindow(mainWindow)
{


    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    rootBox->addWidget(MiscComponents::getGroupBox(vbox, this));

    mTextArea = new TextArea(this);
    mExtensPanel = new MultiTableWidgets(36, 6, this, mTextArea);
    vbox->addWidget(mExtensPanel);
    loadExtensFromRinggroup(Config::Instance()->AllExtensionGroupNumber);
    mTextArea->setExtensCount(mExtensions.size());
    mTextArea->setOnlineExtensCount(0);
    mTextArea->setConferenceRoomCount(0);
}

void ExtensionsZone::loadExtensFromCurrentCall()
{
}

void ExtensionsZone::loadExtensFromRinggroup(QString grpNumber)
{
    QList<PBX::Extension> extensToLoad;
    if (grpNumber != Config::Instance()->AllExtensionGroupNumber) {
        QMap<QString, PBX::RingGroup> ringgroups;
        ringgroups = RPCCommand::getRinggroups(grpNumber);
        if (!ringgroups.contains(grpNumber))
            return;
        PBX::RingGroup ringgroup = ringgroups.value(grpNumber);
        foreach (QString extNumber, ringgroup.extenNumbers) {
            QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed(extNumber);
            if (!extens.contains(extNumber)) {
                LOG(Logger::Debug, "%s not found\n", extNumber.toStdString().c_str());
                continue;
            }
            extensToLoad.append(extens.value(extNumber));
        }
    } else {
        QMap<QString, PBX::Extension> extens = RPCCommand::getExtensionDetailed();
        foreach(PBX::Extension e, extens.values()) {
            extensToLoad.append(e);
        }
    }
    loadExtensions(extensToLoad);
}

void ExtensionsZone::loadExtensions(QList<PBX::Extension>& extens)
{
    mExtensPanel->clear();
    foreach (UiExtension* uiExten, mExtensions)
        delete uiExten;
    mExtensions.clear();
    foreach (PBX::Extension pbxExten, extens) {
        UiExtension* uiExten = new UiExtension(mMainwindow, pbxExten);
        mExtensions.append(uiExten);
        mExtensPanel->appendWidget(uiExten);
    }
    mExtensPanel->refresh();
}

void ExtensionsZone::toggleExten(bool toggle, QString extNumber)
{
    if (extNumber == "") {
        foreach (UiExtension* exten, mExtensions) {
            exten->toggle(toggle);
            if (exten->isToggled())
                mMainwindow->getSelectedExtensZone()->addExten(exten->getPbxExten());
            else
                mMainwindow->getSelectedExtensZone()->removeExten(exten->getPbxExten());
        }
        return;
    }
    foreach (UiExtension* exten, mExtensions) {
        if (exten->getPbxExten().number == extNumber) {
            exten->toggle(toggle);
            return;
        }
    }
}

UiExtension::UiExtension(MainWindow* mw, PBX::Extension e, QWidget *parent) :
    UiExtensionBase(e, parent), mMainWindow(mw)
{
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    {
        QHBoxLayout* hbox = new QHBoxLayout;
        hbox->setMargin(0);
        hbox->setSpacing(0);
        QLabel* lbNumber = new QLabel(e.number, this);
        lbNumber->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        mIcon->setAlignment(Qt::AlignRight);
        hbox->addWidget(mIcon);
        hbox->addWidget(lbNumber);
        QLabel* lbName = new QLabel(e.name, this);
        lbName->setAlignment(Qt::AlignCenter);

        QVBoxLayout* infoBox = new QVBoxLayout;
        infoBox->addLayout(hbox);
        infoBox->addWidget(lbName);
        rootBox->addLayout(infoBox);
    }
    {
        mRollCallIcon = new QLabel(this);
        mRollCallIcon->setAlignment(Qt::AlignRight);
        mLabelPeer = new QLabel(this);
        mLabelPeer->setAlignment(Qt::AlignRight);
        mLabelDuration = new QLabel(this);
        mLabelDuration->setAlignment(Qt::AlignRight);
        mCallDuration = CALL_NOT_CONNECTED;
        QVBoxLayout* callBox = new QVBoxLayout;
        callBox->setMargin(0);
        callBox->setSpacing(0);
        callBox->addWidget(mRollCallIcon);
        callBox->addWidget(mLabelPeer);
        callBox->addWidget(mLabelDuration);
        rootBox->addLayout(callBox);
    }
    setLayout(rootBox);
    connect(this, SIGNAL(stateChangedSignal(int)), this, SLOT(stateChangedSlot(int)));
    connect(this, SIGNAL(widgetClickedSignal()), this, SLOT(clickedSlot()));
    startTimer(1000);
}

void UiExtension::onStateChanged(int state)
{
    emit stateChangedSignal(state);
}

void UiExtension::stateChangedSlot(int state) {
    mIcon->setPixmap(QPixmap(mStateIconsRes.value(state)));
    switch (state) {
    case PBX::eIncall:
        mCallDuration = 0;
        break;
    case PBX::eIdle:
        mCallDuration = CALL_NOT_CONNECTED;
        mLabelDuration->setText("");
        break;
    default:
        break;
    }
}

void UiExtension::clickedSlot()
{
    if (isToggled())
        mMainWindow->getSelectedExtensZone()->addExten(getPbxExten());
    else
        mMainWindow->getSelectedExtensZone()->removeExten(getPbxExten());
}

void UiExtension::timerEvent(QTimerEvent *)
{
    if (mCallDuration == CALL_NOT_CONNECTED)
        return;
    mCallDuration++;
    QString duration = QString("%1").arg(mCallDuration);
    mLabelDuration->setText(duration);
}

TextArea::TextArea(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    setLayout(rootBox);
    {
        QHBoxLayout* extCntLayout = new QHBoxLayout;
        extCntLayout->setMargin(0);
        extCntLayout->addWidget(new QLabel(tr("分机总数"), this), 0, Qt::AlignLeft);
        mExtensCount = new QLabel(this);
        extCntLayout->addWidget(mExtensCount, 0, Qt::AlignRight);
        DingoButton* btn = new DingoButton(this);
        btn->setLayout(extCntLayout);
        btn->setToggleable(false);
        rootBox->addWidget(btn);
    }
    {
        QHBoxLayout* onlineCntLayout = new QHBoxLayout;
        onlineCntLayout->setMargin(0);
        onlineCntLayout->addWidget(new QLabel(tr("在线分机数"), this), 0, Qt::AlignLeft);
        mOnlineExtensCount = new QLabel(this);
        onlineCntLayout->addWidget(mOnlineExtensCount, 0, Qt::AlignRight);
        DingoButton* btn = new DingoButton(this);
        btn->setLayout(onlineCntLayout);
        btn->setToggleable(false);
        rootBox->addWidget(btn);
    }
    {
        QHBoxLayout* roomCntLayout = new QHBoxLayout;
        roomCntLayout->setMargin(0);
        roomCntLayout->addWidget(new QLabel(tr("在线会议室数"), this), 0, Qt::AlignLeft);
        mConferenceRoomCount = new QLabel(this);
        roomCntLayout->addWidget(mConferenceRoomCount, 0, Qt::AlignRight);
        DingoButton* btn = new DingoButton(this);
        btn->setLayout(roomCntLayout);
        btn->setToggleable(false);
        rootBox->addWidget(btn);
    }
}

void TextArea::setExtensCount(int cnt)
{
    QString txtCnt = QString("%1").arg(cnt);
    mExtensCount->setText(txtCnt);
}

void TextArea::setOnlineExtensCount(int cnt)
{
    QString txtCnt = QString("%1").arg(cnt);
    mOnlineExtensCount->setText(txtCnt);
}

void TextArea::setConferenceRoomCount(int cnt)
{
    QString txtCnt = QString("%1").arg(cnt);
    mConferenceRoomCount->setText(txtCnt);
}
