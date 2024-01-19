#pragma once

#include <QObject>
#include <QRect>

class CardLayout : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE QPoint cardArtCenter(int index);

private:
};
