#include <QtGui>
#include "ui/MainWindow.h"
#include "misc/logger.h"
#include "misc/dbwrapper.h"
#include "message_queue/MessageClient.h"
#include "message_queue/RPCCommand.h"
#include "misc/Config.h"
#include <winsock2.h>

static bool initialiseLogger()
{
    Logger* logger = Logger::Instance();
#ifndef QT_NO_DEBUG
    if (!logger->initialise(Logger::Debug, "panel.log")) {
#else
    if (!logger->initialise(Logger::Debug)) {
#endif
        QMessageBox::about(NULL, "failed", "logger initialise failed");
        fprintf(stderr, "logger initialise failed");
        return false;
    }
    return true;
}

static bool initialiseConfig(QApplication& a)
{
    Config* cfg = Config::Instance();
    if (!cfg->initialise(a.applicationDirPath())) {
        LOG(Logger::Error, "config initialise failed\n");
        QMessageBox::about(NULL, "failed", "configuration initialise failed");
        return false;
    }
    return true;
}

static bool initialiasMessageClient()
{
    Config* cfg = Config::Instance();
    MessageClient* mc = MessageClient::Instance();
    //if (!mc->initialise("114.66.192.218", "0.0.0.1", "topic_logs")) {
    if (!mc->initialise(cfg->mq_host, cfg->mq_eventBindingkey, cfg->mq_eventExchange,
                        cfg->mq_rpcRoutingkey, cfg->mq_rpcExchange, cfg->mq_port, cfg->mq_user, cfg->mq_passwd)) {
        LOG(Logger::Error, "message client initialise failed(%s@%s to %s)\n",
            cfg->mq_user.toStdString().c_str(), cfg->mq_passwd.toStdString().c_str(),
            cfg->mq_host.toStdString().c_str());
        QMessageBox::about(NULL, "failed", "message client initialise failed");
        return false;
    }
    return true;
}

static bool checkSingleton(QApplication& a)
{
#ifndef QT_NO_DEBUG
    {
        QStringList args = a.arguments();
        foreach (QString arg, args) {
            QMessageBox::about(NULL, "args", arg);
        }
    }
#endif
    QStringList args = a.arguments();
    if (args.contains(RESTART_FLAG)) {
        QMessageBox::StandardButton btn = QMessageBox::warning(NULL, "与pbx的通讯出现故障", "请确认网络连接是否正常\n点击Yes重新连接，No退出",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (btn == QMessageBox::No)
            return false;
    } else {
        HANDLE hMutex = CreateMutex(NULL, false, QString("['{EFEB2EF6-F8E0-AE44-BABE-1BBEF2C7FD56}']").toStdWString().c_str());
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CloseHandle(hMutex);
            QMessageBox::information(NULL, "警告", "程序已在运行中，不能重复启动", QMessageBox::Ok);
            return false;
        }
    }
    return true;
}

bool initialiseLocalDB(QApplication& a)
{
    /*
    *连接数据库
    */
    Config* cfg = Config::Instance();
    DbWrapper dbWrapper(a.applicationDirPath(), "data.mdb");
    if (!dbWrapper.openDb()) {
        LOG(Logger::Error, "database open failed: %s\n", dbWrapper.lastError().text().toStdString().c_str());
        QMessageBox::about(NULL, "failed", "database open failed");
        return false;
    }
    do {
        int versionInDb = dbWrapper.getVersion();
        int version = cfg->VERSION;
        if (versionInDb >= version)
            break;
        dbWrapper.setVersion(version);
        //此处处理升级
        if (versionInDb >= 30)
            return dbWrapper.initialise();
    } while (false);
    return true;
}

int main(int argc, char *argv[])
{
    int ret = 1;

#ifdef V4
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif
    QApplication a(argc, argv);

    QTranslator translator;
    bool b = false;
    b = translator.load(a.applicationDirPath()+"/en.qm");
    a.installTranslator(&translator);


    //设置全局stylesheet
    QFile style("stylesheet.qss");
    if (style.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a.setStyleSheet(style.readAll());
        style.close();
    }

    if (!checkSingleton(a))
        return ret;
    if (!initialiseLogger())
        return ret;
    if (!initialiseConfig(a)) {
        delete Logger::Instance();
        return ret;
    }
    if (!initialiseLocalDB(a)) {
        delete Logger::Instance();
        delete Config::Instance();
        return ret;
    }
    if (!initialiasMessageClient()) {
        delete Logger::Instance();
        delete Config::Instance();
        return ret;
    }

    MainWindow w(a.arguments());
    w.show();
    ret = a.exec();
    /*
    delete MessageClient::Instance();
    delete Config::Instance();
    delete Logger::Instance();
    */
    MessageClient::Instance()->getEventMonitor()->stop();
    return ret;
}
