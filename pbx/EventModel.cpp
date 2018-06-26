#include "EventModel.h"
#include "misc/logger.h"

EventModel::EventModel()
{
}
/*
<Result>
<PlanList>
    <Plan>
        <event_type>eNormal/eEmergency/eTimedEvent</event_type>
        <model_type>cycleNumberModel/executeTimeModel</model_type>
        <id></id>
        <name>xxx</name>
        <broadcast_file>xxx,oooo,pppp</broadcast_file>
        <numbers>通讯录id-E-D-M-H,通讯录id-M,通讯录id-E-D-H</numbers>
        <date>true,true,true,true,true,true,true</date>
        <execute_everyweeks>true</execute_everyweeks>
        <execute_date>Monday,Tuesday<execute_date>
        <start_time>09:00</start_time>
        <stop_time>10:00</stop_time>
        <ulimited_cycle>true</ulimited_cycle>
        <cycle_number>1</cycle_number>

        <is_emergency></is_emergency>
        <sms_content>xxxx</sms_content>
        <terminals>8001,8002,8003</terminals>
    </Plan>
</PlanList>
</Result>
*/
EventModel::Plan::Plan()
{
    id = "";
    name = "";
    sms = "";
    mModelSelect = "";
    mDateList.clear();
    mIsExecuteEveryWeeks = false;
    mExecuteDate = "";
    mStartTime = "";
    mStopTime = "";
    mCycleNumber = "";
    mIsUlimiteCycle = false;
    broadcastFiles.clear();
    numbers.clear();
    terminals.clear();
}

EventModel::Plan::Plan(const Plan &plan)
{
    this->type = plan.type;
    this->mModelSelect = plan.mModelSelect;
    this->id = plan.id;
    this->name = plan.name;
    this->broadcastFiles = plan.broadcastFiles;
    this->numbers = plan.numbers;
    this->mDateList = plan.mDateList;
    this->mIsExecuteEveryWeeks = plan.mIsExecuteEveryWeeks;
    this->mExecuteDate = plan.mExecuteDate;
    this->mStartTime = plan.mStartTime;
    this->mStopTime = plan.mStopTime;
    this->mIsUlimiteCycle = plan.mIsUlimiteCycle;
    this->mCycleNumber = plan.mCycleNumber;

    this->sms = plan.sms;
    this->terminals = plan.terminals;
}

bool EventModel::Plan::operator ==(const Plan& plan)
{
    return (this->id == plan.id) ? true : false;
}

EventModel::PlanData::~PlanData()
{
    LOG(Logger::Debug, "PlanData deleting.\n");
}

EventModel::EventData::~EventData()
{
    LOG(Logger::Debug, "EventData deleting.\n");
}
