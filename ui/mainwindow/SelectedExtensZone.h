#ifndef SELECTEDEXTENSZONE_H
#define SELECTEDEXTENSZONE_H
#include "ui/base/multitablewidgets.h"
#include "ui/base/dingobutton.h"
#include "pbx/pbx.h"

#include <QtGui>
#include "MainWindow.h"

class SelectedExten;
class SelectedExtensZone : public QWidget
{
    Q_OBJECT
public:
    explicit SelectedExtensZone(MainWindow*, QWidget *parent = 0);
    void addExten(PBX::Extension);
    void removeExten(PBX::Extension);

signals:
    
public slots:
private:
    MainWindow* mMainwindow;
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
private slots:
    void clickedSlot();
private:
    PBX::Extension mPbxExten;
    SelectedExtensZone* mZone;
};

#endif // SELECTEDEXTENSZONE_H
