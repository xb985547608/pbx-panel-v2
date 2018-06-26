#ifndef DESCRIPTIONZONE_H
#define DESCRIPTIONZONE_H

#include <QWidget>
#include "EventWidget.h"

class DescriptionZone : public QWidget
{
    Q_OBJECT
public:
    explicit DescriptionZone(EventWidget*, QWidget *parent = 0);
    void setTitleTxt(const QString text) { mTitleText->setText(text); }
    QString getTitleTxt() { return mTitleText->text(); }
    void setText(QString text) { mEditContent->setPlainText(text); }
    void setRecorder(QString recorder) { mEditRecorder->setText(recorder); }
    void setDateTimeLabel(QString timeStr) { mLbDateTime->setText(timeStr); }
    QString getText() { return mEditContent->toPlainText(); }
    QString getRecorder() { return mEditRecorder->text(); }
    void enableSaveBtn(bool enable) { mBtnSave->setEnabled(enable); }
    
signals:
    
public slots:
    void onBtnSaveClicked();
protected:
    void timerEvent(QTimerEvent *);
private:
    EventWidget* mEventWidget;
    QLineEdit* mTitleText;
    QLabel* mLbDateTime;
    QLineEdit* mEditRecorder;
    QPlainTextEdit* mEditContent;
    QPushButton* mBtnSave;
};

#endif // DESCRIPTIONZONE_H
