#pragma once

#include <QObject>
#include <QPoint>
#include <QTimer>

class InputTracker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool alt MEMBER m_alt NOTIFY modifiersChanged)
    Q_PROPERTY(bool ctrl MEMBER m_ctrl NOTIFY modifiersChanged)

public:
    InputTracker(QObject *parent = nullptr);
    ~InputTracker() = default;

private slots:
    void onTimeout();

signals:
    void modifiersChanged();

private:
    QTimer *m_timer;

    QPoint m_pos;
    bool m_left;
    bool m_middle;
    bool m_right;
    bool m_alt;
    bool m_ctrl;
};
