#include "filter.h"

#include <QCoreApplication>
#include <QTouchEvent>

bool Filter::eventFilter(QObject *obj, QEvent *event)
{
    if (!qobject_cast<QCoreApplication*>(obj))
        return false;

    static int counter = 0;
    qDebug() << ++counter << event;

    const bool isBegin = (event->type() == QEvent::TouchBegin);
    const bool isMove = (event->type() == QEvent::TouchUpdate);
    const bool isEnd = (event->type() == QEvent::TouchEnd);
    const bool isCancel = (event->type() == QEvent::TouchCancel);

    if (!isBegin && !isMove && !isEnd && !isCancel)
        return false;

    QTouchEvent *touch = (QTouchEvent*)event;
    qDebug() << touch->points();
    return false;
}
