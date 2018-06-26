#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif

class MainWindow;
/*
 * 点击back按钮时，会在调用mainwindow::loadactivity(id, params)的params中自动设置"back-from"，表示从哪个activity返回的
 */
class BaseWidget : public QWidget
{
    Q_OBJECT
public:
    typedef enum _Activity { eNone = -1, eMainWidget, eSettings, ePhoneBook, eEventPlanEditor, eEmergency, eEvent, eCDR, eEventLog, eMeetme, eTimedBroadcastEditor } Activity;
    /*
     * BaseWidget(标题, 标题图标文件名，自己的Activity，点击返回时的activity, parent);
     */
    explicit BaseWidget(
            QString title,
            QString titleImage,
            QColor titleBgColor,
            Activity activityId = eNone,
            Activity activityBack = eNone,
            MainWindow* mainWindow = NULL,
            QWidget *parent = 0);

    void setBackActiviy(Activity a) { mActivityBackId = a; }
    void setTitle(QString title);
    virtual void onResume(QMap<QString, QVariant>&); //当widget被显示时调用
    virtual void onPause(QMap<QString, QVariant>&); //当widget被hide时调用
    int getActivityId() { return mActivityId; }
    MainWindow* getMainWindow() { return mMainWindow; }

protected slots:
    void onBtnBackClicked();
protected:
    virtual void initFrames() = 0;
    virtual QWidget* getTopFrame();
    void timerEvent(QTimerEvent *);
    
    QLabel* mTitleLabel;
    QColor mTitleBGColor;
    QString mTitleImage;
    Activity mActivityId;
    Activity mActivityBackId;
    int mCallerActivityId; //默认为eNone，当onresume传入"caller-activity"参数时，按返回键的时候调用该activity，否则调用mActivityBackId
    MainWindow* mMainWindow;
    QVBoxLayout* mFrameRootBox;
    QVBoxLayout* mHeaderBox;
    QWidget* mHeaderWidget;
signals:
    
public slots:

private:
    QPushButton* mBtnBack;
    QWidget *initHeader();
    
};

#endif // BASEWIDGET_H
