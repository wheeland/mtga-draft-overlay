#include "logparser.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

static QByteArray PREFIX_PICK = "[UnityCrossThreadLogger]==> BotDraft_DraftPick ";

LogParser::LogParser(const QString &directory, QObject *parent)
    : QObject(parent)
    , m_watcher(new QFileSystemWatcher({directory}))
{
    connect(
        m_watcher.data(),
        &QFileSystemWatcher::fileChanged,
        this,
        &LogParser::onFileChanged
    );

    onFileChanged(directory + QDir::separator() + "player.log");
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
    QFile file(path);

    if (file.open(QFile::ReadOnly)) {
        const QByteArray data = file.readAll();
        const QVector<QByteArray> lines = data.split('\n');

        for (int i = m_lastLineCount; i < lines.size(); ++i) {
            if (lines[i].startsWith("{")) {
                parseJson(lines[i]);
            }
            else if (lines[i].startsWith(PREFIX_PICK)) {
                parseJson(lines[i].mid(PREFIX_PICK.size()));
            }
        }
    }
}
