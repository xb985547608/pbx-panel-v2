#include "ftpclient.h"
#include "logger.h"
#include "misc/tools.h"
#include "config.h"
#include <QMutex>
#include <QMessageBox>

#ifdef WIN32
static void usleep(qint64 uMsec)
{
    QTime dieTime = QTime::currentTime().addMSecs(uMsec / 1000);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
#else
//#include <unistd.h>
#endif

const int FTPClient::MaxRetryTimes = 3;
static QMutex TransferLock;

FTPClient::FTPClient(QString username, QString password, QString host, int port, QObject* parent) :
    QObject(parent), mUsername(username), mPassword(password), mHost(host), mPort(port), mRetry(0), mError(false), mCurrentJob(NULL)
{
    mFtpClient = new QFtp(this);
    connect(mFtpClient, SIGNAL(commandStarted(int)), this, SLOT(onFtpCommandStarted(int)));
    connect(mFtpClient, SIGNAL(commandFinished(int,bool)), this, SLOT(onFtpCommandFinished(int,bool)));
    mFtpClient->connectToHost(mHost, mPort);
    startTimer(60 * 1000);
    LOG(Logger::Debug, "Creating ftp client using %s:%s@%s:%d",
        mUsername.toStdString().c_str(), mPassword.toStdString().c_str(),
        mHost.toStdString().c_str(), mPort);
}

FTPClient::~FTPClient()
{
    if (mJobs.size() > 0)
        foreach (Job* job, mJobs)
            delete job;
}

bool FTPClient::hasJobs()
{
    bool result = false;
    TransferLock.lock();
    if (mJobs.size() > 0)
        result = true;
    TransferLock.unlock();
    return result;
}

void FTPClient::timerEvent(QTimerEvent *e)
{
    if (!hasJobs())
        mFtpClient->rawCommand("pwd");
}

void FTPClient::onFtpCommandStarted(int)
{
}

void FTPClient::onFtpCommandFinished(int, bool error)
{
    switch (mFtpClient->currentCommand()) {
    case QFtp::ConnectToHost : {
        if (error) {
            mFtpClient->connectToHost(mHost, mPort);
            if (mRetry >= MaxRetryTimes) {
                QMessageBox::about(NULL, tr("出错啦！"), tr("连接ftp服务器失败"));
                LOG(Logger::Debug, QString("FtpClient: failed to connect to host(%1:%2)\n")
                            .arg(mHost).arg(mPort)
                            .toUtf8().constData());
                mError = true;
                return;
            }
            mRetry++;
        } else {
            mFtpClient->login(mUsername, mPassword);
            mRetry = 0;
            nextJob();
        }
        break;
    }
    case QFtp::Login : {
        if (error) {
            QMessageBox::about(NULL, tr("出错啦！"), tr("登录ftp服务器失败"));
            LOG(Logger::Debug, QString("FtpClient: failed to login(%1:%2)\n")
                        .arg(mUsername).arg(mPassword)
                        .toUtf8().constData());
            mError = true;
            return;
        } else
            LOG(Logger::Debug, "FtpClient: login ok\n");
            nextJob();
        break;
    }
    case QFtp::Get :
    case QFtp::Put : {
        TransferLock.lock();
        if (mCurrentJob != NULL) {
            QString text = tr("file transfer: ") + mCurrentJob->localFile->fileName();
            QString msg = tr("文件：") + mCurrentJob->localFile->fileName() + "传输";
            if (error) {
                text += tr(" failed");
                msg += tr("失败");
            } else {
                text += tr(" success");
                msg += tr("成功");
            }
            //暂时关闭，有卡死的bug. TODO:
            /*
            if (mCurrentJob->needNotice) {
                QMessageBox::about(NULL, tr("传输结果"), msg);
            }
            */
            LOG(Logger::Notice, "%s\n", text.toStdString().c_str());
            mCurrentJob->localFile->close();
            if (mFtpClient->currentCommand() == QFtp::Get && error) {
                mCurrentJob->localFile->remove();
                LOG(Logger::Notice, QString("QFtp::Get && error, removing file %1\n")
                            .arg(mCurrentJob->localFile->fileName())
                            .toUtf8().constData());
            }
            if (mCurrentJob->cmdAndArgs.size() > 0) {
                QStringList& cmdAndArgs = mCurrentJob->cmdAndArgs;
                QString cmd = cmdAndArgs.at(0);
                QStringList args;
                for (int i=1;i<cmdAndArgs.size();i++)
                    args.append(cmdAndArgs.at(i));
                Config::Instance()->cmdProcessor()->addJob(cmd, args);
            }
            delete mCurrentJob->localFile;
            delete mCurrentJob;
        }
        mCurrentJob = NULL;
        TransferLock.unlock();
        nextJob();
        break;
    }
    case QFtp::Cd :
        LOG(Logger::DDebug, "FTPClient::onFtpCommandFinished, case CD\n");
        break;
    case QFtp::Close :
        LOG(Logger::DDebug, "FTPClient::onFtpCommandFinished, case CLOSE\n");
        mFtpClient->connectToHost(mHost, mPort);
        break;
    default :
        LOG(Logger::DDebug, "FTPClient::onFtpCommandFinished, case DEFAULT\n");
        break;
    }
}

void FTPClient::nextJob()
{
    if (mError) {
        LOG(Logger::Debug, "FTPClient::addJob: mError = true, failed\n");
        return;
    }
    TransferLock.lock();
    if (mCurrentJob == NULL) {
        usleep(50*1000);
        if (mJobs.size() > 0) {
            mCurrentJob = mJobs[0];
            mJobs.remove(0);
            mFtpClient->cd(mCurrentJob->remoteDir);
            usleep(Config::Instance()->SafetyInterval);
            if (mCurrentJob->direction == Download) {
                mCurrentJob->localFile->open(QIODevice::WriteOnly);
                mFtpClient->get(mCurrentJob->remoteFile, mCurrentJob->localFile);
                LOG(Logger::Debug, QString("FTPClient::nextJob, getting remote(%1) to local(%1)\n")
                            .arg(mCurrentJob->remoteFile)
                            .arg(mCurrentJob->localFile->fileName())
                            .toUtf8().constData());
            } else {
                mFtpClient->put(mCurrentJob->localFile, _toSpecialEncoding(mCurrentJob->remoteFile));
                LOG(Logger::Debug, QString("FTPClient::nextJob, put local(%1) to remote(%2)\n")
                            .arg(mCurrentJob->localFile->fileName())
                            .arg(mCurrentJob->remoteFile)
                            .toUtf8().constData());
            }
            if (mCurrentJob->needRemoveRemote)
                mFtpClient->remove(mCurrentJob->remoteFile);
        }
    }
    TransferLock.unlock();
}

void FTPClient::addJob(Direction dir, QString localFile, QString remoteDir, QString remoteFile, QStringList cmdAndArgs, bool needRemoveRemote, bool needNotice)
{
    if (mError) {
        LOG(Logger::Debug, "FTPClient::addJob: mError = true, failed\n");
        return;
    }
    Job* job = new Job();
    job->direction = dir;
    job->localFile = new QFile(localFile);
    job->remoteDir = remoteDir;
    job->remoteFile = remoteFile;
    job->needNotice = needNotice;
    job->needRemoveRemote = needRemoveRemote;
    job->cmdAndArgs = cmdAndArgs;
    LOG(Logger::Debug, QString("FTPClient::addJob:\n%1\n").arg(job->asString()).toUtf8().constData());
    TransferLock.lock();
    mJobs.push_back(job);
    TransferLock.unlock();
    nextJob();
}

QString FTPClient::_Job::asString()
{
    QString result = "direction: ";
    switch (direction) {
    case Upload :
        result += "Upload";
        break;
    case Download :
        result += "Download";
        break;
    }
    result += "\nremoteDir: " + remoteDir + "\nremoteFile: " + remoteFile + "\n";
    foreach (QString arg, cmdAndArgs)
        result += arg + " ";
    result += "\n";
    return result;
}

QString FTPClient::_fromSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return  QString::fromLocal8Bit(InputStr.toLatin1());
#else
        QTextCodec *codec = QTextCodec::codecForName("gbk");
        if (codec)
        {
            return codec->toUnicode(InputStr.toLatin1());
        }
        else
        {
            return QString("");
        }
#endif
}

QString FTPClient::_toSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return QString::fromLatin1(InputStr.toLocal8Bit());
#else
    QTextCodec *codec= QTextCodec::codecForName("gbk");
    if (codec)
    {
        return QString::fromLatin1(codec->fromUnicode(InputStr));
    }
    else
    {
        return QString("");
    }
#endif
}
