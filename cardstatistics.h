#pragma once

#include <QColor>
#include <QHash>
#include <QNetworkAccessManager>
#include <QObject>
#include <QVector>

#include "carddata.h"

struct CardStatistics;

class CardDatabase : public QObject
{
    Q_OBJECT

public:
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
    Card::Colors color;
    Card::Rarity rarity;
    float avgSeen = 0.f;
    float avgPick = 0.f;
    float winRate = 0.f;

    bool isWhite() const { return color & Card::White; }
    bool isBlue() const { return color & Card::Blue; }
    bool isBlack() const { return color & Card::Black; }
    bool isRed() const { return color & Card::Red; }
    bool isGreen() const { return color & Card::Green; }

    QColor colorMix() const;
};
