#include "RinggroupsZone.h"
#include "ExtensionsZone.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/Config.h"
#include "misc/logger.h"

RinggroupsZone::RinggroupsZone(MainWindow *mainWindow, QWidget *parent) :
    QWidget(parent), mMainWindow(mainWindow)
{
    Config* cfg = Config::Instance();
    QVBoxLayout* rootBox = new QVBoxLayout(this);
    rootBox->setSpacing(0);
    rootBox->setMargin(0);

    MultiTableWidgets* holder = new MultiTableWidgets(12, 2, this);
    rootBox->addWidget(holder);

    QMap<QString, PBX::RingGroup> ringgroups = RPCCommand::getRinggroups();
    foreach (PBX::RingGroup ringgroup, ringgroups.values()) {
        RinggroupBtn* btn = new RinggroupBtn(ringgroup.description.left(cfg->RingGroupFixedDisplayLength), ringgroup.number, holder);
        mRinggroupBtns.append(btn);
        connect(btn, SIGNAL(chkBoxSelectAllSignal(bool)), this, SLOT(onGroupBtnSelectAllChecked(bool)));
        connect(btn, SIGNAL(widgetClickedSignal()), this, SLOT(onGroupBtnClicked()));
        holder->appendWidget(btn);
    }
    RinggroupBtn* allExtenGrp = new RinggroupBtn(cfg->AllExtensionGroupName, cfg->AllExtensionGroupNumber, holder);
    mRinggroupBtns.append(allExtenGrp);
    connect(allExtenGrp, SIGNAL(chkBoxSelectAllSignal(bool)), this, SLOT(onGroupBtnSelectAllChecked(bool)));
    connect(allExtenGrp, SIGNAL(widgetClickedSignal()), this, SLOT(onGroupBtnClicked()));
    holder->setHeader(MiscComponents::getSectionLabel(tr("分组列表"), this));
    holder->setFooter(allExtenGrp);
    holder->refresh();
}

void RinggroupsZone::onGroupBtnSelectAllChecked(bool checked)
{
    //通知mainwindow，右侧窗口的分机全选/全不选
    LOG(Logger::Debug, "onGroupBtnSelectAllChecked, state = %d\n", (checked) ? 1 : 0);
    mMainWindow->getExtensionsZone()->toggleExten(checked);
}

void RinggroupsZone::onGroupBtnClicked()
{
    QObject* object = QObject::sender();
    RinggroupBtn* btn = qobject_cast<RinggroupBtn*>(object);
    toggleGroup(btn);
}

void RinggroupsZone::toggleGroup(RinggroupBtn* btn)
{
    //若为取消选中，则仅清除本btn的checkbox
    if (!btn->isToggled()) {
        btn->disable();
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
    //通知mainwindow，右侧窗口分机列表切换
    LOG(Logger::Debug, "ringgroup: %s toggled\n", btn->getNumber().toStdString().c_str());
    mMainWindow->getExtensionsZone()->loadExtensFromRinggroup(btn->getNumber());
}

RinggroupBtn::RinggroupBtn(QString name, QString number, QWidget* parent) :
    DingoButton(parent), mNumber(number)
{
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);
    mChkBoxSelectAll = new QCheckBox(this);
    QLabel* label = new QLabel(name, this);
    rootBox->addWidget(mChkBoxSelectAll);
    rootBox->addWidget(label);
    connect(mChkBoxSelectAll, SIGNAL(toggled(bool)), SIGNAL(chkBoxSelectAllSignal(bool)));
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
