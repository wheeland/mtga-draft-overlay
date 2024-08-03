#pragma once

#include <QObject>

#include "landsdb.h"
#include "scryfalldb.h"

class CardDatabase : public QObject
{
    Q_OBJECT

public:
    CardDatabase();
    ~CardDatabase() = default;

    /** returns whether data is available already */
    bool request(const QVector<int> &ids);

    bool get(int id, ScryfallCardData &scryfallData, SeventeenLandsCardStats &seventeenLandsStats);

    const ScryfallDatabase &scryfallDB() const { return m_scryfallDB; }
    const SeventeenLandsDatabase &seventeenLandsDB() const { return m_seventeenLandsDB; }

signals:
    void dataAvailable();

public slots:
    void clearCache();

private slots:
    void checkIfAvailable();

private:
    void clearAvailableCards();

    ScryfallDatabase m_scryfallDB;
    SeventeenLandsDatabase m_seventeenLandsDB;

    QVector<int> m_requestedCards;
};
