#ifndef DBWRAPPER_H
#define DBWRAPPER_H

#include <QObject>
#include <QtSql/QtSql>
#include <QtSql/QSqlError>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVector>

class DbWrapper : public QObject
{
public:
    DbWrapper(QString appPath, QString dbPath, QObject* parent = 0) :
        QObject(parent)
        { mAppPath = appPath; mDbPath = dbPath; }
    ~DbWrapper(){}

    bool openDb();
    bool deleteDb();
    bool initialise();
    QSqlError lastError();

    static int getVersion();
    static void setVersion(int version);

    static QString getSystemValue(QString key);
    static void setSystemValue(QString key, QString value);
    //Map<rowid, sms_content>
    static QMap<int, QString> getSmsTemplates();
    static void updateSmsTempate(int rowid, QString content);
private :
    QString mDbPath;
    QString mAppPath;
    static QSqlDatabase mDb;
};

#endif // DBWRAPPER_H
