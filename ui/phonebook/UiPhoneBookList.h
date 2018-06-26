#ifndef UIPHONEBOOKLIST_H
#define UIPHONEBOOKLIST_H

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "pbx/PhoneBook.h"

class UiPhoneBookList : public QWidget
{
    Q_OBJECT
public:
    /*
    eEdit - 编辑模式，row可双击编辑；
    eSelect - 直接调用通讯录拨号时使用的模式;
    ePickup - 从通讯录中选择条目的模式（事件模板编辑中使用）
    eSelectSingle - 直接调用通讯录拨号时使用的模式，单选模式
     */
    typedef enum _MODE { eEdit, eSelect, ePickup, eSelectSingle } MODE;
    explicit UiPhoneBookList(QWidget *parent = 0, MODE mode = eEdit);
    
signals:
    
public slots:
    void onRecordListViewClicked(const QModelIndex&);
    void onRecordDataChanged(QModelIndex,QModelIndex);
private:
    QTreeView* mRecordList;
    QStandardItemModel* mRecordListModel;
    QTreeView* getRecordList();
    int mMode; //对应enum MODE
    void __call(QString channel, QString context, QString exten, QString callerid, QString roomid, bool isAdmin);
public:
    void loadData(QString ringgroupNumber, int = eEdit);
    void clearData(); //清除列表记录
    void save(); //保存已修改的记录
    void call(QString roomid = ""); //呼叫选中的记录到会议，目前只支持发起会议呼叫
    bool redirectCall(QString chan, QString context, QString extraChan);
    bool threeWayCall(QString chan, QString extraChan, QString context);
    bool emergencyCall();

    QList<PhoneBook> getSelectedRecords();
    void addRecord(PhoneBook&); //往list界面添加一条记录
    void removeRecord(PhoneBook&); //从list界面删除一条记录
};

#endif // UIPHONEBOOKLIST_H
