#ifndef RINGGROUPSZONE_H
#define RINGGROUPSZONE_H

#include <QtGui>
#include "ui/base/multitablewidgets.h"
#include "ui/base/dingobutton.h"
#include "MainWindow.h"

class RinggroupBtn;
class RinggroupsZone : public QWidget
{
    Q_OBJECT
public:
    explicit RinggroupsZone(MainWindow* mainWindow, QWidget* parent = 0);
    void toggleGroup(RinggroupBtn*);
private slots:
    void onGroupBtnSelectAllChecked(bool checked);
    void onGroupBtnClicked();
private:
    QList<RinggroupBtn*> mRinggroupBtns;
    MainWindow* mMainWindow;
};

class RinggroupBtn : public DingoButton
{
    Q_OBJECT
public:
    //振铃组描述和组号
    explicit RinggroupBtn(QString name, QString number, QWidget* parent = 0);
    QString getNumber() { return mNumber; }
    void disable();
    void enable();
signals:
    //mChkBoxSelectAll被勾选时触发该信号
    void chkBoxSelectAllSignal(bool);
private:
    QCheckBox* mChkBoxSelectAll;
    QString mNumber;
};

#endif // RINGGROUPSZONE_H
