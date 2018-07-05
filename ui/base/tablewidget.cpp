#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "tablewidget.h"

TableWidget::TableWidget(int itemsPerLine, QWidget *parent) :
    QWidget(parent),
    itemsPerLine(itemsPerLine)
{
    holder = new QVBoxLayout(this);
    holder->setMargin(0);
    this->setLayout(holder);
    //this->setAttribute(Qt::WA_DeleteOnClose);
    header = NULL;
    footer = NULL;
}

int TableWidget::appendItem(QWidget *item, bool needRefresh)
{
    items.push_back(item);
    int pos = items.size() - 1;
    if (needRefresh)
        refresh();
    return pos;
}

int TableWidget::insertItem(QWidget *item, int index, bool needRefresh)
{
    int pos = index;
    if (pos >= items.size()) {
        items.push_back(item);
        pos = items.size() - 1;
    } else
        items.insert(pos, item);
    if (needRefresh)
        refresh();
    return pos;
}

void TableWidget::removeItem(int index, bool needRefresh)
{
    if (index >= items.size())
        return;
    items.remove(index);
    if (needRefresh)
        refresh();
    return;
}

void TableWidget::removeItem(QWidget *item, bool needRefresh)
{
    if (item == NULL || !items.contains(item))
        return;
    for (int i=0;i<items.size();i++)
        if (item == items[i]) {
            items.remove(i);
            break;
        }
    if (needRefresh)
        refresh();
}

void TableWidget::removeAllItems(bool destroy)
{
    if (destroy)
        foreach (QWidget* item, items)
            delete item;
    items.clear();
}

void TableWidget::refresh()
{
    delete holder;
    holder = new QVBoxLayout(this);
    holder->setMargin(0);
    holder->setSpacing(0);
    if (header != NULL)
        holder->addWidget(header);
    QGridLayout* line = new QGridLayout;
    for (int i=0;i<itemsPerLine;i++)
        line->setColumnStretch(i, 1);
    holder->addLayout(line);
    for (int i=0;i<items.size();i++) {
        QWidget* item = NULL;
        item = items[i];
        int row = (int)(i / itemsPerLine);
        line->addWidget(item, row, i % itemsPerLine, 1, 1);
    }
    if (footer != NULL)
        holder->addWidget(footer);
    holder->addStretch();
}
