#include "tools.h"
#include <cstdio>
#include <time.h>
#include <QMutex>

#ifdef WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

#ifndef WIN32
typedef struct _GUID {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
}GUID, UUID;
#else
typedef struct _GUID UUID;
#endif

static QMutex CmdProcessorJobLock;

static GUID CreateGuid()
{
    GUID guid;
#ifdef WIN32
    CoCreateGuid(&guid);
#else
    uuid_generate(reinterpret_cast<unsigned char*>(&guid));
#endif
    return guid;
}

std::string Tools::getUuid()
{
    char buf[64] = {'\0'};
    GUID guid = CreateGuid();
#ifdef __GNUC__
    snprintf(
#else ///msvc
    _snprintf_s(
#endif
                buf,
                sizeof(buf),
                //"%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                "%08lX%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1],
                guid.Data4[2], guid.Data4[3],
                guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);
    return std::string(buf);
}

long Tools::getTimeStamp()
{
    return time(NULL);
}

/*
 *
 */
CmdProcessor::CmdProcessor(QObject *parent) :
    QObject(parent)
{
    mProcessor = NULL;
}

CmdProcessor::~CmdProcessor()
{
    if (mProcessor != NULL)
        delete mProcessor;
}

bool CmdProcessor::initialise()
{
    mProcessor = new QProcess();
    connect(mProcessor, SIGNAL(finished(int)), this, SLOT(onCommandFinished(int)));
    mCurrentJob = NULL;
    return true;
}

void CmdProcessor::addJob(QString cmd, QStringList args, bool detached)
{
    Job* job = new Job();
    job->cmd = cmd;
    job->args = args;
    job->detached = detached;
    CmdProcessorJobLock.lock();
    mJobs.push_back(job);
    CmdProcessorJobLock.unlock();
    nextJob();
}

void CmdProcessor::nextJob()
{
    CmdProcessorJobLock.lock();
    do {
        if (mJobs.size() == 0)
            break;
        if (mCurrentJob != NULL)
            break;
        mCurrentJob = mJobs[0];
        if (mCurrentJob->detached) {
            mProcessor->startDetached(mCurrentJob->cmd, mCurrentJob->args);
        } else
            mProcessor->start(mCurrentJob->cmd, mCurrentJob->args);
        mJobs.remove(0);
    } while (false);
    CmdProcessorJobLock.unlock();
    if (mCurrentJob != NULL && mCurrentJob->detached) {
        onCommandFinished(0);
    }
}

void CmdProcessor::onCommandFinished(int)
{
    CmdProcessorJobLock.lock();
#ifndef QT_NO_DEBUG
    QString result = mProcessor->readAll();
    printf("CmdProcessor, result: %s\n", result.toStdString().c_str());
#endif
    mCurrentJob = NULL;
    CmdProcessorJobLock.unlock();
    nextJob();
}
