#pragma once

#include <QColor>
#include <QObject>
#include <QRect>

class CardLayout : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float dummy READ dummy NOTIFY update);

public:
    Q_INVOKABLE QRect cardArtRect(int index);
    Q_INVOKABLE QColor draftValueColor(float relValue);

    float dummy() const { return 0.f; }
    void setSize(QSize size);

signals:
        void update();

private:
    QSize m_size = QSize(1920, 1080);
};
