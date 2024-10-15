#include "carddata.h"

QVector<Card::Colors> Card::monoColors()
{
    return {
        Color::White,
        Color::Blue,
        Color::Black,
        Color::Red,
        Color::Green
    };
}

QVector<Card::Colors> Card::dualColors()
{
    return {
        Colors(Color::White | Color::Blue),
        Colors(Color::White | Color::Black),
        Colors(Color::White | Color::Green),
        Colors(Color::White | Color::Red),
        Colors(Color::Blue | Color::Black),
        Colors(Color::Blue | Color::Green),
        Colors(Color::Blue | Color::Red),
        Colors(Color::Black | Color::Green),
        Colors(Color::Black | Color::Red),
        Colors(Color::Green | Color::Red),
    };
}

int Card::colorCount(Colors colors)
{
    int count = 0;
    count += (colors & Color::White) ? 1 : 0;
    count += (colors & Color::Blue) ? 1 : 0;
    count += (colors & Color::Black) ? 1 : 0;
    count += (colors & Color::Red) ? 1 : 0;
    count += (colors & Color::Green) ? 1 : 0;
    return count;
}

QByteArray Card::colorString(Colors colors)
{
    QByteArray result;
    if (colors & Color::White) result += "W";
    if (colors & Color::Blue) result += "U";
    if (colors & Color::Black) result += "B";
    if (colors & Color::Red) result += "R";
    if (colors & Color::Green) result += "G";
    return result;
}
