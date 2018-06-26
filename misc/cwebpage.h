#ifndef __CWEBPAGE_H_INCLUDED
#define __CWEBPAGE_H_INCLUDED

#include <windows.h>
#define DLLIMPORT __declspec (dllimport)

#ifdef __cplusplus
extern "C" {
#endif

long DLLIMPORT EmbedBrowserObject(HWND);
#define EMBEDBROWSEROBJECT EmbedBrowserObject
typedef long DLLIMPORT EmbedBrowserObjectPtr(HWND);

void DLLIMPORT UnEmbedBrowserObject(HWND);
#define UNEMBEDBROWSEROBJECT UnEmbedBrowserObject
typedef long DLLIMPORT UnEmbedBrowserObjectPtr(HWND);

long DLLIMPORT DisplayHTMLPage(HWND, LPCTSTR);
#define DISPLAYHTMLPAGE DisplayHTMLPage
typedef long DLLIMPORT DisplayHTMLPagePtr(HWND, LPCTSTR);

long DLLIMPORT DisplayHTMLStr(HWND, LPCTSTR);
#define DISPLAYHTMLSTR DisplayHTMLStr
typedef long DLLIMPORT DisplayHTMLStrPtr(HWND, LPCTSTR);

#ifdef __cplusplus
}
#endif

