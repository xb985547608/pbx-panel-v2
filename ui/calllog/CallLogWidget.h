#ifndef CALLLOGWIDGET_H
#define CALLLOGWIDGET_H
#include "ui/base/BaseWidget.h"

class CallLogWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit CallLogWidget(MainWindow*, QWidget *parent = 0);
    void onResume(QMap<QString, QVariant> &);
    void onPause(QMap<QString, QVariant> &);

private slots:
    void onBtnQueryClicked();
    void onBtnEmergencyQueryClicked();
    void onBtnShowAllClicked();
    void onBtnSelectAllClicked();
    void onBtnSelectNoneClicked();
    void onBtnDeleteClicked();
    void onBtnExportClicked();
    void onCdrViewClicked(const QModelIndex& index);
    void onCdrViewDoubleClicked(const QModelIndex index);

protected:
    void initFrames();
private:
    QStandardItemModel* mCdrListModel;
    QDateTimeEdit* mStartDateTimeEdit;
    QDateTimeEdit* mEndDateTimeEdit;
    QDateTime mSelectedStartTime;
    QDateTime mSelectedEndTime;
    QList<QProcess*> mPlayers;

    void reloadCdrList(bool isEmergencyCall = false);
};

#endif // CALLLOGWIDGET_H
