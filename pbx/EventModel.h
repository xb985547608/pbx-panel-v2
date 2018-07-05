#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QtGui>
class EventModel
{
public:
    typedef enum _EventType { eNormal, eEmergency, eTimedEvent } EventType;

    class Plan {
    public:
        Plan();
        Plan(const Plan&);
        bool operator ==(const Plan&);
        QString id;
        QString name;
        EventType type;
        QString sms;
        //////////定时广播/////////////
        /*********模式选择************/
        QString mModelSelect;
        /***************************/

        /*********日期选择************/
        QList<QString> mDateList;
        bool mIsExecuteEveryWeeks;
        QString mExecuteDate;
        /***************************/

        /*********时间选择************/
        QString mStartTime;
        QString mStopTime;
        /***************************/

        /*********循环次数选择*********/
        QString mCycleNumber;
        bool mIsUlimiteCycle;
        /***************************/

        /////////////////////////////
        QStringList broadcastFiles; //被选中的录音文件列表
        //<通讯录id, qstring - 通讯录中选中的通讯录号码代号，如：通讯录id-E-M-H-D
        //E：分机被选中，M：手机被选中，H：家庭号码被选中，D：直线号码被选中
        QMap<QString, QString> numbers;
        QStringList terminals;
    };

    class PlanData : public QObjectUserData
    {
    public :
        PlanData(EventModel::Plan plan) :
            mPlan(plan)
        {}
        ~PlanData();
        EventModel::Plan getData() { return mPlan; }
    private:
        EventModel::Plan mPlan;
    };

    class Event {
    public:
        Event() {}
        QString id;
        QString planId;
        QDateTime created_date;
        QString title;
        QString description;
        QString recorder;
    };

    class EventData : public QObjectUserData
    {
    public :
        EventData(EventModel::Event event) :
            mEvent(event)
        {}
        ~EventData();
        EventModel::Event getData() { return mEvent; }
    private:
        EventModel::Event mEvent;
    };

    EventModel();
};

#endif // EVENTMODEL_H
