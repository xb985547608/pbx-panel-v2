#include "UiPhoneBookList.h"
#include "ui/base/misccomponents.h"
#include "ui/base/tablewidget.h"
#include "message_queue/RPCCommand.h"
#include "misc/logger.h"
#include "misc/Config.h"

UiPhoneBookList::UiPhoneBookList(QWidget *parent, MODE mode) :
    QWidget(parent), mMode(mode)
{
    QVBoxLayout* rootBox = new QVBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QVBoxLayout* listBox = new QVBoxLayout;
    rootBox->addWidget(MiscComponents::getGroupBox(listBox, this));
    listBox->addWidget(getRecordList());
}

QTreeView* UiPhoneBookList::getRecordList()
{
    //列表
    mRecordListModel = new QStandardItemModel(0, 9, this);
    mRecordList = new QTreeView(this);
    mRecordList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mRecordList->setFocusPolicy(Qt::NoFocus);
    mRecordList->setRootIsDecorated(false);
    mRecordList->setAlternatingRowColors(true);
    mRecordList->setStyleSheet(
                "QTreeView::item {height:20px; show-decoration-selected:0; font: 75 10pt;}"
                "QTreeView::item::selected {color: green;}"
                );
    mRecordList->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical { border: 2px solid grey; width: 30px; }"
                );
    mRecordList->setModel(mRecordListModel);
    mRecordListModel->setHeaderData(0, Qt::Horizontal, tr(""));
    mRecordListModel->setHeaderData(1, Qt::Horizontal, tr("单位"));
    mRecordListModel->setHeaderData(2, Qt::Horizontal, tr("部门"));
    mRecordListModel->setHeaderData(3, Qt::Horizontal, tr("职位"));
    mRecordListModel->setHeaderData(4, Qt::Horizontal, tr("姓名"));
    mRecordListModel->setHeaderData(5, Qt::Horizontal, tr("分机号码"));
    mRecordListModel->setHeaderData(6, Qt::Horizontal, tr("直线号码"));
    mRecordListModel->setHeaderData(7, Qt::Horizontal, tr("手机号码"));
    mRecordListModel->setHeaderData(8, Qt::Horizontal, tr("家庭座机号码"));
    mRecordList->setColumnWidth(0, 30);
    mRecordList->setColumnWidth(1, 150);
    mRecordList->setColumnWidth(6, 150);
    mRecordList->setColumnWidth(7, 150);
    connect(mRecordList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onRecordListViewClicked(const QModelIndex&)));
    return mRecordList;
}

