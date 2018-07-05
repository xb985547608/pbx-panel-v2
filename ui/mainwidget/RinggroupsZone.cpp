#include "RinggroupsZone.h"
#include "ExtensionsZone.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/Config.h"
#include "misc/logger.h"

RinggroupsZone::RinggroupsZone(MainWidget *mainWidget, QWidget *parent) :
    QWidget(parent), mMainWidget(mainWidget)
{
    Config* cfg = Config::Instance();
    mCurrentSelectedGrpNumber = cfg->AllExtensionGroupNumber;
    QVBoxLayout* rootBox = new QVBoxLayout(this);
    rootBox->setSpacing(0);
    rootBox->setMargin(0);

    MultiTableWidgets* holder = new MultiTableWidgets(12, 2, this);
    rootBox->addWidget(holder);

    QMap<QString, PBX::RingGroup> ringgroups = RPCCommand::getRinggroups();
    foreach (PBX::RingGroup ringgroup, ringgroups.values()) {
        RinggroupBtn* btn = new RinggroupBtn(ringgroup.description.left(cfg->RingGroupFixedDisplayLength), ringgroup.number);
        mRinggroupBtns.append(btn);
        connect(btn, SIGNAL(chkBoxSelectAllSignal(bool)), this, SLOT(onGroupBtnSelectAllChecked(bool)));
        connect(btn, SIGNAL(widgetClickedSignal()), this, SLOT(onGroupBtnClicked()));
        holder->appendWidget(btn);
    }
    //当前通话分组
    RinggroupBtn* onCallExtenGrp = new RinggroupBtn(cfg->CurrentCallGroupName, cfg->CurrentCallGroupNumber);
    mRinggroupBtns.append(onCallExtenGrp);
    connect(onCallExtenGrp, SIGNAL(chkBoxSelectAllSignal(bool)), this, SLOT(onGroupBtnSelectAllChecked(bool)));
    connect(onCallExtenGrp, SIGNAL(widgetClickedSignal()), this, SLOT(onGroupBtnClicked()));
    //holder->appendWidget(onCallExtenGrp);
    //onCallExtenGrp->setVisible(false);
    //当前在线分组
    RinggroupBtn* onlineExtenGrp = new RinggroupBtn(cfg->OnlineExtensionGroupName, cfg->OnlineExtensionGroupNumber);
    connect(onlineExtenGrp, SIGNAL(chkBoxSelectAllSignal(bool)), this, SLOT(onGroupBtnSelectAllChecked(bool)));
    connect(onlineExtenGrp, SIGNAL(widgetClickedSignal()), this, SLOT(onGroupBtnClicked()));
    mRinggroupBtns.append(onlineExtenGrp);
    //holder->appendWidget(onlineExtenGrp);
    //所有分机分组
    RinggroupBtn* allExtenGrp = new RinggroupBtn(cfg->AllExtensionGroupName, cfg->AllExtensionGroupNumber);
    allExtenGrp->toggle(true);
    allExtenGrp->enable();
    mRinggroupBtns.append(allExtenGrp);
    connect(allExtenGrp, SIGNAL(chkBoxSelectAllSignal(bool)), this, SLOT(onGroupBtnSelectAllChecked(bool)));
    connect(allExtenGrp, SIGNAL(widgetClickedSignal()), this, SLOT(onGroupBtnClicked()));
    //holder->appendWidget(allExtenGrp);
    //holder->setFooter(allExtenGrp);

    holder->setHeader(MiscComponents::getSectionLabel(tr("分组列表"), holder));
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setSpacing(0);
    hbox->setMargin(0);
    hbox->addWidget(onlineExtenGrp);
    hbox->addWidget(onCallExtenGrp);
    QVBoxLayout* footer = new QVBoxLayout;
    footer->setSpacing(0);
    footer->setMargin(0);
    footer->addLayout(hbox);
    footer->addWidget(allExtenGrp);
    QWidget* footWidget = new QWidget(holder);
    footWidget->setLayout(footer);
    holder->setFooter(footWidget);
    holder->refresh();
}

