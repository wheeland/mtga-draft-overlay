#pragma once

#include <QObject>

class Card : public QObject
{
    Q_OBJECT

public:
    enum Color
    {
        Uncolored    = 0x0,
        White   = 0x1,
        Blue    = 0x2,
        Black   = 0x4,
        Red     = 0x8,
        Green   = 0x10,
    };
    Q_DECLARE_FLAGS(Colors, Color)
    Q_FLAG(Color)

    static QVector<Colors> monoColors();
    static QVector<Colors> dualColors();
    static int colorCount(Colors colors);
    static QByteArray colorString(Colors colors);

    enum Rarity
    {
        Unknown,
        Common,
        Uncommon,
        Rare,
        Mythic,
    };
};
