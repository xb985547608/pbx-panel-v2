#ifndef MULTITABLEWIDGETS_H
#define MULTITABLEWIDGETS_H
/*
 * 多个TabWidget组成的界面
 * 创建时需指定每个TabWidget的元素数量、每行元素个数
 * 向其插入元素时，自动管理TabWidget的数量
 */
#include <QtGui>
#include "tablewidget.h"

/*
* CtrlBtnBelowWidget
+-----------------------+
|       header           |
+-----------------------+
|                       |
|                       |
|      content            |
|                       |
+-----------------------+
|       footer            |
+-----------------------+
| textArea   上一页 下一页  |
+-----------------------+

* CtrlBtnBesideWidget
+-----------------------+
|       header           |
+-----------------------+
| textArea |  content  | 上下|
+-----------------------+
|       footer            |
+-----------------------+
*/

class MultiTableWidgets : public QWidget
{
    Q_OBJECT
public:
    typedef enum _ControlBtnPosition { CtrlBtnBelowWidget, CtrlBtnBesideWidget } ControlBtnPosition;
    /*
        textArea位置见上图
    */
    explicit MultiTableWidgets(int itemsPerPage, int itemsPerLine, QWidget *parent = 0, QWidget *textArea = NULL, ControlBtnPosition pos = CtrlBtnBelowWidget,bool mShowSetType = false);
    void appendWidget(QWidget* , bool needRefresh = false);
    /*
     * @needRefresh: 移除item后是否刷新
     * @del: 是否delete掉被移除的widget
     */
    void removeWidget(QWidget*, bool needRefresh = false, bool del = false);
    /*
     * 清空MultiTableWidgets
     * @needRefresh: 清空后是否刷新
     * @del: 是否delete掉被移除的widget
     */
    void clear(bool needRefresh = false, bool del = false);
    void refresh();
    void setHeader(QWidget* header) { mHeader = header; }
    void setFooter(QWidget* footer) { mFooter = footer; }
    void markTimedRefresh();
    bool hasItem(QWidget* widget) { return mItems.contains(widget) ? true : false; }
    void showBtns(bool show = true) { mShowBtns = show; } //是否显示“上一页、下一页”按钮

    //void showSetType(bool show = false){mShowSetType = show; } //显示设置布局

    
signals:
    
public slots:
    void onBtnNextClicked();
    void onBtnPrevClicked();
    void onTypeChange(int i);

protected :
    void timerEvent(QTimerEvent *);

private:
    bool mShowBtns;
    int mItemsPerPage;
    int mItemsPerLine;
    QPushButton* mBtnNext;
    QPushButton* mBtnPrev;
    QVBoxLayout* mWidgetsHolder;
    QVector<QWidget*> mItems;
    QVector<TableWidget*> mPages;
    int mCurrentSelectedPage;
    QWidget* mHeader;
    QWidget* mFooter;

    //dlj 选择排版
    QComboBox *mComBoxType;
    //bool mShowSetType;//是否显示设置布局

    void switchPage();
    void showInfo();

    bool mTimedRefresh; //定时刷新标志，用于短时间内大量增删items时
};

#endif // MULTITABLEWIDGETS_H
