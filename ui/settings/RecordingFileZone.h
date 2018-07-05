#ifndef RECORDINGFILEZONE_H
#define RECORDINGFILEZONE_H
#include "misc/ftpclient.h"

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif

class RecordingFileZone : public QWidget
{
    Q_OBJECT
public:
    explicit RecordingFileZone(QWidget *parent = 0);
    
signals:
    
public slots:
    void onFileListViewClicked(const QModelIndex&);
    void onBtnUploadClicked();
    void onBtnDeleteClicked();
private slots:
    void onFtpCommand();
private:
    void reloadList();
private:
    QStandardItemModel* mRecordingFileListModel;
    QTreeView* getFileListTree();
    QFtp* mFtpClient;
};

#endif // RECORDINGFILEZONE_H
