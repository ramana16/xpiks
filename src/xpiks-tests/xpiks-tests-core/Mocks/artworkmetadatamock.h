#ifndef ARTWORKMETADATAMOCK
#define ARTWORKMETADATAMOCK

#include <QString>
#include "../../xpiks-qt/Models/imageartwork.h"
#include "../../xpiks-qt/MetadataIO/originalmetadata.h"

namespace Mocks {
    class ArtworkMetadataMock : public Models::ImageArtwork {
    public:
        ArtworkMetadataMock(const QString &filepath, qint64 directoryID = 0):
            Models::ImageArtwork(filepath, 0, directoryID)
        {
        }

        void initialize(const QString &title, const QString &description, const QStringList &keywords, bool overwrite=false) {
            Q_UNUSED(overwrite);
            MetadataIO::OriginalMetadata om;
            om.m_Title = title;
            om.m_Description = description;
            om.m_Keywords = keywords;
            this->initFromOrigin(om, overwrite);
        }

        void set(const QString &title, const QString &description, const QStringList &keywords) {
            this->setTitle(title);
            this->setDescription(description);
            this->setKeywords(keywords);
            this->resetModified();
        }

        QString retrieveKeyword(int index) { return getBasicModel()->retrieveKeyword(index); }
        int rowCount() const { return getBasicModel()->rowCount(); }

        bool isTitleEmpty() { return getBasicModel()->isTitleEmpty(); }
        bool isDescriptionEmpty() { return getBasicModel()->isDescriptionEmpty(); }

        bool areKeywordsEmpty() { return getBasicModel()->areKeywordsEmpty(); }

        void resetAll() { this->clearModel(); this->resetFlags(); }

        //virtual QString getDirectory() const { return "somedirectory"; }
    };
}

#endif // ARTWORKMETADATAMOCK

