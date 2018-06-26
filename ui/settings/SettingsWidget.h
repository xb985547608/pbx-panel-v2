#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QtGui>
#include "ui/base/BaseWidget.h"
#include "TimingBroadcastZone.h"

class MainWindow;
class PlanListZone;
class TimingBroadcastZone;
class SettingsWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit SettingsWidget(MainWindow*, QWidget *parent = 0);
    explicit SettingsWidget(
            QString title,
            QString titleImage,
            QColor titleBgColor,
            Activity activityBack,
            MainWindow* mainWindow,
            QWidget *parent = 0);
    MainWindow* getMainWindow();
    virtual void onResume(QMap<QString, QVariant> &);
    
signals:
    
public slots:
protected:
    virtual void initFrames();
private slots:
    void onBtnBackClicked();
private:
    QWidget* getTopFrame();
    QWidget* getLeftFrame();
    QWidget* getRightFrame();
    //zll
    QWidget* getCenterFrame();
    TimingBroadcastZone* mTimedEventPlansZone;
    PlanListZone* mNormalPlansZone;
    PlanListZone* mEmergencyPlansZone;
};

#endif // SETTINGSWIDGET_H
