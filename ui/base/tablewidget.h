#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif

class TableWidget : public QWidget
{
public:
    explicit TableWidget(int itemsPerLine, QWidget* parent = 0);
    int appendItem(QWidget* item, bool needRefresh = true);
    int insertItem(QWidget* item, int index = 0, bool needRefresh = true);
    void removeItem(int index, bool needRefresh = true);
    void removeItem(QWidget* item, bool needRefresh = true);
    void removeAllItems(bool destroy = false); //删除所有item，如果destroy为true，则也delete掉item
    void refresh();
    int itemCount() { return items.size(); }
    void setHeader(QWidget* header) { this->header = header; }
    void setFooter(QWidget* footer) { this->footer = footer; }

private:
    int itemsPerLine;
    QVector<QWidget*> items;
    QVBoxLayout* holder;
    QWidget* header;
    QWidget* footer;
};

#endif // TABLEWIDGET_H
