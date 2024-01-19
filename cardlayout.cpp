#include "cardlayout.h"

#include <QDebug>
#include <QVector3D>

QPoint CardLayout::cardArtCenter(int index)
{
    static const QPoint topLeft(398, 337);
    static const QPoint cardOfs(140, 200);

    const int x = topLeft.x() + cardOfs.x() * (index % 8);
    const int y = topLeft.y() + cardOfs.y() * (index / 8);
    return QPoint(x, y);
}

QColor CardLayout::draftValueColor(float relValue)
{
    static const QVector<QVector3D> STEPS = {
        QVector3D(240, 240, 240),  // white
        QVector3D(210, 210, 255),  // blue
        QVector3D(160, 230, 160),  // green
        QVector3D(255, 255, 180),  // yellow
        QVector3D(255, 160, 160),  // red
        QVector3D(255, 160, 255),  // purple
    };

    const float rel = relValue * (STEPS.size() - 1);
    const int idx0 = qBound(0, qFloor(rel), STEPS.size() - 1);
    const int idx1 = qBound(0, qCeil(rel), STEPS.size() - 1);
    const float d = rel - idx0;

    const QVector3D color = (1.0f - d) * STEPS[idx0] + d * STEPS[idx1];
    return QColor(color.x(), color.y(), color.z(), 255);
}