void UiPhoneBookList::loadData(QString ringgroupNumber, int mode)
{
    mMode = mode;
    if (mMode != eEdit)
        mRecordList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    else
        mRecordList->setEditTriggers(QAbstractItemView::DoubleClicked);
    QString number = ringgroupNumber;
    if (ringgroupNumber == Config::Instance()->AllExtensionGroupNumber)
        number = "";
    //disconnect(mRecordListModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onRecordDataChanged(QModelIndex,QModelIndex)));
    QMap<QString, PhoneBook> phoneBookRecords = RPCCommand::getPhonebook(number);
    mRecordListModel->removeRows(0, mRecordListModel->rowCount());
    foreach(PhoneBook record, phoneBookRecords.values()) {
        mRecordListModel->insertRow(0);
        mRecordListModel->setData(mRecordListModel->index(0,0), record.id, Qt::UserRole);
        mRecordListModel->setData(mRecordListModel->index(0,0), false, Qt::CheckStateRole);
        mRecordListModel->setData(mRecordListModel->index(0,1), record.company);
        mRecordListModel->setData(mRecordListModel->index(0,1), record.company, Qt::UserRole);
        mRecordListModel->setData(mRecordListModel->index(0,2), record.department);
        mRecordListModel->setData(mRecordListModel->index(0,3), record.position);
        mRecordListModel->setData(mRecordListModel->index(0,3), record.position, Qt::UserRole);
        mRecordListModel->setData(mRecordListModel->index(0,4), record.name);
        mRecordListModel->setData(mRecordListModel->index(0,5), record.extenNumber);
        mRecordListModel->setData(mRecordListModel->index(0,5), false, Qt::CheckStateRole);
        mRecordListModel->setData(mRecordListModel->index(0,6), record.directdid);
        //mRecordListModel->setData(mRecordListModel->index(0,6), record.directdid, Qt::UserRole);
        mRecordListModel->setData(mRecordListModel->index(0,6), false, Qt::CheckStateRole);
        mRecordListModel->setData(mRecordListModel->index(0,7), record.mobile);
        mRecordListModel->setData(mRecordListModel->index(0,7), record.mobile, Qt::UserRole);
        mRecordListModel->setData(mRecordListModel->index(0,7), false, Qt::CheckStateRole);
        mRecordListModel->setData(mRecordListModel->index(0,8), record.homeNumber); //officeNumber 作为家庭号码
        mRecordListModel->setData(mRecordListModel->index(0,8), record.homeNumber, Qt::UserRole);
        mRecordListModel->setData(mRecordListModel->index(0,8), false, Qt::CheckStateRole);
        if (mMode == eEdit) {
            mRecordListModel->item(0,0)->setFlags(Qt::ItemIsEditable);
            mRecordListModel->item(0,2)->setFlags(Qt::ItemIsEditable);
            mRecordListModel->item(0,4)->setFlags(Qt::ItemIsEditable);
            mRecordListModel->item(0,5)->setFlags(Qt::ItemIsEditable);
            mRecordListModel->item(0,6)->setFlags(Qt::ItemIsEditable);
        }
    }
    //connect(mRecordListModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onRecordDataChanged(QModelIndex,QModelIndex)));
}

void UiPhoneBookList::clearData()
{
    mRecordListModel->removeRows(0, mRecordListModel->rowCount());
}

void UiPhoneBookList::onRecordDataChanged(QModelIndex index, QModelIndex)
{
    //此函数未使用
    return;
    QString newData = mRecordListModel->data(mRecordListModel->index(index.row(), index.column())).toString();
    QString oldData = mRecordListModel->data(mRecordListModel->index(index.row(), index.column()), Qt::UserRole).toString();
    if (newData != oldData)
        LOG(Logger::Debug, "data changed on (%d,%d), old %s, new %s, id=%s\n", index.row(), index.column(),
            oldData.toStdString().c_str(), newData.toStdString().c_str(),
            mRecordListModel->data(mRecordListModel->index(index.row(), 0), Qt::UserRole).toString().toStdString().c_str());
}

void UiPhoneBookList::onRecordListViewClicked(const QModelIndex& index)
{
    int column = index.column();
    if (column == 0 || column == 5 || column == 6 || column == 7 || column == 8) {
        bool value = !mRecordListModel->data(mRecordListModel->index(index.row(), column), Qt::CheckStateRole).toBool();
        mRecordListModel->setData(mRecordListModel->index(index.row(), column), value, Qt::CheckStateRole);
    }
    //如果是单选模式，则清除掉其他条目上的选择
    if (mMode == eSelectSingle) {
        for (int i=0;i<mRecordListModel->rowCount();i++) {
            if (i == index.row())
                continue;
            for (int j = 5; j < 9; j++) {
                mRecordListModel->setData(mRecordListModel->index(i, j), false, Qt::CheckStateRole);
            }
        }
    }
}

