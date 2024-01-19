#pragma once

#include <QColor>
#include <QObject>
#include <QRect>

class CardLayout : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE QPoint cardArtCenter(int index);
    Q_INVOKABLE QColor draftValueColor(float relValue);

private:
};
