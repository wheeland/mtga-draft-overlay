#pragma once

#include <QObject>

class Filter : public QObject
{
    Q_OBJECT

public:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
