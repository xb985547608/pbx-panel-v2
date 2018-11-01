#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "devicelist.h"

#include <QApplication>
#include <QPainter>
#include <QPixmapCache>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QHeaderView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QSettings>
#include <QDebug>

DeviceList::DeviceList(QWidget *parent) :
    QTreeView(parent)
{
    mpModel = new QStandardItemModel(this);
    mpModel->setHorizontalHeaderLabels(QStringList() << "视频联动");
    setModel(mpModel);
    header()->setDefaultAlignment(Qt::AlignCenter);
    header()->setFrameStyle(QFrame::NoFrame);
    setFrameStyle(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setItemDelegate(new DeviceListDelegate(this));

    this->setStyleSheet(""
                        "QTreeView {"
                        "	border: 1px solid rgb(50, 50, 50);"
                        "	background-color: rgb(57, 58, 60);"
                        "   outline:none;"
                        "	show-decoration-selected: 1;"
                        "}"
                        "QTreeView::item {"
                        "	height: 25px;"
                        "	color: white;"
                        "	border: none;"
                        "	background-color: transparent;"
                        "}"
                        "QTreeView::item:hover, QTreeView::branch:hover { /** ???? **/"
                        "	background-color: rgba(255, 255, 255, 40);"
                        "}"
                        "QTreeView::item:selected, QTreeView::branch:selected { /** ???? **/"
                        "	background-color: rgb(0, 160, 230);"
                        "}"
                        "QTreeView::branch {"
                        "	background-color: transparent;"
                        "}"
                        "QTreeView::branch:open:has-children {"
                        "	image: url(:/images/branch_open.png);"
                        "}"
                        "QTreeView::branch:closed:has-children {"
                        "	image: url(:/images/branch_closed.png);"
                        "}"
                        "QTreeView::branch:has-siblings:!adjoins-item {"
                        "   border-image: url(:/images/vline.png);"
                        "}"
                        "QTreeView::branch:has-siblings:adjoins-item {"
                        "   border-image: url(:/images/branch-more.png);"
                        "}"
                        "QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
                        "   border-image: url(:/images/branch-end.png);"
                        "}"
                        "QTreeView::branch:closed:has-children {"
                        "   border-image: none;"
                        "   image: url(:/images/branch_closed.png);"
                        "}"
                        "QTreeView::branch:open:has-children {"
                        "   border-image: none;"
                        "   image: url(:/images/branch_open.png);"
                        "}"
                        "QHeaderView::section{"
                        "   color: white;"
                        "   height: 30px;"
                        "   background-color: rgb(57, 58, 60);"
                        "   border: 0px;"
                        "   border-bottom: 2px solid #605F5F"
                        "}"
                        "QScrollBar:vertical {"
                        "    background-color: #2A2929;"
                        "    width: 15px;"
                        "    margin: 15px 3px 15px 3px;"
                        "    border: 1px transparent #2A2929;"
                        "    border-radius: 4px;"
                        "}"
                        ""
                        "QScrollBar::handle:vertical {"
                        "    background-color: #605F5F;"
                        "    min-height: 5px;"
                        "    border-radius: 4px;"
                        "}"
                        ""
                        "QScrollBar::sub-line:vertical {"
                        "    margin: 3px 0px 3px 0px;"
                        "    border-image: url(:/images/up_arrow_disabled.png);"
                        "    height: 10px;"
                        "    width: 10px;"
                        "    subcontrol-position: top;"
                        "    subcontrol-origin: margin;"
                        "}"
                        ""
                        "QScrollBar::add-line:vertical {"
                        "    margin: 3px 0px 3px 0px;"
                        "    border-image: url(:/images/down_arrow_disabled.png);"
                        "    height: 10px;"
                        "    width: 10px;"
                        "    subcontrol-position: bottom;"
                        "    subcontrol-origin: margin;"
                        "}"
                        ""
                        "QScrollBar::sub-line:vertical:hover,"
                        "QScrollBar::sub-line:vertical:on {"
                        "    border-image: url(:/images/up_arrow.png);"
                        "    height: 10px;"
                        "    width: 10px;"
                        "    subcontrol-position: top;"
                        "    subcontrol-origin: margin;"
                        "}"
                        ""
                        "QScrollBar::add-line:vertical:hover,"
                        "QScrollBar::add-line:vertical:on {"
                        "    border-image: url(:/images/down_arrow.png);"
                        "    height: 10px;"
                        "    width: 10px;"
                        "    subcontrol-position: bottom;"
                        "    subcontrol-origin: margin;"
                        "}"
                        ""
                        "QScrollBar::up-arrow:vertical,"
                        "QScrollBar::down-arrow:vertical {"
                        "    background: none;"
                        "}"
                        ""
                        "QScrollBar::add-page:vertical,"
                        "QScrollBar::sub-page:vertical {"
                        "    background: none;"
                        "}"
                        "");

    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(changeState(const QModelIndex &)));

    initMenu();
    refresh();
}

