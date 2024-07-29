#pragma once

#include <QFileSystemWatcher>
#include <QJsonValue>
#include <QString>
#include <QTimer>

class LogParser : public QObject
{
    Q_OBJECT

public:
    enum WatchType {
        Watcher,
        Timer,
    };

    LogParser(const QString &directory, WatchType watchType = Timer, QObject *parent = nullptr);
    void startParsing();

signals:
    void draftPack(int pack, int pick, QVector<int> cards);

private slots:
    void onFileChanged(const QString &path);

private:
    void parseJson(const QByteArray &json);

    QScopedPointer<QTimer> m_timer;
    QScopedPointer<QFileSystemWatcher> m_watcher;
    QString m_directory;

    int m_lastFileSize = 0;
    int m_lastLineCount = 0;
};
