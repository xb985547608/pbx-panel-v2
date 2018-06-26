#ifndef MISCCOMPONENTS_H
#define MISCCOMPONENTS_H

#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QBoxLayout>
#include <QVariant>

class MiscComponents
{
public:
    MiscComponents();
    static QWidget* getHeader(QWidget* parent = NULL);
    static QWidget* gettitle(QColor backgroundColor, QString imgRes, QString text, QWidget* parent = NULL);
    static QWidget* gettitle(QColor backgroundColor, QString imgRes, QString text, QWidget *backBtn = 0, QWidget *parent = NULL);
    static void setToolButton(QToolButton* btn, QString imgRes, QString text, Qt::ToolButtonStyle toolButtonStyle = Qt::ToolButtonTextUnderIcon, QString styleSheet = "");
    static void setToolButton(QToolButton* btn, QString imgRes, QString text, QSize, Qt::ToolButtonStyle toolButtonStyle = Qt::ToolButtonTextUnderIcon, QString styleSheet = "");
    static void setPushButton(QPushButton* btn, QString imgRes, QString text, QString styleSheet = "");
    static void setPushButton(QPushButton* btn, QString imgRes, QString text, QSize, QString styleSheet = "");
    static void setTextToHeader(QLabel *header, QString text = "");
    static QWidget* getSectionLabel(QString text, QWidget* parent = NULL);
    static QWidget* getSectionLabel(QString text, QString imgRes, QWidget* parent = NULL);
    /*
     * 为layout产生一个边框
     */
    static QGroupBox* getGroupBox(QBoxLayout* layout, QWidget* parent = NULL);
};

class UserData : public QObjectUserData
{
public:
    UserData(QVariant data) :
        mData(data)
    {}
    ~UserData();
    QVariant getData() { return mData; }
private:
    QVariant mData;
};

#endif // MISCCOMPONENTS_H
