#include "SelectedExtensZone.h"
#include "ExtensionsZone.h"
#include "FastFuncZone.h"
#include "RinggroupsZone.h"
#include "ui/base/misccomponents.h"
#include "misc/logger.h"

SelectedExtensZone::SelectedExtensZone(MainWidget *mw, QWidget *parent) :
    QWidget(parent), mMainWidget(mw)
{
    QLabel* label = new QLabel(tr("已选分机"), this);
    label->setAlignment(Qt::AlignCenter);
    QPushButton* btnClear = new QPushButton(tr("清除"), this);
    btnClear->setStyleSheet("QPushButton {font: 12pt;}");
    connect(btnClear, SIGNAL(clicked()), this, SLOT(btnClearClicked()));
    QVBoxLayout* textAreaBox = new QVBoxLayout;
    textAreaBox->setMargin(0);
    textAreaBox->addWidget(label);
    textAreaBox->addWidget(btnClear);
    QWidget* textAreaWidget = new QWidget(this);
    textAreaWidget->setLayout(textAreaBox);
    mSelectedExtenHolder = new MultiTableWidgets(12, 6, this, textAreaWidget, MultiTableWidgets::CtrlBtnBesideWidget);
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(mSelectedExtenHolder);
    QGroupBox* groupbox = MiscComponents::getGroupBox(vbox, this);

    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(groupbox);
    setLayout(rootBox);
    selectedCntChanged();
}

void SelectedExtensZone::addExten(PBX::Extension e)
{
    if (mSelectedExtensions.contains(e.number)) {
        LOG(Logger::Notice, "%s already in selected extensions list.\n", e.number.toStdString().c_str());
        return;
    }
    SelectedExten* selExten = new SelectedExten(this, e);
    mSelectedExtensions.insert(e.number, selExten);
    mSelectedExtenHolder->appendWidget(selExten);
    mSelectedExtenHolder->markTimedRefresh();
    selectedCntChanged();
}

void SelectedExtensZone::removeExten(PBX::Extension e)
{
    if (!mSelectedExtensions.contains(e.number)) {
        LOG(Logger::Notice, "%s not in selected extensions list.\n", e.number.toStdString().c_str());
        return;
    }
    SelectedExten* selExten = mSelectedExtensions.value(e.number);
    mSelectedExtensions.remove(e.number);
    mSelectedExtenHolder->removeWidget(selExten);
    mSelectedExtenHolder->markTimedRefresh();
    mMainWidget->getExtensionsZone()->toggleExten(false, e.number);
    delete selExten;
    if (mSelectedExtensions.size() == 0)
        mMainWidget->getRinggroupsZone()->setGroupCheckStatus(mMainWidget->getRinggroupsZone()->getCurrentSelectedGrpNumber(), false);
    selectedCntChanged();
}

QList<PBX::Extension> SelectedExtensZone::getExtens()
{
    QList<PBX::Extension> pbxExtens;
    foreach (SelectedExten* exten, mSelectedExtensions.values()) {
        PBX::Extension pbxExten = exten->getPbxExten();
        pbxExtens.append(pbxExten);
    }
    return pbxExtens;
}

void SelectedExtensZone::selectedCntChanged()
{
    int count = mSelectedExtensions.size();
    mMainWidget->getFastFuncZone()->selectedExtenCntChanged(count);
}

void SelectedExtensZone::btnClearClicked()
{
    foreach (SelectedExten* exten, mSelectedExtensions.values())
        removeExten(exten->getPbxExten());
}

bool SelectedExtensZone::hasExtension(QString extenNumber)
{
    QList<QString> numbers = mSelectedExtensions.keys();
    return numbers.contains(extenNumber) ? true : false;
}

SelectedExten::SelectedExten(SelectedExtensZone *zone, PBX::Extension e, QWidget *parent)
    : DingoButton(parent), mPbxExten(e), mZone(zone)
{
    QLabel* label = new QLabel(e.number, this);
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    rootBox->addWidget(label);
    setLayout(rootBox);
    connect(this, SIGNAL(widgetClickedSignal()), this, SLOT(clickedSlot()));
}

void SelectedExten::clickedSlot()
{
    mZone->removeExten(mPbxExten);
}
