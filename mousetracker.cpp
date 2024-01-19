#include "mousetracker.h"

#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QWindow>

#include <windows.h>
#include <winuser.h>

static bool isPressed(int button)
{
    return GetAsyncKeyState(button) & 0x1;
}

MouseTracker::MouseTracker(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer())
    , m_pos(QCursor::pos())
    , m_left(isPressed(VK_LBUTTON))
    , m_middle(isPressed(VK_MBUTTON))
    , m_right(isPressed(VK_RBUTTON))
{
    m_timer->setInterval(1);
    connect(m_timer, &QTimer::timeout, this, &MouseTracker::onTimeout);
    m_timer->start();
}

void MouseTracker::onTimeout()
{
    const QPoint pos = QCursor::pos();
    const bool left = isPressed(VK_LBUTTON);
    const bool middle = isPressed(VK_MBUTTON);
    const bool right = isPressed(VK_RBUTTON);

    Qt::MouseButtons buttons = Qt::NoButton;
    buttons |= left ? Qt::LeftButton : Qt::NoButton;
    buttons |= middle ? Qt::MiddleButton : Qt::NoButton;
    buttons |= right ? Qt::RightButton : Qt::NoButton;

    for (QWindow *window : QApplication::allWindows()) {
        if (pos != m_pos) {
            QApplication::postEvent(window, new QMouseEvent(
                QEvent::MouseMove, pos, pos, Qt::NoButton, buttons, Qt::NoModifier
            ));
        }
        if (left != m_left) {
            QApplication::postEvent(window, new QMouseEvent(
                QEvent::MouseButtonPress, pos, pos, Qt::LeftButton, buttons, Qt::NoModifier
            ));
        }
        if (middle != m_middle) {
            QApplication::postEvent(window, new QMouseEvent(
                QEvent::MouseButtonPress, pos, pos, Qt::MiddleButton, buttons, Qt::NoModifier
            ));
        }
        if (right != m_right) {
            QApplication::postEvent(window, new QMouseEvent(
                QEvent::MouseButtonPress, pos, pos, Qt::RightButton, buttons, Qt::NoModifier
            ));
        }
    }

    m_pos = pos;
    m_left = left;
    m_middle = middle;
    m_right = right;
}
