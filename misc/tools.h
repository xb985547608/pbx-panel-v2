#ifndef TOOLS_H
#define TOOLS_H
#include <string>
#include <QStringList>
#include <QString>
#include <QProcess>
#include <QVector>
#include <QtGui>

class Tools
{
public:
    Tools() {}
    static std::string getUuid();
    static long getTimeStamp();
};

class CmdProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CmdProcessor(QObject* parent = 0);
    ~CmdProcessor();
    bool initialise();
    void addJob(QString cmd, QStringList args, bool detached = false);
private slots:
    void onCommandFinished(int);
private:
    typedef struct _Job {
        QString cmd;
        QStringList args;
        bool detached;
    } Job;
    QVector<Job*> mJobs;
    Job* mCurrentJob;
    QProcess* mProcessor;
    void nextJob();
};

#endif // TOOLS_H
