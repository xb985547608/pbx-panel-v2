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
��ǰ��BCB��C#��ʱ�򣬶����õ���WebBrowser���ActiveX�ؼ�����BCB���ΪCppWebBrowser��ʹ������Ƕ�뵽�����У��ܲ���Ĺ��ܡ�

������Qt�����ڳ�����Ƕ��һ����ҳ�����������Щ�����أ�

Qt�Դ���һ��QTextBrowser��������ֻ����ʾ�򵥵���ҳ�����ӵľͲ����ˡ�
����Ƕ��ʽ����������棬�����õñȽ϶���Ƿֱ����IE, Firefox(Gecko), Opera, Konqueror(khtml)���ĸ�����ģ�������ʹ�����Ŀ��¾���ǰ�����ˡ�

����IE�ں������������ܶ࣬����Maxton, ��ѸTT�ȡ�
����Gecko��Ҳ�Ƚ϶࣬��Ҫ��Linux�µıȽ϶࣬����Galen, Epiphany

�� Ȼ�����������ϻ��ǻ���IE���ϱȽ϶ࡣ��Gecko���ڷ�չѸ�ͣ�����Դ�빫������Windows�µģ����и���Ƕ�����������mfcEmbed, winEmbed, ActiveX Control�ȣ���Linux�£���GtkMozEmbed�ȡ���Ҫд��ƽ̨��Qt����ʹ�������Ƕ���������ôѡ���أ�
����һ�����ʱ��������Embed Gecko��������Ϻ����飬���ֳ���Linux��GtkMozEmbed��չ�ȽϺã�windows����Ƚ��٣�����windows��Ҫ�����Gecko�������ʹ��VC�����룬��ͷѾ��ˡ�

���ڷ��������ˣ�����Windows��ʹ��WebBrowser ActiveX�ؼ�, ��Linux��ʹ��GtkMozEmbedģ�顣
GtkMozEmbed �Ͳ���˵�ˣ���װ�ñȽϷ���ʹ�ã���Qt�������ĺͣ������Qt��Gtk��λ�ϱ�̾Ϳ����ˡ���Windows��Ҫʹ��WebBrowser��� ActiveX�ؼ�����Ƚ��鷳����Ϊֻ����ҵ���Qt�в��ṩActiveQtģ����֧��ActiveX/COM�ȵĿ�����

���Ǽ��������ϣ��ҵ���һ���������C����д��һ��ʹ��WebBrowser�İ�װ�����ǲ������Ե��������ַȥ�����ϣ�
http://www.codeguru.com/cpp/i-n/ieprogram/article.php/c4379/

���������ַ�����ĳ���Դ������������
http://www.codeguru.com/code/legacy/ieprogram/cwebpage.zip
��ѹ������������������Ҫ���ļ���ԭ��������VC����ģ�
cwebpage.dll     ��̬�⣬��ֱ��������
cwebpage.h     ͷ�ļ�
dll.def           �⵼���ļ�������Ҫʹ���������mingw������Ҫ��.a�ļ�

��һ��������.a�ļ���
Quote:

dlltool -k -d dll.def -l libcwebpage.a

�������ǵõ���Qt����Ҫʹ�õ������ļ���cwebpage.dll, cwebpage.h, libcwebpage.a

���ţ��޸�Qt������.pro�����ļ��м������ӿ⣺
Quote:

LIBS += -Lyour_libcwebpage_dir -lcwebpage


���ţ���Qt�����м���ͷ�ļ���
Quote:

#include "cwebpage.h"

������ͷ�ļ����ݣ�
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


���ǿ��������ͷ�ļ����������ĸ�������
long WINAPI EmbedBrowserObject(HWND);
void WINAPI UnEmbedBrowserObject(HWND);
long WINAPI DisplayHTMLPage(HWND, LPCTSTR);
long WINAPI DisplayHTMLStr(HWND, LPCTSTR);
�ο������ṩ��Դ������ĳ��򣬿�֪��EmbedBrowserObject���ڳ�ʼ����UnEmbedBrowserObject����ж�أ�DisplayHTMLPage��DisplayHTMLStr�ֱ�������ʾ��ַ���ַ������ݡ�

������Щ����Qt�����оͺ����ˣ�
�½�һ��widget��ʱ��
Quote:

QWidget *widget = new QWidget(this);
EmbedBrowserObject(widget->winId());

��ʾ���ݵ�ʱ��
Quote:

DisplayHTMLPage(widget->winId(), (TCHAR *)(QString("http://www.qtcn.org").toStdString().c_str()));

������ʾ�ַ�����
Quote:

DisplayHTMLStr(widget->winId(), (TCHAR *)(QString("<h1>�������</h1>").toStdString().c_str()));

ɾ�����widgetǰ��ʱ��
Quote:

UnEmbedBrowserObject(widget->winId());
delete widget;


Ϊ�˷���ʹ�ã��û��ɴ�QWidget�̳�һ���࣬�����ĸ������Ĳ�����װ��ȥ��


����Ϳ��Կ�ʼ�����ˣ����ǣ�����ʱ����ִ�����Ҫ�������ڿ�Ĳ�ͬ��������ȷʶ����ţ�
ֻҪ��ͷ�ļ�������ͷ���¾Ϳ���Ϊmingw�����ˣ���Ȼ��ǰ�Ǹ�VC��ֱ��ʹ�õģ�
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

����ΪʲôҪ�����ģ���λ�ɲ���һ�²�ͬ��������������DLL��֮��Ĳ�ͬ��������ϡ�
*/

#endif /* __CWEBPAGE_H_INCLUDED */
