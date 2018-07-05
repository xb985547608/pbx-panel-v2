#ifndef DLGEMERGENCYCALL_H
#define DLGEMERGENCYCALL_H

#include <QDialog>
#include <vector>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVector>

typedef struct _EmergencyCallInfo{
    QString m_strDisplayName;
    QString m_StrNumber;
    QString m_StrChannel;
    QString m_strUniqueid;
    QString m_strTime;
    QString m_strUuid;
}EmergencyCallInfo;

class DlgEmergencyCall : public QDialog
{
    Q_OBJECT
public:
    explicit DlgEmergencyCall(QWidget *parent = 0);

    void AddEmergencyCallInfo(EmergencyCallInfo info);
private:
    void PushButton();
    void DelButton(QString strUniqueid);

    virtual void closeEvent(QCloseEvent *);

private slots:
    void OnClick();

signals:

public slots:


private:
    QVBoxLayout * m_layout;
    QVBoxLayout *m_btnLayout;
    QVector<EmergencyCallInfo> m_verInfo;

};

#endif // DLGEMERGENCYCALL_H
