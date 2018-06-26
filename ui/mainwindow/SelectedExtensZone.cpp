#include "SelectedExtensZone.h"
#include "ExtensionsZone.h"
#include "FastFuncZone.h"
#include "ui/base/misccomponents.h"
#include "misc/logger.h"

SelectedExtensZone::SelectedExtensZone(MainWindow* mw, QWidget *parent) :
    QWidget(parent), mMainwindow(mw)
{
    QLabel* label = new QLabel(tr("已选\n分机"), this);
    mSelectedExtenHolder = new MultiTableWidgets(12, 6, this, label, MultiTableWidgets::CtrlBtnBesideWidget);
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
    mMainwindow->getExtensionsZone()->toggleExten(false, e.number);
    delete selExten;
    selectedCntChanged();
}

void SelectedExtensZone::selectedCntChanged()
{
    int count = mSelectedExtensions.size();
    mMainwindow->getFastFuncZone()->selectedExtenCntChanged(count);
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
