#include "inputtracker.h"

#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QWindow>

#include <windows.h>
#include <winuser.h>

static bool isPressed(int button)
{
    return GetAsyncKeyState(button) & ~0x1;
}

InputTracker::InputTracker(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer())
    , m_pos(QCursor::pos())
    , m_left(isPressed(VK_LBUTTON))
    , m_middle(isPressed(VK_MBUTTON))
    , m_right(isPressed(VK_RBUTTON))
    , m_alt(isPressed(VK_MENU))
    , m_ctrl(isPressed(VK_CONTROL))
{
    m_timer->setInterval(1);
    connect(m_timer, &QTimer::timeout, this, &InputTracker::onTimeout);
    m_timer->start();
}

void InputTracker::onTimeout()
{
    const QPoint pos = QCursor::pos();
    const bool left = isPressed(VK_LBUTTON);
    const bool middle = isPressed(VK_MBUTTON);
    const bool right = isPressed(VK_RBUTTON);
    const bool alt = isPressed(VK_MENU);
    const bool ctrl = isPressed(VK_CONTROL);

    Qt::MouseButtons buttons = Qt::NoButton;
    buttons |= left ? Qt::LeftButton : Qt::NoButton;
    buttons |= middle ? Qt::MiddleButton : Qt::NoButton;
    buttons |= right ? Qt::RightButton : Qt::NoButton;

    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    modifiers |= alt ? Qt::AltModifier : Qt::NoModifier;

    for (QWindow *window : QApplication::allWindows()) {
        if (pos != m_pos) {
            QApplication::postEvent(window, new QMouseEvent(
                QEvent::MouseMove, pos, pos, Qt::NoButton, buttons, modifiers
            ));
        }
        if (left != m_left) {
            QEvent::Type type = left ? QEvent::MouseButtonPress : QEvent::MouseButtonRelease;
            QApplication::postEvent(window, new QMouseEvent(
                type, pos, pos, Qt::LeftButton, buttons, modifiers
            ));
        }
        if (middle != m_middle) {
            QEvent::Type type = middle ? QEvent::MouseButtonPress : QEvent::MouseButtonRelease;
            QApplication::postEvent(window, new QMouseEvent(
                type, pos, pos, Qt::MiddleButton, buttons, modifiers
            ));
        }
        if (right != m_right) {
            QEvent::Type type = right ? QEvent::MouseButtonPress : QEvent::MouseButtonRelease;
            QApplication::postEvent(window, new QMouseEvent(
                type, pos, pos, Qt::RightButton, buttons, modifiers
            ));
        }
    }

    const bool changed = (alt != m_alt) || (ctrl != m_ctrl);

    m_pos = pos;
    m_left = left;
    m_middle = middle;
    m_right = right;
    m_alt = alt;
    m_ctrl = ctrl;

    if (changed) {
        emit modifiersChanged();
    }
}