void DeviceList::refresh()
{
//    mpModel->clear();
//    mpModel->setHorizontalHeaderLabels(QStringList() << "视频联动");

//    QSettings settings("settings.ini", QSettings::IniFormat);
//    settings.beginGroup("ExtensionURL");
//    foreach (QString num, settings.allKeys()) {
//        QStandardItem *extensionItem;
////        if (mExtenNode.keys().contains(num)){
////            extensionItem = mExtenNode.value(num);
////        } else {
//            extensionItem = new QStandardItem(num);
////            mExtenNode.insert(num, extensionItem);
////        }

////        if (extensionItem->rowCount())
////            extensionItem->removeRows(0, extensionItem->rowCount());

//        QStringList urls = settings.value(num).toStringList();
//        for (int i=0; i<urls.size(); i++) {
//            if (urls.at(i).isEmpty())
//                break;
//            QStandardItem *item = new QStandardItem(QString("视频%1").arg(i+1));
//            item->setData(false, VideoState);
//            item->setData(urls.at(i), Qt::ToolTipRole);
//            extensionItem->appendRow(item);
//            qDebug() << urls.at(i);
//        }
//        mpModel->appendRow(extensionItem);
//    }
//    settings.endGroup();
}

void DeviceList::changeState(const QModelIndex &index)
{
    /* 忽略根节点 */
    if (index.parent().isValid()) {
        QAbstractItemModel *model = const_cast<QAbstractItemModel *>(index.model());
        bool isOK = model->setData(index, !(model->data(index, VideoState).toBool()), VideoState);
        if (isOK) {
            emit extenStateChanged(index.parent().data().toString(),
                              model->data(index, Qt::ToolTipRole).toString(),
                              model->data(index, VideoState).toBool());
        }
    }
}

void DeviceList::changeState(const QString &number, const QStringList &urls)
{
    QStandardItem *item = mExtenNode.value(number, NULL);
    if (item == NULL) {
        item = new QStandardItem(number);
        mpModel->appendRow(item);
        mExtenNode.insert(number, item);
    }

    if (item->rowCount())
        item->removeRows(0, item->rowCount());

    for (int i=0; i<urls.size(); i++) {
        if (urls.at(i).isEmpty())
            continue;
        QStandardItem *child = new QStandardItem(QString("视频%1").arg(i+1));
        child->setData(false, VideoState);
        child->setData(urls.at(i), Qt::ToolTipRole);
        item->appendRow(child);
    }
}

void DeviceList::changeState(const QString &number, bool state)
{
    QStandardItem *item = mExtenNode.value(number, NULL);
    if (item == NULL)
        return;

    for (int i=0; i<item->rowCount(); i++) {
        item->child(i)->setData(state, VideoState);
    }
}

void DeviceList::removeExten(const QString &number)
{
    if (!mExtenNode.keys().contains(number))
        return;
    QStandardItem *item = mExtenNode.value(number);
    mpModel->removeRow(mpModel->indexFromItem(item).row());
    mExtenNode.remove(number);
}

void DeviceList::closeAll()
{
    foreach (QStandardItem *item, mExtenNode.values()) {
        for (int i=0; i<item->rowCount(); i++) {
            item->child(i)->setData(false, VideoState);
        }
    }

    emit closeAllSignal();
}

void DeviceList::initMenu()
{
    mpMenu = new QMenu(this);
    connect(mpMenu->addAction("关闭全部"), &QAction::triggered, this, &DeviceList::closeAll);
    connect(mpMenu->addAction("展开全部"), &QAction::triggered, this, &DeviceList::expandAll);
    connect(mpMenu->addAction("折叠全部"), &QAction::triggered, this, &DeviceList::collapseAll);
}

void DeviceList::contextMenuEvent(QContextMenuEvent *event)
{
    mpMenu->exec(viewport()->mapToGlobal(event->pos()));
}

