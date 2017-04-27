#ifndef ARTITEMSMODELMOCK_H
#define ARTITEMSMODELMOCK_H

#include <QVector>
#include <QPair>
#include <functional>
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "artworkmetadatamock.h"

namespace Mocks {
    class ArtItemsModelMock : public Models::ArtItemsModel
    {
    public:
        ArtItemsModelMock(): m_BlockUpdates(true) {}

    public:
        virtual Models::ArtworkMetadata *createMetadata(const QString &filepath, qint64 directoryID) {
            ArtworkMetadataMock *metadata = new ArtworkMetadataMock(filepath, directoryID);
            metadata->initialize("Test title", "Test description",
                                 QStringList() << "keyword1" << "keyword2" << "keyword3");
            return metadata;
        }
        void setUpdatesBlocked(bool value) { m_BlockUpdates = value; }

        ArtworkMetadataMock *getMockArtwork(int index) const { return dynamic_cast<ArtworkMetadataMock*>(getArtwork(index)); }

        void removeAll() { this->removeArtworks(QVector<QPair<int, int> >() << qMakePair(0, rowCount() - 1)); }

        virtual void updateItemsAtIndices(const QVector<int> &indices) {  if (!m_BlockUpdates) { Models::ArtItemsModel::updateItemsAtIndices(indices); } }
        virtual void updateItemsInRanges(const QVector<QPair<int, int> > &ranges) { if (!m_BlockUpdates) { Models::ArtItemsModel::updateItemsInRanges(ranges); } }

        void foreachArtwork(std::function<void (int index, ArtworkMetadataMock *metadata)> action) {
            int size = getArtworksCount();
            for (int i = 0; i < size; ++i) {
                auto *item = dynamic_cast<ArtworkMetadataMock*>(getArtwork(i));
                action(i, item);
            }
        }

    private:
        bool m_BlockUpdates;
    };
}

#endif // ARTITEMSMODELMOCK_H