void UiPhoneBookList::save()
{
    QList<PhoneBook>changedPhonebookList;
    for (int i=0;i<mRecordListModel->rowCount();i++) {
        QString companyNew = mRecordListModel->data(mRecordListModel->index(i,1)).toString();
        QString companyOld = mRecordListModel->data(mRecordListModel->index(i,1), Qt::UserRole).toString();
        QString positionNew = mRecordListModel->data(mRecordListModel->index(i,3)).toString();
        QString positionOld = mRecordListModel->data(mRecordListModel->index(i,3), Qt::UserRole).toString();
        /*
        QString didNew = mRecordListModel->data(mRecordListModel->index(i,6)).toString();
        QString didOld = mRecordListModel->data(mRecordListModel->index(i,6), Qt::UserRole).toString();
        */
        QString mobileNew = mRecordListModel->data(mRecordListModel->index(i,7)).toString();
        QString mobileOld = mRecordListModel->data(mRecordListModel->index(i,7), Qt::UserRole).toString();
        QString homeNew = mRecordListModel->data(mRecordListModel->index(i,8)).toString();
        QString homeOld = mRecordListModel->data(mRecordListModel->index(i,8), Qt::UserRole).toString();
        if (companyNew != companyOld ||
                positionNew != positionOld ||
                //didNew != didOld ||
                mobileNew != mobileOld ||
                homeNew != homeOld) {
            PhoneBook phoneBook;
            phoneBook.id = mRecordListModel->data(mRecordListModel->index(i,0), Qt::UserRole).toString();
            phoneBook.company = companyNew;
            //phoneBook.directdid = didNew;
            phoneBook.position = positionNew;
            phoneBook.mobile = mobileNew;
            phoneBook.homeNumber = homeNew;
            changedPhonebookList.append(phoneBook);
        }
    }
    foreach (PhoneBook phonebook, changedPhonebookList)
        RPCCommand::savePhonebook(phonebook);
}

QList<PhoneBook> UiPhoneBookList::getSelectedRecords()
{
    QList<PhoneBook> records;
    for (int i=0;i<mRecordListModel->rowCount();i++) {
        bool checked = mRecordListModel->data(mRecordListModel->index(i, 0), Qt::CheckStateRole).toBool();
        QString id = mRecordListModel->data(mRecordListModel->index(i, 0), Qt::UserRole).toString();
        QString company = mRecordListModel->data(mRecordListModel->index(i, 1)).toString();
        QString depart = mRecordListModel->data(mRecordListModel->index(i, 2)).toString();
        QString position = mRecordListModel->data(mRecordListModel->index(i, 3)).toString();
        QString name = mRecordListModel->data(mRecordListModel->index(i, 4)).toString();
        bool extenChecked = mRecordListModel->data(mRecordListModel->index(i, 5), Qt::CheckStateRole).toBool();
        QString extension = mRecordListModel->data(mRecordListModel->index(i, 5)).toString();
        bool didChecked = mRecordListModel->data(mRecordListModel->index(i, 6), Qt::CheckStateRole).toBool();
        QString did = mRecordListModel->data(mRecordListModel->index(i, 6)).toString();
        bool mobileChecked = mRecordListModel->data(mRecordListModel->index(i, 7), Qt::CheckStateRole).toBool();
        QString mobile = mRecordListModel->data(mRecordListModel->index(i, 7)).toString();
        bool homeChecked = mRecordListModel->data(mRecordListModel->index(i, 8), Qt::CheckStateRole).toBool();
        QString home = mRecordListModel->data(mRecordListModel->index(i, 8)).toString();
        PhoneBook record;
        record.id = id;
        record.company = company;
        record.department = depart;
        record.position = position;
        record.name = name;
        if (checked) {
            record.extenNumber = extension;
            record.mobile = mobile;
            record.directdid = did;
            record.homeNumber = home;
            records.append(record);
            continue;
        }
        if (extenChecked)
            record.extenNumber = extension;
        if (didChecked)
            record.directdid = did;
        if (mobileChecked)
            record.mobile = mobile;
        if (homeChecked)
            record.homeNumber = home;
        if (extenChecked || didChecked || mobileChecked || homeChecked)
            records.append(record);
    }
    return records;
}

