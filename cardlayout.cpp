#include "cardlayout.h"

QPoint CardLayout::cardArtCenter(int index)
{
    static const QPoint topLeft(398, 337);
    static const QPoint cardOfs(140, 200);

    const int x = topLeft.x() + cardOfs.x() * (index % 8);
    const int y = topLeft.y() + cardOfs.y() * (index / 8);
    return QPoint(x, y);
}