/*
http://www.qtcn.org/bbs/read-htm-tid-17571-displayMode-1.html
以前用BCB和C#的时候，都曾用到过WebBrowser这个ActiveX控件，在BCB里称为CppWebBrowser，使用它来嵌入到程序中，很不错的功能。

现在用Qt，想在程序中嵌入一个网页浏览器，有哪些方法呢？

Qt自带了一个QTextBrowser，但是它只能显示简单的网页，复杂的就不行了。
对于嵌入式的浏览器引擎，现在用得比较多的是分别基于IE, Firefox(Gecko), Opera, Konqueror(khtml)这四个引擎的，但现在使用最多的恐怕就属前两者了。

基于IE内核引擎的浏览器很多，比如Maxton, 腾迅TT等。
基于Gecko的也比较多，主要是Linux下的比较多，比如Galen, Epiphany

当 然，现在市面上还是基于IE资料比较多。但Gecko现在发展迅猛，并且源码公开，在Windows下的，它有各种嵌入组件，比如mfcEmbed, winEmbed, ActiveX Control等，在Linux下，有GtkMozEmbed等。那要写跨平台的Qt程序，使用浏览器嵌入组件该怎么选择呢？
花了一整天的时间来查找Embed Gecko方面的资料和试验，发现除了Linux下GtkMozEmbed发展比较好，windows下则比较少，并且windows下要想编译Gecko，最好是使用VC来编译，这就费劲了。

现在方案出来了，可在Windows下使用WebBrowser ActiveX控件, 在Linux下使用GtkMozEmbed模块。
GtkMozEmbed 就不用说了，包装得比较方便使用，在Qt中用它的和，解决了Qt和Gtk如何混合编程就可以了。在Windows下要使用WebBrowser这个 ActiveX控件，则比较麻烦，因为只有商业版的Qt中才提供ActiveQt模块以支持ActiveX/COM等的开发。

于是继续找资料，找到了一个国外的用C语言写的一个使用WebBrowser的包装，很是不错，可以到下面的网址去看资料：
http://www.codeguru.com/cpp/i-n/ieprogram/article.php/c4379/

从下面的网址把他的程序源码下载下来：
http://www.codeguru.com/code/legacy/ieprogram/cwebpage.zip
解压后里面有下面我们需要的文件（原作者是用VC编译的）
cwebpage.dll     动态库，可直接拿来用
cwebpage.h     头文件
dll.def           库导出文件，我们要使用这个生成mingw编译需要的.a文件

第一步，生成.a文件：
Quote:

dlltool -k -d dll.def -l libcwebpage.a

现在我们得到了Qt程序要使用的三个文件：cwebpage.dll, cwebpage.h, libcwebpage.a

接着，修改Qt程序，在.pro工程文件中加上链接库：
Quote:

LIBS += -Lyour_libcwebpage_dir -lcwebpage


接着，在Qt程序中加入头文件：
Quote:

#include "cwebpage.h"

下面是头文件内容：
Quote:

/ *
* This include file is meant to be included with any C source you
* write which uses the cwebpage DLL.
* /

#ifndef __CWEBPAGE_H_INCLUDED
#define __CWEBPAGE_H_INCLUDED

#include <windows.h>

#ifdef __cplusplus
{
#endif

long WINAPI EmbedBrowserObject(HWND);
#define EMBEDBROWSEROBJECT EmbedBrowserObject
typedef long WINAPI EmbedBrowserObjectPtr(HWND);

void WINAPI UnEmbedBrowserObject(HWND);
#define UNEMBEDBROWSEROBJECT UnEmbedBrowserObject
typedef long WINAPI UnEmbedBrowserObjectPtr(HWND);

long WINAPI DisplayHTMLPage(HWND, LPCTSTR);
#define DISPLAYHTMLPAGE DisplayHTMLPage
typedef long WINAPI DisplayHTMLPagePtr(HWND, LPCTSTR);

long WINAPI DisplayHTMLStr(HWND, LPCTSTR);
#define DISPLAYHTMLSTR DisplayHTMLStr
typedef long WINAPI DisplayHTMLStrPtr(HWND, LPCTSTR);

#ifdef __cplusplus
}
#endif
#endif / * __CWEBPAGE_H_INCLUDED * /


我们看到，这个头文件中声明了四个函数：
long WINAPI EmbedBrowserObject(HWND);
void WINAPI UnEmbedBrowserObject(HWND);
long WINAPI DisplayHTMLPage(HWND, LPCTSTR);
long WINAPI DisplayHTMLStr(HWND, LPCTSTR);
参考作者提供的源代码里的程序，可知，EmbedBrowserObject用于初始化，UnEmbedBrowserObject用于卸载，DisplayHTMLPage和DisplayHTMLStr分别用于显示网址和字符串内容。

有了这些，在Qt程序中就好用了：
新建一个widget的时候：
Quote:

QWidget *widget = new QWidget(this);
EmbedBrowserObject(widget->winId());

显示内容的时候：
Quote:

DisplayHTMLPage(widget->winId(), (TCHAR *)(QString("http://www.qtcn.org").toStdString().c_str()));

或者显示字符串：
Quote:

DisplayHTMLStr(widget->winId(), (TCHAR *)(QString("<h1>你好世界</h1>").toStdString().c_str()));

删除这个widget前的时候：
Quote:

UnEmbedBrowserObject(widget->winId());
delete widget;


为了方便使用，用户可从QWidget继承一个类，把这四个函数的操作封装进去。


下面就可以开始编译了，但是，链接时会出现错误，主要问题在于库的不同而不能正确识别符号：
只要把头文件的声明头改下就可以为mingw所用了（当然以前那个VC可直接使用的）
Quote:

/ *
* This include file is meant to be included with any C source you
* write which uses the cwebpage DLL.
* /

#ifndef __CWEBPAGE_H_INCLUDED
#define __CWEBPAGE_H_INCLUDED

#include <windows.h>
#define DLLIMPORT __declspec (dllimport)

#ifdef __cplusplus
extern "C" {
#endif

long DLLIMPORT EmbedBrowserObject(HWND);
#define EMBEDBROWSEROBJECT EmbedBrowserObject
typedef long DLLIMPORT EmbedBrowserObjectPtr(HWND);

void DLLIMPORT UnEmbedBrowserObject(HWND);
#define UNEMBEDBROWSEROBJECT UnEmbedBrowserObject
typedef long DLLIMPORT UnEmbedBrowserObjectPtr(HWND);

long DLLIMPORT DisplayHTMLPage(HWND, LPCTSTR);
#define DISPLAYHTMLPAGE DisplayHTMLPage
typedef long DLLIMPORT DisplayHTMLPagePtr(HWND, LPCTSTR);

long DLLIMPORT DisplayHTMLStr(HWND, LPCTSTR);
#define DISPLAYHTMLSTR DisplayHTMLStr
typedef long DLLIMPORT DisplayHTMLStrPtr(HWND, LPCTSTR);

#ifdef __cplusplus
}
#endif
#endif / * __CWEBPAGE_H_INCLUDED * /

关于为什么要这样改，各位可查找一下不同开发工具所做的DLL库之间的不同方面的资料。
*/

#endif /* __CWEBPAGE_H_INCLUDED */