void UiPhoneBookList::addRecord(PhoneBook &record)
{
    mRecordListModel->insertRow(0);
    mRecordListModel->setData(mRecordListModel->index(0,0), record.id, Qt::UserRole);
    mRecordListModel->setData(mRecordListModel->index(0,0), false, Qt::CheckStateRole);
    mRecordListModel->setData(mRecordListModel->index(0,1), record.company);
    mRecordListModel->setData(mRecordListModel->index(0,1), record.company, Qt::UserRole);
    mRecordListModel->setData(mRecordListModel->index(0,2), record.department);
    mRecordListModel->setData(mRecordListModel->index(0,3), record.position);
    mRecordListModel->setData(mRecordListModel->index(0,3), record.position, Qt::UserRole);
    mRecordListModel->setData(mRecordListModel->index(0,4), record.name);
    mRecordListModel->setData(mRecordListModel->index(0,5), record.extenNumber);
    mRecordListModel->setData(mRecordListModel->index(0,5), false, Qt::CheckStateRole);
    mRecordListModel->setData(mRecordListModel->index(0,6), record.directdid);
    //mRecordListModel->setData(mRecordListModel->index(0,6), record.directdid, Qt::UserRole);
    mRecordListModel->setData(mRecordListModel->index(0,6), false, Qt::CheckStateRole);
    mRecordListModel->setData(mRecordListModel->index(0,7), record.mobile);
    mRecordListModel->setData(mRecordListModel->index(0,7), record.mobile, Qt::UserRole);
    mRecordListModel->setData(mRecordListModel->index(0,7), false, Qt::CheckStateRole);
    mRecordListModel->setData(mRecordListModel->index(0,8), record.homeNumber); //officeNumber 作为家庭号码
    mRecordListModel->setData(mRecordListModel->index(0,8), record.homeNumber, Qt::UserRole);
    mRecordListModel->setData(mRecordListModel->index(0,8), false, Qt::CheckStateRole);
    if (mMode == eEdit) {
        mRecordListModel->item(0,0)->setFlags(Qt::ItemIsEditable);
        mRecordListModel->item(0,2)->setFlags(Qt::ItemIsEditable);
        mRecordListModel->item(0,4)->setFlags(Qt::ItemIsEditable);
        mRecordListModel->item(0,5)->setFlags(Qt::ItemIsEditable);
        mRecordListModel->item(0,6)->setFlags(Qt::ItemIsEditable);
    } else
        mRecordList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void UiPhoneBookList::removeRecord(PhoneBook &record)
{
    int rowid = -1;
    for (int i=0;i<mRecordListModel->rowCount();i++) {
        QString id = mRecordListModel->data(mRecordListModel->index(i, 0), Qt::UserRole).toString();
        if (id == record.id) {
            rowid = i;
            break;
        }
    }
    if (rowid != -1)
        mRecordListModel->removeRow(rowid);
}

void UiPhoneBookList::call(QString roomid)
{
    Config* cfg = Config::Instance();
    QString localChannel = QString("LOCAL/NUMBER@%1/n").arg(cfg->DefaultContext);
    QString context = cfg->OperatorGroupCallContext;
    if (roomid == "")
        roomid = cfg->getGroupcallNumber();
    QString callerid = "\"OPGROUP\"&lt;" + cfg->DefaultCallerId + "&gt;";
    QString exten = cfg->DefaultCallerId;
    QString opChannel = "LOCAL/" + cfg->DefaultCallerId + "@" + cfg->OperatorGroupContext + "/n";
    QList<PhoneBook> selectedRecords = getSelectedRecords();
    if (selectedRecords.size() == 0)
        return;
    //呼叫管理员
    __call(opChannel, context, exten, callerid, roomid, true);
    //呼叫各个选中的号码
    foreach (PhoneBook record, selectedRecords) {
        QString channel;
        if (record.extenNumber != "") {
            channel = "SIP/" + record.extenNumber;
            __call(channel, context, exten, callerid, roomid, false);
        }
        if (record.directdid != "") {
            channel = localChannel.replace("NUMBER", record.directdid);
            __call(channel, context, exten, callerid, roomid, false);
        }
        if (record.mobile != "") {
            channel = localChannel.replace("NUMBER", record.mobile);
            __call(channel, context, exten, callerid, roomid, false);
        }
        if (record.homeNumber != "") {
            channel = localChannel.replace("NUMBER", record.homeNumber);
            __call(channel, context, exten, callerid, roomid, false);
        }
    }
}

void UiPhoneBookList::__call(QString channel, QString context, QString exten, QString callerid, QString roomid, bool isAdmin)
{
    RPCCommand::makeConferenceCall(channel, context, exten, callerid, roomid, isAdmin);
}

bool UiPhoneBookList::threeWayCall(QString chan, QString extraChan, QString context)
{
    QString target = "";
    QList<PhoneBook> selectedRecords = getSelectedRecords();
    if (selectedRecords.size() != 1) {
        LOG(Logger::Warn, "3wayCall called but records count > 1");
        return false;
    }
    foreach (PhoneBook record, selectedRecords) {
        if (record.extenNumber != "")
            target = record.extenNumber;
        if (record.directdid != "")
            target = record.directdid;
        if (record.mobile != "")
            target = record.mobile;
        if (record.homeNumber != "")
            target = record.homeNumber;
    }
    if (target == "") {
        LOG(Logger::Warn, "3wayCall called but target is empty");
        return false;
    }
    Config* cfg = Config::Instance();
    QString roomid = cfg->getGroupcallNumber();
    QMap<QString, QString> vars;
    //把第3方邀请进入会议
    QString thirdChan = "LOCAL/" + target + "@" + cfg->DefaultContext + "/n";
    RPCCommand::originate(thirdChan, context, roomid, cfg->DefaultCallerId, vars);
    //把通话双发转入会议
    RPCCommand::redirectCall(chan, context, roomid, extraChan);
    return true;
}

bool UiPhoneBookList::redirectCall(QString chan, QString context, QString extraChan)
{
    QString target = "";
    QList<PhoneBook> selectedRecords = getSelectedRecords();
    if (selectedRecords.size() != 1) {
        LOG(Logger::Warn, "redirectCall called but records count > 1");
        return false;
    }
    foreach (PhoneBook record, selectedRecords) {
        if (record.extenNumber != "")
            target = record.extenNumber;
        if (record.directdid != "")
            target = record.directdid;
        if (record.mobile != "")
            target = record.mobile;
        if (record.homeNumber != "")
            target = record.homeNumber;
    }
    if (target == "") {
        LOG(Logger::Warn, "redirectCall called but target is empty");
        return false;
    }
    RPCCommand::redirectCall(chan, context, target, extraChan);
    return true;
}

bool UiPhoneBookList::emergencyCall()
{
    QList<PhoneBook> selectedRecords = getSelectedRecords();
     Config* cfg = Config::Instance();
    foreach (PhoneBook record, selectedRecords) {
        QString target = "";
        if (record.extenNumber != "")
            target = record.extenNumber;
        if (record.directdid != "")
            target = record.directdid;
        if (record.mobile != "")
            target = record.mobile;
        if (record.homeNumber != "")
            target = record.homeNumber;

        if (target == "") {
            LOG(Logger::Warn, "emergencyCall called but target is empty");
            return false;
        }
        QString exten = RPCCommand::getOperatorQueueNumber();
        QMap<QString, QString> vars;
        QString thirdChan = "LOCAL/" + target + "@" + cfg->DefaultContext + "/n";
        QString callerid = "&quot;" + cfg->EmergencyNumber + "&quot;" + "&lt;" + cfg->EmergencyNumber + "&gt;";
        RPCCommand::originate(thirdChan, "app-emergency-call", cfg->EmergencyNumber +exten, callerid, vars);
        //RPCCommand::redirectCall(chan, context, roomid, extraChan);
    }
    return true;
}
