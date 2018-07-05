#ifndef PLAYSOUNDWIDGET_H
#define PLAYSOUNDWIDGET_H

#include <QDialog>
#include <QStandardItemModel>
#include <QLabel>
#include <QCheckBox>

class PlaySoundWidget : public QDialog
{
    Q_OBJECT
public:
    //showFileList，是否显示左侧录音文件列表
    explicit PlaySoundWidget(QWidget *parent = 0, bool showFileList = true);
    int getLoopCount();
    QString getSoundFile();
signals:
    
private slots:
    void onFileListClicked(const QModelIndex &index);
    void onBtnOkClicked();
    void onBtnCancelClicked();
    void onBtnIncreaseClicked();
    void onBtnDecreaseClicked();
private:
    QWidget* getLeftFrame();
    QWidget* getRightFrame();
    void loadFiles();
    QStandardItemModel* mFileListModel; //录音文件model
    int mCycleCount;
    QLabel* mLbCycleCount;
    void setLabelCycleCount();
    QCheckBox* mCkForever;
    bool mShowFileList;
};

#endif // PLAYSOUNDWIDGET_H
