#pragma once

#include <QObject>
#include <QPoint>
#include <QTimer>

class MouseTracker : public QObject
{
    Q_OBJECT

public:
    MouseTracker(QObject *parent = nullptr);
    ~MouseTracker() = default;

private slots:
    void onTimeout();

private:
    QTimer *m_timer;
    QPoint m_pos;
    bool m_left;
    bool m_middle;
    bool m_right;
};
