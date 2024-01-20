#pragma once

#include <QColor>
#include <QHash>
#include <QNetworkAccessManager>
#include <QObject>
#include <QVector>

struct CardStatistics;

class CardDatabase : public QObject
{
    Q_OBJECT

public:

    enum Color
    {
        None    = 0x0,
        White   = 0x1,
        Blue    = 0x2,
        Black   = 0x4,
        Red     = 0x8,
        Green   = 0x10,
    };
    Q_DECLARE_FLAGS(Colors, Color)
    Q_FLAG(Color)

    enum Rarity
    {
        Unknown,
        Common,
        Uncommon,
        Rare,
        Mythic,
    };

    CardDatabase();
    ~CardDatabase() = default;

    void addSet(const QByteArray &setName);
    CardStatistics stats(int card) const;

signals:
    void dataChanged();

public slots:
    void clearCache();

private slots:
    void onRequestFinished(QNetworkReply *reply);

private:
    void saveSetData(const QByteArray &set, const QByteArray &data) const;
    bool loadSetData(const QByteArray &set);
    void startDownload(const QByteArray &set);
    bool addCardData(const QByteArray &json);

    QScopedPointer<QNetworkAccessManager> m_network;
    QHash<QNetworkReply*, QByteArray> m_currentRequests;
    QHash<int, CardStatistics> m_cards;
    QVector<QByteArray> m_sets;
};

struct CardStatistics
{
    int id;
    QString name;
    CardDatabase::Colors color;
    CardDatabase::Rarity rarity;
    float avgSeen = 0.f;
    float avgPick = 0.f;
    float winRate = 0.f;

    bool isWhite() const { return color & CardDatabase::White; }
    bool isBlue() const { return color & CardDatabase::Blue; }
    bool isBlack() const { return color & CardDatabase::Black; }
    bool isRed() const { return color & CardDatabase::Red; }
    bool isGreen() const { return color & CardDatabase::Green; }

    QColor colorMix() const;
};
