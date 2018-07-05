#include "SMSWidget.h"
#include "ui/base/misccomponents.h"
#include "message_queue/RPCCommand.h"
#include "misc/Config.h"
#include <QTreeView>
#include <QAbstractItemView>
#include <QScrollBar>

SMSWidget::SMSWidget(QWidget *parent, bool showTemplate) :
    QDialog(parent), mSelectedTemplateId(-1), mShowTemplate(showTemplate)
{
    QHBoxLayout* rootbox = new QHBoxLayout;
    rootbox->setMargin(0);
    rootbox->setSpacing(0);
    QGroupBox* rootWidget = MiscComponents::getGroupBox(rootbox, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(rootWidget);
    setLayout(vbox);
    setMinimumSize(480, 300);

    rootbox->addLayout(getLeftLayout());
    rootbox->addLayout(getRightLayout());
}

QBoxLayout* SMSWidget::getLeftLayout()
{
    QVBoxLayout* rootbox = new QVBoxLayout;
    rootbox->addWidget(MiscComponents::getSectionLabel(tr("短信"), Config::Instance()->Res_icon_sms, this));
    mSmsContent = new QPlainTextEdit(this);
    rootbox->addWidget(mSmsContent);
    QPushButton* btnSaveAs = new QPushButton(tr("另存为模板"), this);
    connect(btnSaveAs, SIGNAL(clicked()), this, SLOT(onBtnSaveAsTemplateClicked()));
    QPushButton* btnSend = new QPushButton(tr("发送"), this);
    connect(btnSend, SIGNAL(clicked()), this, SLOT(onBtnSendClicked()));
    QPushButton* btnClear = new QPushButton(tr("清除"), this);
    connect(btnClear, SIGNAL(clicked()), this, SLOT(onBtnClearClicked()));

    QHBoxLayout* hbox = new QHBoxLayout;
    rootbox->addLayout(hbox);
    hbox->setMargin(0);
    if (mShowTemplate)
        hbox->addWidget(btnSaveAs);
    hbox->addWidget(btnSend);
    hbox->addWidget(btnClear);
    return rootbox;
}

QBoxLayout* SMSWidget::getRightLayout()
{
    QGroupBox* rightWidget;
    QVBoxLayout* rootbox = new QVBoxLayout;
    rootbox->setMargin(0);
    rootbox->setSpacing(0);
    rightWidget = MiscComponents::getGroupBox(rootbox, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(rightWidget);

    mTemplateListModel = new QStandardItemModel(5, 1, this);
    QTreeView* smsTemplateList = new QTreeView(this);
    smsTemplateList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    smsTemplateList->setFocusPolicy(Qt::NoFocus);
    smsTemplateList->setRootIsDecorated(false);
    smsTemplateList->setAlternatingRowColors(true);
    smsTemplateList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    smsTemplateList->setHeaderHidden(true);
    smsTemplateList->setStyleSheet(
                "QTreeView::item {height:50px; show-decoration-selected:0; font: 75 12pt;}"
                "QTreeView::item::selected {color: green;}"
                );
    smsTemplateList->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical { border: 2px solid grey; width: 30px; }"
                );
    smsTemplateList->setModel(mTemplateListModel);
    connect(smsTemplateList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onTemplateListClicked(const QModelIndex&)));
    if (mShowTemplate)
        rootbox->addWidget(smsTemplateList);
    loadSmsTemplates();
    return vbox;
}

void SMSWidget::loadSmsTemplates()
{
    QMap<int, QString> smsTemplates = RPCCommand::getSmsTemplates();
    mTemplateListModel->removeRows(0, mTemplateListModel->rowCount());
    foreach (int id, smsTemplates.keys()) {
        mTemplateListModel->insertRow(0);
        mTemplateListModel->setData(mTemplateListModel->index(0, 0), smsTemplates.value(id));
        mTemplateListModel->setData(mTemplateListModel->index(0, 0), id, Qt::UserRole);
    }
}

void SMSWidget::onBtnClearClicked()
{
    mSmsContent->setPlainText("");
}

void SMSWidget::onBtnSaveAsTemplateClicked()
{
    if (mSelectedTemplateId == -1)
        mSelectedTemplateId = 0;
    RPCCommand::saveSmsTemplate(mSelectedTemplateId, mSmsContent->toPlainText());
    loadSmsTemplates();
}

void SMSWidget::onBtnSendClicked()
{
    accept();
}

void SMSWidget::onTemplateListClicked(const QModelIndex &index)
{
    QString sms = mTemplateListModel->data(index).toString();
    int id = mTemplateListModel->data(index, Qt::UserRole).toInt();
    mSelectedTemplateId = id;
    mSmsContent->setPlainText(sms);
}
