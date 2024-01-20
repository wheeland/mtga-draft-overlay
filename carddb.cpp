#include "carddb.h"

CardDatabase::CardDatabase()
{
    connect(
        &m_scryfallDB,
        &ScryfallDatabase::cardAvailable,
        this,
        &CardDatabase::checkIfAvailable
    );

    connect(
        &m_seventeenLandsDB,
        &SeventeenLandsDatabase::dataChanged,
        this,
        &CardDatabase::checkIfAvailable
    );
}

bool CardDatabase::request(const QVector<int> &ids)
{
    m_requestedCards << ids;
    m_scryfallDB.request(ids);

    clearAvailableCards();
    return m_requestedCards.isEmpty();
}

bool CardDatabase::get(int id, ScryfallCardData &scryfallData, SeventeenLandsCardStats &seventeenLandsStats)
{
    ScryfallCardData cardData = m_scryfallDB.get(id);
    SeventeenLandsCardStats stats = m_seventeenLandsDB.stats(id);
    if (cardData && stats) {
        scryfallData = cardData;
        seventeenLandsStats = stats;
        return true;
    } else {
        return false;
    }
}

void CardDatabase::checkIfAvailable()
{
    const bool wasWaiting = !m_requestedCards.isEmpty();
    clearAvailableCards();
    const bool isReady = m_requestedCards.isEmpty();

    if (wasWaiting && isReady) {
        emit dataAvailable();
    }
}

void CardDatabase::clearAvailableCards()
{
    m_requestedCards.removeIf([=](int id) {
        ScryfallCardData cardData = m_scryfallDB.get(id);
        if (!cardData)
            return false;

        m_seventeenLandsDB.addSet(cardData.set);
        SeventeenLandsCardStats stats = m_seventeenLandsDB.stats(id);
        if (!stats)
            return false;

        return true;
    });
}
