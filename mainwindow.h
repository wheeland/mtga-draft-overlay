#pragma once

#include <QWidget>

class OverlayWindow : public QWidget
{
    Q_OBJECT

public:
    OverlayWindow(QWidget *parent = nullptr);
    ~OverlayWindow();

    void paintEvent(QPaintEvent*) override;
    bool event(QEvent *event) override;

private:
    QString m_targetProcess;
    bool m_wasDown = false;
};
