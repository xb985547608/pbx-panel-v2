#include "multitablewidgets.h"
#include "misccomponents.h"

static QMutex sRefreshLock;

MultiTableWidgets::MultiTableWidgets(int itemsPerPage, int itemsPerLine, QWidget* parent, QWidget *textArea, ControlBtnPosition pos ,bool showSetType) :
    QWidget(parent)
{
    mItemsPerPage = itemsPerPage;
    mItemsPerLine = itemsPerLine;
    mHeader = NULL;
    mFooter = NULL;
    mShowBtns = true;
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mCurrentSelectedPage = 0;

    QBoxLayout* rootBox;
    if (pos == CtrlBtnBelowWidget)
        rootBox = new QVBoxLayout(this);
    else
        rootBox = new QHBoxLayout(this);
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    setLayout(rootBox);
    if (textArea != NULL && pos == CtrlBtnBesideWidget) {
        rootBox->addWidget(textArea);
    }

    mWidgetsHolder = new QVBoxLayout;
    mWidgetsHolder->setMargin(1);
    QGroupBox* groupbox = MiscComponents::getGroupBox(mWidgetsHolder, this);
    rootBox->addWidget(groupbox, 1);

    QBoxLayout* ctrlBtnBox;
    if (pos == CtrlBtnBelowWidget)
        ctrlBtnBox = new QHBoxLayout;
    else
        ctrlBtnBox = new QVBoxLayout;
    rootBox->addLayout(ctrlBtnBox);
    ctrlBtnBox->setMargin(0);
    //if (textArea != NULL && pos == CtrlBtnBelowWidget) {
    if (textArea != NULL) {
        if (pos == CtrlBtnBelowWidget) {
            ctrlBtnBox->addWidget(textArea);
            ctrlBtnBox->addStretch();
        }
    }
    mBtnNext = new QPushButton(tr("下一页"), this);
    //mBtnNext->setObjectName("normalBtn");
    mBtnPrev = new QPushButton(tr("上一页"), this);
    //mBtnPrev->setObjectName("normalBtn");

    if (showSetType)
    {
        mComBoxType = new QComboBox();
        mComBoxType->addItem("6x4");
        mComBoxType->addItem("6x5");
        mComBoxType->addItem("6x6");
        mComBoxType->addItem("6x7");
        ctrlBtnBox->addWidget(mComBoxType);
        connect(mComBoxType, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChange(int)));

    }
    ctrlBtnBox->addWidget(mBtnPrev);
    connect(mBtnPrev, SIGNAL(clicked()), this, SLOT(onBtnPrevClicked()));
    ctrlBtnBox->addWidget(mBtnNext);
    connect(mBtnNext, SIGNAL(clicked()), this, SLOT(onBtnNextClicked()));
    if (!mShowBtns) {
        mBtnPrev->setVisible(false);
        mBtnNext->setVisible(false);
    }
    switchPage();
    startTimer(1000);
}

void MultiTableWidgets::markTimedRefresh()
{
    sRefreshLock.lock();
    mTimedRefresh = true;
    sRefreshLock.unlock();
}

void MultiTableWidgets::timerEvent(QTimerEvent *)
{
    sRefreshLock.lock();
    if (mTimedRefresh)
        refresh();
    mTimedRefresh = false;
    sRefreshLock.unlock();
}

void MultiTableWidgets::onBtnNextClicked()
{
    mCurrentSelectedPage++;
    switchPage();
    showInfo();
}

void MultiTableWidgets::onBtnPrevClicked()
{
    mCurrentSelectedPage--;
    switchPage();
    showInfo();
}

void MultiTableWidgets::showInfo()
{
    return;
    TableWidget* currentPage = mPages[mCurrentSelectedPage];
    QMessageBox::about(this, "info", QString("total pages: %1, items in current page: %2").arg(mPages.size()).arg(currentPage->itemCount()));
}

void MultiTableWidgets::appendWidget(QWidget *item, bool needRefresh)
{
    mItems.push_back(item);
    if (needRefresh)
        refresh();
}

void MultiTableWidgets::switchPage()
{
    if (mCurrentSelectedPage >= mPages.size() - 1) {
        mCurrentSelectedPage = mPages.size() - 1;
        mBtnNext->setEnabled(false);
    } else if (mPages.size() > 1)
        mBtnNext->setEnabled(true);
    if (mCurrentSelectedPage <= 0) {
        mCurrentSelectedPage = 0;
        mBtnPrev->setEnabled(false);
    } else
        mBtnPrev->setEnabled(true);

    for (int i=0;i<mPages.size();i++) {
        TableWidget* widget = mPages[i];
        if (i == mCurrentSelectedPage) {
            widget->refresh();
            widget->show();
        }
        else
            widget->hide();
    }
}

void MultiTableWidgets::removeWidget(QWidget *item, bool needRefresh, bool del)
{
    if (item == NULL || !mItems.contains(item))
        return;
    for (int i=0;i<mItems.size();i++)
        if (item == mItems[i]) {
            mItems.remove(i);
            if (del)
                delete item;
            break;
        }
    if (needRefresh)
        refresh();
}

void MultiTableWidgets::clear(bool needRefresh, bool del)
{
    QVector<QWidget*> oldItems = mItems;
    mItems.clear();
    if (needRefresh)
        refresh();
    if (del)
        foreach (QWidget* item, oldItems)
            delete item;
}

void MultiTableWidgets::refresh()
{
    //清空原有widgets
    for (int i=0;i<mPages.size();i++) {
        TableWidget* page = mPages[i];
        mWidgetsHolder->removeWidget(page);
        page->close();
        //delete page; //有鬼，删除要报段错误，跟踪发现是delete QGridLayout的时候挂的
    }
    mPages.clear();

    //添加新widgets
    TableWidget* page = NULL;
    int i = 0;
    foreach(QWidget* item, mItems) {
        if (page == NULL) {
            page = new TableWidget(mItemsPerLine);
            page->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            page->setHeader(mHeader);
            page->setFooter(mFooter);
            mWidgetsHolder->addWidget(page);
            mPages.push_back(page);
        }
        page->appendItem(item, false);
        i++;
        if (i>=mItemsPerPage) {
            i = 0;
            page = NULL;
        }
    }
    switchPage();
}

void MultiTableWidgets::onTypeChange(int i)
{
     mItemsPerLine = i+4;
     mItemsPerPage = mItemsPerLine * 6;
    refresh();
}
