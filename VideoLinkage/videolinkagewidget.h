#ifndef VIDEOLINKAGEWIDGET_H
#define VIDEOLINKAGEWIDGET_H

#include <QWidget>

#include "videowidget.h"
#include "settingsdialog.h"
#include "devicelist.h"

#define VIDEOWIDGET_MAX_LENGTH 16

class QComboBox;
class QGridLayout;
class OperatorExten;

class VideoLinkageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoLinkageWidget(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void extenStateChangedSignal(QString e, bool state);

public slots:
    /* 变动布局 */
    void changeLayout(const QString &text);
    /* 对发出双击信号的视频窗口进行全屏处理 */
    void fullScreen();
    /* 寻找对应的分机信息并打开或关闭存在的url */
    void open(const QMap<QString, QStringList> &extensions);
    void close(const QMap<QString, QStringList> &extensions);
    void closeAll();

    void extenStateChanged(const QString &numer, const QString &url, bool isPlay);
    void extenStreamChanged(QString e, QStringList urls);
    void extenStreamRemoved(QString e);

    void operatorExtensChanged(QStringList extens);
    void operatorExtenStateChanged(QString number);
    void extenStateChanged(QString number);
    void callExtens(QStringList extens);

private:
    QWidget *mpVideoWidgetsZone;
    DeviceList *mpDeviceList;
    QGridLayout *mpGridLayout;
    QComboBox *mpLayoutSelectCombo;

    QList<VideoWidget *> mVideoWidgets;
    QList<QPair<QString, QString>> mWaitPlayList;
    SettingsDialog *mpSettingsDialog;
    bool mFullScreen;


    QStringList mPasvExtenNumber;    //被呼分机
    QStringList mBroadcastExtenNumber;    //广播分机
    /* ExtensionNumer    URL   on/off */
    QMap<QString, QMap<QString, bool>> mExtenStreamStatus;
    /* operatorExten  extens */
    QStringList mOperatorCallExtens;
    QStringList mOperatorExtens;

    QString mOperatorGroupChannel;
};

#endif // VIDEOLINKAGEWIDGET_H
