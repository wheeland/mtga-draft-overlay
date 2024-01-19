#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <QTouchEvent>
#include <QPainter>
#include <QCursor>
#include <QTimer>

#include <windows.h>
#include <cstring>
#include <tlhelp32.h>

struct EnumWindowsParams
{
    DWORD pid = 0;
    QVector<HWND> windowHandles;
};

static BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
    EnumWindowsParams *params = (EnumWindowsParams*)lParam;

    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd, &lpdwProcessId);
    if (lpdwProcessId == params->pid) {
        params->windowHandles << hwnd;
        return FALSE;
    }
    return TRUE;
}

static QVector<HWND> getWindowsWithName(const wchar_t *name)
{
    QVector<int> pids;
    QVector<HWND> handles;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (!wcscmp(entry.szExeFile, name)) {
                EnumWindowsParams params;
                params.pid = entry.th32ProcessID;
                EnumWindows(EnumWindowsProcMy, (LPARAM)&params);
                handles << params.windowHandles;
                pids << params.pid;
            }
        }
    }

    if (handles.size() == 0) {
        qDebug() << "pids:" << pids;
    }
    else if (handles.size() > 1) {
        qDebug() << "pids:" << pids;
        qDebug() << "handles:" << handles;
    }

    return handles;
}

static int makeLParam(QPoint pos)
{
    return ((pos.x() << 16) | (pos.y() & 0xFFFF));
}

static void sendMouseMove(HWND window, QPoint pos)
{
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
    qDebug() << "move" << pos << SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, makeLParam(pos));
}

static void sendMousePress(HWND window, QPoint pos)
{
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttondown
    qDebug() << "press" << pos << SendMessage(window, WM_LBUTTONDOWN, 0, makeLParam(pos));
}

static void sendMouseRelease(HWND window, QPoint pos)
{
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttonup
    qDebug() << "release" << pos << SendMessage(window, WM_LBUTTONUP, 0, makeLParam(pos));
}

OverlayWindow::OverlayWindow(QWidget *parent)
    : QWidget(parent)
{
    const QSize size = QApplication::primaryScreen()->size();
    m_targetProcess = "MTGA.exe";

    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::WindowTransparentForInput);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    resize(size);
}

OverlayWindow::~OverlayWindow()
{
}

void OverlayWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(0, 0, 0, 255));
    painter.setOpacity(0.1);
    painter.drawRect(rect());
}

bool OverlayWindow::event(QEvent *event)
{
    const bool isBegin = (event->type() == QEvent::TouchBegin);
    const bool isMove = (event->type() == QEvent::TouchUpdate);
    const bool isEnd = (event->type() == QEvent::TouchEnd);
    const bool isCancel = (event->type() == QEvent::TouchCancel);

    // only target touch events
    if (!isBegin && !isMove && !isEnd && !isCancel)
        return QWidget::event(event);

    // only target 1-touch events
    QTouchEvent *touch = (QTouchEvent*)event;
    if (touch->points().size() != 1)
        return false;

    // get target process window
    const wchar_t *name = (const wchar_t*) m_targetProcess.utf16();
    const QVector<HWND> windowHandles = getWindowsWithName(name);
    if (windowHandles.size() != 1) {
        qDebug() << "can't find single window for" << m_targetProcess;
        return false;
    }

    // get target process window geometry
    RECT rect{ 0, 0, 0, 0 };
    POINT topLeft{ 0, 0 };
    if (!GetClientRect(windowHandles[0], &rect) || !ClientToScreen(windowHandles[0], &topLeft)) {
        qDebug() << "can't find window geometry for" << m_targetProcess;
        return false;
    }
    topLeft.x = 0;
    topLeft.y = 0;

    const QRect windowRect(topLeft.x, topLeft.y, rect.right - rect.left, rect.bottom - rect.top);

    const QEventPoint pt = touch->points().first();

    if (!m_wasDown && isBegin) {
        sendMousePress(windowHandles[0], pt.position().toPoint() - windowRect.topLeft());
        m_wasDown = true;
    }
    else if (m_wasDown && isMove) {
        sendMouseMove(windowHandles[0], pt.position().toPoint() - windowRect.topLeft());
    }
    else if (m_wasDown && (isEnd || isCancel)) {
        sendMouseRelease(windowHandles[0], pt.position().toPoint() - windowRect.topLeft());
        m_wasDown = false;
    }

    return true;
}

