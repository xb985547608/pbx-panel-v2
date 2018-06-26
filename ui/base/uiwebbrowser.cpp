#include "uiwebbrowser.h"
#include "misc/cwebpage.h"
#include "misc/Config.h"

UiWebBrowser::UiWebBrowser(QString url, QWidget *parent) :
    QWidget(parent)
{
    Config* cfg = Config::Instance();
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Dialog|Qt::WindowMaximizeButtonHint);
    setWindowTitle(cfg->Window_Title);
    this->resize(parent->size());
    mCentralWidget = this;
    EmbedBrowserObject((HWND)mCentralWidget->winId());
    DisplayHTMLPage((HWND)mCentralWidget->winId(), (TCHAR *)(url.toStdString().c_str()));
}

UiWebBrowser::~UiWebBrowser()
{
    UnEmbedBrowserObject((HWND)mCentralWidget->winId());
}

void UiWebBrowser::loadUrl(QString url)
{
    DisplayHTMLPage((HWND)mCentralWidget->winId(), (TCHAR*)(url.toStdString().c_str()));
}

void UiWebBrowser::showString(QString htmlString)
{
    DisplayHTMLStr((HWND)mCentralWidget->winId(), (TCHAR*)(htmlString.toStdString().c_str()));
}
