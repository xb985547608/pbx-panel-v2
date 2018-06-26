#ifndef SELECTEDEXTENSZONE_H
#define SELECTEDEXTENSZONE_H
#include "ui/base/multitablewidgets.h"
#include "ui/base/dingobutton.h"
#include "pbx/pbx.h"

#include <QtGui>
#include "MainWidget.h"

class SelectedExten;
class SelectedExtensZone : public QWidget
{
    Q_OBJECT
public:
    explicit SelectedExtensZone(MainWidget*, QWidget *parent = 0);
    void addExten(PBX::Extension);
    void removeExten(PBX::Extension);
    QList<PBX::Extension> getExtens();
    int getCount() { return mSelectedExtensions.size(); }
    bool hasExtension(QString extenNumber); //extenNumber所对应分机是否存在于已选分机列表？

signals:
    
private slots:
    void btnClearClicked();
private:
    MainWidget* mMainWidget;
    MultiTableWidgets* mSelectedExtenHolder;
    //<extension number, SelectedExten*>
    QMap<QString, SelectedExten*> mSelectedExtensions;
    void selectedCntChanged();
};

class SelectedExten : public DingoButton
{
    Q_OBJECT
public:
    explicit SelectedExten(SelectedExtensZone*, PBX::Extension, QWidget* parent = 0);
    PBX::Extension getPbxExten() { return mPbxExten; }
private slots:
    void clickedSlot();
private:
    PBX::Extension mPbxExten;
    SelectedExtensZone* mZone;
};

#endif // SELECTEDEXTENSZONE_H
