#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <misc/singleton.h>
#include <QtGui>
#include "qftp.h"

class FTPClient : public QObject
{
    Q_OBJECT
public:
    typedef enum _Direction { Download, Upload } Direction;
    FTPClient(QString username, QString password, QString host, int port = 21, QObject* parent = 0);
    ~FTPClient();
    void addJob(Direction, QString localFile, QString remoteDir, QString remoteFile,
                QStringList cmdAndArgs, /* cmdAndArgs[0] = cmd, 余下的是参数 */
                bool needRemoveRemote = false,
                bool needNotice = false);
    QFtp* getQFtp() { return mFtpClient; }
protected :
    void timerEvent(QTimerEvent * e);

private slots :
    void onFtpCommandStarted(int);
    void onFtpCommandFinished(int, bool error);
private :
    typedef struct _Job {
        QFile* localFile;
        QString remoteDir;
        QString remoteFile;
        Direction direction;
        bool needNotice; //完成后（无论是否成功）是否提示
        bool needRemoveRemote; //完成后(无论是否成功）是否删除远端文件
        /*
        如果cmdAndArgs.size()>0，下载完成后通过Tools::CmdProcessor执行命令
        cmdAndArgs[0]是命令，余下内容是参数
        */
        QStringList cmdAndArgs;

        QString asString();
    } Job;

    QString mUsername;
    QString mPassword;
    QString mHost;
    int mPort;
    int mRetry;
    bool mError;
    Job* mCurrentJob;

    QFtp* mFtpClient;
    QVector<Job*> mJobs;

    void nextJob();
    bool hasJobs();

    const static int MaxRetryTimes;
public:
    static QString _toSpecialEncoding(const QString& inputStr);
    static QString _fromSpecialEncoding(const QString& inputStr);
};

#endif // FTPCLIENT_H
