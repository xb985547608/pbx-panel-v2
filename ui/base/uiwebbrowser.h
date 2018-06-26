#ifndef UIWEBBROWSER_H
#define UIWEBBROWSER_H

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif

class UiWebBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit UiWebBrowser(QString url, QWidget *parent);
    ~UiWebBrowser();
    void loadUrl(QString url);
    void showString(QString);
    
signals:
    
public slots:
private:
    QWidget* mCentralWidget;
};

#endif // UIWEBBROWSER_H
