#pragma once

#include <QFileSystemWatcher>
#include <QJsonValue>
#include <QString>

class LogParser : public QObject
{
    Q_OBJECT

public:
    LogParser(const QString &directory, QObject *parent = nullptr);

signals:
    void draftPack(int pack, int pick, QVector<int> cards);
    void draftPick(int pack, int pick, int card);

private slots:
    void onFileChanged(const QString &path);

private:
    void parseJson(const QByteArray &json);

    QScopedPointer<QFileSystemWatcher> m_watcher;
    int m_lastLineCount = 0;
};
