#include "OpSettingsZone.h"
#include "ui/base/tablewidget.h"
#include "message_queue/RPCCommand.h"
#include "ui/base/misccomponents.h"

OpSettingsZone::OpSettingsZone(SettingsWidget* sw, QWidget* parent) :
    QWidget(parent), mSettingsWidget(sw)
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);

    TableWidget* tableWidget = new TableWidget(2, this);
    rootBox->addWidget(tableWidget);
    mBtnSubmit = new QPushButton(tr("更改"), tableWidget);
    connect(mBtnSubmit, SIGNAL(clicked()), this, SLOT(btnSubmitSlot()));
    tableWidget->setHeader(MiscComponents::getSectionLabel(tr("调度员设置"), tableWidget));
    tableWidget->setFooter(mBtnSubmit);

    tableWidget->appendItem(new QLabel(tr("调度员1"), this));
    mOp1Widget = new QLineEdit(this);
    tableWidget->appendItem(mOp1Widget);
    tableWidget->appendItem(new QLabel(tr("调度员2"), this));
    mOp2Widget = new QLineEdit(this);
    tableWidget->appendItem(mOp2Widget, true);
    loadOpNumber();
    startTimer(10000);
}

void OpSettingsZone::loadOpNumber()
{
    QMap<QString, PBX::Extension> ops = RPCCommand::getOperators();
    QList<QString> opNumbers = ops.keys();
    if (opNumbers.size() > 0)
        mOp1Widget->setText(opNumbers.at(0));
    if (opNumbers.size() > 1)
        mOp2Widget->setText(opNumbers.at(1));
}

void OpSettingsZone::timerEvent(QTimerEvent *)
{
    if (!mBtnSubmit->isEnabled())
        mBtnSubmit->setEnabled(true);
}

void OpSettingsZone::saveOpNumber()
{
    QStringList numbers;
    QString number = mOp1Widget->text();
    if (number != "")
        numbers.append(number);
    number = mOp2Widget->text();
    if (number != "")
        numbers.append(number);
    mBtnSubmit->setEnabled(false);
    RPCCommand::setOperators(numbers);
}


void OpSettingsZone::btnSubmitSlot()
{
    saveOpNumber();
}
