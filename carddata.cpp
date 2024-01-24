#include "carddata.h"

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
