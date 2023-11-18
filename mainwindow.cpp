#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <QTouchEvent>
#include <QPainter>
#include <QCursor>

#include <windows.h>

void sendMousePress(Qt::MouseButton button)
{
    INPUT input;
    memset(&input, 0, sizeof(input));
    input.type = INPUT_MOUSE;

    switch (button) {
    case Qt::LeftButton:
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case Qt::MiddleButton:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    case Qt::RightButton:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    default:
        return;
    }

    SendInput(1, &input, sizeof(INPUT));
}

void sendMouseRelease(Qt::MouseButton button)
{
    INPUT input;
    memset(&input, 0, sizeof(input));
    input.type = INPUT_MOUSE;

    switch (button) {
    case Qt::LeftButton:
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        break;
    case Qt::MiddleButton:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        break;
    case Qt::RightButton:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        break;
    default:
        return;
    }

    SendInput(1, &input, sizeof(INPUT));
}

OverlayWindow::OverlayWindow(QWidget *parent)
    : QWidget(parent)
{
    const QSize size = QApplication::primaryScreen()->size();

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(size);

    // add 'quit' button in top-left
    // filter mouse/touch events
}

OverlayWindow::~OverlayWindow()
{
}

void OverlayWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(0, 0, 0, 255));
    painter.setOpacity(0.01);
    painter.drawRect(rect());
}

bool OverlayWindow::event(QEvent *event)
{
    const bool isBegin = (event->type() == QEvent::TouchBegin);
    const bool isMove = (event->type() == QEvent::TouchUpdate);
    const bool isEnd = (event->type() == QEvent::TouchEnd);
    const bool isCancel = (event->type() == QEvent::TouchCancel);

    if (!isBegin && !isMove && !isEnd && !isCancel)
        return QWidget::event(event);

    QTouchEvent *touch = (QTouchEvent*)event;

    if (touch->points().size() == 1) {
        QEventPoint pt = touch->points().first();
        QCursor::setPos(pt.position().toPoint());

        qDebug() << pt.position().toPoint();

        if (isBegin) {
            qDebug() << "press";
            //sendMousePress(Qt::LeftButton);
        } else if (isEnd || isCancel) {
            qDebug() << "release";
            //sendMouseRelease(Qt::LeftButton);
        }
    }

    return true;
}