DeviceListDelegate::DeviceListDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    mStateIcon(QIcon(":/images/on-line.png"))
{ }

void DeviceListDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    if (index.parent().isValid())
        drawIconWithShadow(painter, opt.rect.adjusted(opt.rect.width() - opt.rect.height(), 0, 0, 0),
                           mStateIcon, index.model()->data(index, VideoState).toBool() ?
                               QIcon::Normal : QIcon::Disabled);
}

void DeviceListDelegate::drawIconWithShadow(QPainter *p,  const QRect &rect,
                                            const QIcon &icon, QIcon::Mode iconMode, int dipRadius,
                                            const QColor &color, const QPoint &dipOffset) const
{
    QPixmap cache;
    const int devicePixelRatio = p->device()->devicePixelRatio();
    QString pixmapName = QString::fromLatin1("icon %0 %1 %2 %3")
            .arg(icon.cacheKey()).arg(iconMode).arg(rect.height()).arg(devicePixelRatio);

    if (!QPixmapCache::find(pixmapName, cache)) {
        // High-dpi support: The in parameters (rect, radius, offset) are in
        // device-independent pixels. The call to QIcon::pixmap() below might
        // return a high-dpi pixmap, which will in that case have a devicePixelRatio
        // different than 1. The shadow drawing caluculations are done in device
        // pixels.
        QWindow *window = dynamic_cast<QWidget*>(p->device())->window()->windowHandle();
        QPixmap px = icon.pixmap(window, rect.size(), iconMode);
        int radius = dipRadius * devicePixelRatio;
        QPoint offset = dipOffset * devicePixelRatio;
        cache = QPixmap(px.size() + QSize(radius * 2, radius * 2));
        cache.fill(Qt::transparent);

        QPainter cachePainter(&cache);
        if (iconMode == QIcon::Disabled) {
            const bool hasDisabledState =
                    icon.availableSizes().count() == icon.availableSizes(QIcon::Disabled).count();
            if (!hasDisabledState)
                px = disabledIcon(icon.pixmap(window, rect.size()));
        } else {
            // Draw shadow
            QImage tmp(px.size() + QSize(radius * 2, radius * 2 + 1), QImage::Format_ARGB32_Premultiplied);
            tmp.fill(Qt::transparent);

            QPainter tmpPainter(&tmp);
            tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
            tmpPainter.drawPixmap(QRect(radius, radius, px.width(), px.height()), px);
            tmpPainter.end();

            // blur the alpha channel
            QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
            blurred.fill(Qt::transparent);
            QPainter blurPainter(&blurred);
            qt_blurImage(&blurPainter, tmp, radius, false, true);
            blurPainter.end();

            tmp = blurred;

            // blacken the image...
            tmpPainter.begin(&tmp);
            tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            tmpPainter.fillRect(tmp.rect(), color);
            tmpPainter.end();

            tmpPainter.begin(&tmp);
            tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            tmpPainter.fillRect(tmp.rect(), color);
            tmpPainter.end();

            // draw the blurred drop shadow...
            cachePainter.drawImage(QRect(0, 0, cache.rect().width(), cache.rect().height()), tmp);
        }

        // Draw the actual pixmap...
        cachePainter.drawPixmap(QRect(QPoint(radius, radius) + offset, QSize(px.width(), px.height())), px);
        cachePainter.end();
        cache.setDevicePixelRatio(devicePixelRatio);
        QPixmapCache::insert(pixmapName, cache);
    }

    QRect targetRect = cache.rect();
    targetRect.setSize(targetRect.size() / cache.devicePixelRatio());
    targetRect.moveCenter(rect.center() - dipOffset);
    p->drawPixmap(targetRect, cache);
}

QPixmap DeviceListDelegate::disabledIcon(const QPixmap &enabledicon) const
{
    QImage im = enabledicon.toImage().convertToFormat(QImage::Format_ARGB32);
    for (int y=0; y<im.height(); ++y) {
        QRgb *scanLine = reinterpret_cast<QRgb*>(im.scanLine(y));
        for (int x=0; x<im.width(); ++x) {
            QRgb pixel = *scanLine;
            char intensity = char(qGray(pixel));
            *scanLine = qRgba(intensity, intensity, intensity, qAlpha(pixel));
            ++scanLine;
        }
    }
    return QPixmap::fromImage(im);
}
