#ifndef UIPHONEBOOK_H
#define UIPHONEBOOK_H

#include "ui/base/BaseWidget.h"

#include <QtGui>

class PhonebookUserData : public QObjectUserData
{
public:
    PhonebookUserData(QMap<QString, QVariant> data) :
        mData(data)
    {}
    QMap<QString, QVariant> getData() { return mData; }
private:
    QMap<QString, QVariant> mData;
};

class UiPhoneBookList;
class UiPhoneBook : public BaseWidget
{
    Q_OBJECT
public:
    explicit UiPhoneBook(
            QString title,
            QString titleImage,
            QColor titleBgColor,
            Activity activityBack,
            MainWindow* mainWindow,
            QWidget *parent = 0);
    explicit UiPhoneBook(
            MainWindow* mainWindow,
            QWidget *parent = 0);
    virtual void onResume(QMap<QString, QVariant> &params);
    UiPhoneBookList* getPhonbookList() { return mUiPhoneBookList; }
    
signals:

protected:
    virtual void initFrames();
    
public slots:
    void save();
    void call();
    //void onGemergencyCall();
private:
    QString mRinggroupNumber;
    QString mRinggroupName;
    UiPhoneBookList* mUiPhoneBookList;
    QPushButton* mBtnSave;
    QPushButton* mBtnCall;
    //QPushButton *mBtnGemergencyCall;
};

#endif // UIPHONEBOOK_H
