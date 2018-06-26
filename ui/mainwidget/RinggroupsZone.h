#ifndef RINGGROUPSZONE_H
#define RINGGROUPSZONE_H

#include <QtGui>
#include "ui/base/multitablewidgets.h"
#include "ui/base/dingobutton.h"
#include "MainWidget.h"

class RinggroupBtn;
class RinggroupsZone : public QWidget
{
    Q_OBJECT
public:
    explicit RinggroupsZone(MainWidget *mainWidget, QWidget* parent = 0);
    void toggleGroup(QString groupNumber);
    QString getCurrentSelectedGrpNumber() { return mCurrentSelectedGrpNumber; }
    QString getCurrentSelectedGrpName();
    void setGroupCheckStatus(QString groupNumber, bool); //去掉或打上ringgroup上的勾，但不触发事件
private slots:
    void onGroupBtnSelectAllChecked(bool checked);
    void onGroupBtnClicked();
private:
    void toggleGroup(RinggroupBtn*);
    QList<RinggroupBtn*> mRinggroupBtns;
    MainWidget* mMainWidget;
    QString mCurrentSelectedGrpNumber; //当前选中的振铃组号
};

class RinggroupBtn : public DingoButton
{
    Q_OBJECT
public:
    //振铃组描述和组号
    explicit RinggroupBtn(QString name, QString number, QWidget* parent = 0);
    QString getNumber() { return mNumber; }
    QString getName() { return mName; }
    void disable();
    void enable();
    void setCheckBoxState(bool); //干掉或加上checkbox上的勾，但不触发signal
signals:
    //mChkBoxSelectAll被勾选时触发该信号
    void chkBoxSelectAllSignal(bool);
private:
    QCheckBox* mChkBoxSelectAll;
    QString mNumber;
    QString mName;
};

#endif // RINGGROUPSZONE_H
