#include "logparser.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

static QByteArray PREFIX_PICK = "[UnityCrossThreadLogger]==> BotDraft_DraftPick ";

LogParser::LogParser(const QString &directory, WatchType watchType, QObject *parent)
    : QObject(parent)
    , m_directory(directory)
{
    if (!QFile::exists(directory + QDir::separator() + "player.log"))
        qFatal("log file doesn't exist");

    if (watchType == Watcher) {
        m_watcher.reset(new QFileSystemWatcher({directory + QDir::separator() + "player.log"}));
        connect(m_watcher.data(), &QFileSystemWatcher::fileChanged, this, &LogParser::onFileChanged);
    } else {
        m_timer.reset(new QTimer(this));
        m_timer->setSingleShot(false);
        m_timer->setInterval(500);
        m_timer->start();
        connect(m_timer.data(), &QTimer::timeout, this, [=]() {
            onFileChanged(m_directory + QDir::separator() + "player.log");
        });
    }
}

void LogParser::startParsing()
{
    onFileChanged(m_directory + QDir::separator() + "Player-prev.log");
    onFileChanged(m_directory + QDir::separator() + "player.log");
}

void LogParser::parseJson(const QByteArray &json)
{
    const QJsonObject root = QJsonDocument::fromJson(json).object();
    if (!root.isEmpty()) {
        if (root["CurrentModule"] == "BotDraft") {
            const QByteArray payloadStr = root["Payload"].toString().toUtf8();
            const QJsonObject payload = QJsonDocument::fromJson(payloadStr).object();
            const int pack = payload["PackNumber"].toInt(-1);
            const int pick = payload["PickNumber"].toInt(-1);

            QVector<int> cards;
            for (const QJsonValue &value : payload["DraftPack"].toArray()) {
                const int card = value.toString().toInt();
                if (card > 0)
                    cards << card;
            }

            emit draftPack(pack, pick, cards);
        }

        if (root["request"].isString()) {
            const QByteArray requestStr = root["request"].toString().toUtf8();
            const QJsonObject request = QJsonDocument::fromJson(requestStr).object();

            QByteArray payloadStr = request["Payload"].toString().toUtf8();
            payloadStr.replace("\\", "");
            const QJsonObject payload = QJsonDocument::fromJson(payloadStr).object();
            const QJsonObject info = payload["PickInfo"].toObject();

            const int pack = info["PackNumber"].toInt(-1);
            const int pick = info["PickNumber"].toInt(-1);
            const int card = info["CardId"].toString().toInt();

            if (card > 0) {
                emit draftPick(pack, pick, card);
            }
        }
    }
}

void LogParser::onFileChanged(const QString &path)
{
    const int fileSize = QFileInfo(path).size();
    if (m_lastFileSize != fileSize) {
        m_lastFileSize = fileSize;

        QFile file(path);
        if (file.open(QFile::ReadOnly)) {
            const QByteArray data = file.readAll();
            const QVector<QByteArray> lines = data.split('\n');

            // new file?
            if (m_lastLineCount > lines.size())
                m_lastLineCount = 0;

            for (int i = m_lastLineCount; i < lines.size(); ++i) {
                if (lines[i].startsWith("{")) {
                    parseJson(lines[i]);
                }
                else if (lines[i].startsWith(PREFIX_PICK)) {
                    parseJson(lines[i].mid(PREFIX_PICK.size()));
                }
            }
            m_lastLineCount = lines.size();
        }
    }
}
