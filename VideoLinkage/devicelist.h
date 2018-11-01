#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QTreeView>
#include <QStyledItemDelegate>

class QMenu;
class QStandardItemModel;
class QStandardItem;

QT_BEGIN_NAMESPACE
// Note, this is exported but in a private header as qtopengl depends on it.
// We should consider adding this as a public helper function.
void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

enum { VideoState = Qt::UserRole + 1 };

class DeviceList : public QTreeView
{
    Q_OBJECT
public:
    explicit DeviceList(QWidget *parent = nullptr);
    ~DeviceList() override{ }

signals:
    void extenStateChanged(const QString &nubmer, const QString &url, bool isPlay);
    void closeAllSignal();

public slots:
    void refresh();
    void changeState(const QModelIndex &index);
    void changeState(const QString &number, const QStringList &urls);
    void changeState(const QString &number, bool state);
    void removeExten(const QString &number);

    void closeAll();

private:
    void initMenu();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QStandardItemModel *mpModel;
    QMenu *mpMenu;

    QMap<QString, QStandardItem *> mExtenNode;
};

class DeviceListDelegate : public QStyledItemDelegate {

public:
    explicit DeviceListDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    void drawIconWithShadow(QPainter *p,  const QRect &rect,
                            const QIcon &icon, QIcon::Mode iconMode, int dipRadius = 3,
                            const QColor &color = QColor(0,0,0,130),
                            const QPoint &dipOffset = QPoint(1, -2)) const;
    QPixmap disabledIcon(const QPixmap &enabledicon) const;

private:
    QIcon mStateIcon;
};

#endif // DEVICELIST_H
