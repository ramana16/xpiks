#ifndef ARTWORKSREPOSITORYMOCK_H
#define ARTWORKSREPOSITORYMOCK_H

#include <QVector>
#include <QPair>
#include "../../xpiks-qt/Models/artworksrepository.h"

namespace Mocks {
    class ArtworksRepositoryMock : public Models::ArtworksRepository {
    public:
        ArtworksRepositoryMock() {}

        void removeFileAndEmitSignal() {
            insertIntoUnavailable(*getFilesSet().begin());
            emit filesUnavailable();
        }

        void removeVectorAndEmitSignal() {
            QString vector = *getFilesSet().begin();
            vector.replace(".jpg", ".eps");
            insertIntoUnavailable(vector);
            emit filesUnavailable();
        }

        void setWasAddedAsFullDirectory(int index) { setFullDirectory(index); }
        void unsetWasAddedAsFullDirectory(int index) { unsetFullDirectory(index); }
    };
}

#endif // ARTWORKSREPOSITORYMOCK_H

