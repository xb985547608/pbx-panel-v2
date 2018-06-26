#ifndef EVENTLOG_H
#define EVENTLOG_H

#include "ui/base/BaseWidget.h"
#include "ui/base/multitablewidgets.h"
#include "pbx/EventModel.h"

class QueryZone;
class EventDetailZone;

class EventLog : public BaseWidget
{
    Q_OBJECT
public:
    explicit EventLog(MainWindow*, QWidget *parent = 0);
    void onResume(QMap<QString, QVariant> &);
    void loadHistoryZone(bool listAll = true); //listAll == false，则根据QueryZone查询条件显示

protected:
    void initFrames();

private slots:
    void onBtnEventClicked();
private:
    QWidget* getLeftFrame();
    EventDetailZone* getRightFrame();
    QueryZone* mQueryZone;
    MultiTableWidgets* mHistoryZone;
    EventDetailZone* mEventDetailZone;
};

class QueryZone : public QWidget
{
    Q_OBJECT
public:
    explicit QueryZone(EventLog* eventLogWidget, QWidget* parent = 0);
    QString getQueryTitle() { return mLETitle->text(); }
    QDateTime getQueryDate() { return mDateTimeEditor->dateTime(); }
    QString getQueryDescription() { return mLEText->text(); }
private slots:
    void onBtnQueryClicked();
    void onBtnQueryAllClicked();
private:
    QLineEdit* mLETitle;
    QDateTimeEdit* mDateTimeEditor;
    QLineEdit* mLEText; //文字内容
    EventLog* mEventLogWidget;
};

class EventDetailZone : public QWidget
{
    Q_OBJECT
public:
    explicit EventDetailZone(QWidget* parent = 0);
    void setDetail(EventModel::Event& event);
private:
    QLineEdit* mLETitle;
    QLineEdit* mLERecorder;
    QDateTimeEdit* mDateTimeEditor;
    QPlainTextEdit* mDescription;
};

#endif // EVENTLOG_H
