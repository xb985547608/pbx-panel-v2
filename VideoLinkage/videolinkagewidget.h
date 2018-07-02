#ifndef VIDEOLINKAGEWIDGET_H
#define VIDEOLINKAGEWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>

#include "videowidget.h"
#include "settingsdialog.h"

class OperatorExten;

#define OPERATOR_RING_GROUNP  "2000"

class VideoLinkageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoLinkageWidget(QWidget *parent = nullptr);

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);

signals:

public slots:
    /* 变动布局 */
    void changeLayout(const QString &text);
    /* 对发出双击信号的视频窗口进行全屏处理 */
    void fullWidget();
    /* 寻找对应的分机信息并打开或关闭存在的url */
    void open(QStringList extensions);
    void close(QStringList extensions);

    void operatorExtenStateChanged(QString number);
    void extenStateChanged(QString number);

private:
    QWidget *mpVideoWidgetsZone;
    QGridLayout *mpGridLayout;
    QComboBox *mpLayoutSelectCombo;

    QList<VideoWidget *> mVideoWidgets;
    SettingsDialog *mpSettingsDialog;


    QStringList mAlreadyUseExten;    //已使用分机
    QMap<QString, QStringList> mActiveExtenNumber;  //主呼分机
    QStringList mPasvExtenNumber;    //被呼分机
};

#endif // VIDEOLINKAGEWIDGET_H