void RinggroupsZone::onGroupBtnSelectAllChecked(bool checked)
{
    //通知mainwidget，右侧窗口的分机全选/全不选
    LOG(Logger::Debug, "onGroupBtnSelectAllChecked, state = %d\n", (checked) ? 1 : 0);
    mMainWidget->getExtensionsZone()->toggleExten(checked);
}

void RinggroupsZone::onGroupBtnClicked()
{
    QObject* object = QObject::sender();
    RinggroupBtn* btn = qobject_cast<RinggroupBtn*>(object);
    toggleGroup(btn);
}

void RinggroupsZone::toggleGroup(QString groupNumber)
{
    foreach (RinggroupBtn* rgpBtn, mRinggroupBtns) {
        if (rgpBtn->getNumber() == groupNumber) {
            rgpBtn->toggle(true);
            toggleGroup(rgpBtn);
            return;
        }
    }
}

void RinggroupsZone::toggleGroup(RinggroupBtn* btn)
{
    //若为取消选中，则仅清除本btn的checkbox
    if (!btn->isToggled()) {
        btn->disable();
        mCurrentSelectedGrpNumber = Config::Instance()->AllExtensionGroupNumber;
        return;
    }
    btn->enable();
    //取消本zone内其他btn的toggled状态及checked状态
    foreach (RinggroupBtn* rgpBtn, mRinggroupBtns) {
        if (btn != rgpBtn) {
            if (rgpBtn->isToggled())
                rgpBtn->toggle(false);
            rgpBtn->disable();
        }
    }
    //通知MainWidget，右侧窗口分机列表切换
    LOG(Logger::Debug, "ringgroup: %s toggled\n", btn->getNumber().toStdString().c_str());
    mCurrentSelectedGrpNumber = btn->getNumber();
    mMainWidget->getExtensionsZone()->loadExtensFromRinggroup(btn->getNumber());
}

QString RinggroupsZone::getCurrentSelectedGrpName()
{
    foreach (RinggroupBtn* grpBtn, mRinggroupBtns) {
        if (grpBtn->getNumber() == mCurrentSelectedGrpNumber)
            return grpBtn->getName();
    }
    return "";
}

void RinggroupsZone::setGroupCheckStatus(QString groupNumber, bool status)
{
    foreach (RinggroupBtn* grpBtn, mRinggroupBtns) {
        if (grpBtn->getNumber() == groupNumber) {
            grpBtn->setCheckBoxState(status);
            break;
        }
    }
}

RinggroupBtn::RinggroupBtn(QString name, QString number, QWidget* parent) :
    DingoButton(parent), mNumber(number), mName(name)
{
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(2);
    rootBox->setSpacing(2);
    setLayout(rootBox);
    //setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    mChkBoxSelectAll = new QCheckBox(this);
    mChkBoxSelectAll->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    if (name.size() > 7)
        name = name.left(6) + "...";
    QLabel* label = new QLabel(name, this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { font: 75 12pt; }");
    rootBox->addWidget(mChkBoxSelectAll);
    rootBox->addWidget(label);
    connect(mChkBoxSelectAll, SIGNAL(toggled(bool)), SIGNAL(chkBoxSelectAllSignal(bool)));
    disable();
}

void RinggroupBtn::enable()
{
    mChkBoxSelectAll->setCheckable(true);
}

void RinggroupBtn::disable()
{
    mChkBoxSelectAll->setCheckState(Qt::Unchecked);
    mChkBoxSelectAll->setCheckable(false);
}

void RinggroupBtn::setCheckBoxState(bool checked)
{
    if (checked)
        mChkBoxSelectAll->setCheckState(Qt::Checked);
    else
        mChkBoxSelectAll->setCheckState(Qt::Unchecked);
}
