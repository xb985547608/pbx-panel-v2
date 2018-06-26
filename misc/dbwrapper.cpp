#include "dbwrapper.h"
#include <QtGui>
#include <QString>
#include <QFile>
#include "misc/logger.h"
#include "config.h"

QSqlDatabase DbWrapper::mDb;// = QSqlDatabase::addDatabase("QSQLITE");

bool DbWrapper::openDb()
{
#ifdef QT_NO_DEBUG
    QString driverPath = mAppPath + "/qsqlite.dll";
#else
    QString driverPath = mAppPath + "/qsqlited5.dll";
#endif

    QPluginLoader loader(driverPath);
    QObject *plugin = loader.instance();
    if (!plugin) {
        LOG(Logger::Error, "plugin instance failed\n");
        return false;
    }
    QSqlDriverPlugin* sqlPlugin = qobject_cast<QSqlDriverPlugin*>(plugin);

    if (!sqlPlugin) {
        LOG(Logger::Error, "sql plugin load failed\n");
        return false;
    }

    mDb = QSqlDatabase::addDatabase(sqlPlugin->create("QSQLITE"));

#ifdef Q_OS_LINUX
    QString path(QDir::home().path());
    path.append(QDir::separator());
    path = QDir::toNativeSeparators(path);
    mDb.setDatabaseName(path);
#else
    // NOTE: File exists in the application private folder, in Symbian Qt implementation
    mDb.setDatabaseName(mDbPath);
#endif
    // Open databasee
    return mDb.open();
}

QSqlError DbWrapper::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return mDb.lastError();
}

bool DbWrapper::deleteDb()
{
    // Close database
    mDb.close();
#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append(mDbPath);
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
#else
    // Remove created database binary file
    return QFile::remove(mDbPath);
#endif
}

bool DbWrapper::initialise()
{
    do {
        QSqlQuery query(mDb);
        //系统设置
        QString sql = "drop table if exists system";
        query.exec(sql);
        sql = "create table system (s_key varchar not null, s_value varchar not null)";
        if (!query.exec(sql)) break;
        sql = "insert into system (s_key, s_value) values ('version', '" + QString("%1").arg(Config::Instance()->VERSION) + "')";
        if (!query.exec(sql)) break;
        sql = "drop table if exists sms_templates";
        if (!query.exec(sql)) break;
        sql = "create table sms_templates (content text)";
        if (!query.exec(sql)) break;
        for (int i=0;i<5;i++) {
            sql = QString("insert into sms_templates (content) values ('短信模板：%1')").arg(i);
            query.exec(sql);
        }
        return true;
    } while (false);
    LOG(Logger::Error, "Database initialise failed.\n");
    return false;
}

int DbWrapper::getVersion()
{
    QString result = "0";
    QString sql = "select s_value from system where s_key = 'version'";
    QSqlQuery query(mDb);
    do {
        if(!query.exec(sql)) break;
        if (query.next())
            result = query.value(0).toString();
    } while (false);
    return result.toInt();
}

void DbWrapper::setVersion(int version)
{
    QString verStr = QString("%1").arg(version);
    QSqlQuery query(mDb);
    QString sql = "update system set s_value = '" + verStr + "' where s_key = 'version'";
    if (!query.exec(sql)) {
        sql = "delete from system where s_key = 'version'";
        query.exec(sql);
        sql = "insert into system (s_key, s_value) values ('version', '" + verStr + "')";
        query.exec(sql);
    }
}

QString DbWrapper::getSystemValue(QString key)
{
    QString result;
    do {
        QString sql = "select s_value from system where s_key = '" + key + "'";
        QSqlQuery query(mDb);
        if (!query.exec(sql))
            break;
        if (query.next())
            result = query.value(0).toString();
    } while (false);
    return result;
}

void DbWrapper::setSystemValue(QString key, QString value)
{
    QSqlQuery query(mDb);
    QString sql = "delete from system where s_key = '" + key + "'";
    query.exec(sql);
    sql = "insert into system (s_key, s_value) values ('" + key + "', '" + value + "')";
    query.exec(sql);
}

QMap<int, QString> DbWrapper::getSmsTemplates()
{
    QMap<int, QString> results;
    QSqlQuery query(mDb);
    QString sql = "select rowid, content from sms_templates";
    do {
        if (!query.exec(sql))
            break;
        while (query.next()) {
            int rowid = query.value(0).toInt();
            QString content = query.value(1).toString();
            results.insert(rowid, content);
        }
    } while (false);
    return results;
}

void DbWrapper::updateSmsTempate(int rowid, QString content)
{
    QString sql = QString("update sms_templates set content = '%1' where rowid = %2").arg(content).arg(rowid);
    QSqlQuery query(mDb);
    query.exec(sql);
}

