#include "cardlayout.h"

#include <QDebug>
#include <QVector3D>

struct Layout {
    static Layout layout(QSize resolution)
    {
        if (resolution == QSize(1920, 1080)) {
            return { {398, 240}, {160, 200} };
        } else if (resolution == QSize(2560, 1440)) {
            return { {520, 310}, {214, 274}};
        }
        else {
            qFatal("unsupported resolution");
        }
    }

    QPoint topLeft;
    QPoint cardOfs;
};

void CardLayout::setSize(QSize size)
{
    m_size = size;
    emit update();
}

QRect CardLayout::cardArtRect(int index)
{
    const Layout layout = Layout::layout(m_size);

    const int x = layout.topLeft.x() + layout.cardOfs.x() * (index % 8);
    const int y = layout.topLeft.y() + layout.cardOfs.y() * (index / 8);
    const int w = 120;
    const int h = 120;

    return QRect(x - w/2, y - h/2, w, h);
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
